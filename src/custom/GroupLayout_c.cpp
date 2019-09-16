
#include "rbe.hpp"
#include "convert.hpp"
#include "call_script.hpp"
#include "protect.hpp"
#include "gvl.hpp"
#include "lock.hpp"
#include "debug.hpp"
#include "gc.hpp"

#include "GroupLayout.hpp"
#include "Layout.hpp"

#include "Message.hpp"
#include "View.hpp"
#include "LayoutItem.hpp"
#include "Archivable.hpp"
#include "TwoDimensionalLayout.hpp"

// haiku/src/kits/interface/GroupLayout.cpp

struct BGroupLayout::ItemLayoutData {
	float	weight;

	ItemLayoutData()
		: weight(1)
	{
	}
};

namespace rbe
{
	namespace B
	{
		VALUE
		GroupLayout::rbe_add_view(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BGroupLayout::rbe_add_view", argc, argv, self);
			VALUE vret = Qnil;
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
				return Layout::rbe_add_view(argc, argv, self);
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
				return Layout::rbe_add_view(argc, argv, self);
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
				float weight = Convert<float >::FromValue(argv[1]);
				vret = Layout::rbe_add_view(1, argv, self);
				if (!NIL_P(vret)) {
					BLayoutItem *item = Convert<BLayoutItem *>::FromValue(vret);
					if (item && item->LayoutData()) {
						ItemLayoutData *data = (ItemLayoutData *)item->LayoutData();
						data->weight = weight;
					}
				}
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
				float weight = Convert<float >::FromValue(argv[2]);
				vret = Layout::rbe_add_view(2, argv, self);
				if (!NIL_P(vret)) {
					BLayoutItem *item = Convert<BLayoutItem *>::FromValue(vret);
					if (item && item->LayoutData()) {
						ItemLayoutData *data = (ItemLayoutData *)item->LayoutData();
						data->weight = weight;
					}
				}
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
				return Layout::rbe_add_item(argc, argv, self);
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
				return Layout::rbe_add_item(argc, argv, self);
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
				float weight = Convert<float >::FromValue(argv[2]);
				vret = Layout::rbe_add_item(1, argv, self);
				if (!NIL_P(vret)) {
					BLayoutItem *item = Convert<BLayoutItem *>::FromValue(vret);
					if (item && item->LayoutData()) {
						ItemLayoutData *data = (ItemLayoutData *)item->LayoutData();
						data->weight = weight;
					}
				}
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
				float weight = Convert<float >::FromValue(argv[2]);
				vret = Layout::rbe_add_item(2, argv, self);
				if (!NIL_P(vret)) {
					BLayoutItem *item = Convert<BLayoutItem *>::FromValue(vret);
					if (item && item->LayoutData()) {
						ItemLayoutData *data = (ItemLayoutData *)item->LayoutData();
						data->weight = weight;
					}
				}
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
