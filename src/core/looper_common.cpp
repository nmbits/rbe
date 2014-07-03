
#include "rbe.hpp"

#define private public
#define protected public

#include <app/MessageFilter.h>
#include <app/Application.h>
#include <interface/Window.h>
#include <interface/Alert.h>

#undef private
#undef protected

#include <ruby.h>

#include "looper_common.hpp"
#include "registory.hpp"
#include "protect.hpp"
#include "funcall.hpp"
#include "call_with_gvl.hpp"
#include "debug.hpp"

#include "Looper.hpp"
#include "Message.hpp"
#include "Handler.hpp"

namespace rbe
{
	namespace LooperCommon
	{
		void
		UbfLooper::operator()()
		{
			looper->PostMessage(RBE_MESSAGE_UBF);
		}

		struct RaiseQuitLooper
		{
			void operator()()
			{
				rb_raise(eQuitLooper, "BLooper::Quit()");
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

		void
		AssertLocked(BLooper *looper, thread_id tid)
		{
			if (looper->LockingThread() != tid)
				rb_raise(rb_eThreadError, "looper must be locked before proceeding\n");
		}

		struct Task1
		{
			BLooper *looper;
			void operator()()
			{
				if (looper->Lock()) {
					looper->task_looper();
				}
			}
		};

		struct Task0
		{
			BLooper *looper;
			void operator()()
			{
				Task1 f = { looper };
				UbfLooper u = { looper };
				CallWithoutGVL<Task1, UbfLooper> g(f, u);
				g();
				if (FuncallState())
					rb_jump_tag(FuncallState());
			}
		};

		VALUE looper_thread_func0(void *data)
		{
			BLooper *looper = static_cast<BLooper *>(data);

			rename_thread(find_thread(NULL), looper->Name());

			Task0 f = { looper };
			Protect<Task0> p(f);
			p();

			PointerOf<BLooper>::Class *ptr = static_cast<PointerOf<BLooper>::Class *>(looper);
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

		VALUE rb_run_common(int argc, VALUE *argv, VALUE self)
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
			VALUE thval = rb_thread_create((VALUE (*)(ANYARGS))looper_thread_func0, static_cast<void *>(_this));

			rb_iv_set(thval, "__rbe_looper", self); // life line
			rb_iv_set(self, "__rbe_thread", thval); // to support Thread()

			_this->Unlock();

			return thval;
		}

		void CallDispatchMessage::operator()()
		{
			volatile VALUE vmessage = B::Message::Wrap(message);
			if (handler != NULL) {
				PointerOf<BHandler>::Class *ptr = handler;
				VALUE v = ObjectRegistory::Instance()->Get(static_cast<void *>(ptr));
				if (NIL_P(v)) {
					// the handler is not under control of RBe
					BAlert *alert = dynamic_cast<BAlert *>(looper);
					if (alert) {
						alert->BAlert::DispatchMessage(message, handler);
						return;
					}
					BWindow *window = dynamic_cast<BWindow *>(looper);
					if (window) {
						window->BWindow::DispatchMessage(message, handler);
						return;
					}
					BApplication *app = dynamic_cast<BApplication *>(looper);
					if (app) {
						app->BApplication::DispatchMessage(message, handler);
						return;
					}
					looper->BLooper::DispatchMessage(message, handler);
					return;
				}
			}
			Funcall<void (BLooper::*)(BMessage *, BHandler *)> f(looper, "dispatch_message", message, handler);
			f();
		}

		void DispatchMessageCommon(BLooper *looper, BMessage *message, BHandler *handler)
		{
			bool interrupted = false;

			switch(message->what) {
			case _QUIT_:
				RemoveChildrenIfWindow(looper);
				looper->fTerminating = true;
				return;

			case B_QUIT_REQUESTED:
				if (handler == looper) {
					// from HAIKU's BLooper#_QuitRequested()
					bool isQuitting = looper->QuitRequested();
					if (isQuitting) {
						RemoveChildrenIfWindow(looper);
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
				RemoveChildrenIfWindow(looper);
				looper->fTerminating = true;
				return;

			default:
				if (FuncallState() == 0) {
					looper->DetachCurrentMessage();
					LooperCommon::CallDispatchMessage f = { looper, message, handler };
					Protect<LooperCommon::CallDispatchMessage> p(f);
					CallWithGVL<Protect<LooperCommon::CallDispatchMessage> > g(p);
					g();
					SetFuncallState(p.State());
				}
			}

			if (FuncallState() != 0) {
				RemoveChildrenIfWindow(looper);
				looper->fTerminating = true;
			}
		}
	}
}
