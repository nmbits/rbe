
#include "protect.hpp"

namespace rbe
{
	/* static */ VALUE
	ProtectBase::_protect_0(VALUE v)
	{
		ProtectBase *prot = reinterpret_cast<ProtectBase *>(v);
		return prot->Exec();
	}
}
