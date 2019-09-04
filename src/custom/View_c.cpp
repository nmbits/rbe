
#include "View.hpp"
#include "Message.hpp"
#include "Rect.hpp"
#include "Layout.hpp"

#include "rbe.hpp"
#include "protect.hpp"
#include "convert.hpp"
#include "gvl.hpp"
#include "lock.hpp"
#include "gc.hpp"

#define protected public
#define private public
#include <interface/ScrollView.h>
#include <interface/Alignment.h>
#include <interface/LayoutContext.h>
#include <support/ObjectList.h>
#undef protected
#undef private

#include <functional>

// copied from haiku/src/kits/interface/View.cpp

struct BView::LayoutData {
	LayoutData()
		:
		fMinSize(),
		fMaxSize(),
		fPreferredSize(),
		fAlignment(),
		fLayoutInvalidationDisabled(0),
		fLayout(NULL),
		fLayoutContext(NULL),
		fLayoutItems(5, false),
		fLayoutValid(true),		// TODO: Rethink these initial values!
		fMinMaxValid(true),		//
		fLayoutInProgress(false),
		fNeedsRelayout(true)
	{
	}

	// status_t
	// AddDataToArchive(BMessage* archive)
	// {
	// 	status_t err = archive->AddSize(kSizesField, fMinSize);

	// 	if (err == B_OK)
	// 		err = archive->AddSize(kSizesField, fMaxSize);

	// 	if (err == B_OK)
	// 		err = archive->AddSize(kSizesField, fPreferredSize);

	// 	if (err == B_OK)
	// 		err = archive->AddAlignment(kAlignmentField, fAlignment);

	// 	return err;
	// }

	// void
	// PopulateFromArchive(BMessage* archive)
	// {
	// 	archive->FindSize(kSizesField, 0, &fMinSize);
	// 	archive->FindSize(kSizesField, 1, &fMaxSize);
	// 	archive->FindSize(kSizesField, 2, &fPreferredSize);
	// 	archive->FindAlignment(kAlignmentField, &fAlignment);
	// }

	BSize			fMinSize;
	BSize			fMaxSize;
	BSize			fPreferredSize;
	BAlignment		fAlignment;
	int				fLayoutInvalidationDisabled;
	BLayout*		fLayout;
	BLayoutContext*	fLayoutContext;
	BObjectList<BLayoutItem> fLayoutItems;
	bool			fLayoutValid;
	bool			fMinMaxValid;
	bool			fLayoutInProgress;
	bool			fNeedsRelayout;
};

namespace rbe
{
	namespace B
	{
		void
		View::rbe__gc_free(void *ptr)
		{
			PointerOf<BView>::Class *tmp = static_cast<PointerOf<BView>::Class *>(ptr);
			BView *view = static_cast<BView *>(tmp);

			// 1. remove self
			view->RemoveSelf();

			// 2. remove children
			while (BView *child = view->ChildAt(0))
				child->RemoveSelf();

			// 3. remove LayoutItems
			// TODO

			Handler::rbe__gc_free(ptr);
		}

		VALUE
		View::rbe_set_layout(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BView::rbe_set_layout", argc, argv, self);
			VALUE vret = Qnil;
			BView *_this = Convert<BView *>::FromValue(self);
			int type_error_index = 0;
			if (1 == argc) {
				BLayout *layout = NULL;
				if (!NIL_P(argv[0])) {
					if (!Convert<BLayout * >::IsConvertable(argv[0])) {
						type_error_index = 0;
						goto break_0;
					}
					layout = Convert<BLayout * >::FromValue(argv[0]);
				}
				// see  haiku/src/kits/interface/View.cpp
				if (layout == _this->fLayoutData->fLayout)
					return Qnil;

				if (layout && layout->Layout())
					rb_raise(rb_eRuntimeError,
							 "B::View#set_layout failed, layout if already in use.");
				// unset and (don't) delete the old layout
				if (_this->fLayoutData->fLayout) {
					_this->fLayoutData->fLayout->RemoveSelf();
					_this->fLayoutData->fLayout->SetOwner(NULL);
					// delete fLayoutData->fLayout;
					_this->fLayoutData->fLayout = NULL;
				}

				std::function<void ()> f = [&]() {
					_this->BView::SetLayout(layout);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				if (layout)
					gc::Up(self, argv[0]);
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				return vret;
			}
		break_0:

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}
	}
}
