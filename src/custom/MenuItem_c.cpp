
#include "Archivable.hpp"
#include "MenuItem.hpp"
#include "Menu.hpp"
#include "Message.hpp"
#include "Window.hpp"
#include "convert.hpp"

namespace rbe
{
	namespace B
	{
		void
		MenuItem::rbe__gc_mark(void *ptr)
		{
			PointerOf<BMenuItem>::Class *p =
				static_cast<PointerOf<BMenuItem>::Class *>(ptr);
			BMenuItem *_this = static_cast<BMenuItem *>(p);
			if (_this->fWindow != NULL) {
				VALUE vwindow = Convert<BWindow *>::ToValue(_this->fWindow);
				rb_gc_mark(vwindow);
			}
			if (_this->fSuper != NULL) {
				VALUE vmenu = Convert<BMenu *>::ToValue(_this->fSuper);
				rb_gc_mark(vmenu);
			}
			Archivable::rbe__gc_mark(ptr);
		}

		void
		MenuItem::rbe__gc_free(void *ptr)
		{
			PointerOf<BMenuItem>::Class *p =
				static_cast<PointerOf<BMenuItem>::Class *>(ptr);
			BMenuItem *_this = static_cast<BMenuItem *>(p);
			_this->fMessage = NULL;
			_this->fSubmenu = NULL;
			Archivable::rbe__gc_free(ptr);
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
				gc::Memorize(self);
				if (!NIL_P(argv[1]))
					gc::Up(self, argv[1]);
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
				gc::Memorize(self);
				if (!NIL_P(argv[0]))
					gc::Up(self, argv[0]);

				if (!NIL_P(argv[1]))
					gc::Up(self, argv[1]);

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
