/* Pet Thread Library
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 *  Akhil Yendluri
 *  Krithika Ganesh
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
    pet_thread_id_t joinfrom;
};

static pet_thread_id_t current     = PET_MASTER_THREAD_ID;
struct pet_thread      master_dummy_thread;

static LIST_HEAD(thread_list);
struct pet_thread readyQueue; 
static int thread_ids = 1;
struct pet_thread * master_thread;
static int pet_thread_ready_count  = -1;

extern void __switch_to_stack(void            * tgt_stack,
			      void            * saved_stack,
			      pet_thread_id_t   current,
			      pet_thread_id_t   tgt);

extern void __abort_to_stack(void * tgt_stack);

static struct pet_thread *
get_thread(pet_thread_id_t thread_id)
{
    if (thread_id == PET_MASTER_THREAD_ID) {
	return master_thread;
    }

    struct list_head *pos;
    struct pet_thread *tmp;
    list_for_each(pos,&readyQueue.node) {
       //tmp = (struct pet_thread *)malloc(sizeof(struct pet_thread));
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
    master_thread->state = PRT_THREAD_READY;
    char * size = (master_thread->stackPtr + STACK_SIZE);
    pet_thread_ready_count  = 0;
    return 0;
}


static void
__dump_stack(struct pet_thread * thread)
{
    //change to DEBUG
    printf("Dump thread> Thread ID: %d \n",thread->thread_id);
    printf("Dump thread> Thread Ptr: %p \n",thread);
    printf("Dump thread> Stack Pointer %p \n", thread->stackPtr);
    printf("Dump thread> FunctionPtr: %p \n",thread->function);
    printf("Dump thread> State:%d \n",thread->state);
    printf("Dump thread> Join From %d \n",thread->joinfrom);

    printf("\n");

    return;
}



int
pet_thread_join(pet_thread_id_t    thread_id,
		void            ** ret_val)
{

    struct pet_thread * tgt_thread;
    struct pet_thread * cur_thread;
    cur_thread = get_thread(current);
    tgt_thread = get_thread(thread_id);
    if (tgt_thread == NULL) {return (-1);}
    tgt_thread->joinfrom = cur_thread->thread_id;
    
    cur_thread->state = PET_THREAD_BLOCKED;
    tgt_thread->state = PET_THREAD_RUNNING;
    __switch_to_stack(&tgt_thread->stackPtr,&cur_thread->stackPtr,cur_thread->thread_id,tgt_thread->thread_id);
    return 0;


 }


void
pet_thread_exit(void * ret_val)
{
    struct pet_thread *runningThread = get_thread(current);
    runningThread->state = PET_THREAD_STOPPED;    
    pet_thread_ready_count--;
    if(thread->joinfrom != -1)    
    {
        struct pet_thread *joinThread = get_thread(thread->joinfrom);
        joinThread->state= PET_THREAD_RUNNING;
        __switch_to_stack(&joinThread->stackPtr,&runningThread->stackPtr,runningThread->thread_id,joinThread->thread_id);
    }
    __abort_to_stack(&master_thread->stackPtr);
}



static int
__thread_invoker(struct pet_thread * thread)
{

    int retVal = thread->function(thread->args);
    thread->state = PET_THREAD_STOPPED;
    pet_thread_ready_count--;

    if(thread->joinfrom != -1)
    {
        struct pet_thread *joinThread = get_thread(thread->joinfrom);
        joinThread->state= PET_THREAD_RUNNING;
        //pet_thread_ready_count++;

        __switch_to_stack(&joinThread->stackPtr,&thread->stackPtr,thread->thread_id,joinThread->thread_id);

    }
    __abort_to_stack(&master_thread->stackPtr);

    //__switch_to_stack(&master_thread->stackPtr,&get_thread(current)->stackPtr,master_thread->thread_id,current);


}


int
pet_thread_create(pet_thread_id_t * thread_id,
		  pet_thread_fn_t   func,
		  void            * arg)
{
    struct pet_thread * new_thread;
    new_thread = (struct pet_thread *)malloc(sizeof(struct pet_thread));
    new_thread->thread_id = (pet_thread_id_t)thread_ids++;
    new_thread->state = PET_THREAD_READY;
    new_thread->joinfrom = -1;
    new_thread->stackPtr = calloc(1,STACK_SIZE);
    char * sptr = (new_thread->stackPtr + STACK_SIZE - sizeof(struct exec_ctx));    
    new_thread->stackPtr = sptr;
    struct exec_ctx *cont = sptr ;
    cont->rip = (uint64_t)__thread_invoker;
    new_thread->function = func;
    new_thread->args = arg;
    cont->rdi = (uint64_t)new_thread;
    list_add_tail(&(new_thread->node),&(readyQueue.node));
    pet_thread_ready_count++;

    DEBUG("Created new Pet Thread (%p):\n", new_thread);
    DEBUG("--Add thread state here--\n");
    __dump_stack(new_thread);

    
    return 0;
}


void
pet_thread_cleanup(pet_thread_id_t prev_id,
		   pet_thread_id_t my_id)
{
    if(get_thread(prev_id)->state==PET_THREAD_STOPPED)
    {
        //free from stack top
        free(get_thread(prev_id)->stackPtr);
    }
}




static void
__yield_to(struct pet_thread * tgt_thread)
{

    struct pet_thread *runningThread = get_thread(current);
    runningThread->state = PET_THREAD_READY;
    tgt_thread->state = PET_THREAD_RUNNING;
    current = tgt_thread->thread_id;
    __switch_to_stack(&tgt_thread->stackPtr,&runningThread->stackPtr,runningThread->thread_id,tgt_thread->thread_id);

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
    struct list_head *pos;
    struct pet_thread *tmp;
    do {
        list_for_each(pos, &readyQueue.node) {
            tmp = list_entry(pos, struct pet_thread, node);
            if(tmp->thread_id != current && tmp->state == PET_THREAD_READY)
            {
                current = tmp->thread_id;
                tmp->state = PET_THREAD_RUNNING;
                __switch_to_stack(&tmp->stackPtr,&master_thread->stackPtr,master_thread->thread_id,tmp->thread_id);
            }
        }
    }while(pet_thread_ready_count>0);

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
	     
