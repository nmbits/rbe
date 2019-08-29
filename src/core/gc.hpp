#ifndef RBE_GC_HPP
#define RBE_GC_HPP

#include <ruby.h>
#include <map>

namespace rbe
{
	namespace gc
	{
		void Up(VALUE self, VALUE obj);
		void Down(VALUE self, VALUE obj);

		void Memorize(VALUE obj);
		void Forget(void *);
		VALUE GetValue(void *);

		void Init();
	}
}

#endif /* RBE_GC_HPP */
