#include "rbe.hpp"
#include "convert.hpp"
#include "debug.hpp"

#include "LayoutItem.hpp"
#include "Layout.hpp"
#include "type_map.hpp"

namespace rbe {
	namespace B {
		void
		LayoutItem::rbe__gc_free(void *ptr)
		{
			PointerOf<BLayoutItem>::Class *p =
				static_cast<PointerOf<BLayoutItem>::Class *>(ptr);
			BLayoutItem *li = static_cast<BLayoutItem *>(p);
			if (li->fLayout)
				li->fLayout->RemoveItem(li);
			Archivable::rbe__gc_free(ptr);
		}
	}
}
