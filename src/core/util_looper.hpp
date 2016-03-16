#ifndef RBE_UTIL_LOOPER_HPP
#define RBE_UTIL_LOOPER_HPP 1

#define private public
#define protected public
#include <app/Handler.h>
#include <app/Looper.h>
#include <app/Message.h>
#undef private
#undef protected

#include <ruby.h>

namespace rbe {
	namespace Util {
		void RemoveChildrenIfWindow(BLooper *looper);
		bool DispatchMessageCommon(BLooper *looper, BMessage *message, BHandler *handler);
		VALUE rbe_run_common(int argc, VALUE *argv, VALUE self);

		static inline void AssertLocked(BLooper *looper, thread_id tid = find_thread(NULL))
		{
			if (looper->LockingThread() != tid)
				rb_raise(rb_eThreadError, "looper must be locked before proceeding\n");
		}
	}
}

#endif
