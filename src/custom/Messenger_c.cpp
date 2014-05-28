
#include <kernel/OS.h>
#include <support/Debug.h>

#include <ruby.h>

#include "rbe.hpp"
#include "funcall.hpp"
#include "convert.hpp"
#include "call_with_gvl.hpp"
#include "call_without_gvl.hpp"
#include "looper_common.hpp"

#include "Looper.hpp"
#include "Handler.hpp"
#include "Messenger.hpp"
#include "Message.hpp"


// 30[s]
#define SEND_MESSAGE_TIMEOUT_MAX 30000000

namespace rbe
{
	namespace B
	{
		namespace MessengerPrivate
		{
			struct SendMessage
			{
				BMessenger *_this;
				BMessage *message;
				BMessage *reply;
				bigtime_t timeout;
				bigtime_t replyTimeout;
				BHandler *handler;
				BMessenger *messenger;
				status_t res;

				void operator()()
				{
					if (reply) {
						res = _this->SendMessage(message, reply, timeout, replyTimeout);
					}
				}
			};
		}

		VALUE
		Messenger::rb_target(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Messenger::rb_target", argc, argv, self);
			VALUE vret = Qnil;

			BMessenger *_this = Convert<BMessenger *>::FromValue(self);

			if (argc)
				rb_raise(rb_eArgError, "wrong number of argument (%1 for 0)");

			BLooper *looper;
			BHandler *handler = _this->BMessenger::Target(&looper);
			if (handler) {
				VALUE vhandler = Convert<BHandler *>::ToValue(handler);
				VALUE vlooper = Convert<BLooper *>::ToValue(looper);
				VALUE vret = rb_ary_new_from_args(2, vhandler, vlooper);
			}

			return vret;
		}

		VALUE
		Messenger::rb_lock_target(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Messenger::rb_lock_target", argc, argv, self);
			VALUE vret = Qnil;

			if (argc > 0)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 0)", argc);

			BMessenger *_this = Convert<BMessenger *>::FromValue(self);
			status_t status = LooperCommon::LockWithTimeout<BMessenger>(_this, B_INFINITE_TIMEOUT);
			if (status == B_OK)
				return Qtrue;
			return Qfalse;
		}

		VALUE
		Messenger::rb_lock_target_with_timeout(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Messenger::rb_lock_target_with_timeout", argc, argv, self);

			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 1)", argc);

			BMessenger *_this = Convert<BMessenger *>::FromValue(self);
			bigtime_t timeout = Convert<bigtime_t>::FromValue(*argv);

			status_t status = LooperCommon::LockWithTimeout<BMessenger>(_this, timeout);

