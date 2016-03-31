
#include "MenuItem.hpp"
#include "Window.hpp"
#include "registory.hpp"
#include "convert.hpp"

namespace rbe
{
	namespace B
	{
		void
		MenuItem::rbe__gc_mark(void *ptr)
		{
			BMenuItem *_this = static_cast<BMenuItem *>(ptr);
			if (_this->fWindow != NULL) {
				VALUE vwindow = Convert<BWindow *>::ToValue(_this->fWindow);
				rb_gc_mark(vwindow);
			}
			ObjectRegistory::Instance()->Mark(ptr);
		}

		void
		MenuItem::rbe__gc_free(void *ptr)
		{
			BMenuItem *_this = static_cast<BMenuItem *>(ptr);
			ObjectRegistory::Instance()->Delete(ptr);
			_this->fMessage = NULL;
			delete _this;
		}
	}
}
