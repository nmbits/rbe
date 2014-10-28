
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

#include "Looper_c.hpp"

namespace rbe
{
	namespace Private
	{
		namespace Application
		{
			struct Run_f
			{
				BApplication *fApp;

				Run_f(BApplication *app)
					: fApp(app)
				{}

				void operator()()
				{
					fApp->BApplication::Run();
				}
			};

			struct Quit_f
			{
				BApplication *fApp;

				Quit_f(BApplication *app)
					: fApp(app)
				{}

				void operator()()
				{
					fApp->Quit();
				}
			};

			struct QuitRequested_f
			{
				BApplication *fApp;
				bool result;

				QuitRequested_f(BApplication *app, bool res = false)
					: fApp(app), result(res)
				{}

				void operator()()
				{
					result = fApp->BApplication::QuitRequested();
				}
			};

			struct ArgvReceived_f
			{
				BApplication *fApp;
				int32 fArgc;
				char **fArgv;

				ArgvReceived_f(BApplication *app, int32 argc, char **argv)
					: fApp(app)
					, fArgc(argc)
					, fArgv(argv)
				{}

				void operator()()
				{
					VALUE self = Convert<BApplication *>::ToValue(fApp);
					VALUE *values = ALLOCA_N(VALUE, fArgc);
					for (int i = 0; i < fArgc; i++)
						values[i] = rb_str_new_cstr(fArgv[i]);
					rb_funcallv(self, rb_intern("argv_received"), fArgc, values);
				}
			};
		}
	}

	namespace Hook
	{
		namespace Application
		{
			void
			ArgvReceived(BApplication *_this, int32 argc, char **argv)
			{
				RBE_TRACE("Application::ArgvReceived");
				if (rbe::FuncallState() != 0)
					return;
	
				Private::Application::ArgvReceived_f f(_this, argc, argv);
				Protect<Private::Application::ArgvReceived_f> p(f);
				CallWithGVL<Protect<Private::Application::ArgvReceived_f> > g(p);
				g();
				SetFuncallState(p.State());
			}

			void
			DispatchMessage(BApplication *_this, BMessage *msg, BHandler *handler)
			{
				RBE_TRACE("Application::DispatchMessage");
				RBE_PRINT(("msg = %p\n", msg));
				bool interrupted = false;
	
				switch (msg->what) {
				case _QUIT_:
					_this->BApplication::DispatchMessage(msg, handler);
					break;
	
				case RBE_MESSAGE_UBF:
					interrupted = true;
					break;
	
				default:
					if (FuncallState() == 0) {
						int state = Util::Looper::DispatchMessageCommon(_this, msg, handler);
						SetFuncallState(state);
					}
				}
				if (FuncallState() != 0 || interrupted)
					_this->BApplication::Quit();
			}
		}
	}

	namespace B
	{
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
			Util::Looper::AssertLocked(_this);

			if (_this->fRunCalled)
				rb_raise(rb_eRuntimeError, "B::Application#run or #quit was already called.");

			rbe::Private::Application::Run_f f(_this);
			Util::Looper::UbfLooper_f u(_this);
			CallWithoutGVL<rbe::Private::Application::Run_f, Util::Looper::UbfLooper_f> g(f, u);
			g();
			rb_thread_check_ints();
			if (FuncallState() > 0)
				rb_jump_tag(FuncallState());
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

			if (_this->LockingThread() != find_thread(NULL))
				rb_raise(rb_eThreadError, "you must Lock the application object before calling Quit()");

			if (!_this->fRunCalled) {
				// We don't call BApplication::Quit().
				// We expect the app will be GCed instead.
				return Qnil;
			}

			rbe::Private::Application::Quit_f f(_this);
			if (find_thread(NULL) != _this->Thread()) {
				CallWithoutGVL<rbe::Private::Application::Quit_f, void> g(f);
				g();
				rb_thread_check_ints();
			} else {
				f();
			}
			return Qnil;
		}

		VALUE
		Application::rb_quit_requested(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Application::rb_quit_requested", argc, argv, self);

			BApplication *_this = Convert<BApplication *>::FromValue(self);

			if (find_thread(NULL) != _this->LockingThread())
				rb_raise(rb_eThreadError, "the looper isn't locked by this thread");

			if (argc)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 0)", argc);

			rbe::Private::Application::QuitRequested_f f(_this);
			CallWithoutGVL<rbe::Private::Application::QuitRequested_f, void> g(f);
			g();
			rb_thread_check_ints();
	        if (FuncallState() > 0)
				rb_jump_tag(FuncallState());
			return Convert<bool>::ToValue(f.result);
		}
	}
}
