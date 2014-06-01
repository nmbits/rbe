
#include <kernel/OS.h>
#include <support/Debug.h>

#include <ruby.h>

#include "rbe.hpp"
#include "protect.hpp"
#include "funcall.hpp"
#include "convert.hpp"
#include "call_with_gvl.hpp"
#include "call_without_gvl.hpp"

#include "Message.hpp"
#include "Handler.hpp"
#include "Looper.hpp"
#include "looper_common.hpp"
#include "thread_attach_filter.hpp"
#include "ft.h"

namespace rbe
{
	namespace B
	{
		namespace LooperPrivate
		{
			struct Quit
			{
				BLooper *looper;
				void operator()()
				{
					if (find_thread(NULL) == looper->Thread())
						LooperCommon::QuitLooper(looper, FuncallState());
					else
						looper->Quit();
				}
			};
		}

		VALUE
		Looper::rb_add_handler(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_add_handler", argc, argv, self);
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			BHandler *handler = Convert<BHandler *>::FromValue(*argv);
			BLooper *_this = Convert<BLooper *>::FromValue(self);
			_this->AddHandler(handler);
			MemorizeObject(self, *argv);
			MemorizeObject(*argv, self);
			return Qnil;
		}

		VALUE
		Looper::rb_remove_handler(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_remove_handler", argc, argv, self);
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			BHandler *handler = Convert<BHandler *>::FromValue(*argv);
			BLooper *_this = Convert<BLooper *>::FromValue(self);
			bool result = _this->RemoveHandler(handler);
			if (result) {
				ForgetObject(self, *argv);
				ForgetObject(*argv, self);
				return Qtrue;
			}
			return Qnil;
		}

		VALUE
		Looper::rb_run(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_run", argc, argv, self);
			if (argc > 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			VALUE vret = Qnil;

			BLooper *_this = Convert<BLooper *>::FromValue(self);
			Looper *_looper = static_cast<Looper *>(_this);

			if (_this->LockingThread() != find_thread(NULL)) {
				rb_raise(rb_eThreadError, "the thread must have lock of this Looper");
				return Qnil;
			}

			if (_looper->fRunCalled) {
				rb_raise(rb_eRuntimeError, "run() already called");
				return Qnil;
			}

			_looper->fRunCalled = true;

			ft_handle_t *ft = ft_thread_create();
			VALUE ret = ft->thval;
			PRINT(("thval: %x\n", ret));
			PRINT(("DATA_PTR(self): %p\n", DATA_PTR(self)));
			rb_iv_set(ret, "__rbe_looper", self); // life line
			rb_iv_set(self, "__rbe_thread", ret); // to support Thread()

			BMessageFilter *filter = new ThreadAttachFilter(ft);
			BMessage message(RBE_MESSAGE_REMOVE_FILTER);
			message.AddPointer("filter", static_cast<void *>(filter));
			PRINT(("AddCommonFilter\n"));
			_this->AddCommonFilter(filter);
			PRINT(("PostMessage\n"));
			_this->PostMessage(&message);
			PRINT(("Run\n"));
			_this->BLooper::Run();
			ft_wait_for_attach_completed(ft);

			return ret;
		}

		VALUE
		Looper::rb_quit(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_quit", argc, argv, self);

			if (argc)
				rb_raise(rb_eArgError,
						 "wrong number of arguments (%d for 0)", argc);

			BLooper *_this = Convert<BLooper *>::FromValue(self);
			Looper *_looper = static_cast<Looper *>(_this);
			thread_id tid = find_thread(NULL);
			
			if (_looper->LockingThread() != tid)
				rb_raise(rb_eThreadError, "This thread doesn't have lock.");

			if (!_looper->fRunCalled) {
				// In this case, we expect that this Looper object
				// will be removed thru GC. So, we do nothing here.
				return Qnil;
			}

			LooperPrivate::Quit f = { _this };
			CallWithoutGVL<LooperPrivate::Quit, void> g(f);
			// If the current thread is a Looper thread,
			// the following function will not return.
			g();
			rb_thread_check_ints();

			return Qnil;
		}

		VALUE
		Looper::rb_lock(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_lock", argc, argv, self);
			VALUE vret = Qnil;

			BLooper *_this = Convert<BLooper *>::FromValue(self);

			if (argc != 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);
			status_t result = LooperCommon::LockWithTimeout<BLooper>(_this, B_INFINITE_TIMEOUT);

			if (result == B_OK)
				return Qtrue;
			return Qfalse;
		}

		VALUE
		Looper::rb_lock_with_timeout(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Looper::rb_lock_with_timeout", argc, argv, self);
			VALUE vret = Qnil;

			BLooper *_this = Convert<BLooper *>::FromValue(self);

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 0)", argc);
			if (!Convert<bigtime_t>::IsConvertable(*argv))
				rb_raise(rb_eTypeError, "wrong type of argument");

			bigtime_t arg0 = Convert<bigtime_t>::FromValue(*argv);
			status_t result = LooperCommon::LockWithTimeout<BLooper>(_this, arg0);

			return Convert<status_t>::ToValue(result);
		}

		void
		Looper::DispatchMessage(BMessage *msg, BHandler *handler)
		{
			RBE_TRACE("Looper::DispatchMessage");
			RBE_PRINT(("msg = %p\n", msg));

			LooperCommon::DispatchMessageCommon(this, msg, handler);
		}
	}
}
