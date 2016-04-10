
#ifndef RBE_CONVERT_HPP
#define RBE_CONVERT_HPP

#include <limits.h>

#include <ruby/ruby.h>

#define private public
#define protected public

#include <app/Message.h>
#include <app/Cursor.h>
#include <app/Invoker.h>
#include <interface/Alert.h>
#include <interface/Window.h>
#include <interface/InterfaceDefs.h>
#include <interface/GraphicsDefs.h>
#include <interface/Button.h>
#include <interface/TextView.h>
#include <interface/Font.h>
#include <interface/Menu.h>
#include <interface/MenuBar.h>

#undef private
#undef protected

#include "registory.hpp"
#include "type_map.hpp"

#include "Archivable.hpp"
#include "Invoker.hpp"
#include "Messenger.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Size.hpp"
#include "Font.hpp"

#include <utility>
#include <string.h>

#define RBE_IV_MOD_INVOKER "__rbe_iv_mod_invoker"

namespace rbe
{
	template<typename _T> struct Convert;

	template<>
	struct Convert<bool>
	{
		static VALUE ToValue(bool a)
		{
			return (a ? Qtrue : Qfalse);
		}

		static bool FromValue(VALUE v)
		{
			return (RTEST(v) ? true : false);
		}

		static bool IsConvertable(VALUE v)
		{
			return true;
		}
	};

	// int8

	template<>
	struct Convert<signed char>
	{
		static VALUE ToValue(signed char a)
		{
			return INT2FIX(a);
		}

