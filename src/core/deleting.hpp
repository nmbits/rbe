#ifndef RBE_DELETING_HPP
#define RBE_DELETING_HPP 1

namespace rbe {
	namespace gc {
		template <typename _OWNER, typename _TARGET>
		void Deleting(_OWNER *o, _TARGET *t);
	}
}

#endif
