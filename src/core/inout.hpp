#ifndef RBE_IN_OUT_HPP
#define RBE_IN_OUT_HPP 1

#include <ruby.h>
#include <utility>

#include "convert.hpp"

namespace rbe {

    template<typename _A0>
    class In
    {
    private:
        _A0 mData;

    public:
        In(_A0 data) : mData(data) {}
        _A0 Data() { return mData; }
    };

	template<>
	class In<std::pair<char *, int32> >
	{
	private:
		std::pair<char *, int32> mData;

	public:
		In(char *s, int32 l)
			: mData(s, l)
		{}
		char *Data() { return mData.first; }
		int32 Length() { return mData.second; }
	};

	template<>
	class In<std::pair<const char *, int32> >
	{
	private:
		std::pair<const char *, int32> mData;

	public:
		In(const char *s, int32 l)
			: mData(s, l)
		{}
		const char *Data() { return mData.first; }
		int32 Length() { return mData.second; }
	};

	template<typename _A0>
    class Out;

    template<typename _A0>
    class Out<_A0 *>
    {
    private:
        _A0 *mA0;
    public:
        Out(_A0 *a0) : mA0(a0) {};
        void Set(VALUE value)
        {
            (*mA0) = Convert<_A0>::FromValue(value);
        }
    };
    
    template<typename... _T>
    class InOut
    {};

    template<typename _A0, typename... _T>
    class InOut<In<_A0>, _T...> : public InOut<_T...>
    {
    private:
        In<_A0> mData;

    public:
        enum {
            COUNT_IN = InOut<_T...>::COUNT_IN + 1,
            COUNT_OUT = InOut<_T...>::COUNT_OUT
        };

        template<typename... _A>
        InOut(_A0 a0, _A... rest)
            : InOut<_T...>(rest...)
            , mData(a0)
        {
        }

        void ConvertToValues(VALUE *vargs)
        {
            *vargs = Convert<_A0>::ToValue(mData.Data());
            InOut<_T...>::ConvertToValues(vargs + 1);
        }
    };

    template<typename... _T>
    class InOut<In<std::pair<char *, int32> >, _T...> : public InOut<_T...>
    {
    private:
        In<std::pair<char *, int32> > mData;

    public:
        enum {
            COUNT_IN = InOut<_T...>::COUNT_IN + 1,
            COUNT_OUT = InOut<_T...>::COUNT_OUT
        };

        template<typename... _A>
        InOut(char *a0, int32 a1, _A... rest)
            : InOut<_T...>(rest...)
            , mData(a0, a1)
        {
        }

        void ConvertToValues(VALUE *vargs)
        {
            *vargs = rb_str_new(mData.Data(), mData.Length());
            InOut<_T...>::ConvertToValues(vargs + 1);
        }
    };

    template<typename... _T>
    class InOut<In<std::pair<const char *, int32> >, _T...> : public InOut<_T...>
    {
    private:
        In<std::pair<const char *, int32> > mData;

    public:
        enum {
            COUNT_IN = InOut<_T...>::COUNT_IN + 1,
            COUNT_OUT = InOut<_T...>::COUNT_OUT
        };

        template<typename... _A>
        InOut(const char *a0, int32 a1, _A... rest)
            : InOut<_T...>(rest...)
            , mData(a0, a1)
        {
        }

        void ConvertToValues(VALUE *vargs)
        {
            *vargs = rb_str_new(mData.Data(), mData.Length());
            InOut<_T...>::ConvertToValues(vargs + 1);
        }
    };
	
    template<typename _A0>
    class InOut<In<_A0> >
    {
    private:
        In<_A0> mData;

    public:
        enum {
            COUNT_IN = 1,
            COUNT_OUT = 0
        };

        InOut(_A0 a0) : mData(a0) {}

        void ConvertToValues(VALUE *vargs)
        {
            *vargs = Convert<_A0>::ToValue(mData.Data());
        }

        void SetOut(VALUE *result)
        {
        }
    };

    template<>
    class InOut<In<std::pair<char *, int32> > >
    {
    private:
        In<std::pair<char *, int32> > mData;

    public:
        enum {
            COUNT_IN = 1,
            COUNT_OUT = 0
        };

        InOut(char *a0, int32 a1) : mData(a0, a1) {}

        void ConvertToValues(VALUE *vargs)
        {
            *vargs = rb_str_new(mData.Data(), mData.Length());
        }

        void SetOut(VALUE *result)
        {
        }
    };

    template<>
    class InOut<In<std::pair<const char *, int32> > >
    {
    private:
        In<std::pair<const char *, int32> > mData;

    public:
        enum {
            COUNT_IN = 1,
            COUNT_OUT = 0
        };

        InOut(const char *a0, int32 a1) : mData(a0, a1) {}

        void ConvertToValues(VALUE *vargs)
        {
            *vargs = rb_str_new(mData.Data(), mData.Length());
        }

        void SetOut(VALUE *result)
        {
        }
    };

    template<typename _A0, typename... _T>
    class InOut<Out<_A0>, _T...> : public InOut<_T...>
    {
    private:
        Out<_A0> mData;

    public:
        enum {
            COUNT_IN = InOut<_T...>::COUNT_IN,
            COUNT_OUT = InOut<_T...>::COUNT_OUT + 1
        };

        template<typename... _A>
        InOut(_A0 a0, _A... rest)
            : InOut<_T...>(rest...)
            , mData(a0)
        {
        }

        void SetOut(VALUE *presult)
        {
            mData.Set(*presult);
            InOut<_T...>::SetOut(presult + 1);
        }

        _A0 Head()
        {
            return mData.Data();
        }
    };

    template<typename _A0>
    class InOut<Out<_A0> >
    {
    private:
        Out<_A0> mData;

    public:
        enum {
            COUNT_IN = 0,
            COUNT_OUT = 1
        };

        InOut(_A0 a0)
            : mData(a0)
        {}

        void ConvertToValues(VALUE *vargs)
        {
        }

        void SetOut(VALUE *presult)
        {
            mData.Set(*presult);
        }

        _A0 Head()
        {
            return mData.Data();
        }
    };
}

#endif