		static signed char FromValue(VALUE v)
		{
			return (signed char)NUM2INT(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_FIXNUM)
					&& (FIX2INT(v) >= SCHAR_MIN)
					&& (FIX2INT(v) <= SCHAR_MAX));
		}
	};

	template<>
	struct Convert<unsigned char>
	{
		static VALUE ToValue(unsigned char a)
		{
			return UINT2NUM(a);
		}

		static unsigned char FromValue(VALUE v)
		{
			return (unsigned char)NUM2UINT(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_FIXNUM)
					&& (FIX2INT(v) >= 0)
					&& (FIX2INT(v) <= UCHAR_MAX));
		}
	};

	// int16

	template<>
	struct Convert<short>
	{
		static VALUE ToValue(short a)
		{
			return INT2FIX(a);
		}

		static short FromValue(VALUE v)
		{
			return (short)NUM2INT(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_FIXNUM)
					&& (NUM2INT(v) >= SHRT_MIN)
					&& (NUM2INT(v) <= SHRT_MAX));
		}
	};

	template<>
	struct Convert<unsigned short>
	{
		static VALUE ToValue(unsigned short a)
		{
			return UINT2NUM(a);
		}

		static unsigned short FromValue(VALUE v)
		{
			return (unsigned short)NUM2UINT(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_FIXNUM) && (NUM2UINT(v) < 65536));
		}
	};

	//// int

	template<>
	struct Convert<int>
	{
		static VALUE ToValue(int a)
		{
			return INT2NUM(a);
		}

		static int FromValue(VALUE v)
		{
			return NUM2INT(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_BIGNUM) || (TYPE(v) == T_FIXNUM));
		}
	};

	template<>
	struct Convert<unsigned int>
	{
		static VALUE ToValue(unsigned int a)
		{
			return UINT2NUM(a);
		}

		static unsigned int FromValue(VALUE v)
		{
			return NUM2UINT(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_BIGNUM) || (TYPE(v) == T_FIXNUM));
		}
	};

	//// int32 (long)

	template<>
	struct Convert<long>
	{
		static VALUE ToValue(long a)
		{
			return LONG2NUM(a);
		}

		static long FromValue(VALUE v)
		{
			return NUM2LONG(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_BIGNUM) || (TYPE(v) == T_FIXNUM));
		}
	};

	template<>
	struct Convert<unsigned long>
	{
		static VALUE ToValue(unsigned long a)
		{
			return ULONG2NUM(a);
		}
		static unsigned long FromValue(VALUE v)
		{
			return NUM2ULONG(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_BIGNUM) || (TYPE(v) == T_FIXNUM));
		}
	};

	//// int 64

	template<>
	struct Convert<long long>
	{
		static VALUE ToValue(long long a)
		{
			return LL2NUM(a);
		}

		static long long FromValue(VALUE v)
		{
			return NUM2LL(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_BIGNUM) || (TYPE(v) == T_FIXNUM));
		}
	};

	template<>
	struct Convert<unsigned long long>
	{
		static VALUE ToValue(unsigned long long a)
		{
			return ULL2NUM(a);
		}

		static unsigned long long FromValue(VALUE v)
		{
			return NUM2ULL(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_BIGNUM) || (TYPE(v) == T_FIXNUM));
		}
	};

	// float
	template<>
	struct Convert<float>
	{
		static VALUE ToValue(float f)
		{
			return DBL2NUM(f);
		}

		static float FromValue(VALUE v)
		{
			return NUM2DBL(v);
		}

		static bool IsConvertable(VALUE v)
		{
			switch (TYPE(v))
			{
				case T_FLOAT:
				case T_BIGNUM:
				case T_FIXNUM:
				case T_RATIONAL:
				return true;
			};
			return false;
		}
	};

	// double
	template<>
	struct Convert<double>
	{
		static VALUE ToValue(double f)
		{
			return DBL2NUM(f);
		}

		static double FromValue(VALUE v)
		{
			return NUM2DBL(v);
		}

		static bool IsConvertable(VALUE v)
		{
			switch (TYPE(v))
			{
				case T_FLOAT:
				case T_BIGNUM:
				case T_FIXNUM:
				case T_RATIONAL:
				return true;
			};
			return false;
		}
	};

	// char *

	template<>
	struct Convert<char *>
	{
		static VALUE ToValue(char *a)
		{
			if (a == NULL)
				return Qnil;

			VALUE s = rb_str_new_cstr(a);
			rb_str_freeze(s);
			return s;
		}

		static char *FromValue(VALUE v)
		{
			return StringValueCStr(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_STRING) || (rb_respond_to(v, rb_intern("to_str"))));
		}
	};

	// const char *

	template<>
	struct Convert<const char *>
	{
		static VALUE ToValue(const char *a)
		{
			if (a == NULL)
				return Qnil;

			VALUE s = rb_str_new_cstr(a);
			rb_str_freeze(s);
			return s;
		}

		static const char *FromValue(VALUE v)
		{
			if (NIL_P(v))
				return NULL;
			return StringValueCStr(v);
		}

		static bool IsConvertable(VALUE v)
		{
			if (NIL_P(v))
				return true;
			return ((TYPE(v) == T_STRING) || (rb_respond_to(v, rb_intern("to_str"))));
		}
	};

	// char * and length

	template<>
	struct Convert<std::pair<char *, int32> >
	{
		static VALUE ToValue(std::pair<char *, int32> a)
		{
			if (a.first == NULL)
				return Qnil;
			VALUE s = rb_str_new(a.first, a.second);
			return s;
		}

		static std::pair<char *, int32> FromValue(VALUE v)
		{
			char *str = StringValueCStr(v);
			std::pair<char *, int32> answer(str, strlen(str));
			return answer;
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_STRING) || (rb_respond_to(v, rb_intern("to_str"))));
		}
	};

	template<>
	struct Convert<std::pair<const char *, int32> >
	{
		static VALUE ToValue(std::pair<const char *, int32> a)
		{
			if (a.first == NULL)
				return Qnil;
			VALUE s = rb_str_new(a.first, a.second);
			return s;
		}

		static std::pair<const char *, int32> FromValue(VALUE v)
		{
			const char *str = StringValueCStr(v);
			std::pair<const char *, int32> answer(str, strlen(str));
			return answer;
		}

		static bool IsConvertable(VALUE v)
		{
			return ((TYPE(v) == T_STRING) || (rb_respond_to(v, rb_intern("to_str"))));
		}
	};
	
