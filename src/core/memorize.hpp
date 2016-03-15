#ifndef RBE_MEMORIZE_HPP
#define RBE_MEMORIZE_HPP 1

#include <map>
#include <ruby.h>

namespace rbe 
{
	void Memorize(VALUE obj, VALUE ref);
	void Forget(VALUE obj, VALUE ref);
}
#endif
