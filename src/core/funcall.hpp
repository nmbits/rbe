#ifndef RBE_FUNCALL_HPP
#define RBE_FUNCALL_HPP

#include <ruby.h>
#include <support/Debug.h>
#include "convert.hpp"
#include "debug.hpp"

namespace rbe
{
	template<typename T>
	class In
	{
	public:
		enum { IN = 1, OUT = 0 };
		typedef T Type;

	private:
		T fValue;

	public:
		In(T value)
			: fValue(value)
		{}

		void SetArg(VALUE **p)
		{
			RBE_TRACE(("In::SetArg"));
			**p = Convert<T>::ToValue(fValue);
			(*p) ++;
		}

		void SetResult(VALUE **p)
		{}
	};

	template<typename T>
	class Out
	{};

	template<typename T>
	class Out<T *>
	{
	public:
		enum { IN = 0, OUT = 1 };
		typedef T * Type;
		typedef T Deref;

	private:
		T *fPointer;

	public:
		Out(T *pointer)
			: fPointer(pointer)
		{}

		void SetArg(VALUE **p)
		{}

		void SetResult(VALUE **p)
		{
			*fPointer = Convert<T>::FromValue(**p);
			(*p) ++;
		}
	};

	class Funcall0
	{
	private:
		const char *fName;

	protected:
		Funcall0(const char *name)
			: fName(name)
		{}

	public:
		virtual ~Funcall0() {}

	protected:
		virtual int Arity() = 0;
		virtual int NumReturn() = 0;
		virtual int NumOut() = 0;
		virtual VALUE ReceiverValue() = 0;
		virtual void SetArgs(VALUE **p) = 0;
		virtual void SetResult(VALUE **p) = 0;

	public:
		void operator()() {
			RBE_TRACE(("Funcall0::operator()"));

			VALUE args[16];
			VALUE *args_ptr = args;

			int arity = Arity();
			SetArgs(&args_ptr);

			VALUE result =
				rb_funcall2(ReceiverValue(), rb_intern(fName), arity, args);


			int num_out = NumOut();
			int expected = NumReturn() + num_out;
			VALUE *result_ptr = &result;
			if (num_out > 1) {
				Check_Type(result, T_ARRAY);
				if (RARRAY_LEN(result) != expected) {
					rb_raise(rb_eRangeError,
						"wrong number of returned values (%d for %d)",
						RARRAY_LEN(result), expected);
				}
				result_ptr = RARRAY_PTR(result);
			}
			SetResult(&result_ptr);
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
		virtual void SetResult(VALUE **p)
		{
			fResult = Convert<R>::FromValue(**p);
			(*p) ++;
		}
		virtual int NumReturn() { return 1; }

	public:
		R Result() { return fResult; }
		virtual ~Funcall2() {}
	};

	template<typename C>
	class Funcall2<C, void> : public Funcall1<C>
	{
	protected:
		Funcall2(const C *r, const char *name)
			: Funcall1<C>(r, name)
		{}
		virtual void SetResult(VALUE **p) {}
		virtual int NumReturn() { return 0; }

	public:
		virtual ~Funcall2() {}
	};

	template<typename X>
	class Funcall {};

	template<typename C, typename R>
	class Funcall<R (C::*)(void)> : public Funcall2<C, R>
	{
	public:
		typedef Funcall2<C, R> Super;
		Funcall(const C *r, const char *name)
			: Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity()
		{
			return 0;
		}

		virtual int NumOut()
		{
			return 0;
		}

		virtual void SetArgs(VALUE **p)
		{}
	};

	template<typename C, typename R,
			 typename A0>
	class Funcall<R (C::*)(A0)> : public Funcall2<C, R>
	{
	private:
		A0 &fA0;

	public:
		typedef Funcall2<C, R> Super;
		Funcall(const C *r, const char *name, A0 &a0)
			: fA0(a0)
			, Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity()
		{
			return A0::IN;
		}

		virtual int NumOut()
		{
			return A0::OUT;
		}

		virtual void SetArgs(VALUE **p)
		{
			fA0.SetArg(p);
		}

		virtual void SetResult(VALUE **p)
		{
			Super::SetResult(p);
			fA0.SetResult(p);
		}
	};

	template<typename C, typename R,
			 typename A0, typename A1>
	class Funcall<R (C::*)(A0, A1)> : public Funcall2<C, R>
	{
	private:
		A0 &fA0; A1 &fA1;

	public:
		typedef Funcall2<C, R> Super;
		Funcall(const C *r, const char *name, A0 &a0, A1 &a1)
			: fA0(a0), fA1(a1)
			, Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity()
		{
			return A0::IN + A1::IN;
		}

		virtual int NumOut()
		{
			return A0::OUT + A1::OUT;
		}

		virtual void SetArgs(VALUE **p)
		{
			RBE_TRACE(("Funcall(2)::SetArgs"));
			fA0.SetArg(p);
			fA1.SetArg(p);
		}

		virtual void SetResult(VALUE **p)
		{
			RBE_TRACE(("Funcall(2)::SetResult"));
			Super::SetResult(p);
			fA0.SetResult(p);
			fA1.SetResult(p);
		}
	};

	template<typename C, typename R,
			 typename A0, typename A1, typename A2>
	class Funcall<R (C::*)(A0, A1, A2)> : public Funcall2<C, R>
	{
	private:
		A0 &fA0; A1 &fA1; A2 &fA2;

	public:
		typedef Funcall2<C, R> Super;
		Funcall(const C *r, const char *name, A0 &a0, A1 &a1, A2 &a2)
			: fA0(a0), fA1(a1), fA2(a2)
			, Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity()
		{
			return A0::IN + A1::IN + A2::IN;
		}

		virtual int NumOut()
		{
			return A0::OUT + A1::OUT + A2::OUT;
		}

		virtual void SetArgs(VALUE **p)
		{
			fA0.SetArg(p);
			fA1.SetArg(p);
			fA2.SetArg(p);
		}

		virtual void SetResult(VALUE **p)
		{
			Super::SetResult(p);
			fA0.SetResult(p);
			fA1.SetResult(p);
			fA2.SetResult(p);
		}
	};

	template<typename C, typename R,
			 typename A0, typename A1, typename A2, typename A3>
	class Funcall<R (C::*)(A0, A1, A2, A3)> : public Funcall2<C, R>
	{
	private:
		A0 &fA0; A1 &fA1; A2 &fA2; A3 &fA3;

	public:
		typedef Funcall2<C, R> Super;
		Funcall(const C *r, const char *name, A0 &a0, A1 &a1, A2 &a2, A3 &a3)
			: fA0(a0), fA1(a1), fA2(a2), fA3(a3)
			, Funcall2<C, R>(r, name) {}
		virtual ~Funcall() {}

	protected:
		virtual int Arity()
		{
			return A0::IN + A1::IN + A2::IN + A3::IN;
		}

		virtual int NumOut()
		{
			return A0::OUT + A1::OUT + A2::OUT + A3::OUT;
		}

		virtual void SetArgs(VALUE **p)
		{
			fA0.SetArg(p);
			fA1.SetArg(p);
			fA2.SetArg(p);
			fA3.SetArg(p);
		}

		virtual void SetResult(VALUE **p)
		{
			Super::SetResult(p);
			fA0.SetResult(p);
			fA1.SetResult(p);
			fA2.SetResult(p);
			fA3.SetResult(p);
		}
	};
}

#endif /* __RBE_FUNCALL_H__ */
