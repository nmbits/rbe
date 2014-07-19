
#include <ruby.h>

#include "Invoker.hpp"
#include "Message.hpp"
#include "Handler.hpp"
#include "Looper.hpp"
#include "Messenger.hpp"

#include "rbe.hpp"
#include "convert.hpp"
#include "registory.hpp"
#include "type_map.hpp"
#include "debug.hpp"

namespace rbe
{
	namespace B
	{
		void
		Invoker::rb_free_(void *ptr)
		{
			RBE_TRACE("Invoker::rb_free_");
			ObjectRegistory::Instance()->Unregister(ptr);
			BInvoker *invoker = static_cast<BInvoker *>(ptr);
			invoker->fMessage = NULL;
			delete invoker;
		}

		VALUE
		Invoker::rb_set_message(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Invoker::rb_set_message", argc, argv, self);
			VALUE vret = Qnil;
		
			BInvoker *_this = Convert<BInvoker *>::FromValue(self);
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);
			VALUE vmessage = *argv;
			if (!NIL_P(vmessage) && !Convert< BMessage *>::IsConvertable(vmessage))
				rb_raise(rb_eTypeError, "wrong type of argument 1");

			BMessage *message = NULL;
			if (!NIL_P(vmessage))
				message = Convert<BMessage *>::FromValue(vmessage);
			BMessage *old_message = _this->Message();
			if (old_message) {
				VALUE vold_message =
					ObjectRegistory::Instance()->Get(static_cast<void *>(old_message));
				if (!NIL_P(vold_message)) {
					ForgetObject(vold_message, self);
					ForgetObject(self, vold_message);
				}
				_this->fMessage = NULL;
			}
			status_t ret = _this->BInvoker::SetMessage(message);
			if (ret == B_OK && message) {
				MemorizeObject(self, vmessage);
				MemorizeObject(vmessage, self);
			}
			vret = Convert< status_t>::ToValue(ret);

			return vret;
		}

		VALUE
		Invoker::rb_set_target(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Invoker::rb_set_target", argc, argv, self);
			VALUE vret = Qnil;

			BInvoker *_this = Convert<BInvoker *>::FromValue(self);
		
			VALUE vargs[2];
			rb_scan_args(argc, argv, "11", &vargs[0], &vargs[1]);
		
			BHandler *handler = NULL;
			BLooper *looper = NULL;

			if (Convert<BHandler *>::IsConvertable(vargs[0])) {
				handler = Convert<BHandler *>::FromValue(vargs[0]);
			} else if (Convert<BMessenger>::IsConvertable(vargs[0])) {
				BMessenger tmp = Convert<BMessenger>::FromValue(vargs[0]);
				handler = tmp.Target(&looper);
			}
			else if (!NIL_P(vargs[0])) {
				rb_raise(rb_eTypeError, "wrong type of argument 1");
			}
			if (argc == 2) {
				if (Convert<BMessenger>::IsConvertable(vargs[0]))
					rb_raise(rb_eArgError, "wrong number of argument (2 for 1)");
				if (Convert<BLooper *>::IsConvertable(vargs[1]))
					looper = Convert<BLooper *>::FromValue(vargs[1]);
				else if (!NIL_P(vargs[1]))
					rb_raise(rb_eTypeError, "wrong type of argument");
			}

			status_t status;
			BMessenger messenger(handler, looper, &status);
			if (status!= B_OK)
				rb_raise(rb_eArgError, "something wrong with arguments");

			BHandler *old_handler;
			BLooper *old_looper;
			old_handler = _this->Target(&old_looper);
			status = _this->SetTarget(messenger);
			if (status == B_OK) {
				if (old_handler) {
					VALUE vold_handler = Convert<BHandler *>::ToValue(old_handler);
					if (!NIL_P(vold_handler)) {
						ForgetObject(vold_handler, self);
						ForgetObject(self, vold_handler);
					}
				}
				if (old_looper) {
					VALUE vold_looper = Convert<BLooper *>::ToValue(old_looper);
					if (!NIL_P(vold_looper)) {
						ForgetObject(vold_looper, self);
						ForgetObject(self, vold_looper);
					}
				}
				if (rb_obj_is_kind_of(argv[0], B::Handler::Class())) {
					MemorizeObject(argv[0], self);
					MemorizeObject(self, argv[0]);
				}
				if (rb_obj_is_kind_of(argv[1], B::Looper::Class())) {
					MemorizeObject(argv[1], self);
					MemorizeObject(self, argv[1]);
				}
			}
			return Convert<status_t>::ToValue(status);
		}

		VALUE
		Invoker::rb_set_handler_for_reply(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Invoker::rb_set_handler_for_reply", argc, argv, self);
			VALUE vret = Qnil;
		
			BInvoker *_this = Convert<BInvoker *>::FromValue(self);
		
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)", argc);

			VALUE vhandler = *argv;
			BHandler *handler;

			if (NIL_P(vhandler))
				handler = NULL;
			else if (Convert< BHandler *>::IsConvertable(vhandler))
				handler = Convert<BHandler *>::FromValue(vhandler);
			else
				rb_raise(rb_eTypeError, "worng type of argument");

			BHandler *old_handler = _this->HandlerForReply();
			status_t ret = _this->BInvoker::SetHandlerForReply(handler);
			if (ret == B_OK) {
				if (old_handler) {
					VALUE vold_handler = Convert<BHandler *>::ToValue(old_handler);
					if (!NIL_P(vold_handler)) {
						ForgetObject(vold_handler, self);
						ForgetObject(self, vold_handler);
					}
				}
				if (handler) {
					MemorizeObject(vhandler, self);
					MemorizeObject(self, vhandler);
				}
			}

			return Convert< status_t>::ToValue(ret);
		}

		VALUE
		Invoker::rb_target(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Invoker::rb_target", argc, argv, self);
			VALUE vret = Qnil;
		
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
		Invoker::rb_invoke_kind(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Invoker::rb_invoke_kind", argc, argv, self);
			VALUE vret = Qnil;

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
