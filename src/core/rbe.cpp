
#include "rbe.hpp"

#define private public
#define protected public

#include <app/AppDefs.h>
#include <app/Clipboard.h>
#include <app/Handler.h>
#include <app/Message.h>

#include <SupportDefs.h>
#include <support/Debug.h>

#include <interface/Window.h>
#include <interface/View.h>

#include <TLS.h>

#undef private
#undef protected

#include <set>
#include <map>

#include "registory.hpp"
#include "Application.hpp"
#include "call_with_gvl.hpp"
#include "call_without_gvl.hpp"
#include "debug.hpp"

#define RBE_CONST_ULONG(name)	\
	rb_define_const(gModule, #name, ULONG2NUM(B_ ## name))

#define RBE_CONST_STR(name)		\
	rb_define_const(gModule, #name, rb_str_new2(B_ ## name))

namespace rbe
{
	VALUE eQuitLooper = Qnil;
	VALUE gModule = Qnil;
	VALUE gMarker = Qnil;
	ThreadLocalFlag *gGVLFlag = NULL;
	ThreadLocalVariable *gFuncallState = NULL;

	void
	init_consts()
	{
		// <app/AppDefs.h>
		RBE_TRACE("rbe::init_consts");
		RBE_CONST_ULONG(ABOUT_REQUESTED);
		RBE_CONST_ULONG(WINDOW_ACTIVATED);
		RBE_CONST_ULONG(APP_ACTIVATED);
		RBE_CONST_ULONG(ARGV_RECEIVED);
		RBE_CONST_ULONG(QUIT_REQUESTED);
		RBE_CONST_ULONG(CANCEL);
		RBE_CONST_ULONG(INVALIDATE);
		RBE_CONST_ULONG(KEY_DOWN);
		RBE_CONST_ULONG(KEY_UP);
		RBE_CONST_ULONG(UNMAPPED_KEY_DOWN);
		RBE_CONST_ULONG(UNMAPPED_KEY_UP);
		RBE_CONST_ULONG(LAYOUT_WINDOW);
		RBE_CONST_ULONG(MODIFIERS_CHANGED);
		RBE_CONST_ULONG(MINIMIZE);
		RBE_CONST_ULONG(MOUSE_DOWN);
		RBE_CONST_ULONG(MOUSE_MOVED);
		RBE_CONST_ULONG(MOUSE_ENTER_EXIT);
		RBE_CONST_ULONG(MOUSE_IDLE);
		RBE_CONST_ULONG(MOUSE_UP);
		RBE_CONST_ULONG(MOUSE_WHEEL_CHANGED);
		RBE_CONST_ULONG(PULSE);
		RBE_CONST_ULONG(READY_TO_RUN);
		RBE_CONST_ULONG(REFS_RECEIVED);
		RBE_CONST_ULONG(RELEASE_OVERLAY_LOCK);
		RBE_CONST_ULONG(ACQUIRE_OVERLAY_LOCK);
		RBE_CONST_ULONG(SCREEN_CHANGED);
		RBE_CONST_ULONG(VALUE_CHANGED);
		RBE_CONST_ULONG(TRANSLATOR_ADDED);
		RBE_CONST_ULONG(TRANSLATOR_REMOVED);
		RBE_CONST_ULONG(VIEW_MOVED);
		RBE_CONST_ULONG(VIEW_RESIZED);
		RBE_CONST_ULONG(WINDOW_MOVED);
		RBE_CONST_ULONG(WINDOW_RESIZED);
		RBE_CONST_ULONG(WORKSPACES_CHANGED);
		RBE_CONST_ULONG(WORKSPACE_ACTIVATED);
		RBE_CONST_ULONG(ZOOM);
		RBE_CONST_ULONG(SET_PROPERTY);
		RBE_CONST_ULONG(GET_PROPERTY);
		RBE_CONST_ULONG(CREATE_PROPERTY);
		RBE_CONST_ULONG(DELETE_PROPERTY);
		RBE_CONST_ULONG(COUNT_PROPERTIES);
		RBE_CONST_ULONG(EXECUTE_PROPERTY);
		RBE_CONST_ULONG(GET_SUPPORTED_SUITES);
		RBE_CONST_ULONG(UNDO);
		RBE_CONST_ULONG(REDO);
		RBE_CONST_ULONG(CUT);
		RBE_CONST_ULONG(COPY);
		RBE_CONST_ULONG(PASTE);
		RBE_CONST_ULONG(SELECT_ALL);
		RBE_CONST_ULONG(SAVE_REQUESTED);
		RBE_CONST_ULONG(MESSAGE_NOT_UNDERSTOOD);
		RBE_CONST_ULONG(NO_REPLY);
		RBE_CONST_ULONG(REPLY);
		RBE_CONST_ULONG(SIMPLE_DATA);
		RBE_CONST_ULONG(MIME_DATA);
		RBE_CONST_ULONG(ARCHIVED_OBJECT);
		RBE_CONST_ULONG(UPDATE_STATUS_BAR);
		RBE_CONST_ULONG(RESET_STATUS_BAR);
		RBE_CONST_ULONG(NODE_MONITOR);
		RBE_CONST_ULONG(QUERY_UPDATE);
		RBE_CONST_ULONG(ENDORSABLE);
		RBE_CONST_ULONG(COPY_TARGET);
		RBE_CONST_ULONG(MOVE_TARGET);
		RBE_CONST_ULONG(TRASH_TARGET);
		RBE_CONST_ULONG(LINK_TARGET);
		RBE_CONST_ULONG(INPUT_DEVICES_CHANGED);
		RBE_CONST_ULONG(INPUT_METHOD_EVENT);
		RBE_CONST_ULONG(WINDOW_MOVE_TO);
		RBE_CONST_ULONG(WINDOW_MOVE_BY);
		RBE_CONST_ULONG(SILENT_RELAUNCH);
		RBE_CONST_ULONG(OBSERVER_NOTICE_CHANGE);
		RBE_CONST_ULONG(CONTROL_INVOKED);
		RBE_CONST_ULONG(CONTROL_MODIFIED);

		// <app/Clipboard.h>
		RBE_CONST_ULONG(CLIPBOARD_CHANGED);

		// <app/Handler.h>
		RBE_CONST_STR(OBSERVE_WHAT_CHANGE);
		RBE_CONST_STR(OBSERVE_ORIGINAL_WHAT);
		RBE_CONST_ULONG(OBSERVER_OBSERVE_ALL);

		// <app/Message.h>
		RBE_CONST_ULONG(NO_SPECIFIER);
		RBE_CONST_ULONG(DIRECT_SPECIFIER);
		RBE_CONST_ULONG(INDEX_SPECIFIER);
		RBE_CONST_ULONG(REVERSE_INDEX_SPECIFIER);
		RBE_CONST_ULONG(RANGE_SPECIFIER);
		RBE_CONST_ULONG(REVERSE_RANGE_SPECIFIER);
		RBE_CONST_ULONG(NAME_SPECIFIER);
		RBE_CONST_ULONG(ID_SPECIFIER);
		RBE_CONST_ULONG(SPECIFIERS_END);

		// <interface/Window.h>
		RBE_CONST_ULONG(UNTYPED_WINDOW);
		RBE_CONST_ULONG(TITLED_WINDOW);
		RBE_CONST_ULONG(MODAL_WINDOW);
		RBE_CONST_ULONG(DOCUMENT_WINDOW);
		RBE_CONST_ULONG(BORDERED_WINDOW);
		RBE_CONST_ULONG(FLOATING_WINDOW);

		RBE_CONST_ULONG(BORDERED_WINDOW_LOOK);
		RBE_CONST_ULONG(NO_BORDER_WINDOW_LOOK);
		RBE_CONST_ULONG(TITLED_WINDOW_LOOK);
		RBE_CONST_ULONG(DOCUMENT_WINDOW_LOOK);
		RBE_CONST_ULONG(MODAL_WINDOW_LOOK);
		RBE_CONST_ULONG(FLOATING_WINDOW_LOOK);

		RBE_CONST_ULONG(NORMAL_WINDOW_FEEL);
		RBE_CONST_ULONG(MODAL_SUBSET_WINDOW_FEEL);
		RBE_CONST_ULONG(MODAL_ALL_WINDOW_FEEL);
		RBE_CONST_ULONG(FLOATING_SUBSET_WINDOW_FEEL);
		RBE_CONST_ULONG(FLOATING_APP_WINDOW_FEEL);
		RBE_CONST_ULONG(FLOATING_ALL_WINDOW_FEEL);

		RBE_CONST_ULONG(BYTE_ALIGNMENT);
		RBE_CONST_ULONG(PIXEL_ALIGNMENT);
		RBE_CONST_ULONG(NOT_MOVABLE);
		RBE_CONST_ULONG(NOT_CLOSABLE);
		RBE_CONST_ULONG(NOT_ZOOMABLE);
		RBE_CONST_ULONG(NOT_MINIMIZABLE);
		RBE_CONST_ULONG(NOT_RESIZABLE);
		RBE_CONST_ULONG(NOT_H_RESIZABLE);
		RBE_CONST_ULONG(NOT_V_RESIZABLE);
		RBE_CONST_ULONG(AVOID_FRONT);
		RBE_CONST_ULONG(AVOID_FOCUS);
		RBE_CONST_ULONG(WILL_ACCEPT_FIRST_CLICK);
		RBE_CONST_ULONG(OUTLINE_RESIZE);
		RBE_CONST_ULONG(NO_WORKSPACE_ACTIVATION);
		RBE_CONST_ULONG(NOT_ANCHORED_ON_ACTIVATE);
		RBE_CONST_ULONG(ASYNCHRONOUS_CONTROLS);
		RBE_CONST_ULONG(QUIT_ON_WINDOW_CLOSE);
		RBE_CONST_ULONG(SAME_POSITION_IN_ALL_WORKSPACES);
		RBE_CONST_ULONG(AUTO_UPDATE_SIZE_LIMITS);
		RBE_CONST_ULONG(CLOSE_ON_ESCAPE);
		RBE_CONST_ULONG(NO_SERVER_SIDE_WINDOW_MODIFIERS);

		RBE_CONST_ULONG(CURRENT_WORKSPACE);
		RBE_CONST_ULONG(ALL_WORKSPACES);

		// <interface/View.h>

		RBE_CONST_ULONG(PRIMARY_MOUSE_BUTTON);
		RBE_CONST_ULONG(SECONDARY_MOUSE_BUTTON);
		RBE_CONST_ULONG(TERTIARY_MOUSE_BUTTON);

		RBE_CONST_ULONG(ENTERED_VIEW);
		RBE_CONST_ULONG(INSIDE_VIEW);
		RBE_CONST_ULONG(EXITED_VIEW);
		RBE_CONST_ULONG(OUTSIDE_VIEW);

		RBE_CONST_ULONG(POINTER_EVENTS);
		RBE_CONST_ULONG(KEYBOARD_EVENTS);

		RBE_CONST_ULONG(LOCK_WINDOW_FOCUS);
		RBE_CONST_ULONG(SUSPEND_VIEW_FOCUS);
		RBE_CONST_ULONG(NO_POINTER_HISTORY);
		RBE_CONST_ULONG(FULL_POINTER_HISTORY);

		RBE_CONST_ULONG(TRACK_WHOLE_RECT);
		RBE_CONST_ULONG(TRACK_RECT_CORNER);

		RBE_CONST_ULONG(FONT_FAMILY_AND_STYLE);
		RBE_CONST_ULONG(FONT_SIZE);
		RBE_CONST_ULONG(FONT_SHEAR);
		RBE_CONST_ULONG(FONT_ROTATION);
		RBE_CONST_ULONG(FONT_SPACING);
		RBE_CONST_ULONG(FONT_ENCODING);
		RBE_CONST_ULONG(FONT_FACE);
		RBE_CONST_ULONG(FONT_FLAGS);
		RBE_CONST_ULONG(FONT_FALSE_BOLD_WIDTH);
		RBE_CONST_ULONG(FONT_ALL);

		RBE_CONST_ULONG(FULL_UPDATE_ON_RESIZE);
		RBE_CONST_ULONG(WILL_DRAW);
		RBE_CONST_ULONG(PULSE_NEEDED);
		RBE_CONST_ULONG(NAVIGABLE_JUMP);
		RBE_CONST_ULONG(FRAME_EVENTS);
		RBE_CONST_ULONG(NAVIGABLE);
		RBE_CONST_ULONG(SUBPIXEL_PRECISE);
		RBE_CONST_ULONG(DRAW_ON_CHILDREN);
		RBE_CONST_ULONG(INPUT_METHOD_AWARE);
		RBE_CONST_ULONG(SUPPORTS_LAYOUT);
		RBE_CONST_ULONG(INVALIDATE_AFTER_LAYOUT);

		RBE_CONST_ULONG(FOLLOW_NONE);
		RBE_CONST_ULONG(FOLLOW_ALL_SIDES);
		RBE_CONST_ULONG(FOLLOW_ALL);

		RBE_CONST_ULONG(FOLLOW_LEFT);
		RBE_CONST_ULONG(FOLLOW_RIGHT);
		RBE_CONST_ULONG(FOLLOW_LEFT_RIGHT);
		RBE_CONST_ULONG(FOLLOW_H_CENTER);

		RBE_CONST_ULONG(FOLLOW_TOP);
		RBE_CONST_ULONG(FOLLOW_BOTTOM);
		RBE_CONST_ULONG(FOLLOW_TOP_BOTTOM);
		RBE_CONST_ULONG(FOLLOW_V_CENTER);
	}

	VALUE
	rb_s_be_app_get(VALUE clazz)
	{
		RBE_TRACE_METHOD_CALL("rbe::rb_s_be_app_get", 0, NULL, clazz);
		if (!be_app)
			return Qnil;
		BArchivable *ptr = static_cast<BArchivable *>(be_app);
		return ObjectRegistory::Instance()->Get(static_cast<void *>(ptr));
	}

	VALUE
	rb_s_debug_set(VALUE clazz, VALUE v)
	{
		if (RTEST(v)) {
			SET_DEBUG_ENABLED(true);
		}
		else {
			SET_DEBUG_ENABLED(false);
		}
		return Qnil;
	}

	static void mark_value_set(void *ptr)
	{
		std::map<VALUE, int> *set_ptr = static_cast<std::map<VALUE, int> *>(ptr);
		std::map<VALUE, int>::iterator itr = set_ptr->begin();
		for(; itr != set_ptr->end(); itr++)
			rb_gc_mark(itr->first);
	}

	static void free_value_set(void *ptr)
	{
		std::map<VALUE, int> *set_ptr = static_cast<std::map<VALUE, int> *>(ptr);
		delete set_ptr;
	}

	void MemorizeObject(VALUE obj, VALUE ref)
	{
		std::map<VALUE, int> *set_ptr = NULL;
		VALUE vset = rb_iv_get(obj, "__rbe_refs");
		if (vset == Qnil) {
			set_ptr = new std::map<VALUE, int>();
			vset = Data_Wrap_Struct(rb_cData, mark_value_set, free_value_set, set_ptr);
			rb_iv_set(obj, "__rbe_refs", vset);
		} else {
			void *ptr = DATA_PTR(vset);
			set_ptr = static_cast<std::map<VALUE, int> *>(ptr);
		}
		std::map<VALUE, int>::iterator itr = set_ptr->find(ref);
		if (itr == set_ptr->end()) {
			std::pair<VALUE, int> data(ref, 1);
			set_ptr->insert(data);
		} else {
			itr->second ++;
		}
	}

	void ForgetObject(VALUE obj, VALUE ref)
	{
		std::map<VALUE, int> *set_ptr = NULL;
		VALUE vset = rb_iv_get(obj, "__rbe_refs");
		if (vset == Qnil)
			return;
		set_ptr = static_cast<std::map<VALUE, int> *>(DATA_PTR(vset));
		std::map<VALUE, int>::iterator itr = set_ptr->find(ref);
		if (itr != set_ptr->end()) {
			itr->second --;
			if (itr->second == 0)
				set_ptr->erase(ref);
		}
	}

	void
	init_threads()
	{
		RBE_TRACE("rbe::init_threads");
		gGVLFlag = new ThreadLocalFlag();
		gFuncallState = new ThreadLocalVariable();
		CallWithGVLBase::InitStatic(gGVLFlag);
		CallWithoutGVLBase::InitStatic(gGVLFlag);
	}		

	void global_mark(void *ptr);
	void global_free(void *ptr);

	void
	init_rbe()
	{
		RBE_TRACE("rbe::init_rbe");
		gModule = rb_define_module("B");
		rb_define_singleton_method(gModule, "app",
								   RUBY_METHOD_FUNC(rb_s_be_app_get), 0);
		rb_define_singleton_method(gModule, "DEBUG=",
									RUBY_METHOD_FUNC(rb_s_debug_set), 1);
		gMarker = Data_Wrap_Struct(rb_cData, global_mark, global_free, 0);
		rb_global_variable(const_cast<VALUE *>(&gMarker));
		eQuitLooper = rb_define_class_under(gModule, "QuitLooper", rb_eException);
		ObjectRegistory::Initialize();
	}

	void
	global_mark(void *ptr)
	{
		RBE_TRACE("rbe::global_mark");
		VALUE app = rb_s_be_app_get(gModule);
		if (!NIL_P(app))
			rb_gc_mark(app);
	}

	void
	global_free(void *ptr)
	{
		RBE_TRACE("rbe::global_free");
		/* do nothing ?? */
	}

	int
	FuncallState()
	{
		return (int)gFuncallState->Get();
	}

	void
	SetFuncallState(int s)
	{
		gFuncallState->Set((void *)s);
	}
}

extern "C"
void Init_rbe()
{
	SET_DEBUG_ENABLED(false);
	rbe::init_threads();
	rbe::init_rbe();
	rbe::init_consts();
	rbe::init_classes(rbe::gModule);
}
