
#include "registory.hpp"
#include "ownership.hpp"
#include "debug.hpp"

#include <list>

namespace rbe
{
	ObjectRegistory *ObjectRegistory::sInstance;

	ObjectRegistory::ObjectRegistory()
		: fRegistory()
	{
	}

	ObjectRegistory::~ObjectRegistory()
	{
		sInstance = NULL;
	}

	/* static */ bool
	ObjectRegistory::Initialize()
	{
		if (sInstance)
			return false;
		sInstance = new ObjectRegistory();
		return true;
	}

	/* static */ ObjectRegistory *
	ObjectRegistory::Instance()
	{
		return sInstance;
	}

	void
	ObjectRegistory::Add(VALUE value)
	{
		void *key = DATA_PTR(value);
		Entry e;
		e.value = value;
		e.owner = Qnil;
		registory_t::value_type pair(key, e);
		fRegistory.insert(pair);
	}

	void
	ObjectRegistory::NotifyDeletion(VALUE vowner, VALUE value)
	{
		void *owner = DATA_PTR(vowner);
		registory_t::iterator owner_itr = fRegistory.find(owner);
		if (owner_itr == fRegistory.end())
			return;
		Entry &e = owner_itr->second;
		std::list<gc::Ownership0 *>::iterator list_itr = e.tickets.begin();
		while (list_itr != e.tickets.end()) {
			gc::Ownership0 *ownership = *list_itr;
			if (ownership->Value() == value) {
				ownership->Deleting(owner);
				delete ownership;
				list_itr = owner_itr->second.tickets.erase(list_itr);
			} else {
				list_itr ++;
			}
		}
	}

	void
	ObjectRegistory::Delete(void *ptr)
	{
		registory_t::iterator itr = fRegistory.find(ptr);
		if (itr == fRegistory.end())
			return;
		VALUE value = itr->second.value;
		VALUE vowner = itr->second.owner;
		if (vowner != Qnil)
			NotifyDeletion(vowner, value);
		fRegistory.erase(ptr);
	}

	VALUE
	ObjectRegistory::Get(void *ptr)
	{
		registory_t::const_iterator itr = fRegistory.find(ptr);
		VALUE ret = Qnil;
		if (itr != fRegistory.end())
			ret = itr->second.value;
		return ret;
	}

	void
	ObjectRegistory::Own(VALUE vowner, gc::Ownership0 *ticket)
	{
		RBE_TRACE(("ObjectRegistory::Own"));
		RBE_PRINT(("vowner %lx (%p)\n", vowner, DATA_PTR(vowner)));
		RBE_PRINT(("ownership %lx (%p)\n", ticket->Value(), DATA_PTR(ticket->Value())));

		VALUE value = ticket->Value();
		void *pvalue = DATA_PTR(value);
		void *owner = DATA_PTR(vowner);
		registory_t::iterator value_itr = fRegistory.find(pvalue);
		if (value_itr == fRegistory.end()) {
			RBE_PRINT(("The value is not registered\n"));
			return;
		}
		registory_t::iterator owner_itr = fRegistory.find(owner);
		if (owner_itr == fRegistory.end()) {
			RBE_PRINT(("The owner is not registered\n"));
			return;
		}
		owner_itr->second.tickets.push_back(ticket);
		value_itr->second.owner = vowner;
	}

	void
	ObjectRegistory::Release(void *owner, void *object)
	{
		registory_t::iterator obj_itr = fRegistory.find(object);
		if (obj_itr == fRegistory.end())
			return;
		registory_t::iterator owner_itr = fRegistory.find(owner);
		if (owner_itr == fRegistory.end())
			return;
		std::list<gc::Ownership0 *>::iterator list_itr = owner_itr->second.tickets.begin();
		while (list_itr != owner_itr->second.tickets.end()) {
			gc::Ownership0 *ownership = *list_itr;
			VALUE value = ownership->Value();
			if (DATA_PTR(value) == object)
				list_itr = owner_itr->second.tickets.erase(list_itr);
			else
				list_itr ++;
		}
		obj_itr->second.owner = Qnil;
	}

	void
	ObjectRegistory::Mark(void *object)
	{
		registory_t::iterator itr = fRegistory.find(object);
		std::list<gc::Ownership0 *>::iterator list_itr = itr->second.tickets.begin();
		while (list_itr != itr->second.tickets.end()) {
			(*list_itr)->Mark();
			list_itr ++;
		}
	}
}
