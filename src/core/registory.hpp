
#ifndef RBE_REGISTORY_HPP
#define RBE_REGISTORY_HPP

#include <ruby.h>
#include <map>
#include <list>

namespace rbe
{
	namespace gc {
		class Ownership0;
	}

	class ObjectRegistory
	{
	private:
		struct Entry
		{
			VALUE value;
			std::list<gc::Ownership0 *> tickets;
			VALUE owner;
		};

		typedef std::map<void *, Entry> registory_t;

		registory_t fRegistory;

		static ObjectRegistory *sInstance;

	public:
		static bool Initialize();
		static ObjectRegistory *Instance();
		~ObjectRegistory();

	private:
		ObjectRegistory();
		void NotifyDeletion(VALUE owner, VALUE value);

	public:
		void Add(VALUE value);
		void Delete(void *ptr);
		VALUE Get(void *ptr);
		void Own(VALUE vowner, gc::Ownership0 *ownership);
		void Release(void *owner, void *target);
		VALUE Owner(VALUE obj);
		void Mark(void *object);
	};
}

#endif
