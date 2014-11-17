
#include "Font.hpp"

#include "rbe.hpp"
#include "convert.hpp"

namespace rbe
{
	namespace B
	{
		VALUE
		Font::rb_initialize(int argc, VALUE *argv, VALUE self)
		{
			VALUE vargs[1];
			rb_scan_args(argc, argv, "01", &vargs[0]);

			BFont *_this = NULL;
			switch (argc) {
			case 0:
				_this = new BFont();
				break;
			case 1:
				if (Convert<BFont>::IsConvertable(vargs[0])) {
					BFont arg0 = Convert<BFont>::FromValue(vargs[0]);
					_this = new BFont(arg0);
				}
			}

			if (_this == NULL) {
				rb_raise(rb_eTypeError, "wrong type of argument");
			}
			PointerOf<BFont>::Class *ptr = static_cast<PointerOf<BFont>::Class *>(_this);
			DATA_PTR(self) = static_cast<void *>(ptr);
			ObjectRegistory::Instance()->Register(self);
			return self;
		}
	}
}
