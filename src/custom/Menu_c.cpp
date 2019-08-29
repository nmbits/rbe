
#include "Menu.hpp"
#include "MenuBar.hpp"
#include "PopUpMenu.hpp"
#include "MenuItem.hpp"
#include "SeparatorItem.hpp"

#include "debug.hpp"
#include "gvl.hpp"

#include "convert.hpp"

namespace rbe
{
	namespace B
	{
		void Menu::rbe__gc_free(void *ptr)
		{
		    RBE_TRACE("BMenu::rb__gc_free");
		    RBE_PRINT(("  ptr = %p\n", ptr));
			PointerOf<BMenu>::Class *obj =
				static_cast<PointerOf<BMenu>::Class *>(ptr);
			BMenu *menu = static_cast<BMenu *>(obj);
			menu->RemoveItems(0, menu->CountItems(), false);
			View::rbe__gc_free(ptr);
		}

		VALUE
		Menu::rbe_add_item(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BMenu::rbe_add_item", argc, argv, self);
			VALUE vret = Qnil;
			BMenu *_this = Convert<BMenu *>::FromValue(self);
			int type_error_index = 0;
			if (1 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_0;
				}
				if (0 < argc && !Convert<BMenuItem * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_0;
				}
				BMenuItem * item = Convert<BMenuItem * >::FromValue(argv[0]);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BMenu::AddItem(item);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				if (item && ret)
					gc::Up(self, argv[0]);
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<bool>::ToValue(ret);
				return vret;
			}
		break_0:

			if (2 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_1;
				}
				if (0 < argc && !Convert<BMenuItem * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_1;
				}
				if (1 < argc && argv[1] == Qnil) {
					type_error_index = 1;
					goto break_1;
				}
				if (1 < argc && !Convert<int32 >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_1;
				}
				BMenuItem * item = Convert<BMenuItem * >::FromValue(argv[0]);
				int32 index = Convert<int32 >::FromValue(argv[1]);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BMenu::AddItem(item, index);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				if (item && ret)
					gc::Up(self, argv[0]);

				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<bool>::ToValue(ret);
				return vret;
			}
		break_1:

			if (2 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_2;
				}
				if (0 < argc && !Convert<BMenuItem * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_2;
				}
				if (1 < argc && argv[1] == Qnil) {
					type_error_index = 1;
					goto break_2;
				}
				if (1 < argc && !Convert<BRect >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_2;
				}
				BMenuItem * item = Convert<BMenuItem * >::FromValue(argv[0]);
				BRect frame = Convert<BRect >::FromValue(argv[1]);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BMenu::AddItem(item, frame);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				if (item && ret)
					gc::Up(self, argv[0]);

				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<bool>::ToValue(ret);
				return vret;
			}
		break_2:

			if (1 <= argc && argc <= 2) {
				VALUE args[2];
				if (argv[0] == Qnil) {
					type_error_index = 0;
					goto break_3;
				}
				if (!Convert<BMenu * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_3;
				}
				args[0] = rb_class_new_instance(1, argv, MenuItem::Class());
				if (argc == 2) {
					if (argv[1] == Qnil) {
						type_error_index = 1;
						goto break_3;
					}
					if (Convert<int32>::IsConvertable(argv[1]) ||
						Convert<BRect>::IsConvertable(argv[1])) {
						args[1] = argv[1];
					} else {
						type_error_index = 1;
						goto break_3;
					}
				}
				return rbe_add_item(argc, args, self);
			}
		break_3:

			if (argc < 1 || argc > 2)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for (1..2))", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}


		VALUE
		Menu::rbe_remove_item(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BMenu::rbe_remove_item", argc, argv, self);
			VALUE vret = Qnil;
			BMenu *_this = Convert<BMenu *>::FromValue(self);
			int type_error_index = 0;
			if (1 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_0;
				}
				if (0 < argc && !Convert<BMenuItem * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_0;
				}
				BMenuItem * item = Convert<BMenuItem * >::FromValue(argv[0]);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BMenu::RemoveItem(item);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				if (ret)
					gc::Down(self, argv[0]);

				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<bool>::ToValue(ret);
				return vret;
			}
		break_0:

			if (1 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_2;
				}
				if (0 < argc && !Convert<BMenu * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_2;
				}
				BMenu *submenu = Convert<BMenu *>::FromValue(argv[0]);
				for (int32 i = 0; i < _this->CountItems(); i++) {
					BMenuItem *item = _this->ItemAt(i);
					if (item->Submenu() == submenu) {
						bool ret = _this->RemoveItem(i);
						if (ret) {
							VALUE item_ = Convert<BMenuItem *>::ToValue(item);
							gc::Down(self, item_);
							return Qtrue;
						}
					}
				}
				return Qfalse;
			}

		break_2:
			if (1 == argc) {
				if (argv[0] == Qnil) {
					type_error_index = 0;
					goto break_3;
				}
				if (!Convert<int32>::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_3;
				}
				int32 index = Convert<int32>::FromValue(argv[0]);
				if (index < 0 || index >= _this->CountItems())
					rb_raise(rb_eRangeError, "index exceeds the number of items");
				BMenuItem *menu_item = _this->ItemAt(index);
				if (menu_item == NULL)
					rb_raise(rb_eSystemCallError, "item not found. index = %d", index);
				PointerOf<BMenuItem>::Class *ptr = static_cast<PointerOf<BMenuItem>::Class *>(menu_item);
				VALUE vmenu_item = gc::GetValue(ptr);
				if (NIL_P(vmenu_item))
					debugger("MenuItem may not be a ruby object");
				bool ret = _this->RemoveItem(index);
				if (ret) {
					gc::Down(self, vmenu_item);
					return Qtrue;
				}
				return Qfalse;
			}

		break_3:

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}

		VALUE
		Menu::rbe_add_separator_item(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BMenu::rbe_add_separator_item", argc, argv, self);
			if (0 == argc) {
				VALUE vseparator_item = rb_class_new_instance(0, NULL, SeparatorItem::Class());
				return rbe_add_item(1, &vseparator_item, self);
			}

			rb_raise(rb_eArgError, "wrong number of arguments (%d for 0)", argc);
			return Qnil;
		}
	}
}
