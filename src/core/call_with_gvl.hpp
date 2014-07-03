
#ifndef RBE_CALL_WITH_GVL_HPP
#define RBE_CALL_WITH_GVL_HPP

#include <ruby.h>
#include "thread_local_flag.hpp"

namespace rbe {
	class CallWithGVLBase
	{
	private:
		static void *call0(void *data);
		static ThreadLocalFlag *sGVLFlag;

	protected:
		CallWithGVLBase() {}
		virtual void *Exec() = 0;

	public:
		virtual ~CallWithGVLBase() {}
		void *operator()();

		static void InitStatic(ThreadLocalFlag *flag);
	};

	template<typename F>
	class CallWithGVL : public CallWithGVLBase
	{
	private:
		F &fFunction;

	public:
		CallWithGVL(F &f) : fFunction(f) {}
		virtual ~CallWithGVL() {}

	protected:
		virtual void *Exec()
		{
			fFunction();
			return NULL;
		}
	};
}
#endif
