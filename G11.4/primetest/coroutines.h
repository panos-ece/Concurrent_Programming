#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ucontext.h>
#include <sys/time.h>

void mycoroutines_init(ucontext_t *main_context,long line);
void mycoroutines_create(ucontext_t *new_co,ucontext_t *link_context, void(*body)(), void *arg,long line);
void mycoroutines_switch(ucontext_t *old_co,ucontext_t *new_co,long line);
void mycoroutines_destroy(ucontext_t *co,long line);
