#include "rbe.hpp"
#include "convert.hpp"
#include "registory.hpp"
#include "debug.hpp"

#include "LayoutItem.hpp"
#include "View.hpp"

namespace rbe {
	namespace B {
		void LayoutItem::rbe__gc_mark(void *ptr)
		{
		    RBE_PRINT(("LayoutItem::rb__gc_mark: %p\n", ptr));
			BLayoutItem *_this = static_cast<BLayoutItem *>(ptr);
			BView *view = _this->View();
			if (view) {
				VALUE vview = Convert<BView *>::ToValue(view);
				rb_gc_mark(vview);
			}
		    ObjectRegistory::Instance()->Mark(ptr);
		}
	}
}
