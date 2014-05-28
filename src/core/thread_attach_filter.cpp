
#include <kernel/OS.h>
#include <support/Debug.h>
#include <app/Looper.h>

#include "rbe.hpp"
#include "ft.h"
#include "looper_common.hpp"

#include "thread_attach_filter.hpp"

#include "debug.hpp"

namespace rbe
{
	ThreadAttachFilter::ThreadAttachFilter(ft_handle_t *ft)
		: fFt(ft)
		, fFilterCalled(false)
		, BMessageFilter(B_ANY_DELIVERY, B_ANY_SOURCE)
	{
	}

	ThreadAttachFilter::~ThreadAttachFilter()
	{
	}

	filter_result
	ThreadAttachFilter::Filter(BMessage *message, BHandler **target)
	{
		RBE_TRACE("ThreadAttachFilter::Filter");
		if (!fFilterCalled) {
			PRINT(("processing attach\n"));
			thread_info tinfo;
			thread_id tid = find_thread(NULL);
			get_thread_info(tid, &tinfo);
			SetFuncallState(0);
			BLooper *looper = static_cast<BLooper *>(*target);
			ft_attach((ft_handle_t *)fFt, tinfo.stack_end, LooperCommon::PostUbfMessage, static_cast<void *>(looper));
			fFilterCalled = true;
		}
		return B_DISPATCH_MESSAGE;
	}
}
