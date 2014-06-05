
#include "View.hpp"
#include "Message.hpp"
#include "Rect.hpp"

#include "rbe.hpp"
#include "protect.hpp"
#include "funcall.hpp"
#include "convert.hpp"
#include "call_with_gvl.hpp"
#include "call_without_gvl.hpp"
#include "looper_common.hpp"

#include <interface/ScrollView.h>

namespace rbe
{
	namespace B
	{
		namespace ViewPrivate
		{
			struct CallKeyUpDown
			{
				BView *view;
				const char *string;
				int length;
				bool up;
				void operator()()
				{
					VALUE str = rb_str_new(string, length);
					VALUE self = Convert<BView *>::ToValue(view);
					if (up)
						rb_funcall(self, rb_intern("key_up"), 1, str);
					else
						rb_funcall(self, rb_intern("key_down"), 1, str);
				}
			};

			VALUE
			rb_key_up_down(int argc, VALUE *argv, VALUE self, bool up)
			{
				if (argc != 1)
					rb_raise(rb_eArgError, "wrong number of argument (%d for 1)", argc);
	
				BView *_this = Convert<BView *>::FromValue(self);
				Check_Type(*argv, T_STRING);
				const char *str = RSTRING_PTR(*argv);
				int32 len = RSTRING_LEN(*argv);
				if (up)
					_this->BView::KeyUp(str, len);
				else
					_this->BView::KeyDown(str, len);
				if (FuncallState() > 0)
					rb_jump_tag(FuncallState());
				return Qnil;
			}

			struct
			CallGetPreferredSize
			{
				BView *view;
				float *arg0;
				float *arg1;
				void operator()()
				{
					VALUE self = Convert<BView *>::ToValue(view);
					VALUE res = rb_funcall(self, rb_intern("get_preferred_size"), 0);
					Check_Type(res, T_ARRAY);
					if (RARRAY_LEN(res) != 2)
						rb_raise(rb_eRangeError, "return value should be an array of 2 floats");
					float res0 = NUM2DBL(RARRAY_AREF(res, 0));
					float res1 = NUM2DBL(RARRAY_AREF(res, 1));
					*arg0 = res0;
					*arg1 = res1;
				}
			};
		}

		VALUE
		View::rb_initialize(int argc, VALUE *argv, VALUE self)
		{
			VALUE vargs[4];
			rb_scan_args(argc, argv, "13", &vargs[0], &vargs[1], &vargs[2], &vargs[3]);
			View *_this = NULL;

		 break_0:
			if (4 == argc) {
				if (0 < argc && !Convert<BRect>::IsConvertable(vargs[0])) goto break_1;
				if (1 < argc && !Convert<const char *>::IsConvertable(vargs[1])) goto break_1;
				if (2 < argc && !Convert<uint32>::IsConvertable(vargs[2])) goto break_1;
				if (3 < argc && !Convert<uint32>::IsConvertable(vargs[3])) goto break_1;
				BRect arg0 = Convert<BRect>::FromValue(vargs[0]);
				const char * arg1 = Convert<const char *>::FromValue(vargs[1]);
				uint32 arg2 = Convert<uint32>::FromValue(vargs[2]);
				uint32 arg3 = Convert<uint32>::FromValue(vargs[3]);
				_this = new View(arg0, arg1, arg2, arg3);
				goto fin;
			}
		 break_1:
			if (1 == argc) {
				if (0 < argc && !Convert<BMessage *>::IsConvertable(vargs[0])) goto break_2;
				BMessage * arg0 = Convert<BMessage *>::FromValue(vargs[0]);
				_this = new View(arg0);
				goto fin;
			}
		 break_2:
		 fin:
			if (_this == NULL) {
				rb_raise(rb_eTypeError, "wrong type of argument");
			}
			PointerOf<BView>::Class *ptr = static_cast<PointerOf<BView>::Class *>(_this);
			DATA_PTR(self) = static_cast<void *>(ptr);
			ObjectRegistory::Instance()->Register(self);
			return self;
		}

		void
		View::KeyDown(const char * arg0, int32 arg1)
		{
		    RBE_TRACE("View::KeyDown");
		    if (FuncallState() != 0)
		        return;

			ViewPrivate::CallKeyUpDown f = { this, arg0, arg1, false };
			Protect<ViewPrivate::CallKeyUpDown> p(f);
			CallWithGVL<Protect<ViewPrivate::CallKeyUpDown> > g(p);
			g();
			SetFuncallState(p.State());
		}

		void
		View::KeyUp(const char * arg0, int32 arg1)
		{
		    RBE_TRACE("View::KeyDown");
		    if (FuncallState() != 0)
		        return;

			ViewPrivate::CallKeyUpDown f = { this, arg0, arg1, true };
			Protect<ViewPrivate::CallKeyUpDown> p(f);
			CallWithGVL<Protect<ViewPrivate::CallKeyUpDown> > g(p);
			g();
			SetFuncallState(p.State());
		}

		VALUE
		View::rb_key_down(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("View::rb_key_down", argc, argv, self);
			return ViewPrivate::rb_key_up_down(argc, argv, self, false);
		}

		VALUE
		View::rb_key_up(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("View::rb_key_down", argc, argv, self);
			return ViewPrivate::rb_key_up_down(argc, argv, self, true);
		}

