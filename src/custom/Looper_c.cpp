
#include <kernel/OS.h>
#include <support/Debug.h>

#include "rbe.hpp"

#include <ruby.h>

#include "protect.hpp"
#include "funcall.hpp"
#include "convert.hpp"
#include "call_with_gvl.hpp"
#include "call_without_gvl.hpp"
#include "lock.hpp"

#include "Message.hpp"
#include "Handler.hpp"
#include "Looper.hpp"
#include "Looper_c.hpp"

#define private public
#define protected public

#include <app/Application.h>
#include <interface/Window.h>
#include <interface/Alert.h>

#undef private
#undef protected

namespace rbe
{
	namespace Private
	{
		namespace Looper
		{
			struct Quit_f
			{
				BLooper *fLooper;

				Quit_f(BLooper *looper)
					: fLooper(looper)
				{}

				void operator()()
				{
					fLooper->Quit();
				}
			};

			struct Task1_f
			{
				BLooper *fLooper;

				Task1_f(BLooper *looper)
					: fLooper(looper)
				{}

				void operator()()
				{
					if (fLooper->Lock()) {
						fLooper->task_looper();
					}
				}
			};

			struct Task0_f
			{
				BLooper *fLooper;

				Task0_f(BLooper *looper)
					: fLooper(looper)
				{}

				void operator()()
				{
					Task1_f f(fLooper);
					Util::Looper::UbfLooper_f u(fLooper);
					CallWithoutGVL<Task1_f, Util::Looper::UbfLooper_f> g(f, u);
					g();
					if (FuncallState())
						rb_jump_tag(FuncallState());
				}
			};

			VALUE
			looper_thread_func0(void *data)
			{
				BLooper *looper = static_cast<BLooper *>(data);

				rename_thread(find_thread(NULL), looper->Name());

				Task0_f f(looper);
				Protect<Task0_f> p(f);
				p();

				PointerOf<BLooper>::Class *ptr =
					static_cast<PointerOf<BLooper>::Class *>(looper);
				VALUE v = ObjectRegistory::Instance()->Get(ptr);
				DATA_PTR(v) = NULL;
				ObjectRegistory::Instance()->Unregister(ptr);
				delete looper;

				rb_thread_check_ints();

				if (p.State() != 0) {
					VALUE errinfo = rb_errinfo();
					if (!rb_obj_is_kind_of(errinfo, eQuitLooper))
						rb_jump_tag(p.State());
				}

				return Qnil;
			}

			struct DispatchMessage_f
			{
				BLooper *fLooper;
				BMessage *fMessage;
				BHandler *fHandler;

				DispatchMessage_f(BLooper *looper, BMessage *message, BHandler *handler)
					:fLooper(looper), fMessage(message), fHandler(handler)
				{}

				void operator()()
				{
					volatile VALUE vmessage = B::Message::Wrap(fMessage);
					if (fHandler != NULL) {
						PointerOf<BHandler>::Class *ptr = fHandler;
						VALUE v = ObjectRegistory::Instance()->Get(static_cast<void *>(ptr));
						if (NIL_P(v)) {
							// the handler is not under control of RBe
							BAlert *alert = dynamic_cast<BAlert *>(fLooper);
							if (alert) {
								alert->BAlert::DispatchMessage(fMessage, fHandler);
								return;
							}
							BWindow *window = dynamic_cast<BWindow *>(fLooper);
							if (window) {
								window->BWindow::DispatchMessage(fMessage, fHandler);
								return;
							}
							BApplication *app = dynamic_cast<BApplication *>(fLooper);
							if (app) {
								app->BApplication::DispatchMessage(fMessage, fHandler);
								return;
							}
							fLooper->BLooper::DispatchMessage(fMessage, fHandler);
							return;
						}
					}
					In<BMessage *> a0(fMessage);
					In<BHandler *> a1(fHandler);
					Funcall<void (BLooper::*)(In<BMessage *>, In<BHandler *>)> f(fLooper, "dispatch_message", a0, a1);
					f();
				}
			};

			static void
			RemoveChildrenIfWindow(BLooper *looper)
			{
				BWindow *window = dynamic_cast<BWindow *>(looper);
				if (window) {
					while (window->CountChildren())
						window->RemoveChild(window->ChildAt(0));
				}
			}
		}
	}

	namespace Util
	{
		namespace Looper
		{
			void
			AssertLocked(BLooper *looper, thread_id tid)
			{
				if (looper->LockingThread() != tid)
					rb_raise(rb_eThreadError, "looper must be locked before proceeding\n");
			}

			VALUE
			rb_run_common(int argc, VALUE *argv, VALUE self)
			{
				if (argc > 0)
					rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

				VALUE vret = Qnil;

				BLooper *_this = Convert<BLooper *>::FromValue(self);
	
				if (_this->LockingThread() != find_thread(NULL)) {
					rb_raise(rb_eThreadError, "the thread must have lock of this Looper");
					return Qnil;
				}

				if (_this->fRunCalled)
					rb_raise(rb_eThreadError, "can't call B::Looper#run twice!");

				// assume rb_thread_create never fail!
				_this->fRunCalled = true;
				VALUE thval = rb_thread_create((VALUE (*)(ANYARGS))Private::Looper::looper_thread_func0, static_cast<void *>(_this));
	
				rb_iv_set(thval, "__rbe_looper", self); // life line
				rb_iv_set(self, "__rbe_thread", thval); // to support Thread()

				_this->Unlock();

				return thval;
			}

