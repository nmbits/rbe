
#ifndef RBE_LOCK_HPP
#define RBE_LOCK_HPP 1

#define private public
#define protected public

#include <app/Looper.h>
#include <app/Handler.h>
#include <app/Messenger.h>

#undef private
#undef protected

#include <ruby/ruby.h>
#include <ruby/thread.h>

#include "gvl.hpp"
#include <functional>

#define TICK_TIMEOUT 500000

namespace rbe
{
	namespace Util
	{
		inline status_t LockWithTimeoutCommon(std::function<status_t (bigtime_t)> &fn, bigtime_t timeout)
		{
			status_t status = fn(0);
			if (status != B_OK) {
				bool unblock = false;
				std::function<void ()> f = [&]() {
					while (!unblock) {
						bigtime_t tm = (TICK_TIMEOUT < timeout ? TICK_TIMEOUT : timeout);
						status = fn(tm);
						if (status != B_TIMED_OUT)
							break;
						if (timeout == B_INFINITE_TIMEOUT)
							continue;
						if (timeout < TICK_TIMEOUT)
							break;
						timeout -= TICK_TIMEOUT;
					}
				};
				CallWithoutGVL<std::function<void ()>, bool> g(f, &unblock);
				g();
				rb_thread_check_ints(); // correct?
			}
			return status;
		}

		inline status_t LockLooperWithTimeout(BLooper *looper, bigtime_t timeout)
		{
			std::function<status_t (bigtime_t)> f = [&](bigtime_t tm) -> status_t {
				return looper->LockWithTimeout(tm);
			};
			return LockWithTimeoutCommon(f, timeout);
		}
	} // Util
} // rbe

#endif
