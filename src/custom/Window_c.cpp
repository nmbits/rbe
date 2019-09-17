
#include <ruby.h>

#define private public
#define protected public

#include <app/Application.h>
#include <interface/LayoutContext.h>
#include <support/ObjectList.h>

#undef private
#undef protected

#include "Looper.hpp"
#include "util_looper.hpp"
#include "util_view.hpp"
#include "Window.hpp"
#include "View.hpp"
#include "Message.hpp"
#include "LayoutItem.hpp"
#include "Layout.hpp"

#include "rbe.hpp"
#include "lock.hpp"
#include "convert.hpp"
#include "util_view.hpp"

#include <functional>

// copied from haiku/src/kits/interface/View.cpp
// TODO

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
		Window::rbe__gc_free(void *ptr)
		{
		    RBE_PRINT(("BWindow::rb__gc_free: %p\n", ptr));
		    PRINT(("ptr = %p\n", ptr));

			PointerOf<BWindow>::Class *tmp = static_cast<PointerOf<BWindow>::Class *>(ptr);
			BWindow *_this = static_cast<BWindow *>(tmp);
			Util::RemoveChildrenIfWindow(_this);
			Looper::rbe__gc_free(ptr);
		}

		void
		Window::DispatchMessageST(BWindow *_this, BMessage *message, BHandler *handler)
		{
			RBE_TRACE("Window::DispatchMessageST");

			switch(message->what) {
			case RBE_MESSAGE_UBF:
				_this->fTerminating = true;
				break;

			default:
				if (!Util::DispatchMessageCommon(_this, message, handler))
					_this->BWindow::DispatchMessage(message, handler);
			}

			if (ThreadException())
				_this->fTerminating = true;
		}

		void
		Window::QuitST(BWindow *_this)
		{
			RBE_TRACE(("Window::QuitST"));

			std::function<void ()> f = [&]() {
				VALUE self = Convert<BWindow *>::ToValue(_this);
				rbe_quit(0, NULL, self);
			};

			int state = ProtectedCallWithGVL(f);
			SetThreadException(state);
		}

		//
		// B::Window.new rect, name, window_type, flags, workspace = B::CURRENT_WORKSPACE
		// B::Window.new rect, name, [look, feel], flags, workspace = B::CURRENT_WORKSPACE
		// B::Window.new message
		//
		VALUE
		Window::rbe__initialize(int argc, VALUE *argv, VALUE self)
		{
			VALUE vargs[5];
			rb_scan_args(argc, argv, "14", &vargs[0], &vargs[1], &vargs[2], &vargs[3], &vargs[4]);

			Window *_this = NULL;

			if (!be_app)
				rb_raise(rb_eRuntimeError, "a valid B::Application object is needed before interacting with tha app_server");

			switch (argc) {
			case 1:
				{
					if (!Convert<BMessage *>::IsConvertable(vargs[0]))
						rb_raise(rb_eTypeError, "arg 0 must be a Message");
					BMessage * arg0 = Convert<BMessage *>::FromValue(vargs[0]);
					_this = new Window(arg0);
				}
				break;

			case 4:
			case 5:
				{
					if (!Convert<BRect>::IsConvertable(vargs[0])) break;
					if (!Convert<char *>::IsConvertable(vargs[1])) break;
					if (!Convert<uint32>::IsConvertable(vargs[3])) break;
					if (argc == 5 && !Convert<uint32>::IsConvertable(vargs[4])) break;

					BRect rect = Convert<BRect>::FromValue(vargs[0]);
					char * name = Convert<char *>::FromValue(vargs[1]);
					uint32 flags = Convert<uint32>::FromValue(vargs[3]);
					uint32 workspace = (4 < argc ? Convert<uint32>::FromValue(vargs[4]) : B_CURRENT_WORKSPACE);

					if (::TYPE(vargs[2]) == T_ARRAY) {
						if (RARRAY_LEN(vargs[2]) != 2)
							rb_raise(rb_eArgError, "arg 2 should have 2 integers");
						VALUE vlook = RARRAY_AREF(vargs[2], 0);
						VALUE vfeel = RARRAY_AREF(vargs[2], 1);
						if (!Convert<window_look>::IsConvertable(vlook) ||
							!Convert<window_feel>::IsConvertable(vfeel))
							rb_raise(rb_eArgError, "arg 2 should have 2 integers");
						window_look look = (window_look)Convert<window_look>::FromValue(vlook);
						window_feel feel = (window_feel)Convert<window_feel>::FromValue(vfeel);
						_this = new Window(rect, name, look, feel, flags, workspace);
					} else {
						if (!Convert<window_type>::IsConvertable(vargs[2])) break;
						window_type type = (window_type)Convert<window_type>::FromValue(vargs[2]);
						_this = new Window(rect, name, type, flags, workspace);
					}
				}
				break;

			default:
				rb_raise(rb_eArgError, "wrong number of argument (%d for 1..5)", argc);
			}

			if (!_this)
				rb_raise(rb_eTypeError, "wrong type of argument");

			PointerOf<BWindow>::Class *ptr = static_cast<PointerOf<BWindow>::Class *>(_this);
			DATA_PTR(self) = static_cast<void *>(ptr);
			gc::Memorize(self);
			return self;
		}

		VALUE
		Window::rbe_run(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Window::rbe_run", argc, argv, self);
			return Util::rbe_run_common(argc, argv, self);
		}

		VALUE
		Window::rbe_show(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Window::rbe_show", argc, argv, self);

			if (argc > 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			BWindow *_this = Convert<BWindow *>::FromValue(self);
			if (!_this->fRunCalled)
				Window::rbe_run(0, NULL, self);
			RBE_PRINT(("  _this->Thread(): %d\n", _this->Thread()));
			std::function<status_t (bigtime_t)> f = [&](bigtime_t tm) -> status_t {
				return _this->LockWithTimeout(tm);
			};
			status_t status = Util::LockWithTimeoutCommon(f, B_INFINITE_TIMEOUT);
			if (status == B_OK) {
				_this->Show(); // TODO might block ?
				_this->Unlock();
			}
			RBE_PRINT(("  _this->Thread(): %d\n", _this->Thread()));
			if (ThreadException() > 0)
				rb_jump_tag(ThreadException());

			return Qnil;
		}

		VALUE
		Window::rbe_quit(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Window::rbe_quit", argc, argv, self);
			if (argc > 0)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 0)", argc);
			BWindow *_this = Convert<BWindow *>::FromValue(self);
			if (!_this->IsLocked())
				rb_raise(rb_eThreadError, "You must lock a looper before calling quit()");
			if (!_this->Lock())
				return Qnil;
			while (!_this->IsHidden())
				_this->Hide();
			Util::RemoveChildrenIfWindow(_this);
			RBE_PRINT(("  _this->Thread(): %d\n", _this->Thread()));
			if (_this->fFlags & B_QUIT_ON_WINDOW_CLOSE)
				be_app->PostMessage(B_QUIT_REQUESTED);
			B::Looper::rbe_quit(0, NULL, self);
			return Qnil;
		}

		VALUE
		Window::rbe_add_child(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BWindow::rbe_add_child", argc, argv, self);
			VALUE vret = Qnil;
			BWindow *_this = Convert<BWindow *>::FromValue(self);
			bool unlock = false;
			if (find_thread(NULL) != _this->LockingThread()) {
				status_t status =
					Util::LockLooperWithTimeout(_this, B_INFINITE_TIMEOUT);
				if (status != B_OK)
					return Qnil;
				unlock = true;
			}
			int type_error_index = 0;
			if (1 <= argc && argc <= 2) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_0;
				}
				if (0 < argc && !Convert<BView * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_0;
				}
				if (1 < argc && argv[1] == Qnil) {
					type_error_index = 1;
					goto break_0;
				}
				if (1 < argc && !Convert<BView * >::IsConvertable(argv[1])) {
					type_error_index = 1;
					goto break_0;
				}
				BView * child = Convert<BView * >::FromValue(argv[0]);
				BView * before = (1 < argc ? Convert<BView * >::FromValue(argv[1]) : (BView *)NULL);
				// see BView::_AddChild()

				if (child->fParent != NULL) {
					if (unlock) _this->Unlock();
					rb_raise(rb_eRuntimeError,
							 "B::Window#add_child failed - the view already has a parent.");
				}

				if (before && before->fParent != _this->fTopView) {
					if (unlock) _this->Unlock();
					rb_raise(rb_eRuntimeError, "Invalid before view");
				}

				std::function<void ()> f = [&]() {
					_this->BWindow::AddChild(child, before);
				};
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				if (child->Window() == _this)
					gc::Up(self, argv[0]);
				if (unlock)
					_this->Unlock();
				rb_thread_check_ints();
				if (ThreadException() > 0) {
					rb_jump_tag(ThreadException());
				}
				return vret;
			}
		break_0:

			if (1 == argc) {
				if (0 < argc && argv[0] == Qnil) {
					type_error_index = 0;
					goto break_1;
				}
				if (0 < argc && !Convert<BLayoutItem * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_1;
				}
				BLayoutItem * child = Convert<BLayoutItem * >::FromValue(argv[0]);

				BLayout *layout = _this->fTopView->fLayoutData->fLayout;
				if (layout == NULL) {
					if (unlock) _this->Unlock();
					return Qnil;
				}
				VALUE layout_ = Convert<BLayout *>::ToValue(layout);
				vret = Layout::rbe_add_item(1, argv, layout_);
				BView *view = child->View();
				if (view && (view->Window() == _this)) {
					VALUE view_ = Convert<BView *>::ToValue(view);
					gc::Up(self, view_);
				}

				if (unlock) _this->Unlock();
				return vret;
			}
		break_1:

			if (unlock)
			    _this->Unlock();
			if (argc < 1 || argc > 2)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for (1..2))", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}

		VALUE
		Window::rbe_remove_child(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BWindow::rbe_remove_child", argc, argv, self);
			VALUE vret = Qfalse;
			BWindow *_this = Convert<BWindow *>::FromValue(self);
			bool unlock = false;
			if (find_thread(NULL) != _this->LockingThread()) {
				status_t status =
					Util::LockLooperWithTimeout(_this, B_INFINITE_TIMEOUT);
				if (status != B_OK)
					return Qnil;
				unlock = true;
			}
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

				if (child->Window() != _this)
					return Qfalse;

				std::function<void ()> f =
					[&](){ vret = View::rbe_remove_self(0, NULL, argv[0]); };
				Protect<std::function<void ()>> g(f);
				g();
				if (unlock)
					_this->Unlock();
				if (g.State() > 0)
					rb_jump_tag(g.State());

				return vret;
			}
		break_0:

			if (unlock)
			    _this->Unlock();
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			else
				rb_raise(rb_eArgError, "wrong type of argument at %d", type_error_index);
			return Qnil;
		}

		VALUE
		Window::rbe_set_layout(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("BWindow::rbe_set_layout", argc, argv, self);
			VALUE vret = Qnil;
			BWindow *_this = Convert<BWindow *>::FromValue(self);
			int type_error_index = 0;
			if (1 == argc) {
				if (0 < argc && !Convert<BLayout * >::IsConvertable(argv[0])) {
					type_error_index = 0;
					goto break_0;
				}
				BLayout * layout = Convert<BLayout * >::FromValue(argv[0]);
				if (layout != NULL)
					_this->fTopView->AdoptViewColors(layout->View());
				if (Util::SetLayoutCommon(_this->fTopView, layout)) {
					if (layout)
						gc::Up(self, argv[0]);
				}
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
