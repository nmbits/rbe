
#ifndef RBE_HPP__
#define RBE_HPP__

#include <ruby.h>

#include "thread_local_variable.hpp"
#include "thread_local_flag.hpp"

enum {
	RBE_MESSAGE_REMOVE_FILTER = 'rbrf',
	RBE_MESSAGE_UBF = 'rbub'
};

namespace rbe
{
	extern VALUE eQuitLooper;
	extern VALUE gModule;
	extern VALUE gMarker;
	extern ThreadLocalFlag *gGVLFlag;

	int FuncallState();
	void SetFuncallState(int);

	void MemorizeObject(VALUE, VALUE);
	void ForgetObject(VALUE, VALUE);

	void init_rbe();
	void init_consts();
	void init_classes(VALUE);
}

#endif
