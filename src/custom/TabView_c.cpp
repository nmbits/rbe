
#include "Tab.hpp"
#include "View.hpp"
#include "deleting.hpp"
#include "debug.hpp"
#include "convert.hpp"
#include "type_map.hpp"
#include "gvl.hpp"
#include "ownership.hpp"

#include <functional>

namespace rbe
{
	namespace gc
	{
		template<>
		void Deleting<BTab, BView>(BTab *o, BView *t)
		{
			if (o->fView == t) {
				t->RemoveSelf();
				o->fView = NULL;
			}
		}
	}

	namespace B
	{
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
				if (!NIL_P(*argv)) {
					if (Convert<BView *>::IsConvertable(*argv)) {
						view = Convert<BView *>::FromValue(*argv);
					} else {
						rb_raise(rb_eTypeError, "wrong type of argument");
						return Qnil;
					}
				}
				break;
			
			default:
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 0..1)", argc);
				return Qnil;
			}
			if (_this->fView != NULL) {
				BView *old_view = _this->fView;
				PointerOf<BView>::Class *ptr_old_view = static_cast<PointerOf<BView>::Class *>(old_view);
				PointerOf<BTab>::Class *ptr_this = static_cast<PointerOf<BTab>::Class *>(_this);
				ObjectRegistory::Instance()->Release(ptr_this, ptr_old_view);
			}
			if (view != NULL) {
				gc::Ownership<BTab, BView> *ownership = new gc::Ownership<BTab, BView>(*argv);
				ObjectRegistory::Instance()->Own(self, ownership);
			}
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
