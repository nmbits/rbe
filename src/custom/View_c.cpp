
#include "View.hpp"
#include "Message.hpp"
#include "Rect.hpp"

#include "rbe.hpp"
#include "protect.hpp"
#include "convert.hpp"
#include "gvl.hpp"
#include "lock.hpp"
#include "deleting.hpp"

#define protected public
#define private public
#include <interface/ScrollView.h>
#undef protected
#undef private

#include <functional>

namespace rbe
{
	namespace gc
	{
		template<>
		void Deleting<BView, BView>(BView *o, BView *t)
		{
			o->RemoveChild(t);
		}

		template<>
		void Deleting<BView, BFont>(BView *o, BFont *t)
		{
			// TODO
		}
	}

	// static inline VALUE
	// rbe_key_up_down(int argc, VALUE *argv, VALUE self, bool up)
	// {
	// 	if (argc != 1)
	// 		rb_raise(rb_eArgError, "wrong number of argument (%d for 1)", argc);
	
	// 	BView *_this = Convert<BView *>::FromValue(self);
	// 	Check_Type(*argv, T_STRING);
	// 	const char *str = RSTRING_PTR(*argv);
	// 	int32 len = RSTRING_LEN(*argv);
	// 	auto f = [&]() {
	// 		if (up)
	// 			_this->BView::KeyUp(str, len);
	// 		else
	// 			_this->BView::KeyDown(str, len);
	// 	};
	// 	CallWitoutGVL<std::function<void ()> > g(f);
	// 	g();
	// 	rb_thread_check_intr();
	// 	if (ThreadException() > 0)
	// 		rb_jump_tag(ThreadException());
	// 	return Qnil;
	// }
	
	namespace B
	{
		// void
		// View::TargetedByScrollViewST(BView *_this, BScrollView *scroll_view)
		// {
		//	return;
		// }

		// void
		// View::KeyDownST(BView *_this, const char *str, int32 len)
		// {
		// 	RBE_TRACE("View::KeyDownST");
		// 	if (ThreadException())
		// 		return;
		// 	auto f = [&]() {
		// 		VALUE str = rb_str_new(str, len);
		// 		VALUE self = Convert<BView *>::ToValue(_this);
		// 		rb_funcall(self, rb_intern("key_down"), 1, str);
		// 	};
		// 	status_t status = ProtectedCallWithGVL(f);
		// 	SetThreadException(status);
		// }

		// void
		// View::KeyUpST(BView *_this, const char * arg0, int32 arg1)
		// {
		// 	RBE_TRACE("View::KeyDownST");
		// 	if (ThreadException())
		// 		return;
		// 	auto f = [&]() {
		// 		VALUE str = rb_str_new(str, len);
		// 		VALUE self = Convert<BView *>::ToValue(_this);
		// 		rb_funcall(self, rb_intern("key_up"), 1, str);
		// 	};
		// 	status_t status = ProtectedCallWithGVL(f);
		// 	SetThreadException(status);
		// }

		// VALUE
		// View::rbe_key_down(int argc, VALUE *argv, VALUE self)
		// {
		// 	RBE_TRACE_METHOD_CALL("View::rbe_key_down", argc, argv, self);
		// 	return rbe_key_up_down(argc, argv, self, false);
		// }

		// VALUE
		// View::rbe_key_up(int argc, VALUE *argv, VALUE self)
		// {
		// 	RBE_TRACE_METHOD_CALL("View::rbe_key_down", argc, argv, self);
		// 	return rbe_key_up_down(argc, argv, self, true);
		// }

		// VALUE
		// View::rbe_targeted_by_scroll_view(int argc, VALUE *argv, VALUE self)
		// {
		// 	RBE_TRACE_METHOD_CALL("View::rbe_targeted_by_scroll_view", argc, argv, self);
		// 	return Qnil;
		// }
	}
}
