
#include "rbe.hpp"

#include <TLS.h>

#include <set>
#include <map>
#include <functional>

#include "registory.hpp"
#include "Application.hpp"
#include "debug.hpp"
#include "gvl.hpp"

namespace rbe
{
	VALUE eQuitLooper = Qnil;
	VALUE gModule = Qnil;
	VALUE gMarker = Qnil;

	int32 g_tls_thread_exception = 0;

	VALUE
	rbe_s_be_app_get(VALUE clazz)
	{
		RBE_TRACE_METHOD_CALL("rbe::rb_s_be_app_get", 0, NULL, clazz);
		if (!be_app)
			return Qnil;
		BArchivable *ptr = static_cast<BArchivable *>(be_app);
		return ObjectRegistory::Instance()->Get(static_cast<void *>(ptr));
	}

	VALUE
	rbe_s_debug_set(VALUE clazz, VALUE v)
	{
		if (RTEST(v)) {
			SET_DEBUG_ENABLED(true);
		}
		else {
			SET_DEBUG_ENABLED(false);
		}
		return Qnil;
	}

	VALUE
	rbe_s_debugger(VALUE clazz, VALUE str)
	{
		char *text = StringValueCStr(str);
		std::function<void ()> f = [&]() {
			debugger(text);
		};
		CallWithoutGVL<std::function<void ()>, void> g(f);
		g();
		return Qnil;
	}

	void
	global_mark(void *ptr)
	{
		RBE_TRACE("rbe::global_mark");
		VALUE app = rbe_s_be_app_get(gModule);
		if (!NIL_P(app))
			rb_gc_mark(app);
	}

	void
	global_free(void *ptr)
	{
		RBE_TRACE("rbe::global_free");
		/* do nothing ?? */
	}

	void
	init_rbe()
	{
		RBE_TRACE("rbe::init_rbe");
		gModule = rb_define_module("B");
		rb_define_singleton_method(gModule, "app",
								   RUBY_METHOD_FUNC(rbe_s_be_app_get), 0);
		rb_define_singleton_method(gModule, "DEBUG=",
									RUBY_METHOD_FUNC(rbe_s_debug_set), 1);
		rb_define_singleton_method(gModule, "debugger",
									RUBY_METHOD_FUNC(rbe_s_debugger), 1);
		gMarker = Data_Wrap_Struct(rb_cData, global_mark, global_free, 0);
		rb_global_variable(&gMarker);
		eQuitLooper = rb_define_class_under(gModule, "QuitLooper", rb_eException);
		ObjectRegistory::Initialize();
	}

	int
	ThreadException()
	{
		void * sp = tls_get(g_tls_thread_exception);
		return (int)reinterpret_cast<std::intptr_t>(sp);
	}

	void
	SetThreadException(int s)
	{
		std::intptr_t sp = s;
		tls_set(g_tls_thread_exception, reinterpret_cast<void *>(sp));
	}

	void
	init_tls()
	{
		g_tls_thread_exception = tls_allocate();
	}

	void
	init_consts(VALUE mod);

	void
	init_classes(VALUE mod);
}

extern "C"
void Init_rbe()
{
	SET_DEBUG_ENABLED(false);
	rbe::init_rbe();
	rbe::init_tls();
	rbe::init_classes(rbe::gModule);
	rbe::init_consts(rbe::gModule);
}
