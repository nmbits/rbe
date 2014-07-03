
#include <ruby.h>
#include <ruby/thread.h>
#include <support/TLS.h>
#include "call_with_gvl.hpp"
#include "debug.hpp"

namespace rbe
{
	ThreadLocalFlag *CallWithGVLBase::sGVLFlag = NULL;

	/* static */ void *
	CallWithGVLBase::call0(void *data)
	{
		CallWithGVLBase *_this = static_cast<CallWithGVLBase *>(data);
		return _this->Exec();
	}

	/* static */ void
	CallWithGVLBase::InitStatic(ThreadLocalFlag *flag)
	{
		sGVLFlag = flag;
	}

	void *
	CallWithGVLBase::operator()()
	{
		RBE_TRACE("CallWithGVLBase::operator()()");
		void *ret;
		if (sGVLFlag->TrySet()) {
			RBE_PRINT(("CallWithGVLBase::operator()(): acquire GVL\n"));
			ret = rb_thread_call_with_gvl(&call0, (void *)this);
			sGVLFlag->Set(false);
		} else {
			RBE_PRINT(("CallWithGVLBase::operator()(): already have GVL\n"));
			ret = call0((void *)this);
		}
		return ret;
	}
}
