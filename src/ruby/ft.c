#include <ruby.h>
#include <ruby/thread.h>
#include <ruby/st.h>
#include <ruby/version.h>

#include <pthread.h>
#include "ft.h"

static struct ft_thread_struct_info
{
	off_t machine_stack_start_offset;
	off_t machine_stack_end_offset;
	off_t machine_regs_offset;
} ft_thread_struct_profile = { 0, 0, 0 };

static const struct ft_thread_struct_info ft_i386_linux_020100 = { 308, 312, 320 };
static const struct ft_thread_struct_info ft_i586_haiku_020100 = { 248, 252, 260 };

#define FT_ATTACHED 1
#define FT_DETACHED 2

#ifdef __HAIKU__

#include <support/Debug.h>
#define FT_PRINT(x) PRINT(x)

#define FT_CREATE_SEM(n) create_sem((n), "ft")
#define FT_DELETE_SEM(s) delete_sem((s))
#define FT_ACQUIRE_SEM(s) acquire_sem((s))
#define FT_TRY_ACQUIRE_SEM(s) (B_OK == acquire_sem_etc((s), 1, B_RELATIVE_TIMEOUT, 0))
#define FT_RELEASE_SEM(s) release_sem((s))

#include <support/TLS.h>
typedef int32 ft_tls_key_t;
#define FT_TLS_ALLOCATE(k) ((k) = tls_allocate())
#define FT_TLS_GET(k) tls_get((k))
#define FT_TLS_SET(k, v) tls_set((k), (v))

#else

#define FT_PRINT(x) (void)0

static ft_sem_t
ft_create_sem(int n)
{
	sem_t *sem;
	sem = xmalloc(sizeof(sem_t));
	if (sem == 0)
		return 0;
	if (sem_init(sem, 0, n) != 0) {
		xfree(sem);
		rb_bug("sem_init: failed");
	}
	return sem;
}

static void
ft_delete_sem(ft_sem_t sem)
{
	sem_destroy(sem);
	xfree(sem);
}

#define FT_CREATE_SEM(n) ft_create_sem((n))
#define FT_DELETE_SEM(s) ft_delete_sem((s))
#define FT_ACQUIRE_SEM(s) sem_wait((s))
#define FT_TRY_ACQUIRE_SEM(s) (0 == sem_trywait((s)))
#define FT_RELEASE_SEM(s) sem_post((s))

typedef pthread_key_t ft_tls_key_t;
#define FT_TLS_ALLOCATE(k) pthread_key_create(&(k), NULL)
#define FT_TLS_GET(k) pthread_getspecific((k))
#define FT_TLS_SET(k, v) pthread_setspecific((k), (v))
#endif

static ft_tls_key_t ft_tls_ft;

static void
ft_mark(ft_handle_t *ft)
{
    if (ft->status == FT_ATTACHED) {
        if (ft->machine_stack_start < ft->machine_stack_end) {
            rb_gc_mark_locations(ft->machine_stack_start,
								 ft->machine_stack_end);
        }
        else {
            rb_gc_mark_locations(ft->machine_stack_end,
								 ft->machine_stack_start);
        }
        rb_gc_mark_locations((VALUE *)&ft->machine_regs,
                             (VALUE *)&ft->machine_regs +
							 sizeof(ft->machine_regs) / sizeof(VALUE));
    }
}

static void
ft_free(ft_handle_t *ft)
{
	FT_DELETE_SEM(ft->sem0);
	FT_DELETE_SEM(ft->sem1);
    xfree(ft);
}

static size_t
ft_size(void *ptr)
{
	return sizeof(ft_handle_t);
}

static void *
ft_get_threadptr(VALUE thval)
{
	if (ruby_api_version[0] == 2 && ruby_api_version[1] == 1) {
		return DATA_PTR(thval);
	}
	return NULL;
}

static pthread_key_t ruby_native_thread_key = 0;

static pthread_key_t
ft_detect_thread_key()
{
	VALUE thval = rb_thread_current();
	void *th = ft_get_threadptr(thval);
	pthread_key_t key;

	FT_PRINT(("ft_detect_thread_key\n"));

	for (key = 0; key <= PTHREAD_KEYS_MAX; key++) {
		void *val = pthread_getspecific(key);
		FT_PRINT(("ft_detect_thread_key_func: %d\n", key));
		if (val == th) {
			ruby_native_thread_key = key;
			break;
		}
	}

	FT_PRINT(("ft_detect_thread_key done\n"));

	return ruby_native_thread_key;
}

static int
ft_thread_set_native(void *th)
{
    return pthread_setspecific(ruby_native_thread_key, th) == 0;
}

static void
ft_set_thread_struct_profile()
{
	int major = ruby_api_version[0];
	int minor = ruby_api_version[1];

	if (major == 2) {
		if (minor == 1) {
			ft_thread_struct_profile = ft_i586_haiku_020100;
		}
	}
	if (ft_thread_struct_profile.machine_stack_start_offset == 0)
		rb_raise(rb_eNotImpError, "This library cannot run on this ABI version.");
}

#define FT_THREADPTR_STACK_START_ADDR(th)								\
	((VALUE **)((char *)(th) + ft_thread_struct_profile.machine_stack_start_offset))
#define FT_THREADPTR_STACK_END_ADDR(th)									\
	((VALUE **)((char *)(th) + ft_thread_struct_profile.machine_stack_end_offset))
#define FT_THREADPTR_REGS_ADDR(th)									\
	((jmp_buf *)((char *)(th) + ft_thread_struct_profile.machine_regs_offset))

