
#include "looper_common.hpp"

#include <app/MessageFilter.h>
#include <app/Application.h>
#include <interface/Window.h>
#include <interface/Alert.h>

#include <ruby.h>
#include "rbe.hpp"
#include "registory.hpp"
#include "protect.hpp"
#include "funcall.hpp"
#include "call_with_gvl.hpp"
#include "debug.hpp"
#include "ft.h"
#include "thread_attach_filter.hpp"

#include "Looper.hpp"
#include "Message.hpp"
#include "Handler.hpp"

namespace rbe
{
	namespace LooperCommon
	{
		void PostUbfMessage(void *looper_ptr)
		{
			BLooper *looper = static_cast<BLooper *>(looper_ptr);
			looper->PostMessage(RBE_MESSAGE_UBF);
		}

		static void FinalizeLooper(void *data)
		{
			BLooper *looper = static_cast<BLooper *>(data);
			PointerOf<BLooper>::Class *ptr = looper;
			VALUE v = ObjectRegistory::Instance()->Get(ptr);
			ObjectRegistory::Instance()->Unregister(ptr);
			DATA_PTR(v) = NULL;
		}

		static void RemoveChildren(BWindow *window)
		{
			while (window->CountChildren())
				window->RemoveChild(window->ChildAt(0));
		}

		void DetachLooper(BLooper *looper, int state)
		{
			ft_detach2(state, FinalizeLooper, looper);
		}

		void QuitLooper(BLooper *looper, int state)
		{
			DetachLooper(looper, state);
			looper->Quit();
		}

		void AssertLocked(BLooper *looper, thread_id tid)
		{
			if (looper->LockingThread() != tid)
				rb_raise(rb_eThreadError, "looper must be locked before proceeding\n");
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

			VALUE run_called = rb_iv_get(self, "__rbe_run_called");
			if (RTEST(run_called))
				rb_raise(rb_eRuntimeError, "run() already called");
			rb_iv_set(self, "__rbe_run_called", Qtrue);

			ft_handle_t *ft = ft_thread_create();
			VALUE ret = ft->thval;
			rb_iv_set(ret, "__rbe_looper", self); // life line
			rb_iv_set(self, "__rbe_thread", ret); // to support Thread()

			BMessageFilter *filter = new ThreadAttachFilter(ft);
			BMessage message(RBE_MESSAGE_REMOVE_FILTER);
			message.AddPointer("filter", static_cast<void *>(filter));
			_this->AddCommonFilter(filter);
			_this->PostMessage(&message);
			_this->Run();
			ft_wait_for_attach_completed(ft);

			return ret;
		}

		void Unregister::operator()()
		{
			RBE_TRACE("LooperCommon::Unregister::operator()()");
			PointerOf<BLooper>::Class *ptr = looper;
			VALUE self = ObjectRegistory::Instance()->Get(ptr);
			PRINT(("Unregister::operator()(): self = %d\n", self));
			ObjectRegistory::Instance()->Unregister(ptr);
			DATA_PTR(self) = NULL;
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
				PRINT(("_QUIT_ received\n"));
				{
					BWindow *window = dynamic_cast<BWindow *>(looper);
					if (window)
						LooperCommon::RemoveChildren(window);
					looper->BLooper::DispatchMessage(message, handler);
					LooperCommon::DetachLooper(looper, FuncallState());
				}
				return;

			case B_QUIT_REQUESTED:
				if (handler == looper) {
					// from HAIKU's BLooper#_QuitRequested()
					bool isQuitting = looper->QuitRequested();
					if (isQuitting) {
						BWindow *window = dynamic_cast<BWindow *>(looper);
						if (window)
							LooperCommon::RemoveChildren(window);
						LooperCommon::QuitLooper(looper, FuncallState());
						// NEVER RETURN (?)
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

			case RBE_MESSAGE_REMOVE_FILTER:
				{
					PRINT(("Looper: RBE_MESSAGE_REMOVE_FILTER\n"));
					void *ptr;
					if (B_OK == message->FindPointer("filter", &ptr)) {
						PRINT(("removing\n"));
						BMessageFilter *filter = static_cast<BMessageFilter *>(ptr);
						looper->RemoveCommonFilter(filter);
						delete filter;
					}
				}
				break;

			case RBE_MESSAGE_UBF:
				interrupted = true;
				break;

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

			if (interrupted || FuncallState() != 0) {
				BWindow *window = dynamic_cast<BWindow *>(looper);
				if (window)
					LooperCommon::RemoveChildren(window);
				LooperCommon::QuitLooper(looper, FuncallState());
				// NEVER RETURN
			}
		}
	}
}
