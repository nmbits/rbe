
#ifndef RBE_THREAD_LOCAL_VARIABLE_HPP
#define RBE_THREAD_LOCAL_VARIABLE_HPP

#include <support/TLS.h>

namespace rbe
{
	class ThreadLocalVariable
	{
	protected:
		const int32 fTls;

	public:
		ThreadLocalVariable()
			: fTls(tls_allocate())
		{
		}

		virtual ~ThreadLocalVariable()
		{
		}

		void *Get()
		{
			return tls_get(fTls);
		}

		void Set(void *data)
		{
			tls_set(fTls, data);
		}
	};
}

#endif