			int DispatchMessageCommon(BLooper *looper, BMessage *message, BHandler *handler)
			{
				RBE_TRACE(("Looper::DispatchMessageCommon"));
				looper->DetachCurrentMessage();
				Private::Looper::DispatchMessage_f f(looper, message, handler);
				Protect<Private::Looper::DispatchMessage_f> p(f);
				CallWithGVL<Protect<Private::Looper::DispatchMessage_f> > g(p);
				g();
				return p.State();
			}
		}
	}

	namespace Hook
	{
		namespace Looper
		{
			void
			DispatchMessage(BLooper *looper, BMessage *message, BHandler *handler)
			{
				bool interrupted = false;
	
				switch(message->what) {
				case _QUIT_:
					Private::Looper::RemoveChildrenIfWindow(looper);
					looper->fTerminating = true;
					return;
	
				case B_QUIT_REQUESTED:
					if (handler == looper) {
						// from HAIKU's BLooper#_QuitRequested()
						bool isQuitting = looper->QuitRequested();
						if (isQuitting) {
							Private::Looper::RemoveChildrenIfWindow(looper);
							looper->fTerminating = true;
							return;
						}
						bool shutdown;
				        if (message->IsSourceWaiting()
							|| (message->FindBool("_shutdown_", &shutdown) == B_OK && shutdown)) {
							BMessage replyMessage(B_REPLY);
							replyMessage.AddBool("result", isQuitting);
							replyMessage.AddInt32("thread", find_thread(NULL));
							message->SendReply(&replyMessage);
				        }
					}
					break;
	
				case RBE_MESSAGE_UBF:
					Private::Looper::RemoveChildrenIfWindow(looper);
					looper->fTerminating = true;
					return;
	
				default:
					if (FuncallState() == 0) {
						int state = Util::Looper::DispatchMessageCommon(looper, message, handler);
						SetFuncallState(state);
					}
				}
	
				if (FuncallState() != 0) {
					Private::Looper::RemoveChildrenIfWindow(looper);
					looper->fTerminating = true;
				}
			}
		}
	}

	namespace B
	{
		VALUE
		Looper::rb_add_handler(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_add_handler", argc, argv, self);
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			BHandler *handler = Convert<BHandler *>::FromValue(*argv);
			BLooper *_this = Convert<BLooper *>::FromValue(self);
			_this->AddHandler(handler);
			MemorizeObject(self, *argv);
			MemorizeObject(*argv, self);
			return Qnil;
		}

		VALUE
		Looper::rb_remove_handler(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_remove_handler", argc, argv, self);
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			BHandler *handler = Convert<BHandler *>::FromValue(*argv);
			BLooper *_this = Convert<BLooper *>::FromValue(self);
			bool result = _this->RemoveHandler(handler);
			if (result) {
				ForgetObject(self, *argv);
				ForgetObject(*argv, self);
				return Qtrue;
			}
			return Qnil;
		}

		VALUE
		Looper::rb_run(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_run", argc, argv, self);
			return Util::Looper::rb_run_common(argc, argv, self);
		}

		VALUE
		Looper::rb_quit(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_quit", argc, argv, self);

			if (argc)
				rb_raise(rb_eArgError,
						 "wrong number of arguments (%d for 0)", argc);

			BLooper *_this = Convert<BLooper *>::FromValue(self);
			thread_id tid = find_thread(NULL);
			
			if (_this->LockingThread() != tid)
				rb_raise(rb_eThreadError, "This thread doesn't have lock.");

			if (!_this->fRunCalled) {
				return Qnil;
			}

			if (_this->Thread() == tid)
				rb_raise(eQuitLooper, "Looper::rb_quit");

			Private::Looper::Quit_f f(_this);
			CallWithoutGVL<Private::Looper::Quit_f, void> g(f);
			g();
			rb_thread_check_ints();

			return Qnil;
		}

		VALUE
		Looper::rb_lock(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_lock", argc, argv, self);
			VALUE vret = Qnil;

			BLooper *_this = Convert<BLooper *>::FromValue(self);

			if (argc != 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);
			status_t result = Util::lock::LockWithTimeout(_this, B_INFINITE_TIMEOUT);

			if (result == B_OK)
				return Qtrue;
			return Qfalse;
		}

		VALUE
		Looper::rb_lock_with_timeout(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_lock_with_timeout", argc, argv, self);
			VALUE vret = Qnil;

			BLooper *_this = Convert<BLooper *>::FromValue(self);

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 0)", argc);
			if (!Convert<bigtime_t>::IsConvertable(*argv))
				rb_raise(rb_eTypeError, "wrong type of argument");

			bigtime_t arg0 = Convert<bigtime_t>::FromValue(*argv);
			status_t result = Util::lock::LockWithTimeout(_this, arg0);

			return Convert<status_t>::ToValue(result);
		}
	}
}
