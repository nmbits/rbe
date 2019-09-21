
#include "rbe.hpp"
#include "convert.hpp"
#include "call_script.hpp"
#include "protect.hpp"
#include "gvl.hpp"
#include "lock.hpp"
#include "debug.hpp"
#include "gc.hpp"

#include "GridLayout.hpp"

#include "Message.hpp"
#include "LayoutItem.hpp"
#include "View.hpp"
#include "Archivable.hpp"
#include "TwoDimensionalLayout.hpp"

#include "util_layout.hpp"

#include <utility>

namespace rbe
{
	namespace B
	{
		VALUE
		GridLayout::rbe_add_view(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BGridLayout::rbe_add_view", argc, argv, self);
			int type_error_index = 0;

			if (2 == argc || 1 == argc)
				return Layout::rbe_add_view(argc, argv, self);

			if (3 <= argc && argc <= 5) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_2;
				}
				if (0 < argc && !Convert<BView * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_2;
				}
				if (1 < argc && argv[1] == Qnil) {
					type_error_index = 1;
					goto break_2;
				}
				if (1 < argc && !Convert<int32 >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_2;
				}
				if (2 < argc && argv[2] == Qnil) {
					type_error_index = 2;
					goto break_2;
				}
				if (2 < argc && !Convert<int32 >::IsConvertable(argv[2])) {
					type_error_index = 2;
					goto break_2;
				}
				if (3 < argc && argv[3] == Qnil) {
					type_error_index = 3;
					goto break_2;
				}
				if (3 < argc && !Convert<int32 >::IsConvertable(argv[3])) {
					type_error_index = 3;
					goto break_2;
				}
				if (4 < argc && argv[4] == Qnil) {
					type_error_index = 4;
					goto break_2;
				}
				if (4 < argc && !Convert<int32 >::IsConvertable(argv[4])) {
					type_error_index = 4;
					goto break_2;
				}
				VALUE item_ = Util::AllocateLayoutItemForView(argv[0]);
				argv[0] = item_;
				VALUE vret = rbe_add_item(argc, argv, self);
				if (vret == Qtrue)
					return item_;
				return Qnil;
			}
		break_2:

			if (argc < 1 || argc > 5)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for (1..5))", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}


		VALUE
		GridLayout::rbe_add_item(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BGridLayout::rbe_add_item", argc, argv, self);
			VALUE vret = Qnil;
			BGridLayout *_this = Convert<BGridLayout *>::FromValue(self);
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
					ret = _this->BGridLayout::AddItem(item);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				gc::Up(self, argv[1]);
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
					ret = _this->BGridLayout::AddItem(index, item);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				gc::Up(self, argv[1]);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<bool>::ToValue(ret);
				return vret;
			}
		break_1:

			if (3 <= argc && argc <= 5) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_2;
				}
				if (0 < argc && !Convert<BLayoutItem * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_2;
				}
				if (1 < argc && argv[1] == Qnil) {
					type_error_index = 1;
					goto break_2;
				}
				if (1 < argc && !Convert<int32 >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_2;
				}
				if (2 < argc && argv[2] == Qnil) {
					type_error_index = 2;
					goto break_2;
				}
				if (2 < argc && !Convert<int32 >::IsConvertable(argv[2])) {
					type_error_index = 2;
					goto break_2;
				}
				if (3 < argc && argv[3] == Qnil) {
					type_error_index = 3;
					goto break_2;
				}
				if (3 < argc && !Convert<int32 >::IsConvertable(argv[3])) {
					type_error_index = 3;
					goto break_2;
				}
				if (4 < argc && argv[4] == Qnil) {
					type_error_index = 4;
					goto break_2;
				}
				if (4 < argc && !Convert<int32 >::IsConvertable(argv[4])) {
					type_error_index = 4;
					goto break_2;
				}
				BLayoutItem * item = Convert<BLayoutItem * >::FromValue(argv[0]);
				int32 column = Convert<int32 >::FromValue(argv[1]);
				int32 row = Convert<int32 >::FromValue(argv[2]);
				int32 columnCount = (3 < argc ? Convert<int32 >::FromValue(argv[3]) : (int32)1);
				int32 rowCount = (4 < argc ? Convert<int32 >::FromValue(argv[4]) : (int32)1);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BGridLayout::AddItem(item, column, row, columnCount, rowCount);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				gc::Up(self, argv[0]);
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<bool>::ToValue(ret);
				return vret;
			}
		break_2:

			if (argc < 1 || argc > 5)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for (1..5))", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}
	}
}
