#!/bin/sh

if [ "x" = "x$1" ]; then
    echo "usage: $0 <ruby source dir>" >&2
    exit 1
fi

ruby_src_dir=$1
CC=gcc

cat <<EOF > tmp.c
#include <ruby.h>
#include <vm_core.h>
#include <stddef.h>
#include <stdio.h>

int main()
{
    printf("machine_stack_start: %d\n", offsetof(struct rb_thread_struct, machine_stack_start));
    printf("machine_stack_end: %d\n", offsetof(struct rb_thread_struct, machine_stack_end));
    printf("machine_regs: %d\n", offsetof(struct rb_thread_struct, machine_regs));
    return 0;
}
EOF

$CC -I$ruby_src_dir -I$ruby_src_dir/include -I$ruby_src_dir/.ext/include/i586-haiku tmp.c
if [ -e ./a.out ]; then
    ./a.out
    rm ./a.out
fi

rm tmp.c
