
#include <ruby.h>

#include "Archivable.hpp"
#include "Invoker.hpp"
#include "Message.hpp"
#include "Handler.hpp"
#include "Looper.hpp"
#include "Messenger.hpp"

#include "rbe.hpp"
#include "convert.hpp"
#include "type_map.hpp"
#include "debug.hpp"
#include "gc.hpp"

namespace rbe
{
	namespace B
	{
		void Invoker::rbe__gc_free(void *ptr)
		{
			RBE_PRINT(("Invoker::rb__gc_free: %p\n", ptr));
			BInvoker *invoker = static_cast<BInvoker *>(ptr);
			invoker->fMessage = NULL;
		}

		void Invoker::rbe__gc_mark(void *ptr)
		{
			RBE_TRACE("Invoker::rbe_gc_mark");
			RBE_PRINT(("  ptr = %p\n", ptr));
			BInvoker *invoker = static_cast<BInvoker *>(ptr);
			BLooper *looper;
			BHandler *handler = invoker->Target(&looper);
			if (looper) {
				VALUE vlooper = Convert<BLooper *>::ToValue(looper);
				rb_gc_mark(vlooper);
			}
			if (handler) {
				VALUE vhandler = Convert<BHandler *>::ToValue(handler);
				rb_gc_mark(vhandler);
			}
			handler = invoker->HandlerForReply();
			if (handler) {
				VALUE vhandler = Convert<BHandler *>::ToValue(handler);
				rb_gc_mark(vhandler);
			}
		}

		VALUE
		Invoker::rbe_set_message(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Invoker::rbe_set_message", argc, argv, self);
			VALUE vret = Qnil;

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);

			BInvoker *invoker = Convert<BInvoker *>::FromValue(self);

			VALUE vmessage = *argv;
			if (!NIL_P(vmessage) && !Convert<BMessage *>::IsConvertable(vmessage))
				rb_raise(rb_eTypeError, "wrong type of argument 1");

			if (invoker->fMessage) {
				VALUE vold = Convert<BMessage *>::ToValue(invoker->fMessage);
				gc::Down(self, vold);
				invoker->fMessage = NULL;
			}

			BMessage *message = (!NIL_P(vmessage) ?
								 Convert<BMessage *>::FromValue(vmessage) : NULL);
			status_t ret = invoker->BInvoker::SetMessage(message);
			if (ret == B_OK && message)
				gc::Up(self, vmessage);

			vret = Convert<status_t>::ToValue(ret);
			return vret;
		}

		VALUE
		Invoker::rbe_target(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Invoker::rbe_target", argc, argv, self);
		
			BInvoker *_this = Convert<BInvoker *>::FromValue(self);

			if (argc != 0)
				rb_raise(rb_eArgError, "wrong number of arguments (0 for %d)", argc);
			BHandler *handler;
			BLooper *looper;

			handler = _this->Target(&looper);
			VALUE vhandler = Convert<BHandler *>::ToValue(handler);
			VALUE vlooper = Convert<BLooper *>::ToValue(looper);
			return rb_ary_new_from_args(2, vhandler, vlooper);
		}

		VALUE
		Invoker::rbe_invoke_kind(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Invoker::rbe_invoke_kind", argc, argv, self);

			BInvoker *_this = Convert<BInvoker *>::FromValue(self);

			if (argc != 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			VALUE vnotify;
			VALUE vvalue;
			if (_this->fNotifyKind != 0) {
				vnotify = Qtrue;
				vvalue = Convert<uint32>::ToValue(_this->fNotifyKind);
			} else {
				vnotify = Qfalse;
				vvalue = Convert<uint32>::ToValue(B_CONTROL_INVOKED);
			}
			return rb_ary_new_from_args(2, vnotify, vvalue);
		}
	}
}
