
#include <ruby.h>

#include "Cursor.hpp"
#include "Message.hpp"

#include "convert.hpp"
#include "registory.hpp"
#include "type_map.hpp"

namespace rbe
{
	namespace B
	{
		VALUE
		Cursor::rb_initialize(int argc, VALUE *argv, VALUE self)
		{
			VALUE varg;
			rb_scan_args(argc, argv, "10", &varg);
			BCursor *_this = NULL;

			if (Convert<const BCursor &>::IsConvertable(varg)) {
				const BCursor& arg0 = Convert<const BCursor&>::FromValue(varg);
				_this = new BCursor(arg0);
			} else if (Convert<BCursorID>::IsConvertable(varg)) {
				BCursorID arg0 = Convert<BCursorID>::FromValue(varg);
				_this = new BCursor(arg0);
			} else if (Convert<BMessage *>::IsConvertable(varg)) {
				BMessage * arg0 = Convert<BMessage *>::FromValue(varg);
				_this = new BCursor(arg0);
			}

			if (_this == NULL)
				rb_raise(rb_eTypeError, "wrong type of argument");

			PointerOf<BCursor>::Class *ptr = static_cast<PointerOf<BCursor>::Class *>(_this);
			DATA_PTR(self) = static_cast<void *>(ptr);
			ObjectRegistory::Instance()->Register(self);
			return self;
		}
	}
}
