#ifndef RBE_INTERFACE_HPP
#define RBE_INTERFACE_HPP 1

#include <ruby.h>
#include "inout.hpp"

namespace rbe {

	template<typename _InOut, typename _R>
	struct Interface
	{
		_R mReturn;
		_InOut &mArgs;

		Interface(_InOut &a)
			: mArgs(a)
		{}

		_R Return()
		{
			return mReturn;
		}
		
		void SetResults(VALUE *values)
		{
			mReturn = Convert<_R>::FromValue(*values);
			mArgs.SetOut(values + 1);
		}

		void ArgsToValues(VALUE *p)
		{
			mArgs.ConvertToValues(p);
		}

		int NumIn()
		{
			return (int)_InOut::COUNT_IN;
		}

		int NumOut()
		{
			return (int)(_InOut::COUNT_OUT + 1);
		}
	};

	template<typename _InOut>
	struct Interface<_InOut, void>
	{
		_InOut &mArgs;

		Interface(_InOut &a)
			: mArgs(a)
		{}

		void SetResults(VALUE *values)
		{
			mArgs.SetOut(values);
		}

		void ArgsToValues(VALUE *p)
		{
			mArgs.ConvertToValues(p);
		}

		int NumIn()
		{
			return (int)_InOut::COUNT_IN;
		}

		int NumOut()
		{
			return (int)(_InOut::COUNT_OUT);
		}
	};

	template<typename _R>
	struct Interface<void, _R>
	{
		_R mReturn;

		Interface() {}

		_R Return()
		{
			return mReturn;
		}
		
		void SetResults(VALUE *values)
		{
			mReturn = Convert<_R>::FromValue(*values);
		}

		void ArgsToValues(VALUE *p)
		{
		}

		int NumIn()
		{
			return 0;
		}

		int NumOut()
		{
			return 1;
		}
	};

	template<>
	struct Interface<void, void>
	{
		Interface()	{}
		void SetResults(VALUE *values) {}
		void ArgsToValues(VALUE *p) {}
	};
}

#endif