static void
ft_ubf(ft_handle_t *ft)
{
	if (ft->ubf) {
		(ft->ubf)(ft->ubf_data);
	}
}

typedef void *thread_func_t(void *);

static void
ft_copy_machine_regs(jmp_buf *src, jmp_buf *dst)
{
	memcpy(dst, src, sizeof(jmp_buf));
}

static void *
ft_thread_func_1(ft_handle_t *ft)
{
	FT_RELEASE_SEM(ft->sem0);
	FT_ACQUIRE_SEM(ft->sem1);

	return NULL;
}

static VALUE
ft_thread_func_0(ft_handle_t *ft)
{
	void *th = ft->th;

	FT_PRINT(("ft_thread_func_0\n"));
	rb_thread_call_without_gvl2((thread_func_t *)&ft_thread_func_1, ft,
								(rb_unblock_function_t *)&ft_ubf, ft);

	*FT_THREADPTR_STACK_START_ADDR(th) = ft->machine_stack_start;
	*FT_THREADPTR_STACK_END_ADDR(th) = ft->machine_stack_end;
	ft_copy_machine_regs(&ft->machine_regs, FT_THREADPTR_REGS_ADDR(th));
	ft->status = FT_DETACHED;

	if (ft->finish)
		(*ft->finish)(ft->finish_data);
	FT_RELEASE_SEM(ft->sem0);

	rb_thread_check_ints();
	if (ft->detach_status)
		rb_jump_tag(ft->detach_status);

	return Qnil;
}

rb_data_type_t ft_handle_data_type = {
	"ft/handle",
	{ (void (*)(void *))&ft_mark,
	  (void (*)(void *))&ft_free,
	  (size_t (*)(const void *))&ft_size }
};

ft_handle_t *
ft_thread_create()
{
    ft_handle_t *ft;
    VALUE ftval;
	void *th;

	FT_PRINT(("ft_thread_create\n"));
    ftval = TypedData_Make_Struct(rb_cData, ft_handle_t,
								  &ft_handle_data_type, ft);
	ft->sem0 = FT_CREATE_SEM(0);
	ft->sem1 = FT_CREATE_SEM(0);

    ft->thval = rb_thread_create(ft_thread_func_0, ft);
    rb_iv_set(ft->thval, "__ft", ftval);
	th = ft_get_threadptr(ft->thval);
	ft->th = th;

	do {
		FT_PRINT(("calling rb_thread_schedule\n"));
		rb_thread_schedule();
	} while (!FT_TRY_ACQUIRE_SEM(ft->sem0));

	ft->machine_stack_start = *FT_THREADPTR_STACK_START_ADDR(th);
	ft->machine_stack_end = *FT_THREADPTR_STACK_END_ADDR(th);
	ft_copy_machine_regs(FT_THREADPTR_REGS_ADDR(th), &ft->machine_regs);

	return ft;
}

static void
ft_set_stack_end(VALUE **stack_end_p)
{
	VALUE stack_end;
	*stack_end_p = &stack_end;
}

int
ft_wait_for_attach_completed(ft_handle_t *ft)
{
	FT_PRINT(("ft_wait_for_attach_completed\n"));
	FT_ACQUIRE_SEM(ft->sem0);

	return 1;
}

int
ft_attach(ft_handle_t *ft, void *stack_start, void (*ubf)(void *), void *ubf_data)
{
    void *th = ft->th;

	FT_PRINT(("ft_attach\n"));
	*(FT_THREADPTR_STACK_START_ADDR(th)) = stack_start;
	ft_set_stack_end(FT_THREADPTR_STACK_END_ADDR(th));
	ft_thread_set_native(th);

	FT_TLS_SET(ft_tls_ft, (void *)ft);
	ft->status = FT_ATTACHED;
	ft->ubf = ubf;
	ft->ubf_data = ubf_data;
	FT_RELEASE_SEM(ft->sem0);

	return 1; // todo
}

void
ft_detach(int status)
{
	ft_handle_t *ft = (ft_handle_t *)FT_TLS_GET(ft_tls_ft);

	FT_PRINT(("ft_detach\n"));
    ft->detach_status = status;

	FT_RELEASE_SEM(ft->sem1);
	FT_ACQUIRE_SEM(ft->sem0);
	FT_PRINT(("ft_detach done\n"));
}

void
ft_detach2(int status, void (*fn)(void *), void *data)
{
	ft_handle_t *ft = (ft_handle_t *)FT_TLS_GET(ft_tls_ft);

	ft->finish = fn;
	ft->finish_data = data;
	ft_detach(status);
}

struct ft_call_arg
{
	void *(*fn)(void *);
	void *arg;
	int *state;
};

static void *
ft_call_with_gvl_0(void *arg)
{
	struct ft_call_arg *call_arg = (struct ft_call_arg *)arg;
	return (void *)rb_protect((VALUE (*)(VALUE))call_arg->fn,
		(VALUE)call_arg->arg, call_arg->state);
}

void *
ft_call_with_gvl_protected(void *(*fn)(void *), void *arg, int *stats)
{
	struct ft_call_arg call_arg = {fn, arg, stats};
	return rb_thread_call_with_gvl(ft_call_with_gvl_0, (void *)&call_arg);
}

void
ft_init()
{
	FT_PRINT(("ft_init\n"));
	ft_detect_thread_key();
	ft_set_thread_struct_profile();
	FT_TLS_ALLOCATE(ft_tls_ft);
	FT_PRINT(("ft_init done\n"));
}
