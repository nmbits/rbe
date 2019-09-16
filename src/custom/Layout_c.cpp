#include "rbe.hpp"
#include "convert.hpp"
#include "debug.hpp"

#include "gvl.hpp"

#include "View.hpp"
#include "Layout.hpp"
#include "type_map.hpp"

#define private public
#define protected public
#include "ViewLayoutItem.h"
#undef protected
#undef private

#include <functional>

namespace rbe {
	namespace B {
		void
		Layout::rbe__gc_free(void *ptr)
		{
			PointerOf<BLayout>::Class *p = static_cast<PointerOf<BLayout>::Class *>(ptr);
			BLayout *layout = static_cast<BLayout *>(p);
			while (layout->CountItems() > 0)
				layout->RemoveItem(0);
			LayoutItem::rbe__gc_free(ptr);
		}

		VALUE
		Layout::rbe_add_view(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BLayout::rbe_add_view", argc, argv, self);
			int type_error_index = 0;
			if (1 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_0;
				}
				if (0 < argc && !Convert<BView * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_0;
				}
				BView * child = Convert<BView * >::FromValue(argv[0]);
				BLayoutItem *item = child->GetLayout();
				VALUE item_ = Qnil;
				if (!item) {
					item = new BViewLayoutItem(child);
					if (item) {
						item_ = LayoutItem::Wrap(item);
						gc::Up(item_, argv[0]);
					} else {
						return Qnil;
					}
				}
				VALUE add_item = rbe_add_item(1, &item_, self);
				if (add_item == Qtrue)
					return item_;
				return Qnil;

				// std::function<void ()> f = [&]() {
				// 	ret = _this->BLayout::AddView(child);
				// };
				// CallWithoutGVL<std::function<void ()>, void> g(f);
				// g();
				// rb_thread_check_ints();
				// if (ThreadException() > 0) {
				// 	rb_jump_tag(ThreadException());
				// }
				// vret = Convert<BLayoutItem *>::ToValue(ret);
				// if (!NIL_P(vret))
				// 	gc::Up(self, vret);
				// return vret;
			}
		break_0:

			if (2 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_1;
				}
				if (0 < argc && !Convert<int32 >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_1;
				}
				if (1 < argc && argv[1] == Qnil) {
					type_error_index = 1;
					goto break_1;
				}
				if (1 < argc && !Convert<BView * >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_1;
				}

				BView * child = Convert<BView * >::FromValue(argv[1]);
				BLayoutItem *item = child->GetLayout();
				VALUE item_ = Qnil;
				if (!item) {
					item = new BViewLayoutItem(child);
					if (item) {
						item_ = LayoutItem::Wrap(item);
						gc::Up(item_, argv[1]);
					} else {
						return Qnil;
					}
				}
				VALUE tmp[2];
				tmp[0] = argv[0];
				tmp[1] = item_;
				VALUE add_item = rbe_add_item(2, tmp, self);
				if (add_item == Qtrue)
					return item_;
				return Qnil;

				// BLayoutItem * ret;
				// std::function<void ()> f = [&]() {
				// 	ret = _this->BLayout::AddView(index, child);
				// };
				// CallWithoutGVL<std::function<void ()>, void> g(f);
				// g();
				// rb_thread_check_ints();
				// if (ThreadException() > 0) {
				// 	rb_jump_tag(ThreadException());
				// }
				// vret = Convert<BLayoutItem *>::ToValue(ret);
				// if (!NIL_P(vret))
				// 	gc::Up(self, vret);
				// return vret;
			}
		break_1:

			if (argc < 1 || argc > 2)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for (1..2))", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}

		VALUE
		Layout::rbe_add_item(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BLayout::rbe_add_item", argc, argv, self);
			VALUE vret = Qnil;
			BLayout *_this = Convert<BLayout *>::FromValue(self);
			int type_error_index = 0;
			if (1 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_0;
				}
				if (0 < argc && !Convert<BLayoutItem * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_0;
				}
				BLayoutItem * item = Convert<BLayoutItem * >::FromValue(argv[0]);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BLayout::AddItem(item);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				if (ret)
					gc::Up(self, argv[0]);
				vret = Convert<bool>::ToValue(ret);
				return vret;
			}
		break_0:

			if (2 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_1;
				}
				if (0 < argc && !Convert<int32 >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_1;
				}
				if (1 < argc && argv[1] == Qnil) {
					type_error_index = 1;
					goto break_1;
				}
				if (1 < argc && !Convert<BLayoutItem * >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_1;
				}
				int32 index = Convert<int32 >::FromValue(argv[0]);
				BLayoutItem * item = Convert<BLayoutItem * >::FromValue(argv[1]);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BLayout::AddItem(index, item);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				if (ret)
					gc::Up(self, argv[1]);
				vret = Convert<bool>::ToValue(ret);
				return vret;
			}
		break_1:

			if (argc < 1 || argc > 2)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for (1..2))", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}

		VALUE
		Layout::rbe_remove_view(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BLayout::rbe_remove_view", argc, argv, self);
			BLayout *_this = Convert<BLayout *>::FromValue(self);
			int type_error_index = 0;
			if (1 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_0;
				}
				if (0 < argc && !Convert<BView * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_0;
				}
				BView * child = Convert<BView * >::FromValue(argv[0]);

				// From src/kits/interface/Layout.cpp
				bool removed = false;
				for (int32 i = _this->CountItems() - 1; i >= 0; i--) {
					BLayoutItem* item = _this->ItemAt(i);
					if (item->View() != child)
						continue;
					VALUE item_ = Convert<BLayoutItem *>::ToValue(item);
					rbe_remove_item(1, &item_, self);
					removed = true;
				}
				return Convert<bool>::ToValue(removed);
			}
		break_0:

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}

		VALUE
		Layout::rbe_remove_item(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BLayout::rbe_remove_item", argc, argv, self);
			VALUE vret = Qnil;
			BLayout *_this = Convert<BLayout *>::FromValue(self);
			int type_error_index = 0;
			if (1 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_0;
				}
				if (0 < argc && !Convert<BLayoutItem * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_0;
				}
				BLayoutItem * item = Convert<BLayoutItem * >::FromValue(argv[0]);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BLayout::RemoveItem(item);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<bool>::ToValue(ret);
				if (ret)
					gc::Down(self, argv[0]);
				return vret;
			}
		break_0:

			if (1 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_1;
				}
				if (0 < argc && !Convert<int32 >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_1;
				}
				int32 index = Convert<int32 >::FromValue(argv[0]);

				BLayoutItem * ret;
				std::function<void ()> f = [&]() {
					ret = _this->BLayout::RemoveItem(index);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<BLayoutItem *>::ToValue(ret);
				if (ret)
					gc::Down(self, argv[0]);
				return vret;
			}
		break_1:

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}
	}
}
