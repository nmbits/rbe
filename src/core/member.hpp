#ifndef RBE_MEMBER_OF_HPP
#define RBE_MEMBER_OF_HPP 1

#define private public
#define protected public
#include <app/Handler.h>
#include <app/Message.h>
#include <app/Invoker.h>
#include <interface/View.h>
#include <interface/Window.h>
#undef private
#undef protected

namespace rbe
{
	namespace Util {
		static inline bool IsMemberOf(BMessage *c, BInvoker *i)
		{
			if (i->Message() == c)
				return true;
			return false;
		}

		static inline bool IsMemberOf(BView *c, BWindow *p)
		{
			if (c->Window() == p)
				return true;
			return false;
		}

		static inline bool IsMemberOf(BHandler *c, BLooper *p)
		{
			if (c->Looper() == p)
				return true;
			return false;
		}

		static inline bool IsMemberOf(BView *c, BView *p)
		{
			return (c->Parent() == p ? true : false);
		}
	}
}

#endif
