
#include <ruby.h>

#include <app/Application.h>

#include "Looper.hpp"
#include "Window.hpp"
#include "View.hpp"

#include "rbe.hpp"
#include "looper_common.hpp"
#include "convert.hpp"

namespace rbe
{
	namespace B
	{
		void
		Window::DispatchMessage(BMessage *message, BHandler *handler)
		{
			if (!message)
				return;

			RBE_TRACE("Window::DispatchMessage");
			RBE_PRINT(("message = %p, handler = %p\n", message, handler));
			LooperCommon::DispatchMessageCommon(this, message, handler);
		}

		//
		// B::Window.new rect, name, window_type, flags, workspace = B::CURRENT_WORKSPACE
		// B::Window.new rect, name, [look, feel], flags, workspace = B::CURRENT_WORKSPACE
		// B::Window.new message
		//

		VALUE
		Window::rb_initialize(int argc, VALUE *argv, VALUE self)
		{
			VALUE vargs[5];
			rb_scan_args(argc, argv, "14", &vargs[0], &vargs[1], &vargs[2], &vargs[3], &vargs[4]);

			Window *_this = NULL;

			if (!be_app)
				rb_raise(rb_eRuntimeError, "You need a valid B::Application object before interacting with tha app_server");

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
					if (!Convert<const char *>::IsConvertable(vargs[1])) break;
					if (!Convert<uint32>::IsConvertable(vargs[3])) break;
					if (argc == 5 && !Convert<uint32>::IsConvertable(vargs[4])) break;

					BRect rect = Convert<BRect>::FromValue(vargs[0]);
					const char * name = Convert<const char *>::FromValue(vargs[1]);
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
			ObjectRegistory::Instance()->Register(self);
			return self;
		}

		VALUE
		Window::rb_run(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Window::rb_run", argc, argv, self);
			return LooperCommon::rb_run_common(argc, argv, self);
		}

		VALUE
		Window::rb_show(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Window::rb_show", argc, argv, self);

			if (argc > 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			BWindow *_this = Convert<BWindow *>::FromValue(self);
			VALUE run_called = rb_iv_get(self, "__rbe_run_called");
			if (!RTEST(run_called))
				LooperCommon::rb_run_common(0, NULL, self);
			status_t status = LooperCommon::LockWithTimeout(static_cast<BLooper *>(_this), B_INFINITE_TIMEOUT);
			if (status == B_OK) {
				_this->Show(); // TODO might block ?
				_this->Unlock();
			}
			if (FuncallState() > 0)
				rb_jump_tag(FuncallState());

			return Qnil;
		}

		VALUE
		Window::rb_add_child(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Window::rb_add_child", argc, argv, self);

			BWindow *_this = Convert<BWindow *>::FromValue(self);

			VALUE vview1, vview2;
			rb_scan_args(argc, argv, "11", &vview1, &vview2);

			if (!Convert<BView *>::IsConvertable(vview1))
				rb_raise(rb_eTypeError, "B::View required for arg 0");
			if (argc == 2 && !NIL_P(vview2) && !Convert<BView *>::IsConvertable(vview2))
				rb_raise(rb_eTypeError, "B::View required for arg 1");
			BView *view1 = Convert<BView *>::FromValue(vview1);
			if (view1->Parent() != NULL)
				rb_raise(rb_eArgError, "arg 0 already has a parent");
			BView *view2 = (NIL_P(vview2) ? NULL : Convert<BView *>::FromValue(vview2));
			if (view2 && (view2->Window() != _this || view2->Parent() != NULL))
				rb_raise(rb_eArgError, "invalid arg 1");
			// pre lock
			status_t status = LooperCommon::LockWithTimeout(static_cast<BLooper *>(_this), B_INFINITE_TIMEOUT);
			if (status == B_OK) {
				_this->AddChild(view1, view2);
				_this->Unlock();
			}
			if (view1->Window() == _this)
				MemorizeObject(self, vview1);
			if (FuncallState() > 0)
				rb_jump_tag(FuncallState());
			return Qnil;
		}
	}
}
