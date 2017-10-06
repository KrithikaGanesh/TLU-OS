/* Pet Thread Library
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 */


#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "pet_thread.h"
#include "pet_hashtable.h"
#include "pet_list.h"
#include "pet_log.h"


#define STACK_SIZE (4096 * 32)



//adding another state PET_THREAD_INIT
typedef enum {PET_THREAD_INIT,PET_THREAD_STOPPED,
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
    /* Implement this */
    pet_thread_id_t id;
    char *stackblockPtr; //Top Stack Pointer : created in pet_thread_create() and released in pet_thread_exit;
    pet_thread_fn_t *stackPtr; //Current stack pointer will be saved here during a switch stack. If you dereference it will give you the address to return to from where you left it
    //should we store the arguments here?
    thread_run_state_t state;
    pet_thread_id_t joinfrom;

};

//We provide space for all stacks in one block in pet_thread_init()
//and initialize the top of stack for each thread in pet_thread_create()
static char *stackblockAllPtr= NULL;

// We will block allocate for all TCBs in pet_thread_init()
// We will update individual attributes for each TCB in pet_thread_create()
static struct pet_thread *tcbsAllPtr = NULL;

static int nof_running_threads = -1;


static pet_thread_id_t current     = PET_MASTER_THREAD_ID; // did not find use still
struct pet_thread      master_dummy_thread; // did not find use still

static LIST_HEAD(thread_list);


extern void __switch_to_stack(void            * tgt_stack,
			      void            * saved_stack,
			      pet_thread_id_t   current,
			      pet_thread_id_t   tgt);

extern void __abort_to_stack(void * tgt_stack);

static struct pet_thread *
get_thread(pet_thread_id_t thread_id)
{

    if (thread_id == PET_MASTER_THREAD_ID) {
	return &(master_dummy_thread);
    }


    /* Implement this */
    
    return NULL;
}

static pet_thread_id_t
get_thread_id(struct pet_thread * thread)
{
    if (thread == &(master_dummy_thread)) {
	return PET_MASTER_THREAD_ID;
    }

    /* Implement this */
    
    return 0;
}


int
pet_thread_init(void)
{
    printf("Initializing Pet Thread library\n");

    stackblockAllPtr = calloc(STACK_SIZE,PET_MAX_TCOUNT+1); //+1 as we are allocating even for main thread
    tcbsAllPtr = calloc(sizeof(struct pet_thread),PET_MAX_TCOUNT+1);
    nof_running_threads = 0 ;

    //initialize master thread
    tcbsAllPtr[0].id = 0;
    tcbsAllPtr[0].stackblockPtr = NULL;
    tcbsAllPtr[0].stackPtr = NULL;
    tcbsAllPtr[0].state = PET_THREAD_READY;

    /* Implement this */
    
    return 0;
}


static void
__dump_stack(struct pet_thread * thread)
{

    printf("DumpStack> Thread id %lu \n", thread->id);
    printf("DumpStack>  TCB pointer %p \n", thread);
    printf("DumpStack> Stack block pointer %p \n", thread->stackblockPtr);
    printf("DumpStack>  Stack pointer %p \n", thread->stackPtr);
    printf("DumpStack>  Function Pointer %p \n", *(pet_thread_fn_t)(char *)thread->stackPtr+0);

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
    /* Implement this */
}



static int
__thread_invoker(struct pet_thread * thread)
{

    /* Implement this */
    
    return 0;
    
}


int
pet_thread_create(pet_thread_id_t * thread_id,
		  pet_thread_fn_t   func,
		  void            * arg)
{
    int index = nof_running_threads+1;
    if(index <= PET_MAX_TCOUNT) {
        struct pet_thread *new_threadTCB = NULL;
        //Use one TCB from the array based on index
        new_threadTCB = &tcbsAllPtr[index];
        new_threadTCB->id = index;

        // We allocate stack in pet_thread_init and let us use here
        // let us go to the bottom of stack

        char * stackBlockPtr = stackblockAllPtr + (index*STACK_SIZE);
        char * stackPtr = stackBlockPtr + STACK_SIZE - sizeof(pet_thread_fn_t);
        new_threadTCB->stackblockPtr = stackBlockPtr;
        new_threadTCB->stackPtr = (pet_thread_fn_t*) stackPtr;

        //Bottom of stack should hold the start address of thread function
        *(new_threadTCB->stackPtr) = func;
        new_threadTCB->state = PET_THREAD_INIT;
        nof_running_threads++;

        DEBUG("Created new Pet Thread (%p):\n", new_threadTCB);
        DEBUG("--Add thread state here--\n");
        __dump_stack(new_threadTCB);


        return 0;
    }
    else
    {
        return -1;
    }
}


void
pet_thread_cleanup(pet_thread_id_t prev_id,
		   pet_thread_id_t my_id)
{
    /* Implement this */
    
}




static void
__yield_to(struct pet_thread * tgt_thread)
{
    /* Implement this */
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

    /* Implement this */
    
    return 0;
}






int
pet_thread_run()
{

    printf("Starting Pet Thread execution\n");
    
    
    pet_thread_schedule();
    
    printf("Pet Thread execution has finished\n");

    return 0;
}
	     
