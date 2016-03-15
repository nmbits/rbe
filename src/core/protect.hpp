#ifndef RBE_PROTECT_HPP
#define RBE_PROTECT_HPP 1

#include <ruby.h>

namespace rbe
{
	struct Protect0
	{
		int fState;

		Protect0()
			: fState(0)
		{}

		static VALUE call0(VALUE v);

		void operator()()
		{
			rb_protect(Protect0::call0, (VALUE)this, &fState);
		}

		int State() const
		{
			return fState;
		}

		virtual void call1() = 0;
	};

	template<typename _F>
	struct Protect : public Protect0
	{
		_F &fFunction;

		Protect(_F &f)
			: Protect0()
			, fFunction(f)
		{}

		virtual void call1()
		{
			fFunction();
		}
	};
}

#endif
