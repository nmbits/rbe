#ifndef RBE_UTIL_VIEW_HPP
#define RBE_UTIL_VIEW_HPP 1

#define private public
#define protected public
#include <interface/View.h>
#undef private
#undef protected

namespace rbe
{
	namespace Util
	{
		bool SetLayoutCommon(BView *view, BLayout *layout);
	}
}

#endif
