
#include "protect.hpp"

namespace rbe {
	VALUE Protect0::call0(VALUE v)
	{
		Protect0 *p = (Protect0 *)(v);
		p->call1();
		return Qnil;
	}
}
