#ifndef RBE_GVL_HPP
#define RBE_GVL_HPP 1

#include <ruby/ruby.h>
#include <ruby/thread.h>

#define private public
#define protected public
#include <app/Looper.h>
#undef private
#undef protected

#include "rbe.hpp"
#include "protect.hpp"
#include "debug.hpp"

#include <functional>

namespace rbe {

	// call with GVL

	struct CallWithGVL0
	{
		static void *call0(void *p);
		void operator()();
		virtual void call1() = 0;
	};

	template<typename _F>
	struct CallWithGVL : public CallWithGVL0
	{
		_F &fFunction;

		CallWithGVL(_F &f) : fFunction(f) {}

		virtual void call1()
		{
			fFunction();
		}
	};

	static inline int ProtectedCallWithGVL(std::function<void ()> &f)
	{
		Protect<std::function<void()> > p(f);
		CallWithGVL<Protect<std::function<void()> > > g(p);
		g();
		return p.State();
	}

	// call without GVL

	struct CallWithoutGVL0
	{
		bool fCalled;
		CallWithoutGVL0()
			: fCalled(false)
		{}

		static void *call0(void *data);
		static void ubf0(void *data);
		void operator()();
		virtual void call1() = 0;
		virtual void ubf1() = 0;
	};

	template<typename F, typename U>
	struct CallWithoutGVL : public CallWithoutGVL0
	{
		F &fFunction;
		U &fUbf;

		CallWithoutGVL(F &f, U &u)
			: fFunction(f)
			, fUbf(u)
		{}

		virtual void call1()
		{
			RBE_TRACE("CallWithoutGVL<F, U>::call1");
			fFunction();
		}

		virtual void ubf1()
		{
			fUbf();
		}
	};

	template<typename F>
	struct CallWithoutGVL<F, void> : public CallWithoutGVL0
	{
		F &fFunction;

		CallWithoutGVL(F &f)
			: fFunction(f)
		{}

		virtual void call1()
		{
			RBE_TRACE("CallWithoutGVL<F, void>::call1");
			fFunction();
		}

		virtual void ubf1()
		{
		}
	};

	template<typename F>
	struct CallWithoutGVL<F, bool> : public CallWithoutGVL0
	{
		F &fFunction;
		bool *fFlag;

		CallWithoutGVL(F &f, bool *b)
			: fFunction(f)
			, fFlag(b)
		{
			*fFlag = false;
		}

		virtual void call1()
		{
			RBE_TRACE("CallWithoutGVL<F, bool>::call1");
			fFunction();
		}

		virtual void ubf1()
		{
			*fFlag = true;
		}
	};

	template<typename F>
	struct CallWithoutGVL<F, BLooper> : public CallWithoutGVL0
	{
		F &fFunction;
		BLooper *fLooper;

		CallWithoutGVL(F &f, BLooper *looper)
			: fFunction(f)
			, fLooper(looper)
		{
		}

		virtual void call1()
		{
			RBE_TRACE("CallWithoutGVL<F, bool>::call1");
			fFunction();
		}

		virtual void ubf1()
		{
			fLooper->PostMessage(RBE_MESSAGE_UBF);
		}
	};
}
#endif
