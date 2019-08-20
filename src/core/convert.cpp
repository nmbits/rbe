
#include <support/Debug.h>
#include <kernel/OS.h>

#include <ruby.h>
#include "convert.hpp"

namespace rbe {
	// pattern (interface/GraphicsDefs.h)
	VALUE
	Convert<pattern>::ToValue(pattern a)
	{
		VALUE ary = rb_ary_new();
		for (int i = 0; i < 8; i++) {
			rb_ary_push(ary, UINT2NUM(a.data[i]));
		}
		return ary;
	}

	pattern
	Convert<pattern>::FromValue(VALUE v)
	{
		Check_Type(v, T_ARRAY);
		if (RARRAY_LEN(v) < 8) {
			rb_raise(rb_eArgError, "array is too short");
		}
		pattern answer;
		VALUE *ptr = RARRAY_PTR(v);
		for (int i = 0; i < 8; i++) {
			answer.data[i] = (uint8)NUM2UINT(ptr[i]);
		}
		return answer;
	}

	bool
	Convert<pattern>::IsConvertable(VALUE v)
	{
		if (TYPE(v) == T_ARRAY)
			return true;
		return false;	
	}

	// rgb_color (interface/GraphicsDefs.h)

	VALUE
	Convert<rgb_color>::ToValue(rgb_color c)
	{
		VALUE v = rb_ary_new();
		rb_ary_push(v, UINT2NUM(c.red));
		rb_ary_push(v, UINT2NUM(c.green));
		rb_ary_push(v, UINT2NUM(c.blue));
		rb_ary_push(v, UINT2NUM(c.alpha));
		return v;
	}

	rgb_color
	Convert<rgb_color>::FromValue(VALUE v)
	{
		int value;
		rgb_color c;

		Check_Type(v, T_ARRAY);

		switch (RARRAY_LEN(v)) {
		case 3:
			value = 0;
			break;
		case 4:
			value = NUM2INT(RARRAY_AREF(v, 3));
			break;
		default:
			rb_raise(rb_eRangeError, "given array cannot be interpreted as rgb_color (length should be 4)");
		}
		if (value < 0 || value > 255)
			rb_raise(rb_eRangeError, "given array cannot be interpreted as rgb_color (alpha out of range (0..255))");
		c. alpha = (uint8) value;
		value = NUM2INT(RARRAY_AREF(v, 0));
		if (value < 0 || value > 255)
			rb_raise(rb_eRangeError, "given array cannot be interpreted as rgb_color (red out of range (0..255))");
		c.red = (uint8)value;
		value = NUM2INT(RARRAY_AREF(v, 1));
		if (value < 0 || value > 255)
			rb_raise(rb_eRangeError, "given array cannot be interpreted as rgb_color (green out of range (0..255))");
		c.green = (uint8)value;
		value = NUM2INT(RARRAY_AREF(v, 2));
		if (value < 0 || value > 255)
			rb_raise(rb_eRangeError, "given array cannot be interpreted as rgb_color (blue out of range (0..255))");
		c.blue = (uint8)value;
		return c;
	}

	bool
	Convert<rgb_color>::IsConvertable(VALUE v)
	{
		if (TYPE(v) == T_ARRAY && ((RARRAY_LEN(v) == 3) || (RARRAY_LEN(v) == 4)))
			return true;
		return false;
	}
}
