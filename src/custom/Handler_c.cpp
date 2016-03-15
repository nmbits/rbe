
#include <ruby.h>

#include "convert.hpp"
#include "lock.hpp"

#include "Handler.hpp"

#include <functional>

namespace rbe
{
	namespace B
	{
		VALUE
		Handler::rbe_lock_looper(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Handler::rbe_lock_looper", argc, argv, self);
			if (argc > 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);
			BHandler *_this = Convert<BHandler *>::FromValue(self);
			std::function<status_t (bigtime_t)> f = [&](bigtime_t timeout) -> status_t {
				return _this->BHandler::LockLooperWithTimeout(timeout);
			};
			status_t result = Util::LockWithTimeoutCommon(f, B_INFINITE_TIMEOUT);
			if (result == B_OK)
				return Qtrue;
			return Qfalse;
		}

		VALUE
		Handler::rbe_lock_looper_with_timeout(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Handler::rbe_lock_looper_with_timeout", argc, argv, self);
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 1)", argc);
			BHandler *_this = Convert<BHandler *>::FromValue(self);
			bigtime_t timeout = Convert<bigtime_t>::FromValue(*argv);
			std::function<status_t (bigtime_t)> f = [&](bigtime_t tm) -> status_t {
				return _this->BHandler::LockLooperWithTimeout(tm);
			};
			status_t result = Util::LockWithTimeoutCommon(f, timeout);
			return Convert<status_t>::ToValue(result);
		}

		VALUE
		Handler::rbe_unlock_looper(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Handler::rbe_unlock_looper", argc, argv, self);
			if (argc)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);
			BHandler *_this = Convert<BHandler *>::FromValue(self);
			if (!_this->Looper())
				rb_raise(rb_eRuntimeError, "This handler is not a member of any Loopers");
			_this->BHandler::UnlockLooper();
			return Qnil;
		}
	}
}