			return Convert<status_t>::ToValue(status);
		}

		VALUE
		Messenger::rb_initialize(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Messenger::rb_initialize", argc, argv, self);

			VALUE vargs[2];
			rb_scan_args(argc, argv, "11", &vargs[0], &vargs[1]);
			BMessenger *_this = NULL;
			status_t status = B_OK;
			bool local = true;

			if (Convert<BHandler *>::IsConvertable(vargs[0])) {
				BLooper *looper = NULL;
				if (argc > 1 && Convert<BLooper *>::IsConvertable(vargs[1]))
					looper = Convert<BLooper *>::FromValue(vargs[1]);
				BHandler *handler = Convert<BHandler *>::FromValue(vargs[0]);
				_this = new BMessenger(handler, looper, &status);
			} else if (Convert<const char *>::IsConvertable(vargs[0])) {
				team_id tid = -1;
				local = false;
				if (argc > 1 && Convert<team_id>::IsConvertable(vargs[1]))
					tid = Convert<team_id>::FromValue(vargs[1]);
				const char *c = Convert<const char *>::FromValue(vargs[0]);
				_this = new BMessenger(c, tid, &status);
			}

			if (!_this)
				rb_raise(rb_eTypeError, "wrong type of arguments");

			PointerOf<BMessenger>::Class *ptr = _this;
			DATA_PTR(self) = static_cast<void *>(ptr);
			ObjectRegistory::Instance()->Register(self);

			switch (status) {
			case B_BAD_VALUE:
				if (local)
					rb_raise(rb_eArgError, "invalid handler or looper");
				else
					rb_raise(rb_eArgError, "invalid signature");
				break;
			case B_BAD_TEAM_ID:
				rb_raise(rb_eArgError, "invalid team");
				break;
			case B_MISMATCHED_VALUES:
				if (local)
					rb_raise(rb_eArgError, "handler is associated with a BLooper other than looper");
				else
					rb_raise(rb_eArgError, "team isn't a signature app");
				break;
			case B_BAD_HANDLER:
				rb_raise(rb_eArgError, "handler isn't associated with a BLooper");
			}
			return self;
		}

		// messenger.send_message(msg [, timeout] [, replyTimeout]) do |reply| ...  end
		// messenger.send_message(msg [, handler] [, timeout])
		// messenger.send_message(msg [, messenger] [, timeout])
		VALUE
		Messenger::rb_send_message(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("Messenger::rb_send_message", argc, argv, self);
			VALUE vret = Qnil;
		
			BMessenger *msgr = Convert<BMessenger *>::FromValue(self);

			VALUE vargs[3];
			rb_scan_args(argc, argv, "12", &vargs[0], &vargs[1], &vargs[2]);

			BMessage *message;
			BMessage send;

			if (Convert<uint32>::IsConvertable(vargs[0])) {
				send.what = Convert<uint32>::FromValue(vargs[0]);
				message = &send;
			} else if (Convert<BMessage *>::IsConvertable(vargs[0])){
				message = Convert<BMessage *>::FromValue(vargs[0]);
			} else {
				rb_raise(rb_eTypeError, "arg 0 should be an Integer or a BMessage");
				return Qnil;
			}

			MessengerPrivate::SendMessage f = { ._this = msgr, .message = message };
			CallWithoutGVL<MessengerPrivate::SendMessage, void> g(f);

			if (rb_block_given_p()) {
				bigtime_t timeouts[2] = { SEND_MESSAGE_TIMEOUT_MAX, SEND_MESSAGE_TIMEOUT_MAX };
				for (int i = 1; i < argc; i++) {
					if (!Convert<bigtime_t>::IsConvertable(vargs[i])) {
						rb_raise(rb_eTypeError, "arg %d should be an Integer", i);
						return Qnil;
					}
					timeouts[i - 1] = Convert<bigtime_t>::FromValue(vargs[i]);
					if (timeouts[i - 1] > SEND_MESSAGE_TIMEOUT_MAX) {
						rb_raise(rb_eRangeError, "timeout and reply timeout should be less than 30sec (restriction of this library)");
						return Qnil;
					}
				}
				BMessage *reply = new BMessage();
				VALUE vreply = Message::Wrap(reply);
				f.reply = reply;
				f.timeout = timeouts[0];
				f.replyTimeout = timeouts[1];
				g();
				rb_thread_check_ints();
				rb_yield(vreply);
			} else {
				f.reply = NULL;
				if (argc > 1) {
					if (Convert<BHandler *>::IsConvertable(vargs[1])) {
						f.handler = Convert<BHandler *>::FromValue(vargs[1]);
					} else if (Convert<BMessenger *>::IsConvertable(vargs[1])) {
						f.handler = NULL;
						f.messenger = Convert<BMessenger *>::FromValue(vargs[1]);
					} else if (NIL_P(vargs[1])) {
						f.handler = NULL;
						f.messenger = NULL;
					} else {
						rb_raise(rb_eTypeError, "the first argument should be a BHandler or a BMessenger");
						return Qnil;
					}
				}
				f.timeout = SEND_MESSAGE_TIMEOUT_MAX;
				if (argc > 2) {
					if (Convert<bigtime_t>::IsConvertable(vargs[2])) {
						f.timeout = Convert<bigtime_t>::FromValue(vargs[2]);
						if (f.timeout > SEND_MESSAGE_TIMEOUT_MAX) {
							rb_raise(rb_eRangeError, "timeout and reply timeout should be less than 30sec (restriction of this library)");
							return Qnil;
						}
					}
				}
				g();
				rb_thread_check_ints();
			}
			return Convert<status_t>::ToValue(f.res);
		}
	}
}
