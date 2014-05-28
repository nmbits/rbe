
#ifndef RBE_DEBUG_HPP
#define RBE_DEBUG_HPP

#if DEBUG

#include <support/Debug.h>
#include <kernel/OS.h>

#include <ruby.h>

namespace rbe {
	namespace debug {

		class TracePrinter
		{
		private:
			const char *fFunc;

		public:
			TracePrinter(const char *func, int argc, VALUE *argv, VALUE self)
				: fFunc(func)
			{
				PRINT(("(%d)\t", find_thread(NULL)));
				PRINT(("%s start: self = 0x%x(%s):%p, argc = %d", func, self, rb_obj_classname(self), DATA_PTR(self), argc));
				if (argc > 0) {
					PRINT((" argv = "));
					for (int i = 0; i < argc; i++) {
						PRINT(("%x (%s) ", argv[i], rb_obj_classname(argv[i])));
					}
				}
				PRINT(("\n"));
			}

			TracePrinter(const char *func)
				: fFunc(func)
			{
				PRINT(("(%d)\t", find_thread(NULL)));
				PRINT(("%s start\n", func));
			}

			~TracePrinter()
			{
				PRINT(("(%d)\t", find_thread(NULL)));
				PRINT(("%s end\n", fFunc));
			}
		};
	}
}

#define RBE_TRACE_METHOD_CALL(func, argc, argv, self) \
	::rbe::debug::TracePrinter __rbe_trace_mcall__((func), (argc), (argv), (self))
#define RBE_TRACE(func)							\
	::rbe::debug::TracePrinter __rbe_trace__((func))
#define RBE_PRINT(m) \
	PRINT(("(%d)\t", find_thread(NULL))); PRINT(m)
#else
#define RBE_TRACE_METHOD_CALL(func, argc, argv, self) \
	do{}while(false)
#define RBE_TRACE(func) \
	do{}while(false)
#define RBE_PRINT(m) \
	do{}while(false)
#endif

#endif
