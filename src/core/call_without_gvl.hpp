
#ifndef RBE_CALL_WITHOUT_GVL_HPP
#define RBE_CALL_WITHOUT_GVL_HPP

#include <ruby/ruby.h>
#include <ruby/thread.h>

#include "thread_local_flag.hpp"
#include "debug.hpp"

namespace rbe
{
	class CallWithoutGVLBase
	{
	private:
		static ThreadLocalFlag *sGVLFlag;
		bool fCalled; // work around (?)

	protected:
		CallWithoutGVLBase() : fCalled(false) {}

	public:
		virtual ~CallWithoutGVLBase() {}

		static void InitStatic(ThreadLocalFlag *gvlflag);

		void *operator()();

	private:
		static void *call0(void *);
		static void ubf0(void *);

	protected:
		virtual void *Exec() = 0;
		virtual void Ubf() = 0;
	};

	template<typename F, typename U>
	class CallWithoutGVL : public CallWithoutGVLBase
	{
	private:
		F &fFunction;
		U &fUbf;

	public:
		CallWithoutGVL(F &f, U &u)
			: fFunction(f)
			, fUbf(u)
		{}
		virtual ~CallWithoutGVL() {}

	protected:
		virtual void *Exec()
		{
			RBE_TRACE("CallWithoutGVL<F, U>::Exec");
			fFunction();
			return NULL;
		}

		virtual void Ubf()
		{
			fUbf();
		}
	};

	template<typename F>
	class CallWithoutGVL<F, void> : public CallWithoutGVLBase
	{
	private:
		F &fFunction;

	public:
		CallWithoutGVL(F &f)
			: fFunction(f)
		{}
		virtual ~CallWithoutGVL() {}

	protected:
		virtual void *Exec()
		{
			RBE_TRACE("CallWithoutGVL<F, void>::Exec");
			fFunction();
			return NULL;
		}

		virtual void Ubf()
		{
		}
	};

	template<typename F>
	class CallWithoutGVL<F, bool> : public CallWithoutGVLBase
	{
	private:
		F &fFunction;
		bool *fFlag;

	public:
		CallWithoutGVL(F &f, bool *b)
			: fFunction(f)
			, fFlag(b)
		{
			*fFlag = false;
		}
		virtual ~CallWithoutGVL() {}

	protected:
		virtual void *Exec()
		{
			fFunction();
			return NULL;
		}

		virtual void Ubf()
		{
			*fFlag = true;
		}
	};
}

#endif
