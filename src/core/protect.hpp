#ifndef RBE_PROTECT_HPP
#define RBE_PROTECT_HPP

#include <ruby.h>

namespace rbe
{
	class ProtectBase
	{
	private:
		int fState;
		VALUE fResult;

	protected:
		ProtectBase() : fState(0), fResult(Qnil) {}

	public:
		virtual ~ProtectBase() {}

		int State() const { return fState; }

	private:
		static VALUE _protect_0(VALUE v);

	protected:
		virtual VALUE Exec() = 0;

	public:
		void operator()()
		{
			fResult = rb_protect(_protect_0,
								 (VALUE)((void *)this), &fState);
		}
	};

	template<typename F>
	class Protect : public ProtectBase
	{
	private:
		F &fFunction;

	public:
		Protect(F &fn) : fFunction(fn) {}
		virtual ~Protect() {}

	protected:
		virtual VALUE Exec()
		{
			fFunction();
			return Qnil;
		}
	};
}

#endif
