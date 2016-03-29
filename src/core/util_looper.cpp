
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
			RBE_TRACE("RemoveChildrenIfWindow");
			RBE_PRINT(("  looper = %p\n", looper));
			BWindow *window = dynamic_cast<BWindow *>(looper);
			if (window) {
				RBE_PRINT(("    is window\n"));
				while (window->CountChildren()) {
					BView *view = window->ChildAt(0);
					RBE_PRINT(("     removing %p\n", view));
					window->RemoveChild(view);
				}
			}
		}

		bool DispatchMessageCommon(BLooper *looper, BMessage *message, BHandler *handler)
		{
			bool under_control = true;

			if (ThreadException())
				return false;

			std::function<void ()> f = [&]() {
				VALUE vhandler = Convert<BHandler *>::ToValue(handler);
				if (handler != NULL && vhandler == Qnil) {
					under_control = false;
					return;
				}
				VALUE self = Convert<BLooper *>::ToValue(looper);
				looper->DetachCurrentMessage();
				VALUE vmessage = Convert<BMessage *>::ToValue(message);
				if (vmessage == Qnil)
					vmessage = B::Message::Wrap(message);

				std::function<void ()> c = [&]() {
					rb_funcall(self, rb_intern("dispatch_message"), 2, vmessage, vhandler);
				};

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
				rb_thread_check_ints();
				if (ThreadException())
					rb_jump_tag(ThreadException());
			};

			Protect<std::function<void ()> > p(task0);
			p();

			PointerOf<BLooper>::Class *ptr =
				static_cast<PointerOf<BLooper>::Class *>(looper);
			VALUE v = ObjectRegistory::Instance()->Get(ptr);
			ObjectRegistory::Instance()->Delete(ptr);
			DATA_PTR(v) = NULL;
			RemoveChildrenIfWindow(looper);
			delete looper;

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
