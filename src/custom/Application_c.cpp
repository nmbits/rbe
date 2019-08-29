
#include <support/Debug.h>

#include "rbe.hpp"

#include "call_script.hpp"
#include "convert.hpp"
#include "protect.hpp"
#include "gvl.hpp"

#include "Application.hpp"
#include "Handler.hpp"
#include "Message.hpp"

#include "util_looper.hpp"

#include <functional>

namespace rbe
{
	namespace B
	{
		/* static */ void
		Application::ArgvReceivedST(BApplication *_this, int32 argc, char **argv)
		{
			RBE_TRACE("Application::ArgvReceived");
			if (rbe::ThreadException())
				return;

			std::function<void ()> f = [&]() {
				VALUE self = Convert<BApplication *>::ToValue(_this);
				VALUE *values = ALLOCA_N(VALUE, argc);
				for (int i = 0; i < argc; i++)
					values[i] = rb_str_new_cstr(argv[i]);
				rb_funcallv(self, rb_intern("argv_received"), argc, values);
			};
			int state = ProtectedCallWithGVL(f);
			SetThreadException(state);
		}

		/* static */ void
		Application::DispatchMessageST(BApplication *_this, BMessage *msg, BHandler *handler)
		{
			RBE_TRACE("Application::DispatchMessage");

			if (handler != _this) {
				Looper::DispatchMessageST(_this, msg, handler);
				return;
			}
	
			switch (msg->what) {
			case RBE_MESSAGE_UBF:
				_this->fTerminating = true;

			default:
				if (!Util::DispatchMessageCommon(_this, msg, handler))
					_this->BApplication::DispatchMessage(msg, handler);
				break;
			}

			if (ThreadException())
				_this->fTerminating = true;
		}

		void
		Application::QuitST(BApplication *_this)
		{
			RBE_TRACE(("Application::QuitST"));

			std::function<void ()> f = [&]() {
				VALUE self = Convert<BApplication *>::ToValue(_this);
				rbe_quit(0, NULL, self);
			};

			int state = ProtectedCallWithGVL(f);
			SetThreadException(state);
		}

		VALUE
		Application::rbe__initialize(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rbe__initialize", argc, argv, self);
			VALUE varg0;
			status_t status;
			Application *_this = NULL;

			if (be_app)
				rb_raise(rb_eRuntimeError, "another B::Application exists");

			rb_scan_args(argc, argv, "10", &varg0);

			if (Convert<char *>::IsConvertable(varg0)) {
				char * arg0 = Convert<char *>::FromValue(varg0);
				_this = new Application(arg0, &status);
				if (status != B_OK)
					rb_raise(rb_eArgError, "BApplication cannot be initialized (%d)", status);
			} else if (Convert<BMessage *>::IsConvertable(varg0)) {
				BMessage *arg0 = Convert<BMessage *>::FromValue(varg0);
				_this = new Application(arg0);
			} else {
				rb_raise(rb_eArgError, "invalid argument");
			}
			PointerOf<BApplication>::Class *p = _this;
			DATA_PTR(self) = p;
			gc::Memorize(self);
			return self;
		}

		VALUE
		Application::rbe_argv_received(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rbe_argv_received", argc, argv, self);
			return Qnil;
		}

		VALUE
		Application::rbe_run(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rbe_run", argc, argv, self);
			if (argc)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			BApplication *_this = Convert<BApplication *>::FromValue(self);
			Util::AssertLocked(_this);

			if (_this->fRunCalled)
				rb_raise(rb_eRuntimeError, "B::Application#run or #quit was already called.");

			std::function<void ()> f = [&]() {
				_this->BApplication::Run();
			};

			CallWithoutGVL<std::function<void ()>, BLooper> g(f, _this);
			g();
			rb_thread_check_ints();
			if (ThreadException())
				rb_jump_tag(ThreadException());
			return rb_thread_current();
		}

		VALUE
		Application::rbe_quit(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rbe_quit", argc, argv, self);

			if (argc)
				rb_raise(rb_eArgError,
						 "wrong number of arguments (%d for 0)", argc);

			BApplication *_this = Convert<BApplication *>::FromValue(self);

			if (_this->LockingThread() != find_thread(NULL))
				rb_raise(rb_eThreadError, "you must Lock the application object before calling Quit()");

			if (!_this->fRunCalled) {
				// We don't call BApplication::Quit().
				// We expect the app will be GCed instead.
				return Qnil;
			}

			std::function<void ()> f = [&]() {
				_this->BApplication::Quit();
			};

			if (find_thread(NULL) != _this->Thread()) {
				CallWithoutGVL<std::function<void ()>, void> g(f);
				g();
				rb_thread_check_ints();
			} else {
				_this->fTerminating = true;
			}
			return Qnil;
		}

		VALUE
		Application::rbe_quit_requested(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rbe_quit_requested", argc, argv, self);

			BApplication *_this = Convert<BApplication *>::FromValue(self);

			if (find_thread(NULL) != _this->LockingThread())
				rb_raise(rb_eThreadError, "the looper isn't locked by this thread");

			if (argc)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 0)", argc);

			bool result = false;
			std::function<void ()> f = [&]() {
				result = _this->BApplication::QuitRequested();
			};

			CallWithoutGVL<std::function<void ()>, void> g(f);
			g();
			rb_thread_check_ints();
			if (ThreadException())
				rb_jump_tag(ThreadException());
			return Convert<bool>::ToValue(result);
		}

		void Application::rbe__gc_free(void *ptr)
		{
		    RBE_PRINT(("B::Application::rb__gc_free: %p\n", ptr));
		    RBE_PRINT(("do nothing\n"));
		}
	}
}
