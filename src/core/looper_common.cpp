
#include "looper_common.hpp"

#include <ruby.h>
#include "rbe.hpp"
#include "registory.hpp"
#include "funcall.hpp"
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
	}
}
