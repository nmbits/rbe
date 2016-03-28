
#include <ruby.h>

#define private public
#define protected public

#include <app/Application.h>

#undef private
#undef protected

#include "Looper.hpp"
#include "util_looper.hpp"
#include "Window.hpp"
#include "View.hpp"
#include "Message.hpp"

#include "rbe.hpp"
#include "lock.hpp"
#include "convert.hpp"
#include "deleting.hpp"

#include <functional>

namespace rbe
{
	namespace gc
	{
		template<>
		void Deleting<BWindow, BView>(BWindow *o, BView *t)
		{
			o->RemoveChild(t);
		}
	}

	namespace B
	{
		void
		Window::DispatchMessageST(BWindow *_this, BMessage *message, BHandler *handler)
		{
			RBE_TRACE("Window::DispatchMessageST");

			bool terminate = false;

			switch(message->what) {
			case _QUIT_:
				terminate = true;
				return;

			case B_QUIT_REQUESTED:
				if (handler == _this) {
					// from HAIKU's BLooper#_QuitRequested()
					terminate = _this->QuitRequested();
					if (terminate)
						break;
					bool shutdown;
					if (message->IsSourceWaiting()
						|| (message->FindBool("_shutdown_", &shutdown) == B_OK && shutdown)) {
						BMessage replyMessage(B_REPLY);
						replyMessage.AddBool("result", terminate);
						replyMessage.AddInt32("thread", find_thread(NULL));
						message->SendReply(&replyMessage);
					}
				}
				break;

			case RBE_MESSAGE_UBF:
				terminate = true;
				break;

			default:
				if (!Util::DispatchMessageCommon(_this, message, handler))
					_this->BWindow::DispatchMessage(message, handler);
			}

			if (terminate || ThreadException()) {
				Util::RemoveChildrenIfWindow(_this);
				_this->fTerminating = true;
			}
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
			ObjectRegistory::Instance()->Add(self);
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
			std::function<status_t (bigtime_t)> f = [&](bigtime_t tm) -> status_t {
				return _this->LockWithTimeout(tm);
			};
			status_t status = Util::LockWithTimeoutCommon(f, B_INFINITE_TIMEOUT);
			if (status == B_OK) {
				_this->Show(); // TODO might block ?
				_this->Unlock();
			}
			if (ThreadException() > 0)
				rb_jump_tag(ThreadException());

			return Qnil;
		}
	}
}
