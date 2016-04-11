
#include "Alert.hpp"
#include "debug.hpp"
#include "convert.hpp"
#include "gvl.hpp"

#include <ruby.h>

#define RBE_ALERT_GO_CALLED "__rbe_alert_go_called"

namespace rbe
{
	namespace B
	{
		VALUE
		Alert::rbe_go(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BAlert::rbe_go", argc, argv, self);
			VALUE called = rb_iv_get(self, RBE_ALERT_GO_CALLED);
			if (!NIL_P(called))
				rb_raise(rb_eRuntimeError, "B::Alert#go can not be called twice");
			rb_iv_set(self, RBE_ALERT_GO_CALLED, Qtrue);
			VALUE vret = Qnil;
			BAlert *_this = Convert<BAlert *>::FromValue(self);
			int type_error_index = 0;
			if (0 == argc) {

				int32 ret;
				std::function<void ()> f = [&]() {
					ret = _this->BAlert::Go();
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				DATA_PTR(self) = NULL;
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<int32>::ToValue(ret);
				return vret;
			}

			if (argc < 0 || argc > 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for (0..1))", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}
	}
}
