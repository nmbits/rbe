
#include "call_without_gvl.hpp"
#include "debug.hpp"

namespace rbe
{
	ThreadLocalFlag *CallWithoutGVLBase::sGVLFlag = NULL;

	/* static */ void *
	CallWithoutGVLBase::call0(void *data)
	{
		RBE_TRACE("CallWithoutGVLBase::call0");
		CallWithoutGVLBase *_this = static_cast<CallWithoutGVLBase *>(data);
		_this->fCalled = true;
		return _this->Exec();
	}

	/* static */ void
	CallWithoutGVLBase::ubf0(void *data)
	{
		CallWithoutGVLBase *_this = static_cast<CallWithoutGVLBase *>(data);
		_this->Ubf();
	}

	/* static */ void
	CallWithoutGVLBase::InitStatic(ThreadLocalFlag *flag)
	{
		sGVLFlag = flag;
	}

	void *
	CallWithoutGVLBase::operator()()
	{
		void *ret;
		RBE_TRACE("CallWithoutGVLBase::operator()()");
		if (sGVLFlag->TryUnset()) {
			RBE_PRINT(("CallWithoutGVLBase::operator()() release gvl\n"));
			RBE_PRINT(("call0 = %p\n", CallWithoutGVLBase::call0));

			// work around (?)
			//  Sometime, rb_thread_call_without_gvl2 returns before
			//   calling call0, even when the thread seems not to be
			//   interrupted.
			while (true) {
				ret = rb_thread_call_without_gvl2(call0, (void *)this,
												  ubf0, (void *)this);
				if (fCalled || rb_thread_interrupted(rb_thread_current()))
					break;
				rb_thread_schedule();
			}
			sGVLFlag->Set(true);
		} else {
			RBE_PRINT(("CallWithoutGVLBase::operator()() already released\n"));
			ret = call0((void *)this);
		}
		return ret;
	}
}
