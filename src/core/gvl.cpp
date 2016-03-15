
#include "gvl.hpp"

extern "C" int ruby_thread_has_gvl_p(void);

namespace rbe {
	/* static */ void *CallWithGVL0::call0(void *p)
	{
		CallWithGVL0 *cwg = (CallWithGVL0 *)p;
		cwg->call1();
		return NULL;
	}

	void CallWithGVL0::operator()()
	{
		RBE_TRACE("CallWithGVL::operator()()");

		if (ruby_thread_has_gvl_p()) {
			RBE_PRINT(("CallWithGVLBase::operator()(): has GVL\n"));
			call1();
		} else {
			RBE_PRINT(("CallWithGVLBase::operator()(): hasn't GVL\n"));
			rb_thread_call_with_gvl(call0, (void *)this);
		}
	}

	/* static */ void *CallWithoutGVL0::call0(void *data)
	{
		RBE_TRACE("CallWithoutGVL0::call0");
		CallWithoutGVL0 *cwog = (CallWithoutGVL0 *)data;
		cwog->fCalled = true;
		cwog->call1();
		return NULL;
	}

	/* static */ void CallWithoutGVL0::ubf0(void *data)
	{
		CallWithoutGVL0 *cwog = (CallWithoutGVL0 *)data;
		cwog->ubf1();
	}

	void CallWithoutGVL0::operator()()
	{
		RBE_TRACE("CallWithoutGVL0::operator()");

		if (ruby_thread_has_gvl_p()) {
			RBE_PRINT(("CallWithoutGVL0::operator()() release gvl\n"));
			while (true) {
				rb_thread_call_without_gvl2(call0, (void *)this,
											ubf0, (void *)this);
				if (fCalled || rb_thread_interrupted(rb_thread_current()))
					break;
				rb_thread_schedule();
			}
		} else {
			RBE_PRINT(("CallWithoutGVL0::operator()() already released\n"));
			call1();
		}
	}
}