		void
		View::GetPreferredSize(float * arg0, float * arg1)
		{
			RBE_TRACE("View::GetPreferredSize");
			if (FuncallState() != 0)
				return;
		
			ViewPrivate::CallGetPreferredSize f = { this, arg0, arg1 };
			Protect<ViewPrivate::CallGetPreferredSize> p(f);
			CallWithGVL<Protect<ViewPrivate::CallGetPreferredSize> > g(p);
			g();
			SetFuncallState(p.State());
		}

		VALUE
		View::rb_get_preferred_size(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("View::rb_get_preferred_size", argc, argv, self);

			BView *_this = Convert<BView *>::FromValue(self);
			float arg0;
			float arg1;
			_this->BView::GetPreferredSize(&arg0, &arg1);
			VALUE ary = rb_ary_new();
			rb_ary_push(ary, DBL2NUM(arg0));
			rb_ary_push(ary, DBL2NUM(arg1));
			return ary;
		}

		void
		View::TargetedByScrollView(BScrollView *scroll_view)
		{
			return;
		}

		VALUE
		View::rb_targeted_by_scroll_view(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("View::rb_targeted_by_scroll_view", argc, argv, self);
			return Qnil;
		}

		VALUE
		View::rb_convert_to_parent(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("View::rb_convert_to_parent", argc, argv, self);
			VALUE vret = Qnil;
		
			BView *_this = Convert<BView *>::FromValue(self);
		
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 1)", argc);
		
			if (Convert<BPoint>::IsConvertable(*argv)) {
				BPoint point = Convert<BPoint>::FromValue(*argv);
				BPoint rpoint = _this->BView::ConvertToParent(point);
				return Convert<BPoint>::ToValue(point);
			}

			if (Convert<BRect>::IsConvertable(*argv)) {
				BRect rect = Convert<BRect>::FromValue(*argv);
				BRect rrect = _this->BView::ConvertToParent(rect);
				return Convert<BRect>::ToValue(rect);
			}

			rb_raise(rb_eTypeError, "B::Point or B::Rect expected");
			return Qnil;		
		}

		VALUE
		View::rb_convert_from_parent(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("View::rb_convert_from_parent", argc, argv, self);
			VALUE vret = Qnil;
		
			BView *_this = Convert<BView *>::FromValue(self);
		
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 1)", argc);
		
			if (Convert<BPoint>::IsConvertable(*argv)) {
				BPoint point = Convert<BPoint>::FromValue(*argv);
				BPoint rpoint = _this->BView::ConvertFromParent(point);
				return Convert<BPoint>::ToValue(point);
			}

			if (Convert<BRect>::IsConvertable(*argv)) {
				BRect rect = Convert<BRect>::FromValue(*argv);
				BRect rrect = _this->BView::ConvertFromParent(rect);
				return Convert<BRect>::ToValue(rect);
			}

			rb_raise(rb_eTypeError, "B::Point or B::Rect expected");
			return Qnil;		
		}

		VALUE
		View::rb_convert_to_screen(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("View::rb_convert_to_screen", argc, argv, self);
			VALUE vret = Qnil;
		
			BView *_this = Convert<BView *>::FromValue(self);
		
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 1)", argc);
		
			if (Convert<BPoint>::IsConvertable(*argv)) {
				BPoint point = Convert<BPoint>::FromValue(*argv);
				BPoint rpoint = _this->BView::ConvertToScreen(point);
				return Convert<BPoint>::ToValue(point);
			}

			if (Convert<BRect>::IsConvertable(*argv)) {
				BRect rect = Convert<BRect>::FromValue(*argv);
				BRect rrect = _this->BView::ConvertToScreen(rect);
				return Convert<BRect>::ToValue(rect);
			}

			rb_raise(rb_eTypeError, "B::Point or B::Rect expected");
			return Qnil;		
		}

		VALUE
		View::rb_convert_from_screen(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("View::rb_convert_from_screen", argc, argv, self);
			VALUE vret = Qnil;
		
			BView *_this = Convert<BView *>::FromValue(self);
		
			if (argc != 1)
				rb_raise(rb_eArgError, "wrong number of argument (%d for 1)", argc);
		
			if (Convert<BPoint>::IsConvertable(*argv)) {
				BPoint point = Convert<BPoint>::FromValue(*argv);
				BPoint rpoint = _this->BView::ConvertFromScreen(point);
				return Convert<BPoint>::ToValue(point);
			}

			if (Convert<BRect>::IsConvertable(*argv)) {
				BRect rect = Convert<BRect>::FromValue(*argv);
				BRect rrect = _this->BView::ConvertFromScreen(rect);
				return Convert<BRect>::ToValue(rect);
			}

			rb_raise(rb_eTypeError, "B::Point or B::Rect expected");
			return Qnil;		
		}

		VALUE
		View::rb_invalidate(int argc, VALUE *argv, VALUE self)
		{
			RBE_TRACE_METHOD_CALL("View::rb_invalidate", argc, argv, self);

			BView *_this = Convert<BView *>::FromValue(self);
			_this->BView::Invalidate();
			if (FuncallState() > 0)
				rb_jump_tag(FuncallState());
			return Qnil;
		}
	}
}
