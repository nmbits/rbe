
#ifndef RBE_HPP__
#define RBE_HPP__

#include <ruby.h>

enum {
	RBE_MESSAGE_REMOVE_FILTER = 'rbrf',
	RBE_MESSAGE_UBF = 'rbub'
};

namespace rbe
{
	extern VALUE eQuitLooper;
	extern VALUE gModule;
	extern VALUE gMarker;

	int ThreadException();
	void SetThreadException(int);
}

#endif
