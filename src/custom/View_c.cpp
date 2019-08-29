
#include "View.hpp"
#include "Message.hpp"
#include "Rect.hpp"

#include "rbe.hpp"
#include "protect.hpp"
#include "convert.hpp"
#include "gvl.hpp"
#include "lock.hpp"
#include "gc.hpp"

#define protected public
#define private public
#include <interface/ScrollView.h>
#undef protected
#undef private

#include <functional>

namespace rbe
{
	namespace B
	{
		void
		View::rbe__gc_free(void *ptr)
		{
			PointerOf<BView>::Class *tmp = static_cast<PointerOf<BView>::Class *>(ptr);
			BView *view = static_cast<BView *>(tmp);

			// 1. remove self
			view->RemoveSelf();

			// 2. remove children
			while (BView *child = view->ChildAt(0))
				child->RemoveSelf();

			// 3. remove LayoutItems
			// TODO

			Handler::rbe__gc_free(ptr);
		}
	}
}
