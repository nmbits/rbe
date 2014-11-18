
#include <ruby.h>

#include "convert.hpp"
#include "lock.hpp"

#include "TextView.hpp"
#include "Font.hpp"
#include "Rect.hpp"
#include "Message.hpp"
#include "convert.hpp"

namespace rbe
{
	namespace B
	{
		VALUE
		TextView::rb_initialize(int argc, VALUE *argv, VALUE self)
		{
			VALUE vargs[7];
			rb_scan_args(argc, argv, "16", &vargs[0], &vargs[1], &vargs[2], &vargs[3], &vargs[4], &vargs[5], &vargs[6]);
			TextView *_this = NULL;
			if (4 <= argc && argc <= 5) {
				if (0 < argc && !Convert<BRect>::IsConvertable(vargs[0])) goto break_0;
				if (1 < argc && !Convert<const char *>::IsConvertable(vargs[1])) goto break_0;
				if (2 < argc && !Convert<BRect>::IsConvertable(vargs[2])) goto break_0;
				if (3 < argc && !Convert<uint32>::IsConvertable(vargs[3])) goto break_0;
				if (4 < argc && !Convert<uint32>::IsConvertable(vargs[4])) goto break_0;
				BRect arg0 = Convert<BRect>::FromValue(vargs[0]);
				const char * arg1 = Convert<const char *>::FromValue(vargs[1]);
				BRect arg2 = Convert<BRect>::FromValue(vargs[2]);
				uint32 arg3 = Convert<uint32>::FromValue(vargs[3]);
				uint32 arg4 = (4 < argc ? Convert<uint32>::FromValue(vargs[4]) : B_WILL_DRAW | B_PULSE_NEEDED);
				_this = new TextView(arg0, arg1, arg2, arg3, arg4);
				goto fin;
			}
		 break_0:
			if (7 == argc) {
				if (0 < argc && !Convert<BRect>::IsConvertable(vargs[0])) goto break_1;
				if (1 < argc && !Convert<const char *>::IsConvertable(vargs[1])) goto break_1;
				if (2 < argc && !Convert<BRect>::IsConvertable(vargs[2])) goto break_1;
				if (3 < argc && !Convert<const BFont *>::IsConvertable(vargs[3])) goto break_1;
				if (4 < argc && !Convert<const rgb_color>::IsConvertable(vargs[4])) goto break_1;
				if (5 < argc && !Convert<uint32>::IsConvertable(vargs[5])) goto break_1;
				if (6 < argc && !Convert<uint32>::IsConvertable(vargs[6])) goto break_1;
				BRect arg0 = Convert<BRect>::FromValue(vargs[0]);
				const char * arg1 = Convert<const char *>::FromValue(vargs[1]);
				BRect arg2 = Convert<BRect>::FromValue(vargs[2]);
				const BFont * arg3 = Convert<const BFont *>::FromValue(vargs[3]);
				const rgb_color arg4 = Convert<const rgb_color>::FromValue(vargs[4]);
				uint32 arg5 = Convert<uint32>::FromValue(vargs[5]);
				uint32 arg6 = Convert<uint32>::FromValue(vargs[6]);
				_this = new TextView(arg0, arg1, arg2, arg3, &arg4, arg5, arg6);
				goto fin;
			}
		 break_1:
			if (1 <= argc && argc <= 2) {
				if (0 < argc && !Convert<const char *>::IsConvertable(vargs[0])) goto break_2;
				if (1 < argc && !Convert<uint32>::IsConvertable(vargs[1])) goto break_2;
				const char * arg0 = Convert<const char *>::FromValue(vargs[0]);
				uint32 arg1 = (1 < argc ? Convert<uint32>::FromValue(vargs[1]) : B_WILL_DRAW | B_PULSE_NEEDED);
				_this = new TextView(arg0, arg1);
				goto fin;
			}
		 break_2:
			if (4 == argc) {
				if (0 < argc && !Convert<const char *>::IsConvertable(vargs[0])) goto break_3;
				if (1 < argc && !Convert<const BFont *>::IsConvertable(vargs[1])) goto break_3;
				if (2 < argc && !Convert<const rgb_color>::IsConvertable(vargs[2])) goto break_3;
				if (3 < argc && !Convert<uint32>::IsConvertable(vargs[3])) goto break_3;
				const char * arg0 = Convert<const char *>::FromValue(vargs[0]);
				const BFont * arg1 = Convert<const BFont *>::FromValue(vargs[1]);
				const rgb_color arg2 = Convert<const rgb_color>::FromValue(vargs[2]);
				uint32 arg3 = Convert<uint32>::FromValue(vargs[3]);
				_this = new TextView(arg0, arg1, &arg2, arg3);
				goto fin;
			}
		 break_3:
			if (1 == argc) {
				if (0 < argc && !Convert<BMessage *>::IsConvertable(vargs[0])) goto break_4;
				BMessage * arg0 = Convert<BMessage *>::FromValue(vargs[0]);
				_this = new TextView(arg0);
				goto fin;
			}
		 break_4:
		 fin:
			if (_this == NULL) {
				rb_raise(rb_eTypeError, "wrong type of argument");
			}
			PointerOf<BTextView>::Class *ptr = static_cast<PointerOf<BTextView>::Class *>(_this);
			DATA_PTR(self) = static_cast<void *>(ptr);
			ObjectRegistory::Instance()->Register(self);
			return self;
		}
	}
}