#define RBE_COMPATIBLE_CONVERT(orig, compat)				\
	template<>												\
	struct Convert<orig>									\
	{														\
		static VALUE ToValue(orig a)						\
		{													\
			return Convert<compat>::ToValue((compat)a);		\
		}													\
		static orig FromValue(VALUE v)						\
		{													\
			return (orig) Convert<compat>::FromValue(v);	\
		}													\
		static bool IsConvertable(VALUE v)					\
		{													\
			return Convert<compat>::IsConvertable(v);		\
		}													\
	}

	RBE_COMPATIBLE_CONVERT(char, signed char);

	RBE_COMPATIBLE_CONVERT(window_type, unsigned long);
	RBE_COMPATIBLE_CONVERT(window_feel, unsigned long);
	RBE_COMPATIBLE_CONVERT(window_look, unsigned long);
	RBE_COMPATIBLE_CONVERT(window_alignment, unsigned long);

	// interface/InterfaceDefs.h
	RBE_COMPATIBLE_CONVERT(mode_mouse, unsigned long);
	RBE_COMPATIBLE_CONVERT(mode_focus_follows_mouse, unsigned long);
	RBE_COMPATIBLE_CONVERT(border_style, unsigned long);
	RBE_COMPATIBLE_CONVERT(orientation, unsigned long);
	RBE_COMPATIBLE_CONVERT(button_width, unsigned long);
	RBE_COMPATIBLE_CONVERT(alignment, unsigned long);
	RBE_COMPATIBLE_CONVERT(vertical_alignment, unsigned long);
	RBE_COMPATIBLE_CONVERT(join_mode, unsigned long);
	RBE_COMPATIBLE_CONVERT(cap_mode, unsigned long);
	RBE_COMPATIBLE_CONVERT(bitmap_tiling, unsigned long);
	RBE_COMPATIBLE_CONVERT(overlay_options, unsigned long);
	RBE_COMPATIBLE_CONVERT(bitmap_drawing_options, unsigned long);
	RBE_COMPATIBLE_CONVERT(color_which, unsigned long);

	// interface/GraphicsDefs.h

	RBE_COMPATIBLE_CONVERT(color_space, unsigned long);
	RBE_COMPATIBLE_CONVERT(buffer_orientation, unsigned long);
	RBE_COMPATIBLE_CONVERT(buffer_layout, unsigned long);
	RBE_COMPATIBLE_CONVERT(drawing_mode, unsigned long);
	RBE_COMPATIBLE_CONVERT(source_alpha, unsigned long);
	RBE_COMPATIBLE_CONVERT(alpha_function, unsigned long);

	// interface/Button.h
	RBE_COMPATIBLE_CONVERT(BButton::BBehavior, unsigned long);

	// interface/Menu.h
	RBE_COMPATIBLE_CONVERT(menu_layout, unsigned long);

	// interface/MenuBar.h
	RBE_COMPATIBLE_CONVERT(menu_bar_border, unsigned long);

	// interface/Alert.h
	RBE_COMPATIBLE_CONVERT(alert_type, unsigned long);
	RBE_COMPATIBLE_CONVERT(button_spacing, unsigned long);

	// app/Cursor.h
	RBE_COMPATIBLE_CONVERT(BCursorID, unsigned long);

	template<>
	struct Convert<rgb_color>
	{
		static VALUE ToValue(rgb_color c);
		static rgb_color FromValue(VALUE v);
		static bool IsConvertable(VALUE v);
	};

	// support/SupportDefs.h

	template<>
	struct Convert<pattern>
	{
		static VALUE ToValue(pattern a);
		static pattern FromValue(VALUE v);
		static bool IsConvertable(VALUE v);
	};

	// Messenger, Point, Rect
