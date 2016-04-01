
#include "Menu.hpp"
#include "MenuItem.hpp"
#include "deleting.hpp"
#include "debug.hpp"
#include "registory.hpp"

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

	namespace B
	{
		void Menu::rbe__gc_free(void *ptr)
		{
		    RBE_TRACE("BMenu::rb__gc_free");
		    RBE_PRINT(("  ptr = %p\n", ptr));
			ObjectRegistory::Instance()->Delete(ptr);
			PointerOf<BMenu>::Class *obj =
				static_cast<PointerOf<BMenu>::Class *>(ptr);
			BMenu *menu = dynamic_cast<BMenu *>(obj);
			if (!menu)
				debugger("Menu::rbe__gc_free cast failed.");
			menu->RemoveItems(0, menu->CountItems(), false);
			delete menu;
		}
	}
}
