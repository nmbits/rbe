#ifndef FT_HEADER
#define FT_HEADER 1

#include <ruby.h>
#include <ruby/thread.h>

#include <setjmp.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#ifdef __HAIKU__
#include <kernel/OS.h>
typedef sem_id ft_sem_t;
#else
#include <semaphore.h>
typedef sem_t *ft_sem_t;
#endif

typedef struct ft_handle_struct
{
    void *th;
	VALUE thval;
    int status;
	int detach_status;
    VALUE *machine_stack_start;
    VALUE *machine_stack_end;
    jmp_buf machine_regs;
	ft_sem_t sem0;
	ft_sem_t sem1;
	void (*ubf)(void *);
	void *ubf_data;
	void (*finish)(void *);
	void *finish_data;
} ft_handle_t;

ft_handle_t *ft_thread_create();
int ft_wait_for_attach_completed(ft_handle_t *ft);
int ft_attach(ft_handle_t *ft, void *stack_start, void (*ubf)(void *), void *ubf_data);
void ft_detach(int status);
void ft_detach2(int status, void (*fn)(void *), void *data);
void *ft_call_with_gvl_protected(void *(*)(void *), void *, int *);
void ft_init();

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif
