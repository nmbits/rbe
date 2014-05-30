
#include <support/Debug.h>

#include "rbe.hpp"

#include "convert.hpp"
#include "funcall.hpp"
#include "protect.hpp"
#include "call_with_gvl.hpp"
#include "call_without_gvl.hpp"

#include "Application.hpp"
#include "Handler.hpp"
#include "Message.hpp"
#include "looper_common.hpp"

namespace rbe
{
	namespace B
	{
		namespace ApplicationPrivate
		{
			struct Run
			{
				BApplication *app;
				void operator()()
				{
					app->BApplication::Run();
				}
			};

			struct Quit
			{
				BApplication *app;
				void operator()()
				{
					app->Quit();
				}
			};

			struct CallArgvReceived
			{
				BApplication *app;
				int32 argc;
				char **argv;
				void operator()()
				{
					VALUE self = Convert<BApplication *>::ToValue(app);
					VALUE *values = ALLOCA_N(VALUE, argc);
					for (int i = 0; i < argc; i++)
						values[i] = rb_str_new_cstr(argv[i]);
					rb_funcallv(self, rb_intern("argv_received"), argc, values);
				}
			};

			struct Ubf
			{
				BApplication *app;
				void operator()()
				{
					LooperCommon::PostUbfMessage(static_cast<BLooper *>(app));
				}
			};
		}

		VALUE
		Application::rb_initialize(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rb_initialize", argc, argv, self);
			VALUE varg0;
			status_t status;
			Application *_this = NULL;

			if (be_app)
				rb_raise(rb_eRuntimeError, "another B::Application exists");

			rb_scan_args(argc, argv, "10", &varg0);

			if (Convert<const char *>::IsConvertable(varg0)) {
				const char * arg0 = Convert<const char *>::FromValue(varg0);
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
			ObjectRegistory::Instance()->Register(self);
			return self;
		}

		VALUE
		Application::rb_argv_received(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rb_argv_received", argc, argv, self);
			return Qnil;
		}

		VALUE
		Application::rb_run(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rb_run", argc, argv, self);
			if (argc)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			BApplication *_this = Convert<BApplication *>::FromValue(self);
			Application *app = static_cast<Application *>(_this);

			LooperCommon::AssertLocked(_this);

			if (app->fRunCalled)
				rb_raise(rb_eRuntimeError, "B::Application#run or #quit was already called.");

			app->fRunCalled = true;

			ApplicationPrivate::Run f = { _this };
			ApplicationPrivate::Ubf u = { _this };
			CallWithoutGVL<ApplicationPrivate::Run, ApplicationPrivate::Ubf> g(f, u);
			g();
			rb_thread_check_ints();
			return rb_thread_current();
		}

		VALUE
		Application::rb_quit(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rb_quit", argc, argv, self);

			if (argc)
				rb_raise(rb_eArgError,
						 "wrong number of arguments (%d for 0)", argc);

			BApplication *_this = Convert<BApplication *>::FromValue(self);
			Application *app = static_cast<Application *>(_this);

			if (_this->LockingThread() != find_thread(NULL))
				rb_raise(rb_eThreadError, "you must Lock the application object before calling Quit()");

			if (!app->fRunCalled) {
				app->fRunCalled = true;
				// We don't call BApplication::Quit().
				// We expect the app will be GCed instead.
				return Qnil;
			}

			PointerOf<BApplication>::Class *ptr = _this;

			ApplicationPrivate::Quit f = { _this };

			if (find_thread(NULL) != _this->Thread()) {
				CallWithoutGVL<ApplicationPrivate::Quit, void> g(f);
				g();
				rb_thread_check_ints();
			} else {
				f();
			}
			return Qnil;
		}

		void
		Application::ArgvReceived(int32 argc, char **argv)
		{
			RBE_TRACE("Application::ArgvReceived");
			if (rbe::FuncallState() != 0)
				return;

			ApplicationPrivate::CallArgvReceived f = {this, argc, argv};
			Protect<ApplicationPrivate::CallArgvReceived> p(f);
			CallWithGVL<Protect<ApplicationPrivate::CallArgvReceived> > g(p);
			g();
			SetFuncallState(p.State());
		}

		void
		Application::DispatchMessage(BMessage *msg, BHandler *handler)
		{
			RBE_TRACE("Application::DispatchMessage");
			RBE_PRINT(("msg = %p\n", msg));
			bool interrupted = false;

			switch (msg->what) {
			case _QUIT_:
				this->BApplication::DispatchMessage(msg, handler);
				break;

			case RBE_MESSAGE_UBF:
				interrupted = true;
				break;

			default:
				if (FuncallState() == 0) {
					DetachCurrentMessage();

					LooperCommon::CallDispatchMessage f = { this, msg, handler };
					Protect<LooperCommon::CallDispatchMessage> p(f);
					CallWithGVL<Protect<LooperCommon::CallDispatchMessage> > g(p);
					g();
					SetFuncallState(p.State());
				}
			}
			if (FuncallState() != 0 || interrupted)
				BApplication::Quit();
		}
	}
}