#define RBE_IMM_STRUCT_CONVERT(R)						\
	template<>											\
	struct Convert<R>									\
	{													\
		typedef typename WrapperOf<R>::Class wrapper_t;	\
		typedef typename PointerOf<R>::Class pointer_t;	\
		static VALUE ToValue(const R &m)				\
		{												\
			R *obj = new R(m);							\
			return wrapper_t::Wrap(obj);				\
		}												\
														\
		static R FromValue(VALUE v)						\
		{												\
			if (!rb_obj_is_kind_of(v, wrapper_t::Class()))		\
				rb_raise(rb_eTypeError, "%s required",	\
						 rb_class2name(wrapper_t::Class()));	\
			pointer_t *ptr =							\
				static_cast<pointer_t *>(DATA_PTR(v));	\
			R *obj = static_cast<R *>(ptr);				\
			return *obj;								\
		}												\
														\
		static bool IsConvertable(VALUE v)				\
		{												\
			if (rb_obj_is_kind_of(v, wrapper_t::Class()))		\
				return true;							\
			return false;								\
		}												\
	}

	RBE_IMM_STRUCT_CONVERT(BMessenger);
	RBE_IMM_STRUCT_CONVERT(BPoint);
	RBE_IMM_STRUCT_CONVERT(BRect);
	RBE_IMM_STRUCT_CONVERT(BSize);

	template<typename R>
	struct Convert<const R>
	{
		static VALUE ToValue(const R &m)
		{
			return Convert<R>::ToValue(m);
		}

		static const R FromValue(VALUE v)
		{
			return (const R)Convert<R>::FromValue(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return Convert<R>::IsConvertable(v);
		}
	};

	// References

	template<typename R>
	struct Convert<R &>
	{
		typedef typename WrapperOf<R>::Class wrapper_t;
		typedef typename PointerOf<R>::Class pointer_t;

//		static VALUE ToValue(const R& m)
//		{
//			const pointer_t *ptr = static_cast<pointer_t *>(&m);
//			VALUE value = wrapper_t::Wrap(ptr);
//			ObjectRegistory::Instance()->Register(value, false);
//			return value;
//		}

		static R& FromValue(VALUE v)
		{
			if (!rb_obj_is_kind_of(v, wrapper_t::Class()))
				rb_raise(rb_eTypeError, "%s required", rb_class2name(wrapper_t::Class()));
			pointer_t *ptr = static_cast<pointer_t *>(DATA_PTR(v));
			if (!ptr)
				rb_raise(rb_eRuntimeError, "the object was deleted or not initialized.");
			R *ret = static_cast<R *>(ptr);
			return *ret;
		}

		static bool IsConvertable(VALUE v)
		{
			return (rb_obj_is_kind_of(v, wrapper_t::Class()) ? true : false);
		}
	};

	template<typename R>
	struct Convert<const R&>
	{
		static const R& FromValue(VALUE v)
		{
			return (const R&)Convert<R&>::FromValue(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return Convert<R&>::IsConvertable(v);
		}
	};

	// BInvoker
	template<>
	struct Convert<BInvoker *>
	{
		typedef WrapperOf<BInvoker>::Class wrapper_t;
		typedef PointerOf<BInvoker>::Class pointer_t;

		static VALUE ToValue(BInvoker *invoker)
		{
			if (!invoker)
				return Qnil;

			BArchivable *archivable = dynamic_cast<BArchivable *>(invoker);
			if (archivable)
				return ObjectRegistory::Instance()->Get(static_cast<void *>(archivable));
			pointer_t *ptr = static_cast<pointer_t *>(invoker);
			return ObjectRegistory::Instance()->Get(static_cast<void *>(ptr));
		}

		static BInvoker *FromValue(VALUE v)
		{
			BInvoker *answer = NULL;
			if (rb_obj_is_kind_of(v, B::Archivable::Class())) {
				BArchivable *archivable = static_cast<BArchivable *>(DATA_PTR(v));
				answer = dynamic_cast<BInvoker *>(archivable);
			}
			if (answer == NULL && rb_obj_is_kind_of(v, B::Invoker::Class())) {
				VALUE modproxy = rb_iv_get(v, RBE_IV_MOD_INVOKER);
				if (NIL_P(modproxy)) {
					answer = new BInvoker;
					modproxy = B::Invoker::Wrap(answer);
					rb_iv_set(v, RBE_IV_MOD_INVOKER, modproxy);
				} else {
					answer = static_cast<BInvoker *>(DATA_PTR(modproxy));
				}
			}
			return answer;
		}

		static bool IsConvertable(VALUE v)
		{
			if (rb_obj_is_kind_of(v, B::Invoker::Class()))
				return true;
			return false;
		}
	};

	// Other objects

	template<typename R>
	struct Convert<R *>
	{
		typedef class WrapperOf<R>::Class wrapper_t;
		typedef class PointerOf<R>::Class pointer_t;

		static VALUE ToValue(R *a)
		{
			if (!a)
				return Qnil;

			pointer_t *ptr = a;
			return ObjectRegistory::Instance()->Get(static_cast<void *>(ptr));
		}

		static R *FromValue(VALUE v)
		{
			if (NIL_P(v))
				return NULL;

			if (!rb_obj_is_kind_of(v, wrapper_t::Class()))
				rb_raise(rb_eTypeError, "%s required", rb_class2name(wrapper_t::Class()));
			pointer_t *ptr = static_cast<pointer_t *>(DATA_PTR(v));
			if (!ptr)
				rb_raise(rb_eRuntimeError, "the object was deleted or not initialized.");
			R *ret = static_cast<R *>(ptr);
			return ret;
		}

		static bool IsConvertable(VALUE v)
		{
			if (NIL_P(v))
				return true;
			return (rb_obj_is_kind_of(v, wrapper_t::Class()) ? true : false);
		}
	};

	template<typename R>
	struct Convert<const R *>
	{
		static VALUE ToValue(const R *a)
		{
			return Convert<R *>::ToValue(const_cast<R *>(a));
		}

		static const R *FromValue(VALUE v)
		{
			return (const R *) Convert<R *>::FromValue(v);
		}

		static bool IsConvertable(VALUE v)
		{
			return Convert<R *>::IsConvertable(v);
		}
	};
}

#endif /* __RBE_CONVERT_HPP__ */
