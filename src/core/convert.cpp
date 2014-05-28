
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
}
