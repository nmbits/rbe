
#ifndef RBE_TLS_FLAG_HPP
#define RBE_TLS_FLAG_HPP

#include <support/TLS.h>
#include "thread_local_variable.hpp"

namespace rbe
{
	class ThreadLocalFlag : public ThreadLocalVariable
	{
		// 0: undef
		// 1: false
		// 2: true
	public:
		ThreadLocalFlag()
			: ThreadLocalVariable()
		{
		}

		virtual ~ThreadLocalFlag()
		{
		}

		bool TrySet()
		{
			void *data = this->ThreadLocalVariable::Get();
			if (data == (void *)1 || data == (void *)0) {
				this->ThreadLocalVariable::Set((void *)2);
				return true;
			}
			return false;
		}

		bool TryUnset()
		{
			void *data = this->ThreadLocalVariable::Get();
			if (data == (void *)2 || data == (void *)0) {
				this->ThreadLocalVariable::Set((void *)1);
				return true;
			}
			return false;
		}

		void Set(bool ini)
		{
			this->ThreadLocalVariable::Set((void *)(ini ? 2 : 1));
		}

		bool Get()
		{
			void *data = this->ThreadLocalVariable::Get();
			return (data == (void *)2);
		}
	};
}

#endif
