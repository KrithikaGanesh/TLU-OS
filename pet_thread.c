/* Pet Thread Library
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 */


#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include<inttypes.h>

#include "pet_thread.h"
#include "pet_hashtable.h"
#include "pet_list.h"
#include "pet_log.h"


#define STACK_SIZE (4096 * 32)




typedef enum {PET_THREAD_STOPPED,
	      PET_THREAD_RUNNING,
 	      PET_THREAD_READY,
	      PET_THREAD_BLOCKED} thread_run_state_t;

struct exec_ctx {
    uint64_t rbp;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rip;
} __attribute__((packed));


struct pet_thread {
    struct exec_ctx context;
    pet_thread_id_t thread_id;
    thread_run_state_t state;
    char * stackPtr;
    pet_thread_fn_t function;
    void * args;
    struct list_head node;
};

static pet_thread_id_t current     = PET_MASTER_THREAD_ID;
struct pet_thread      master_dummy_thread;

static LIST_HEAD(thread_list);
struct pet_thread readyQueue; 
static int thread_ids = 1;
struct pet_thread * master_thread;

extern void __switch_to_stack(void            * tgt_stack,
			      void            * saved_stack,
			      pet_thread_id_t   current,
			      pet_thread_id_t   tgt);

extern void __abort_to_stack(void * tgt_stack);

static struct pet_thread *
get_thread(pet_thread_id_t thread_id)
{
    printf("Inside get_thread\n");
    if (thread_id == PET_MASTER_THREAD_ID) {
	return master_thread;
    }

    struct list_head *pos;
    struct pet_thread *tmp;
    list_for_each(pos,&readyQueue.node) {
       tmp = (struct pet_thread *)malloc(sizeof(struct pet_thread));
       tmp = list_entry(pos,struct pet_thread,node);
       if(tmp->thread_id == thread_id) {
          return tmp;
       }
    }
    
    
    return NULL;
}

static pet_thread_id_t
get_thread_id(struct pet_thread * thread)
{
    if (thread == &(master_dummy_thread)) {
	return PET_MASTER_THREAD_ID;
    }

    return thread->thread_id;
}


int
pet_thread_init(void)
{
    INIT_LIST_HEAD(&readyQueue.node);
    master_thread = &master_dummy_thread;
    master_thread->thread_id = PET_MASTER_THREAD_ID;
    printf("master thread = %d\n",(int)master_thread->thread_id);
    char * size = (master_thread->stackPtr + STACK_SIZE);
    printf("Initializing Pet Thread library\n");    
    return 0;
}


static void
__dump_stack(struct pet_thread * thread)
{

    /* Implement this */
    
    return;
}



int
pet_thread_join(pet_thread_id_t    thread_id,
		void            ** ret_val)
{

    /* Implement this */
    
    return 0;
}


void
pet_thread_exit(void * ret_val)
{
    
}



static int
__thread_invoker(struct pet_thread * thread)
{

    printf("Inside Invoker\n");
    fflush(stdout);
    int retVal = thread->function(thread->args);
    pet_thread_exit((void*)retVal);
    __switch_to_stack(&master_thread->stackPtr,&thread->stackPtr,master_thread->thread_id,thread->thread_id);
}


int
pet_thread_create(pet_thread_id_t * thread_id,
		  pet_thread_fn_t   func,
		  void            * arg)
{
    struct pet_thread * new_thread;
    new_thread = (struct pet_thread *)malloc(sizeof(struct pet_thread));
    new_thread->thread_id = (pet_thread_id_t)++thread_ids;
    printf("new_thread->thread_id = %d\n",(int)new_thread->thread_id);
    new_thread->state = PET_THREAD_READY;
    new_thread->stackPtr = calloc(1,STACK_SIZE);
    char * size = (new_thread->stackPtr + STACK_SIZE - sizeof(struct exec_ctx));    
    new_thread->stackPtr = size;
    struct exec_ctx *cont = size ;
    cont->rip = (uint64_t)__thread_invoker;
    printf("sizeof func = %lu\n",sizeof(func));
    new_thread->function = func;
    printf("sizeof arg = %lu\n",sizeof(arg));
    new_thread->args = arg;
    printf("sizeof new_thread = %lu\n",sizeof(new_thread));
    cont->rdi = (uint64_t)new_thread;
    list_add_tail(&(new_thread->node),&(readyQueue.node));
//    __switch_to_stack(&new_thread->stackPtr,&master_thread->stackPtr,new_thread->thread_id,master_thread->thread_id);    
    DEBUG("Created new Pet Thread (%p):\n", new_thread);
    DEBUG("--Add thread state here--\n");
    __dump_stack(new_thread);

    
    return 0;
}


void
pet_thread_cleanup(pet_thread_id_t prev_id,
		   pet_thread_id_t my_id)
{
    pet_thread_id_t a = prev_id, b = my_id;
    printf("cleanup prev_id = %"PRIu64"\n",a);
    fflush(stdout);
    printf("cleanup my_id = %"PRIu64"\n",b);
    fflush(stdout);
}




static void
__yield_to(struct pet_thread * tgt_thread)
{
    printf("tgt_threadid = %d\nmaster_threadid = %d\n",(int)tgt_thread->thread_id,(int)master_thread->thread_id);
    __switch_to_stack(&tgt_thread->stackPtr,&master_thread->stackPtr,tgt_thread->thread_id,master_thread->thread_id);
    printf("\nfinished execution --- In yield\n");
}


int
pet_thread_yield_to(pet_thread_id_t thread_id)
{
    __yield_to(get_thread(thread_id));
     
    return 0;
}





int
pet_thread_schedule()
{
    printf("Scheduling thread\n");
    struct list_head *pos;
    struct pet_thread *tmp;
    list_for_each(pos,&readyQueue.node) {
       printf("Inside Schedule loop");
       tmp = (struct pet_thread *)malloc(sizeof(struct pet_thread));
       tmp = list_entry(pos,struct pet_thread,node);
       printf("id = %d\n",(int)tmp->thread_id);
       printf("going to pet_thread_yield_to with above thread id");
       pet_thread_yield_to(tmp->thread_id);
    }
    
    return 0;
}






int
pet_thread_run()
{  
    int pet_id;
    pet_id = pet_thread_schedule();
    printf("Pet Thread execution has finished\n");

    return 0;
}
	     
