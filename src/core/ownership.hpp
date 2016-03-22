
#ifndef RBE_OWNERSHIP_HPP
#define RBE_OWNERSHIP_HPP 1

#include <ruby.h>
#include "type_map.hpp"
#include "convert.hpp"

namespace rbe
{
	namespace gc
	{
		class Ownership0
		{
		private:
			VALUE mValue;

		public:
			Ownership0(VALUE value)
				: mValue(value)
			{}
			virtual ~Ownership0() {}

			virtual void Deleting(void *owner) = 0;
			void Mark() { rb_gc_mark(mValue); }
			VALUE Value() { return mValue; }
		};
	
		template<typename _O, typename _T>
		class Ownership : public Ownership0
		{
		public:
			Ownership(VALUE value)
				: Ownership0(value)
			{}
			virtual ~Ownership() {}

			virtual void Deleting(_O *, _T *) = 0;
			virtual void Deleting(void *vpowner)
			{
				class PointerOf<_O>::Class *powner = (class PointerOf<_O>::Class *) vpowner;
				_O *owner = static_cast<_O *>(powner);
				_T *target = Convert<_T *>::FromValue(Value());
				Deleting(owner, target);
			}
		};
	
		class Ownership_BInvoker_BMessage
			: public Ownership<BInvoker, BMessage>
		{
		public:
			virtual void Deleting(BInvoker *o, BMessage *t)
			{
				o->fMessage = NULL;
			}
		};
	
		class Ownership_BView_BView
			: public Ownership<BView, BView>
		{
		public:
			virtual void Deleting(BView *l, BView *t)
			{
				l->RemoveChild(t);
			}
		};
	
		class Ownership_BView_BFont
			: public Ownership<BView, BFont>
		{
		};
	}
}

#endif
