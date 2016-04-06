
#include "PopUpMenu.hpp"
#include "Menu.hpp"
#include "MenuItem.hpp"
#include "Point.hpp"
#include "debug.hpp"
#include "gvl.hpp"
#include "convert.hpp"

#include <functional>

namespace rbe
{
	namespace B
	{
		void PopUpMenu::rbe__gc_free(void *ptr)
		{
		    RBE_TRACE("BPopUpMenu::rb__gc_free");
		    RBE_PRINT(("  ptr = %p\n", ptr));
		    RBE_PRINT(("  calling BMenu::rbe__gc_free\n"));
		    Menu::rbe__gc_free(ptr);
		}

		VALUE
		PopUpMenu::rbe_go(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE("VALUE PopUpMenu::go()");
			VALUE vWhere, vKeepOpen;
			rb_scan_args(argc, argv, "11", &vWhere, &vKeepOpen);
			int type_error_index = 0;
			BPopUpMenu *_this = Convert<BPopUpMenu *>::FromValue(self);

			do {
				if (NIL_P(vWhere))
					break;
				if (!Convert<BPoint>::IsConvertable(vWhere))
					break;
				type_error_index = 1;
				if (argc == 2 && NIL_P(vKeepOpen))
					break;
				if (argc == 2 && !Convert<bool>::IsConvertable(vKeepOpen))
					break;
				BPoint where = Convert<BPoint>::FromValue(vWhere);
				bool keepOpen = (argc == 2 ? Convert<bool>::FromValue(vKeepOpen) : false);
				BMenuItem *result = NULL;
				std::function<void ()> f = [&]() {
					result = _this->BPopUpMenu::Go(where, false, keepOpen, false);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				return Convert<BMenuItem *>::ToValue(result);
			} while(false);
			rb_raise(rb_eTypeError, "wrong type of argument %d", type_error_index);
			return Qnil;
		}
	}
}
