#include "Tab.hpp"
#include "TabView.hpp"
#include "View.hpp"

#include <gvl.hpp>
#include "debug.hpp"
#include "convert.hpp"
#include "type_map.hpp"

#include <functional>

namespace rbe
{
	namespace B
	{
		void
		Tab::rbe__gc_free(void *ptr)
		{
			PointerOf<BTab>::Class *tmp = static_cast<PointerOf<BTab>::Class *>(ptr);
			BTab *this_ = static_cast<BTab *>(tmp);
			if (this_->fTabView) {
				BTabView *tabView = this_->fTabView;
				int i;
				for (i = 0; i < tabView->CountTabs(); i++) {
					if (this_ == tabView->TabAt(i)) {
						tabView->RemoveTab(i);
						break;
					}
				}
			}
			if (this_->fView) {
				VALUE view_ = Convert<BView *>::ToValue(this_->fView);
				if (!NIL_P(view_)) { /* the view is still living */
					/* from ~BTab */
					if (this_->fSelected)
						this_->fView->RemoveSelf();
					this_->fView = NULL;
				}
			}
			Archivable::rbe__gc_free(ptr);
		}

		VALUE
		Tab::rbe_set_view(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Tab::rbe_set_view", argc, argv, self);
			BView *view = NULL;
			BTab *_this = Convert<BTab *>::FromValue(self);

			switch (argc) {
			case 0:
				break;

			case 1:
				if (NIL_P(*argv))
					break;
				if (!Convert<BView *>::IsConvertable(*argv))
					rb_raise(rb_eTypeError, "wrong type of argument");
				view = Convert<BView *>::FromValue(*argv);
				break;
			
			default:
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 0..1)", argc);
				return Qnil;
			}

			if (_this->fView != NULL)
				gc::Down(self, Convert<BView *>::ToValue(_this->fView));

			if (!NIL_P(*argv))
				gc::Up(self, *argv);

			std::function<void ()> f = [&]() {
				if (_this->fView) {
					_this->fView->RemoveSelf();
					_this->fView = NULL;
				}
				_this->SetView(view);
			};
			CallWithoutGVL<std::function<void ()>, void> g(f);
			g();
			rb_thread_check_ints();
			return Qnil;
		}
	}
}
