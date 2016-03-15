#ifndef RBE_CALL_SCRIPT_HPP
#define RBE_CALL_SCRIPT_HPP 1

#include "interface.hpp"
#include "gvl.hpp"
#include "protect.hpp"

#include <functional>

namespace rbe {

	template<typename _Recv, typename _Interface>
	int CallScript(_Recv *recv, const char *name, _Interface &intf)
	{
		std::function<void ()> f = [&]() {
			VALUE vargs[intf.NumIn()];
			intf.ArgsToValues(vargs);
			ID id = rb_intern(name);
			VALUE self = Convert<_Recv *>::ToValue(recv);
			VALUE result = rb_funcall2(self, id, intf.NumIn(), vargs);
			VALUE *presult;
			switch (intf.NumOut()) {
			case 0:
				break;
			case 1:
				presult = &result;
				intf.SetResults(presult);
				break;
			default:
				if (TYPE(result) != T_ARRAY ||
					RARRAY_LEN(result) != intf.NumOut()) {
					rb_raise(rb_eRangeError,
						"unexpected number of returned values (%ld for %d)",
						RARRAY_LEN(result), intf.NumOut());
				}
				presult = RARRAY_PTR(result);
				intf.SetResults(presult);
			}
		};
		return ProtectedCallWithGVL(f);
	}

	template<typename _Recv>
	int CallScript(_Recv *recv, const char *name)
	{
		std::function<void ()> f = [&]() {
			ID id = rb_intern(name);
			VALUE self = Convert<_Recv *>::ToValue(recv);
			rb_funcall2(self, id, 0, NULL);
		};
		return ProtectedCallWithGVL(f);
	}
}
#endif
