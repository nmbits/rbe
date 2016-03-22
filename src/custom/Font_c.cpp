
#define private public
#define protected public

#include <app/Application.h>

#undef private
#undef protected

#include "Font.hpp"

#include "rbe.hpp"
#include "convert.hpp"

namespace rbe
{
	namespace B
	{
		VALUE
		Font::rbe__initialize(int argc, VALUE *argv, VALUE self)
		{
			VALUE vargs[1];
			rb_scan_args(argc, argv, "01", &vargs[0]);

			BFont *_this = NULL;
			switch (argc) {
			case 0:
				if (!be_app)
					rb_raise(rb_eRuntimeError, "be_plain_font was not initialized");
				_this = new BFont();
				break;
			case 1:
				if (Convert<BFont *>::IsConvertable(vargs[0])) {
					BFont *arg0 = Convert<BFont *>::FromValue(vargs[0]);
					_this = new BFont(arg0);
				}
			}

			if (_this == NULL)
				rb_raise(rb_eTypeError, "wrong type of argument");

			PointerOf<BFont>::Class *ptr = static_cast<PointerOf<BFont>::Class *>(_this);
			DATA_PTR(self) = static_cast<void *>(ptr);
			ObjectRegistory::Instance()->Add(self);
			return self;
		}
	}
}
