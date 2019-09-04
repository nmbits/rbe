
#include "rbe.hpp"
#include "convert.hpp"
#include "call_script.hpp"
#include "protect.hpp"
#include "gvl.hpp"
#include "lock.hpp"
#include "debug.hpp"
#include "gc.hpp"

#include "GroupLayout.hpp"

#include "Message.hpp"
#include "View.hpp"
#include "LayoutItem.hpp"
#include "Archivable.hpp"
#include "TwoDimensionalLayout.hpp"

namespace rbe
{
	namespace B
	{
		VALUE
		GroupLayout::rbe_add_view(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BGroupLayout::rbe_add_view", argc, argv, self);
			VALUE vret = Qnil;
			BGroupLayout *_this = Convert<BGroupLayout *>::FromValue(self);
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

				BLayoutItem * ret;
				std::function<void ()> f = [&]() {
					ret = _this->BGroupLayout::AddView(child);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<BLayoutItem *>::ToValue(ret);
				if (!NIL_P(vret))
					gc::Up(self, vret);
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
				if (1 < argc && !Convert<BView * >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_1;
				}
				int32 index = Convert<int32 >::FromValue(argv[0]);
				BView * child = Convert<BView * >::FromValue(argv[1]);

				BLayoutItem * ret;
				std::function<void ()> f = [&]() {
					ret = _this->BGroupLayout::AddView(index, child);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<BLayoutItem *>::ToValue(ret);
				if (!NIL_P(vret))
					gc::Up(self, vret);
				return vret;
			}
		break_1:

			if (2 == argc) {
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
				if (1 < argc && !Convert<float >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_2;
				}
				BView * child = Convert<BView * >::FromValue(argv[0]);
				float weight = Convert<float >::FromValue(argv[1]);

				BLayoutItem * ret;
				std::function<void ()> f = [&]() {
					ret = _this->BGroupLayout::AddView(child, weight);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<BLayoutItem *>::ToValue(ret);
				if (!NIL_P(vret))
					gc::Up(self, vret);
				return vret;
			}
		break_2:

			if (3 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_3;
				}
				if (0 < argc && !Convert<int32 >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_3;
				}
				if (1 < argc && argv[1] == Qnil) {
					type_error_index = 1;
					goto break_3;
				}
				if (1 < argc && !Convert<BView * >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_3;
				}
				if (2 < argc && argv[2] == Qnil) {
					type_error_index = 2;
					goto break_3;
				}
				if (2 < argc && !Convert<float >::IsConvertable(argv[2])) {
					type_error_index = 2;
					goto break_3;
				}
				int32 index = Convert<int32 >::FromValue(argv[0]);
				BView * child = Convert<BView * >::FromValue(argv[1]);
				float weight = Convert<float >::FromValue(argv[2]);

				BLayoutItem * ret;
				std::function<void ()> f = [&]() {
					ret = _this->BGroupLayout::AddView(index, child, weight);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				vret = Convert<BLayoutItem *>::ToValue(ret);
				if (!NIL_P(vret))
					gc::Up(self, vret);
				return vret;
			}
		break_3:

			if (argc < 1 || argc > 3)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for (1..3))", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}


		VALUE
		GroupLayout::rbe_add_item(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BGroupLayout::rbe_add_item", argc, argv, self);
			VALUE vret = Qnil;
			BGroupLayout *_this = Convert<BGroupLayout *>::FromValue(self);
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
					ret = _this->BGroupLayout::AddItem(item);
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
					ret = _this->BGroupLayout::AddItem(index, item);
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

			if (2 == argc) {
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
				if (1 < argc && !Convert<float >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_2;
				}
				BLayoutItem * item = Convert<BLayoutItem * >::FromValue(argv[0]);
				float weight = Convert<float >::FromValue(argv[1]);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BGroupLayout::AddItem(item, weight);
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
		break_2:

			if (3 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_3;
				}
				if (0 < argc && !Convert<int32 >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_3;
				}
				if (1 < argc && argv[1] == Qnil) {
					type_error_index = 1;
					goto break_3;
				}
				if (1 < argc && !Convert<BLayoutItem * >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_3;
				}
				if (2 < argc && argv[2] == Qnil) {
					type_error_index = 2;
					goto break_3;
				}
				if (2 < argc && !Convert<float >::IsConvertable(argv[2])) {
					type_error_index = 2;
					goto break_3;
				}
				int32 index = Convert<int32 >::FromValue(argv[0]);
				BLayoutItem * item = Convert<BLayoutItem * >::FromValue(argv[1]);
				float weight = Convert<float >::FromValue(argv[2]);

				bool ret;
				std::function<void ()> f = [&]() {
					ret = _this->BGroupLayout::AddItem(index, item, weight);
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
		break_3:

			if (argc < 1 || argc > 3)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for (1..3))", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}
	}
}
