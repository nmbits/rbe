#ifndef RBE_FUNCALL_HPP
#define RBE_FUNCALL_HPP

#include <ruby.h>
#include <support/Debug.h>
#include "convert.hpp"
#include "debug.hpp"

namespace rbe
{
	class Funcall0
	{
	private:
		const char *fName;

	protected:
		Funcall0(const char *name) : fName(name) {}

	public:
		virtual ~Funcall0() {}

	protected:
		virtual int Arity() = 0;
		virtual VALUE ReceiverValue() = 0;
		virtual void ToArgs(VALUE *) = 0;
		virtual void SetResult(VALUE) = 0;

	public:
		void operator()() {
			VALUE args[16];
			VALUE recv = ReceiverValue();
			ID id = rb_intern(fName);
			int arity = Arity();
			ToArgs(args);
			SetResult(rb_funcall2(recv, id, arity, args));
		}
	};

	template<typename C>
	class Funcall1 : public Funcall0
	{
	private:
		const C *fReceiver;

	protected:
		Funcall1(const C *r, const char *name) : fReceiver(r), Funcall0(name) {}
		virtual VALUE ReceiverValue() { return Convert<const C *>::ToValue(fReceiver); }

	public:
		virtual ~Funcall1() {}
	};

	template<typename C, typename R>
	class Funcall2 : public Funcall1<C>
	{
	private:
		R fResult;

	protected:
		Funcall2(const C *r, const char *name) : Funcall1<C>(r, name) {}
		virtual void SetResult(VALUE v) { fResult = Convert<R>::FromValue(v); }

	public:
		R Result() { return fResult; }
		virtual ~Funcall2() {}
	};

	template<typename C>
	class Funcall2<C, void> : public Funcall1<C>
	{
	protected:
		Funcall2(const C *r, const char *name) : Funcall1<C>(r, name) {}
		virtual void SetResult(VALUE v) {}

	public:
		virtual ~Funcall2() {}
	};

	template<typename X>
	class Funcall {};

	template<typename C, typename R>
	class Funcall<R (C::*)(void)> : public Funcall2<C, R>
	{
	public:
		enum { ARITY = 0 };
		Funcall(const C *r, const char *name)
			:
			Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity() { return ARITY; }
		virtual void ToArgs(VALUE *args) {}
	};

	template<typename C, typename R,
			 typename A0>
	class Funcall<R (C::*)(A0)> : public Funcall2<C, R>
	{
	private:
		A0 fA0;

	public:
		enum { ARITY = 1 };
		Funcall(const C *r, const char *name, A0 a0)
			: fA0(a0)
			, Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity() { return ARITY; }
		virtual void ToArgs(VALUE *args)
		{
			args[0] = Convert<A0>::ToValue(fA0);
		}
	};

	template<typename C, typename R,
			 typename A0, typename A1>
	class Funcall<R (C::*)(A0, A1)> : public Funcall2<C, R>
	{
	private:
		A0 fA0; A1 fA1;

	public:
		enum { ARITY = 2 };
		Funcall(const C *r, const char *name, A0 a0, A1 a1)
			: fA0(a0), fA1(a1)
			, Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity() { return ARITY; }
		virtual void ToArgs(VALUE *args)
		{
			args[0] = Convert<A0>::ToValue(fA0);
			args[1] = Convert<A1>::ToValue(fA1);
		}
	};

	template<typename C, typename R,
			 typename A0, typename A1, typename A2>
	class Funcall<R (C::*)(A0, A1, A2)> : public Funcall2<C, R>
	{
	private:
		A0 fA0;	A1 fA1; A2 fA2;

	public:
		enum { ARITY = 3 };
		Funcall(const C *r, const char *name, A0 a0, A1 a1, A2 a2)
			: fA0(a0), fA1(a1), fA2(a2)
			, Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity() { return ARITY; }
		virtual void ToArgs(VALUE *args)
		{
			args[0] = Convert<A0>::ToValue(fA0);
			args[1] = Convert<A1>::ToValue(fA1);
			args[2] = Convert<A2>::ToValue(fA2);
		}
	};

	template<typename C, typename R,
			 typename A0, typename A1, typename A2, typename A3>
	class Funcall<R (C::*)(A0, A1, A2, A3)> : public Funcall2<C, R>
	{
	private:
		A0 fA0;	A1 fA1; A2 fA2; A3 fA3;

	public:
		enum { ARITY = 4 };
		Funcall(const C *r, const char *name, A0 a0, A1 a1, A2 a2, A3 a3)
			: fA0(a0), fA1(a1), fA2(a2), fA3(a3)
			, Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity() { return ARITY; }
		virtual void ToArgs(VALUE *args)
		{
			args[0] = Convert<A0>::ToValue(fA0);
			args[1] = Convert<A1>::ToValue(fA1);
			args[2] = Convert<A2>::ToValue(fA2);
			args[3] = Convert<A3>::ToValue(fA3);
		}
	};
}

#endif /* __RBE_FUNCALL_H__ */
