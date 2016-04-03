
#include "MenuItem.hpp"
#include "Menu.hpp"
#include "Message.hpp"
#include "Window.hpp"
#include "registory.hpp"
#include "convert.hpp"
#include "deleting.hpp"
#include "ownership.hpp"

namespace rbe
{
	namespace gc
	{
		template<>
		void Deleting<BMenuItem, BMenu>(BMenuItem *o, BMenu *t)
		{
			if (o->fSubmenu == t)
				o->fSubmenu = NULL;
		}
	}

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
			if (_this->fSuper != NULL) {
				VALUE vmenu = Convert<BMenu *>::ToValue(_this->fSuper);
				rb_gc_mark(vmenu);
			}
			ObjectRegistory::Instance()->Mark(ptr);
		}

		void
		MenuItem::rbe__gc_free(void *ptr)
		{
			BMenuItem *_this = static_cast<BMenuItem *>(ptr);
			ObjectRegistory::Instance()->Delete(ptr);
			_this->fMessage = NULL;
			_this->fSubmenu = NULL;
			delete _this;
		}

		VALUE MenuItem::rbe__initialize(int argc, VALUE *argv, VALUE self)
		{
			VALUE vargs[4];
			BMenuItem *_this = NULL;

			rb_scan_args(argc, argv, "13", &vargs[0], &vargs[1], &vargs[2], &vargs[3]);

			if (2 <= argc && argc <= 4) {
				if (!Convert<const char *>::IsConvertable(vargs[0])) goto break_0;
				if (!Convert<BMessage *>::IsConvertable(vargs[1])) goto break_0;
				if (2 < argc && !Convert<char>::IsConvertable(vargs[2])) goto break_0;
				if (3 < argc && !Convert<uint32>::IsConvertable(vargs[3])) goto break_0;
				const char * label = Convert<const char *>::FromValue(vargs[0]);
				BMessage * message = Convert<BMessage *>::FromValue(vargs[1]);
				char shortcut = (2 < argc ? Convert<char>::FromValue(vargs[2]) : 0);
				uint32 modifiers = (3 < argc ? Convert<uint32>::FromValue(vargs[3]) : 0);
				_this = new BMenuItem(label, message, shortcut, modifiers);
				PointerOf<BMenuItem>::Class *ptr = static_cast<PointerOf<BMenuItem>::Class *>(_this);
				DATA_PTR(self) = static_cast<void *>(ptr);
				ObjectRegistory::Instance()->Add(self);
				if (!NIL_P(argv[1])) {
					gc::Ownership0 *ownership = new gc::Ownership<BInvoker, BMessage>(argv[1]);
					ObjectRegistory::Instance()->Own(self, ownership);
				}
				goto fin;
			}
		break_0:

			if (1 <= argc && argc <= 2) {
				if (!Convert<BMenu *>::IsConvertable(vargs[0])) goto break_1;
				if (1 < argc && !Convert<BMessage *>::IsConvertable(vargs[1])) goto break_1;
				BMenu * menu = Convert<BMenu *>::FromValue(vargs[0]);
				BMessage * message = (1 < argc ? Convert<BMessage *>::FromValue(vargs[1]) : NULL);
				_this = new BMenuItem(menu, message);
				PointerOf<BMenuItem>::Class *ptr = static_cast<PointerOf<BMenuItem>::Class *>(_this);
				DATA_PTR(self) = static_cast<void *>(ptr);
				ObjectRegistory::Instance()->Add(self);
				if (!NIL_P(argv[0])) {
					gc::Ownership0 *ownership = new gc::Ownership<BMenuItem, BMenu>(argv[0]);
					ObjectRegistory::Instance()->Own(self, ownership);
				}
				if (!NIL_P(argv[1])) {
					gc::Ownership0 *ownership = new gc::Ownership<BInvoker, BMessage>(argv[1]);
					ObjectRegistory::Instance()->Own(self, ownership);
				}
				goto fin;
			}
		break_1:

		fin:
			if (_this == NULL)
				rb_raise(rb_eTypeError, "wrong type of arguments");
			return self;
		}
	}
}
