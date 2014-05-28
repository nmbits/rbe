
#ifndef RBE_REGISTORY_HPP
#define RBE_REGISTORY_HPP

#include <ruby.h>
#include <map>

namespace rbe
{
	class ObjectRegistory
	{
	private:
		typedef std::map<void *, VALUE> registory_t;

		registory_t fRegistory;

		static ObjectRegistory *sInstance;

	public:
		static bool Initialize();
		static ObjectRegistory *Instance();
		~ObjectRegistory();

	private:
		ObjectRegistory();

	public:
		void Register(VALUE value);
		void Unregister(void *ptr);
		VALUE Get(void *ptr);
	};
}

#endif
