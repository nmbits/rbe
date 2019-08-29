
#include "gc.hpp"
#include "debug.hpp"

#define RBE_GC_REF_IV "__rbe_gc_refs"

namespace rbe
{
	namespace gc
	{
		typedef std::map<void *, VALUE> ReverseMap;
		static ReverseMap *rmap;

		void
		Init()
		{
			rmap = new ReverseMap();
		}

		void
		Memorize(VALUE obj_)
		{
			void *ptr = DATA_PTR(obj_);
			ReverseMap::iterator itr = rmap->find(ptr);
			if (itr != rmap->end())
				rb_bug("gc::Memorize(): %p already exists.", ptr);
			std::pair<void *, VALUE> ent(ptr, obj_);
			rmap->insert(ent);
		}

		void
		Forget(void *ptr)
		{
			if(rmap->erase(ptr) != 1)
				rb_bug("gc::Forget(): %p does not exist.", ptr);
		}

		VALUE
		GetValue(void *ptr)
		{
			ReverseMap::iterator itr = rmap->find(ptr);
			if (itr == rmap->end())
				return Qnil;
			return itr->second;
		}

		static void
		Up0(VALUE from_, VALUE to_)
		{
			RBE_TRACE("gc::Up0");
			RBE_PRINT(("  from_: %lu, to_: %lu\n", from_, to_));
			VALUE hash_ = rb_iv_get(from_, RBE_GC_REF_IV);
			RBE_PRINT(("  hash_: %lu\n", hash_));
			if (NIL_P(hash_)) {
				hash_ = rb_hash_new();
				rb_iv_set(from_, RBE_GC_REF_IV, hash_);
			}
			RBE_PRINT(("  hash_: %lu\n", hash_));
			VALUE cnt_ = rb_hash_aref(hash_, to_);
			int cnt = (NIL_P(cnt_) ? 0 : NUM2INT(cnt_)) + 1;
			rb_hash_aset(hash_, to_, INT2NUM(cnt));
		}

		void
		Up(VALUE self_, VALUE obj_)
		{
			RBE_TRACE("gc::Up");
			Up0(self_, obj_);
			// Up0(obj_, self_);
		}

		static void
		Down0(VALUE from_, VALUE to_)
		{
			RBE_TRACE("gc::Down0");
			VALUE hash_ = rb_iv_get(from_, RBE_GC_REF_IV);
			if (NIL_P(hash_))
				return;
			VALUE cnt_ = rb_hash_aref(hash_, to_);
			int cnt = (NIL_P(cnt_) ? 1 : NUM2INT(cnt_)) - 1;
			if (cnt == 0)
				rb_hash_delete(hash_, to_);
			else
				rb_hash_aset(hash_, to_, INT2NUM(cnt));
		}

		void
		Down(VALUE self_, VALUE obj_)
		{
			RBE_TRACE("gc::Down");
			Down0(self_, obj_);
			// Down0(obj_, self_);
		}
	}
}
