#ifndef RBE_UTIL_LAYOUT_HPP
#define RBE_UTIL_LAYOUT_HPP

#include <ruby.h>

// #define private public
// #define protected public
// #include <interface/View.h>
// #undef protected
// #undef private

namespace rbe
{
	namespace Util
	{
		VALUE
		AllocateLayoutItemForView(VALUE view_);
	}
}

#endif
