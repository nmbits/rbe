
#include "memorize.hpp"
#define KEY "__rbe_refs"

namespace rbe {

	class ReferenceCounts
	{
		typedef std::map<VALUE, int> Counter;
		typedef Counter::iterator Iterator;
		typedef std::pair<VALUE, int> Pair;

	private:
		Counter mCounter;

	public:
		ReferenceCounts();
		virtual ~ReferenceCounts();
		int Up(VALUE obj);
		int Down(VALUE obj);
		void Mark();
		static ReferenceCounts *Get(VALUE v, bool alloc = false);
		static void _mark(void *ptr);
		static void _free(void *ptr);
	};

	ReferenceCounts::ReferenceCounts()
		: mCounter()
	{}

	ReferenceCounts::~ReferenceCounts()
	{}

	int ReferenceCounts::Up(VALUE obj)
	{
		Iterator itr = mCounter.find(obj);
		int ret = 0;
		if (itr == mCounter.end()) {
			Pair data(obj, 1);
			mCounter.insert(data);
		} else {
			ret = itr->second;
			itr->second ++;
		}
		return ret;
	}

	int ReferenceCounts::Down(VALUE obj)
	{
		Iterator itr = mCounter.find(obj);
		int ret = -1;
		if (itr != mCounter.end()) {
			ret = itr->second;
			itr->second --;
			if (itr->second == 0)
				mCounter.erase(obj);
		}
		return ret;
	}

	void ReferenceCounts::Mark()
	{
		Iterator itr = mCounter.begin();
		for(; itr != mCounter.end(); itr++)
			rb_gc_mark(itr->first);
	}

	ReferenceCounts *ReferenceCounts::Get(VALUE obj, bool alloc)
	{
		ReferenceCounts *rc = NULL;
		VALUE vset = rb_iv_get(obj, KEY);
		if (vset != Qnil) {
			rc = static_cast<ReferenceCounts *>(DATA_PTR(vset));
		} else if (alloc) {
			rc = new ReferenceCounts();
			vset = Data_Wrap_Struct(rb_cData, ReferenceCounts::_mark, ReferenceCounts::_free, rc);
			rb_iv_set(obj, "__rbe_refs", vset);
		}
		return rc;
	}
	
	void ReferenceCounts::_mark(void *ptr)
	{
		ReferenceCounts *rc = static_cast<ReferenceCounts *>(ptr);
		rc->Mark();
	}

	void ReferenceCounts::_free(void *ptr)
	{
		ReferenceCounts *rc = static_cast<ReferenceCounts *>(ptr);
		delete rc;
	}

	void Memorize(VALUE obj, VALUE ref)
	{
		ReferenceCounts::Get(obj, true)->Up(ref);
	}

	void Forget(VALUE obj, VALUE ref)
	{
		ReferenceCounts *rc = ReferenceCounts::Get(obj, false);
		if (rc)
			rc->Down(ref);
	}
}
