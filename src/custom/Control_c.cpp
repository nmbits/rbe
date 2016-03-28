
#include "Control.hpp"
#include "Invoker.hpp"
#include "deleting.hpp"

namespace rbe {
	namespace gc {
		template<>
		void Deleting<BControl, BMessage>(BControl *o, BMessage *t)
		{
			gc::Deleting<BInvoker, BMessage>(o, t);
		}
	}
}
