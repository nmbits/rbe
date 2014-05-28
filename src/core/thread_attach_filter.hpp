
#ifndef RBE_THREAD_ATTACH_FILTER_HPP
#define RBE_THREAD_ATTACH_FILTER_HPP

#include <app/MessageFilter.h>
#include "ft.h"

namespace rbe
{
	class ThreadAttachFilter : public BMessageFilter
	{
	private:
		ft_handle_t *fFt;
		bool fFilterCalled;

	public:
		ThreadAttachFilter(ft_handle_t *ft);
		virtual ~ThreadAttachFilter();
		virtual filter_result Filter(BMessage *message, BHandler **target);
	};
}

#endif
