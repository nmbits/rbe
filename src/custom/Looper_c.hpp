#ifndef RBE_LOOPER_C_HPP
#define RBE_LOOPER_C_HPP

#include "rbe.hpp"

#include <app/Looper.h>

namespace rbe
{
	namespace Util
	{
		namespace Looper
		{
			struct UbfLooper_f
			{
				BLooper *fLooper;
				UbfLooper_f(BLooper *looper)
					: fLooper(looper)
				{}
				void operator()()
				{
					fLooper->PostMessage(RBE_MESSAGE_UBF);
				}
			};

			int
			DispatchMessageCommon(BLooper *looper, BMessage *message, BHandler *handler);

			void
			AssertLocked(BLooper *looper, thread_id tid = find_thread(NULL));

			VALUE
			rb_run_common(int argc, VALUE *argv, VALUE self);
		}
	}
}

#endif
