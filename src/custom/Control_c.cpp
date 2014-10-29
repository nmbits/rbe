
#include "Control.hpp"
#include "View.hpp"

namespace rbe
{
	namespace Hook
	{
		namespace Control
		{
			void
			GetPreferredSize(BControl *_this, float *arg0, float *arg1)
			{
				Hook::View::GetPreferredSize(_this, arg0, arg1);
			}
		}
	}

	namespace B
	{
		VALUE
		Control::rb_get_preferred_size(int argc, VALUE *argv, VALUE self)
		{
			return View::rb_get_preferred_size(argc, argv, self);
		}
	}
}
