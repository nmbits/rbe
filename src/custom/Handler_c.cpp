
#include <ruby.h>

#include "convert.hpp"
#include "lock.hpp"

#include "Handler.hpp"

namespace rbe
{
	namespace B
	{
		VALUE
		Handler::rb_lock_looper(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Handler::rb_lock_looper", argc, argv, self);

			if (argc > 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			BHandler *_this = Convert<BHandler *>::FromValue(self);

			status_t result = Util::lock::LockWithTimeout(_this, B_INFINITE_TIMEOUT);
			if (result == B_OK)
				return Qtrue;
			return Qfalse;
		}

		VALUE
		Handler::rb_lock_looper_with_timeout(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Handler::rb_lock_looper_with_timeout", argc, argv, self);

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 1)", argc);

			BHandler *_this = Convert<BHandler *>::FromValue(self);
			bigtime_t timeout = Convert<bigtime_t>::FromValue(*argv);

			status_t result = Util::lock::LockWithTimeout(_this, timeout);

			return Convert<status_t>::ToValue(result);
		}

		VALUE
		Handler::rb_unlock_looper(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Handler::rb_unlock_looper", argc, argv, self);

			if (argc)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			BHandler *_this = Convert<BHandler *>::FromValue(self);
			if (!_this->Looper())
				rb_raise(rb_eRuntimeError, "called for no looper");
			_this->BHandler::UnlockLooper();
			return Qnil;
		}
	}
}
