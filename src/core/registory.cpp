
#include "registory.hpp"

namespace rbe
{
	ObjectRegistory *ObjectRegistory::sInstance;

	ObjectRegistory::ObjectRegistory()
		: fRegistory()
	{
	}

	ObjectRegistory::~ObjectRegistory()
	{
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
	ObjectRegistory::Register(VALUE value)
	{
		void *key = DATA_PTR(value);
		registory_t::value_type pair(key, value);
		fRegistory.insert(pair);
	}

	void
	ObjectRegistory::Unregister(void *ptr)
	{
		fRegistory.erase(ptr);
	}

	VALUE
	ObjectRegistory::Get(void *ptr)
	{
		registory_t::const_iterator itr = fRegistory.find(ptr);
		VALUE ret = Qnil;
		if (itr != fRegistory.end())
			ret = itr->second;
		return ret;
	}
}
