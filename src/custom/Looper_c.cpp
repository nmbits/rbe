
#include <kernel/OS.h>
#include <support/Debug.h>

#include "rbe.hpp"

#include <ruby.h>

#include "protect.hpp"
#include "convert.hpp"
#include "gvl.hpp"
#include "lock.hpp"

#include "Message.hpp"
#include "Handler.hpp"
#include "Looper.hpp"
#include "util_looper.hpp"

#define private public
#define protected public

#include <app/Application.h>
#include <interface/Window.h>
#include <interface/Alert.h>

#undef private
#undef protected

#include <functional>

namespace rbe
{
	static inline VALUE
	lock_with_timeout(BLooper *looper, bigtime_t timeout)
	{
		std::function<status_t (bigtime_t)> f = [&](bigtime_t timeout) -> status_t {
			return looper->LockWithTimeout(timeout);
		};
		return Util::LockWithTimeoutCommon(f, timeout);
	}

	namespace B
	{
		void
		Looper::DispatchMessageST(BLooper *looper, BMessage *message, BHandler *handler)
		{
			RBE_TRACE(("Looper::DispatchMessageST"));
			Util::DispatchMessageCommon(looper, message, handler);
		}

		VALUE
		Looper::rbe_run(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rbe_run", argc, argv, self);
			return Util::rbe_run_common(argc, argv, self);
		}

		VALUE
		Looper::rbe_quit(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rbe_quit", argc, argv, self);

			if (argc)
				rb_raise(rb_eArgError,
						 "wrong number of arguments (%d for 0)", argc);

			BLooper *_this = Convert<BLooper *>::FromValue(self);
			thread_id tid = find_thread(NULL);
			
			if (_this->LockingThread() != tid)
				rb_raise(rb_eThreadError, "This thread doesn't have lock.");

			if (!_this->fRunCalled)
				return Qnil;

			if (_this->Thread() == tid)
				rb_raise(eQuitLooper, "Looper::rb_quit");

			std::function<void ()> f = [&]() {
				_this->BLooper::Quit();
			};
			
			CallWithoutGVL<std::function<void ()>, void> g(f);
			g();
			rb_thread_check_ints();

			return Qnil;
		}

		VALUE
		Looper::rbe_lock(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rbe_lock", argc, argv, self);

			BLooper *_this = Convert<BLooper *>::FromValue(self);
			if (argc != 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);
			status_t result = lock_with_timeout(_this, B_INFINITE_TIMEOUT);
			if (result == B_OK)
				return Qtrue;
			return Qfalse;
		}

		VALUE
		Looper::rbe_lock_with_timeout(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_lock_with_timeout", argc, argv, self);

			BLooper *_this = Convert<BLooper *>::FromValue(self);

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 0)", argc);
			if (!Convert<bigtime_t>::IsConvertable(*argv))
				rb_raise(rb_eTypeError, "wrong type of argument");

			bigtime_t arg0 = Convert<bigtime_t>::FromValue(*argv);
			status_t result = lock_with_timeout(_this, arg0);
			return Convert<status_t>::ToValue(result);
		}
	}
}
