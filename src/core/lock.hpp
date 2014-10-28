
#ifndef RBE_LOOPER_COMMON_HPP
#define RBE_LOOPER_COMMON_HPP

#include "rbe.hpp"

#define private public
#define protected public

#include <app/Looper.h>
#include <app/Handler.h>
#include <app/Messenger.h>

#undef private
#undef protected

#include <ruby/ruby.h>
#include <ruby/thread.h>

#include "call_without_gvl.hpp"

#define TICK_TIMEOUT 500000

namespace rbe
{
	namespace Private
	{
		namespace lock
		{
			template<typename T>
			inline status_t
			LockWithTimeoutFunc(T *target, bigtime_t time);

			template<>
			inline status_t
			LockWithTimeoutFunc<BLooper>(BLooper *target, bigtime_t time)
			{
				return target->BLooper::LockWithTimeout(time);
			}

			template<>
			inline status_t
			LockWithTimeoutFunc<BHandler>(BHandler *target, bigtime_t time)
			{
				return target->BHandler::LockLooperWithTimeout(time);
			}

			template<>
			inline status_t
			LockWithTimeoutFunc<BMessenger>(BMessenger *target, bigtime_t time)
			{
				return target->BMessenger::LockTargetWithTimeout(time);
			};

			template<typename T>
			struct LockWithTimeout_f
			{
				T *fTarget;
				bigtime_t fTimeout;
				bool unblock;
				status_t res;

				LockWithTimeout_f(T *t, bigtime_t to)
					: fTarget(t)
					, fTimeout(to)
				{}

				void operator()()
				{
					bigtime_t tm = (TICK_TIMEOUT < fTimeout ? TICK_TIMEOUT : fTimeout);
					while (!unblock) {
						res = LockWithTimeoutFunc<T>(fTarget, tm);
						if (res != B_TIMED_OUT)
							break;
						if (fTimeout == B_INFINITE_TIMEOUT)
							continue;
						if (fTimeout < TICK_TIMEOUT)
							break;
						fTimeout -= TICK_TIMEOUT;
					}
				}
			};

			template<typename T>
			status_t LockWithTimeout(T *target, bigtime_t time)
			{
				status_t status = LockWithTimeoutFunc<T>(target, 0);
				if (status == B_OK)
					return B_OK;
				LockWithTimeout_f<T> f(target, time);
				CallWithoutGVL<LockWithTimeout_f<T>, bool> g(f, &f.unblock);
				g();
				rb_thread_check_ints();
				return f.res;
			}
		}
	}

	namespace Util
	{
		namespace lock
		{
			inline status_t LockWithTimeout(BLooper *looper, bigtime_t time)
			{
				return Private::lock::LockWithTimeout<BLooper>(looper, time);
			}

			inline status_t LockWithTimeout(BHandler *handler, bigtime_t time)
			{
				return Private::lock::LockWithTimeout<BHandler>(handler, time);
			}

			inline status_t LockWithTimeout(BMessenger *messenger, bigtime_t time)
			{
				return Private::lock::LockWithTimeout<BMessenger>(messenger, time);
			}
		}
	}
}

#endif
