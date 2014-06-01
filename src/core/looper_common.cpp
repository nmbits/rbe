
#include "looper_common.hpp"

#include <app/MessageFilter.h>

#include <ruby.h>
#include "rbe.hpp"
#include "registory.hpp"
#include "protect.hpp"
#include "funcall.hpp"
#include "call_with_gvl.hpp"
#include "debug.hpp"
#include "ft.h"

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
			Funcall<void (BLooper::*)(BMessage *, BHandler *)> f(looper, "dispatch_message", message, handler);
			f();
		}

		void DispatchMessageCommon(BLooper *looper, BMessage *message, BHandler *handler)
		{
			bool interrupted = false;

			switch(message->what) {
			case _QUIT_:
				PRINT(("_QUIT_ received\n"));
				looper->BLooper::DispatchMessage(message, handler);
				LooperCommon::DetachLooper(looper, FuncallState());
				return;

			case B_QUIT_REQUESTED:
				if (handler == looper) {
					// from HAIKU's BLooper#_QuitRequested()
					bool isQuitting = looper->QuitRequested();
					if (isQuitting) {
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
				LooperCommon::QuitLooper(looper, FuncallState());
				// NEVER RETURN
			}
		}
	}
}
