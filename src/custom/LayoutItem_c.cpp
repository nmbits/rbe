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
		    RBE_PRINT(("BLayoutItem::rb__gc_free: %p\n", ptr));
		    PRINT(("ptr = %p\n", ptr));
			PointerOf<BLayoutItem>::Class *p =
				static_cast<PointerOf<BLayoutItem>::Class *>(ptr);
			BLayoutItem *li = static_cast<BLayoutItem *>(p);
			if (li->fLayout)
				li->fLayout->RemoveItem(li);
			Archivable::rbe__gc_free(ptr);
		}

		VALUE
		LayoutItem::rbe_remove_self(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BLayoutItem::rbe_remove_self", argc, argv, self);
			BLayoutItem *_this = Convert<BLayoutItem *>::FromValue(self);
			if (0 == argc) {
				if (_this->Layout() == NULL)
					return Qnil;
				VALUE layout_ = Convert<BLayout *>::ToValue(_this->Layout());
				return Layout::rbe_remove_item(1, &self, layout_);
			}

			rb_raise(rb_eArgError, "wrong number of arguments (%d for 0)", argc);
			return Qnil;
		}
	}
}
