
#include <ruby.h>

#include "Looper.hpp"
#include "Window.hpp"

#include "rbe.hpp"
#include "looper_common.hpp"
#include "convert.hpp"

namespace rbe
{
	namespace B
	{
		void
		Window::DispatchMessage(BMessage *message, BHandler *handler)
		{
			if (!message)
				return;

			RBE_TRACE("Window::DispatchMessage");
			RBE_PRINT(("message = %p, handler = %p\n", message, handler));
			LooperCommon::DispatchMessageCommon(this, message, handler);
		}

		VALUE
		Window::rb_run(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Window::rb_run", argc, argv, self);
			return LooperCommon::rb_run_common(argc, argv, self);
		}

		VALUE
		Window::rb_show(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Window::rb_show", argc, argv, self);

			if (argc > 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			BWindow *_this = Convert<BWindow *>::FromValue(self);
			VALUE run_called = rb_iv_get(self, "__rbe_run_called");
			if (!RTEST(run_called))
				LooperCommon::rb_run_common(0, NULL, self);
			status_t status = LooperCommon::LockWithTimeout(static_cast<BLooper *>(_this), B_INFINITE_TIMEOUT);
			if (status == B_OK) {
				_this->Show(); // TODO might block ?
				_this->Unlock();
			}
			if (FuncallState() > 0)
				rb_jump_tag(FuncallState());

			return Qnil;
		}
	}
}
