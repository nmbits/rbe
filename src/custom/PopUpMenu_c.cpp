
#include "PopUpMenu.hpp"
#include "Menu.hpp"
#include "MenuItem.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "debug.hpp"
#include "gvl.hpp"
#include "convert.hpp"

#include <functional>

#define RBE_POPUPMENU_GO_CALLED "__rbe_popupmenu_go_called"

namespace rbe
{
	namespace B
	{
		VALUE
		PopUpMenu::rbe_go(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE("VALUE PopUpMenu::go()");

			VALUE called = rb_iv_get(self, RBE_POPUPMENU_GO_CALLED);
			if (!NIL_P(called))
				rb_raise(rb_eRuntimeError, "B::PopUpMenu#go can not be called twice");
			rb_iv_set(self, RBE_POPUPMENU_GO_CALLED, Qtrue);


			VALUE vWhere, vKeepOpen, vAutoInvoke, vClickToOpen;
			rb_scan_args(argc, argv, "13", &vWhere, &vAutoInvoke, &vKeepOpen, &vClickToOpen);
			int type_error_index = 0;
			BPopUpMenu *_this = Convert<BPopUpMenu *>::FromValue(self);

			do {
				if (NIL_P(vWhere))
					break;
				if (!Convert<BPoint>::IsConvertable(vWhere))
					break;
				type_error_index = 1;
				if (argc == 2 && NIL_P(vAutoInvoke))
					break;
				if (argc == 2 && !Convert<bool>::IsConvertable(vAutoInvoke))
					break;
				type_error_index = 2;
				if (argc == 3 && NIL_P(vKeepOpen))
					break;
				if (argc == 3 && !Convert<bool>::IsConvertable(vKeepOpen))
					break;
				type_error_index = 3;
				if (argc == 4 && !Convert<BRect>::IsConvertable(vClickToOpen))
					break;
				if (argc == 4 && NIL_P(vClickToOpen))
					break;

				BPoint where = Convert<BPoint>::FromValue(vWhere);
				bool autoInvoke = (argc == 2 ? Convert<bool>::FromValue(vAutoInvoke) : false);
				bool keepOpen = (argc == 3 ? Convert<bool>::FromValue(vKeepOpen) : false);
				BRect clickToOpen = (argc == 4 ? Convert<BRect>::FromValue(vClickToOpen) : BRect(0, 0, -1, -1));

				BMenuItem *result = NULL;
				std::function<void ()> f = [&]() {
					if (argc == 4)
						result = _this->BPopUpMenu::Go(where, false, keepOpen, clickToOpen, false);
					else
						result = _this->BPopUpMenu::Go(where, false, keepOpen, false);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();

				VALUE vresult = Convert<BMenuItem *>::ToValue(result);
				if (result && autoInvoke) {
					ID send = rb_intern("__send__");
					VALUE syminvoke = ID2SYM(rb_intern("invoke"));
					rb_funcall(vresult, send, 1, syminvoke);
				}
				return vresult;
			} while(false);
			rb_raise(rb_eTypeError, "wrong type of argument %d", type_error_index);
			return Qnil;
		}
	}
}
