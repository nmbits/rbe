
#include "Menu.hpp"
#include "MenuItem.hpp"
#include "deleting.hpp"


namespace rbe
{
	namespace gc
	{
		template<>
		void Deleting<BMenu, BMenuItem>(BMenu *o, BMenuItem *t)
		{
			o->RemoveItem(t);
		}
	}
}
