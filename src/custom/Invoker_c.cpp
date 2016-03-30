
#include <ruby.h>

#include "Archivable.hpp"
#include "Invoker.hpp"
#include "Message.hpp"
#include "Handler.hpp"
#include "Looper.hpp"
#include "Messenger.hpp"

#include "rbe.hpp"
#include "memorize.hpp"
#include "convert.hpp"
#include "registory.hpp"
#include "ownership.hpp"
#include "type_map.hpp"
#include "debug.hpp"
#include "deleting.hpp"

namespace rbe
{
	namespace gc
	{
		template<>
		void Deleting<BInvoker, BMessage>(BInvoker *o, BMessage *t)
		{
			RBE_TRACE(("rbe::gc::Deleting<BInvoker, BMessage>()"));
			RBE_PRINT(("BMessage = %p\n", t));
			RBE_PRINT(("BInvoker = %p\n", o));
			RBE_PRINT(("o->fMessage = %p\n", o->fMessage));
			if (o->fMessage == t)
				o->fMessage = NULL;
		}
	}

	static VALUE get_invoker_value(VALUE obj)
	{
		if (TYPE(obj) == T_DATA && rb_obj_is_kind_of(obj, B::Archivable::Class())) {
			BArchivable *archivable = static_cast<BArchivable *>(DATA_PTR(obj));
			BInvoker *invoker = dynamic_cast<BInvoker *>(archivable);
			if (invoker)
				return obj;
		}
		return rb_iv_get(obj, RBE_IV_MOD_INVOKER);
	}

	namespace B
	{
		void Invoker::rbe__gc_free(void *ptr)
		{
			RBE_PRINT(("Invoker::rb__gc_free: %p\n", ptr));
			BInvoker *invoker = static_cast<BInvoker *>(ptr);
			ObjectRegistory::Instance()->Delete(ptr);
			invoker->fMessage = NULL;
			delete invoker;
		}

		void Invoker::rbe__gc_mark(void *ptr)
		{
			RBE_TRACE("Invoker::rbe_gc_mark");
			RBE_PRINT(("  ptr = %p\n", ptr));
			ObjectRegistory::Instance()->Mark(ptr);
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

			VALUE vinvoker = get_invoker_value(self);
			BInvoker *invoker = Convert<BInvoker *>::FromValue(vinvoker);

			VALUE vmessage = *argv;
			if (!NIL_P(vmessage) && !Convert<BMessage *>::IsConvertable(vmessage))
				rb_raise(rb_eTypeError, "wrong type of argument 1");
			BMessage *message = NULL;
			if (!NIL_P(vmessage))
				message = Convert<BMessage *>::FromValue(vmessage);
			BMessage *old_message = invoker->Message();
			if (old_message) {
				ObjectRegistory::Instance()->Release(DATA_PTR(vinvoker), static_cast<void *>(old_message));
				invoker->fMessage = NULL;  // because, Invoker automatically deletes the old message
			}
			status_t ret = invoker->BInvoker::SetMessage(message);
			if (ret == B_OK && message) {
				gc::Ownership0 *ownership = new gc::Ownership<BInvoker, BMessage>(vmessage);
				ObjectRegistory::Instance()->Own(vinvoker, ownership);
			}
			vret = Convert< status_t>::ToValue(ret);
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
