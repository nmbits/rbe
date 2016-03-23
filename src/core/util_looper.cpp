
#include "util_looper.hpp"
#include "interface.hpp"
#include "call_script.hpp"
#include "memorize.hpp"

#include "Application.hpp"
#include "Looper.hpp"
#include "Window.hpp"
#include "Handler.hpp"
#include "Message.hpp"

#define private public
#define protected public
#include <interface/Alert.h>
#undef protected
#undef private

#include <functional>

namespace rbe {
	namespace Util {

		void
		RemoveChildrenIfWindow(BLooper *looper)
		{
			BWindow *window = dynamic_cast<BWindow *>(looper);
			if (window) {
				while (window->CountChildren())
					window->RemoveChild(window->ChildAt(0));
			}
		}

		bool DispatchMessageCommon(BLooper *looper, BMessage *message, BHandler *handler)
		{
			bool under_control = true;
			VALUE self = Qnil;
			VALUE vhandler = Qnil;
			VALUE vmessage = Qnil;

			if (ThreadException())
				return false;

			std::function<void ()> c = [&]() {
				rb_funcall(self, rb_intern("dispatch_message"), 2, vmessage, vhandler);
			};

			std::function<void ()> f = [&]() {
				vhandler = Convert<BHandler *>::ToValue(handler);
				if (handler != NULL && vhandler == Qnil) {
					under_control = false;
					return;
				}
				looper->DetachCurrentMessage();
				self = Convert<BLooper *>::ToValue(looper);
				vmessage = Convert<BMessage *>::ToValue(message);
				if (vmessage == Qnil)
					vmessage = B::Message::Wrap(message);
				Protect<std::function<void ()> > p(c);
				p();
				SetThreadException(p.State());
			};

			CallWithGVL<std::function<void ()> > g(f);
			g();
			return under_control;
		}

		static VALUE
		looper_thread_func0(void *data)
		{
			BLooper *looper = static_cast<BLooper *>(data);

			rename_thread(find_thread(NULL), looper->Name());

			std::function<void ()> task1 = [&]() {
				if (looper->Lock())
					looper->task_looper();
			};

			std::function<void ()> task0 = [&]() {
				CallWithoutGVL<std::function<void ()>, BLooper> g(task1, looper);
				g();
				if (ThreadException())
					rb_jump_tag(ThreadException());
			};

			Protect<std::function<void ()> > p(task0);
			p();

			PointerOf<BLooper>::Class *ptr =
				static_cast<PointerOf<BLooper>::Class *>(looper);
			VALUE v = ObjectRegistory::Instance()->Get(ptr);
			DATA_PTR(v) = NULL;
			ObjectRegistory::Instance()->Delete(ptr);
			delete looper;

			rb_thread_check_ints();

			if (p.State() != 0) {
				VALUE errinfo = rb_errinfo();
				if (!rb_obj_is_kind_of(errinfo, eQuitLooper))
					rb_jump_tag(p.State());
			}

			return Qnil;
		}

		VALUE
		rbe_run_common(int argc, VALUE *argv, VALUE self)
		{
			if (argc > 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			BLooper *_this = Convert<BLooper *>::FromValue(self);
	
			if (_this->LockingThread() != find_thread(NULL)) {
				rb_raise(rb_eThreadError, "the thread must have lock of this Looper");
				return Qnil;
			}

			if (_this->fRunCalled)
				rb_raise(rb_eThreadError, "can't call B::Looper#run twice!");

			// assume rb_thread_create never fail!
			_this->fRunCalled = true;
			VALUE thval = rb_thread_create((VALUE (*)(ANYARGS))looper_thread_func0, (void *)_this);
	
			rb_iv_set(thval, "__rbe_looper", self); // life line
			rb_iv_set(self, "__rbe_thread", thval); // to support Thread()

			_this->Unlock();

			return thval;
		}
	}
}
