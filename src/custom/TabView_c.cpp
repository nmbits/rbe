#include "Tab.hpp"
#include "TabView.hpp"
#include "View.hpp"

#include "debug.hpp"
#include "convert.hpp"
#include "type_map.hpp"
#include "gvl.hpp"

#include <functional>

#define private public
#define protected public
#include <interface/Layout.h>
#undef protected
#undef private

namespace rbe
{
	namespace B
	{
		void
		TabView::rbe__gc_free(void *ptr)
		{
			PointerOf<BTabView>::Class *tmp = static_cast<PointerOf<BTabView>::Class *>(ptr);
			BTabView *this_ = static_cast<BTabView *>(tmp);
			while (this_->CountTabs() > 0) {
				this_->RemoveTab(0);
			}
			View::rbe__gc_free(ptr);
		}

		VALUE
		TabView::rbe_add_tab(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("TabView#add_tab", argc, argv, self);

			VALUE vview, vtab;
			rb_scan_args(argc, argv, "11", &vview, &vtab);
			BTabView *_this = Convert<BTabView *>::FromValue(self);
			RBE_PRINT(("  _this: %p\n", _this));
			RBE_PRINT(("  vtab: %lu\n", vtab));
			if (NIL_P(vtab)) {
				vtab = rb_class_new_instance(0, NULL, Tab::Class());
			} else {
				if (Convert<BTab *>::IsConvertable(vtab)) {
					rb_raise(rb_eTypeError, "wrong type of argument 2");
					return Qnil;
				}
			}
			BTab *tab = Convert<BTab *>::FromValue(vtab);
			RBE_PRINT(("  tab: %p\n", tab));
			if (!Convert<BView *>::IsConvertable(vview) || NIL_P(vview)) {
				rb_raise(rb_eTypeError, "wrong type of argument 1");
				return Qnil;
			}
			BView *view = Convert<BView *>::FromValue(vview);
			RBE_PRINT(("  view: %p\n", view));
			Tab::rbe_set_view(1, &vview, vtab);
			std::function<void ()> f =
				[&]() {
					// from Haiku's TabView.cpp
					RBE_PRINT(("  _this: %p\n", _this));
					RBE_PRINT(("  _this->fContainerView: %p\n", _this->fContainerView));
					RBE_PRINT(("  _this->fContainerView->GetLayout(): %p\n", _this->fContainerView->GetLayout()));
					if (_this->fContainerView->GetLayout())
						_this->fContainerView->GetLayout()->AddView(_this->CountTabs(), view);
					_this->fTabList->AddItem(tab);
					// BTab::Private(tab).SetTabView(_this);
					tab->fTabView = _this;
					if (_this->CountTabs() == 1 && _this->Window() != NULL)
						_this->Select(0);
				};
			CallWithoutGVL<std::function<void ()>, void> g(f);
			g();
			gc::Up(self, vtab);
			rb_thread_check_ints();
			return Qnil;
		}

		VALUE
		TabView::rbe_remove_tab(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("B::TabView#remove_tab", argc, argv, self);
			VALUE vindex;
			rb_scan_args(argc, argv, "10", &vindex);
			BTabView *_this = Convert<BTabView *>::FromValue(self);

			int32 index = 0;
			if (!Convert<int32>::IsConvertable(vindex)) {
				rb_raise(rb_eTypeError, "wrong type of argument 1");
				return Qnil;
			}
			index = Convert<int32>::FromValue(vindex);
			BTab *tab = NULL;
			std::function<void ()> f = [&]() {
				tab = _this->RemoveTab(index);
			};
			CallWithoutGVL<std::function<void ()>, void> g(f);
			g();
			if (tab) {
				VALUE tab_ = Convert<BTab *>::ToValue(tab);
				if (!NIL_P(tab_))
					gc::Down(self, tab_);
			}
			return Convert<BTab *>::ToValue(tab);
		}
	}
}
