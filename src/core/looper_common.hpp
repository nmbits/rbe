
#ifndef RBE_LOOPER_COMMON_HPP
#define RBE_LOOPER_COMMON_HPP

#include <app/Looper.h>
#include <app/Handler.h>
#include <app/Messenger.h>

#include <ruby/ruby.h>
#include <ruby/thread.h>
#include "call_without_gvl.hpp"

#define TICK_TIMEOUT 500000

namespace rbe
{
	namespace LooperCommon
	{
		void PostUbfMessage(void *looper_ptr);
		void DetachLooper(BLooper *looper, int state);
		void QuitLooper(BLooper *looper, int state);

		template<typename T>
		inline status_t LockWithTimeoutFunc(T *target, bigtime_t time);

		template<>
		inline status_t LockWithTimeoutFunc<BLooper>(BLooper *target, bigtime_t time)
		{
			return target->BLooper::LockWithTimeout(time);
		}

		template<>
		inline status_t LockWithTimeoutFunc<BHandler>(BHandler *target, bigtime_t time)
		{
			return target->BHandler::LockLooperWithTimeout(time);
		}

		template<>
		inline status_t LockWithTimeoutFunc<BMessenger>(BMessenger *target, bigtime_t time)
		{
			return target->BMessenger::LockTargetWithTimeout(time);
		};

		template<typename T>
		struct LockWithTimeoutStruct
		{
			T *target;
			bigtime_t timeout;
			bool unblock;
			status_t res;

			LockWithTimeoutStruct(T *t, bigtime_t to)
				: target(t)
				, timeout(to)
			{}

			void operator()()
			{
				bigtime_t tm = (TICK_TIMEOUT < timeout ? TICK_TIMEOUT : timeout);
				while (!unblock) {
					res = LockWithTimeoutFunc<T>(target, tm);
					if (res != B_TIMED_OUT)
						break;
					if (timeout == B_INFINITE_TIMEOUT)
						continue;
					if (timeout < TICK_TIMEOUT)
						break;
					timeout -= TICK_TIMEOUT;
				}
			}
		};

		template<typename T>
		inline status_t LockWithTimeout(T *target, bigtime_t time)
		{
			status_t status = LockWithTimeoutFunc<T>(target, 0);
			if (status == B_OK)
				return B_OK;
			LockWithTimeoutStruct<T> f(target, time);
			CallWithoutGVL<LockWithTimeoutStruct<T>, bool> g(f, &f.unblock);
			g();
			rb_thread_check_ints();
			return f.res;
		}

		struct Unregister
		{
			BLooper *looper;
			void operator()();
		};

		struct CallDispatchMessage
		{
			BLooper *looper;
			BMessage *message;
			BHandler *handler;
			void operator()();
		};
	}
}

#endif
