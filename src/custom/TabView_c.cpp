
#include "Tab.hpp"
#include "TabView.hpp"
#include "View.hpp"
#include "deleting.hpp"
#include "debug.hpp"
#include "convert.hpp"
#include "type_map.hpp"
#include "gvl.hpp"
#include "ownership.hpp"

#include <functional>

#define private public
#define protected public
#include <interface/Layout.h>
#undef protected
#undef private

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

		template<>
		void Deleting<BTabView, BTab>(BTabView *o, BTab *t)
		{
			for (int i = 0; i < o->CountTabs(); i++)
			{
				BTab *tab = o->TabAt(i);
				if (tab == t) {
					o->RemoveTab(i);
					break;
				}
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

		VALUE
		TabView::rbe_add_tab(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("TabView#add_tab", argc, argv, self);

			VALUE vview, vtab;
			rb_scan_args(argc, argv, "11", &vview, &vtab);
			BTabView *_this = Convert<BTabView *>::FromValue(self);
			if (NIL_P(vtab)) {
				vtab = rb_class_new_instance(0, NULL, Tab::Class());
			} else {
				if (Convert<BTab *>::IsConvertable(vtab)) {
					rb_raise(rb_eTypeError, "wrong type of argument 2");
					return Qnil;
				}
			}
			BTab *tab = Convert<BTab *>::FromValue(vtab);
			if (!Convert<BView *>::IsConvertable(vview) || NIL_P(vview)) {
				rb_raise(rb_eTypeError, "wrong type of argument 1");
				return Qnil;
			}
			BView *view = Convert<BView *>::FromValue(vview);
			Tab::rbe_set_view(1, &vview, vtab);
			std::function<void ()> f = [&]() {
				// from Haiku's TabView.cpp
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
			gc::Ownership<BTabView, BTab> *ownership = new gc::Ownership<BTabView, BTab>(vtab);
			ObjectRegistory::Instance()->Own(self, ownership);
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
				PointerOf<BTabView>::Class *pthis = static_cast<PointerOf<BTabView>::Class *>(_this);
				PointerOf<BTab>::Class *ptab = static_cast<PointerOf<BTab>::Class *>(tab);
				ObjectRegistory::Instance()->Release((void *)pthis, (void *)ptab);
			}
			return Convert<BTab *>::ToValue(tab);
		}
	}
}
