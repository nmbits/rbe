#include "rbe.hpp"
#include "convert.hpp"
#include "debug.hpp"

#include "Layout.hpp"
#include "type_map.hpp"

namespace rbe {
	namespace B {
		void
		Layout::rbe__gc_free(void *ptr)
		{
			PointerOf<BLayout>::Class *p = static_cast<PointerOf<BLayout>::Class *>(ptr);
			BLayout *layout = static_cast<BLayout *>(p);
			while (layout->CountItems() > 0)
				layout->RemoveItem(0);
			LayoutItem::rbe__gc_free(ptr);
		}
	}
}
