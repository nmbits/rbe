
#include <ruby.h>
#include "Window.hpp"
#include "looper_common.hpp"

namespace rbe
{
	namespace B
	{
		void
		Window::DispatchMessage(BMessage *message, BHandler *handler)
		{
			if (!message)
				return;

			RBE_TRACE("Window::DispatchMessage");
			LooperCommon::DispatchMessageCommon(this, message, handler);			
		}
	}
}
