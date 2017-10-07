
/* Pet Thread Library
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include "pet_thread.h"
#include "string.h"

int nof_switches = 0; // This is just a counter to store total switch count

typedef enum {
    PT_INIT,
    PT_STOPPED,
    PT_RUNNING,
    PT_READY,
    PT_BLOCKED
} PT_STATE;


typedef struct  {
    PT_ID       id;
    char        *stackblockPtr;         // Top Stack Pointer.
                                        // We allocate in the pet_thread_create() and release in pet_thread_exit()

    char        *stackPtr;              // Current stack pointer will be saved here during switch stack
                                        // If you dereference it will give you the address to return to from where you left last time
                                        // We initialize this to the top of stack pet_thread_create()
                                        // and also update *top of stack to have start thread function address
    PT_FNPTR    user_fn_ptr;
    PT_STATE    state;
    PT_ARG      *user_arg_ptr;
    PT_ID       joinfrom;
} PT_TCB;

// We will provide space for stack for all threads in one block in pet_thread_init()
// and initialize top of stack for each thread in pet_thread_create()
static char  *stackblocksAllPtr = NULL;


// We will block allocate for all TCBs in pet_thread_init()
// We will update individual attributes for each TCB in pet_thread_create()
static PT_TCB *tcbsAllPtr = NULL;

int pt_running_count = -1;


static PT_ID        pt_running_tid = -1;
static PT_TCB       tcb_master; // Did not find use still?


extern void __switch_to_stack(void            * current_thread_stackPtr,
                              void            * next_thread_stackPtr,
                              PT_ID             current_threadId,
                              PT_ID             next_threadId);

extern void __abort_to_stack(void * target_thread_stackPtr);

extern int set_timer_interval(unsigned long int interval);
extern int time_to_yield;

extern void __push_context(void *tcb_ptr,void *target_thread_stackPtr);

void pt_busy_counter(char *string)
{
    for (unsigned long int i=0x3;i>0;i--) {
        for (unsigned long int j=0xffffff;j>0;j--) {
            // printf("IC %c",c);
        }
        printf("%s%lu ",string,pt_running_tid);
    }

}


static void
__dump_stack(PT_TCB *tcb)
{
    printf("Inside Dump Stack");
    printf(" tid: %lu ",tcb->id);
    printf(" tcbPtr: %p ",tcb);
    printf(" stackBlockPtr: %p", tcb->stackblockPtr);
    printf(" stackPtr: %p ",tcb->stackPtr);
    printf(" fnPtr: %p",*(PT_FNPTR *)((char *)tcb->stackPtr+112)); //112 sizeof(struct exec_ctx)));
    printf("\n");
}

int pet_thread_init(void)
{
    printf("Initializing Pet Thread library\n");

    stackblocksAllPtr = calloc(PT_STACK_SIZE,PT_MAX_TCOUNT+1);
    tcbsAllPtr   = calloc(sizeof(PT_TCB),PT_MAX_TCOUNT+1);
    pt_running_count = 0;

    tcbsAllPtr[0].id  =  PT_MASTER_ID;
    tcbsAllPtr[0].stackblockPtr = NULL;
    tcbsAllPtr[0].stackPtr = NULL;
    tcbsAllPtr[0].state = PT_READY;

    pt_running_tid = PT_MASTER_ID;

    set_timer_interval(250000);
    pt_running_count = 1;
    return 0;
}

static int
pt_thread_invoker(PT_TCB *tcb)
{
    char *temp;

    __dump_stack(tcb);

    *temp = tcb->user_fn_ptr(tcb->user_arg_ptr);

    pet_thread_exit(&temp);


    return 0;

}

int pet_thread_create(PT_ID * id,
                  PT_FNPTR   func,
                  PT_ARG     * arg) {

    char * stackBlockPtr;
    char * stackPtr;


    int idx = pt_running_count;

    if (idx  <=  PT_MAX_TCOUNT) {

        PT_TCB *tcb_ptr = NULL;

        // Use one TCB from the array based on index
        tcb_ptr = &tcbsAllPtr[idx];

        // Initialize TCB with idx and other values
        tcb_ptr->id = idx;

        // We allocated stack in pit_thread_init and let us use it here.
        stackBlockPtr    = stackblocksAllPtr + (idx*PT_STACK_SIZE);
        tcb_ptr->stackblockPtr  = stackBlockPtr;

        stackPtr         = stackBlockPtr + PT_STACK_SIZE; // - sizeof(PT_FNPTR);


        stackPtr = stackPtr - sizeof(PT_FNPTR);
        *(PT_FNPTR *)stackPtr = &pt_thread_invoker;


        printf("Before Save Reg");
        printf(" fnPtr:     %p", *(PT_FNPTR *)stackPtr);
        printf(" stackPtr:  %p", stackPtr);
        printf("\n");


        __push_context (tcb_ptr,&stackPtr);

        printf("After  Save Reg");
        printf(" fnPtr:     %p", *(PT_FNPTR *)(stackPtr+112));
        printf(" stackPtr:  %p", stackPtr);
        printf("\n");

        tcb_ptr->stackPtr = stackPtr;

        tcb_ptr->state = PT_READY;  // For simplicity all created threads are Ready to Run

        tcb_ptr->user_fn_ptr = *func;

        tcb_ptr->user_arg_ptr = arg;           // For simplity argument will be passed as a pointer and can be type casted by thread function as required

        pt_running_count++;        // One more thread is ready to run

        *id = tcb_ptr->id;            // Return Thread Id

        __dump_stack(tcb_ptr);        // This is only for debug purpose I suppose


        return 0;
    } else
        return -1;
}
int
pet_thread_yield_to(PT_ID next_tid) {
    // The order of assginments is critical here.
    // Remember after context switch stack variable will be refreshed to next task values
    PT_ID current_tid = pt_running_tid;


    tcbsAllPtr[next_tid].state = PT_RUNNING;
    tcbsAllPtr[current_tid].state = PT_READY;

    nof_switches++;
    printf("\n%d ", nof_switches);
    printf("BCS:%lu ", pt_running_tid);

    pt_running_tid = next_tid;
    __switch_to_stack(
            &tcbsAllPtr[current_tid].stackPtr,
            &tcbsAllPtr[next_tid].stackPtr,
            tcbsAllPtr[current_tid].id,
            tcbsAllPtr[next_tid].id
    );
    printf("ACS:%lu\n", pt_running_tid);


    return 0;
}

int
pet_thread_schedule()
{
    int cur_tid;
    int next_tid;

    // Currently simple round robin
    // Assumption : all threads are initialized before this function is called
    cur_tid  = pt_running_tid;
    do {
        cur_tid++;
        if (cur_tid > PT_MAX_TCOUNT) cur_tid=0;
    } while (tcbsAllPtr[cur_tid].state != PT_READY);

    next_tid = cur_tid;

    return next_tid;
}

int
pet_thread_run(void)
{
    int next_tid=-1;
    if (time_to_yield) {
        time_to_yield = 0;
        next_tid = pet_thread_schedule();
        pet_thread_yield_to(next_tid);
        return(next_tid);
    }
    return 0;
}


void
pet_thread_cleanup(PT_ID prev_id,
                   PT_ID my_id)
{
    /* Implement this */

}


int
pet_thread_join(PT_ID    thread_id,
                void            ** ret_val)
{

    /* Implement this */

    return 0;
}

void
pet_thread_exit(void * ret_val)
{
    /* Implement this */

    PT_TCB *tcb_ptr, *next_tcb_ptr;

    tcb_ptr = &tcbsAllPtr[pt_running_tid];

    tcb_ptr->state=PT_STOPPED;

    PT_ID next_tid = pet_thread_schedule();

    pt_running_tid = next_tid;


    next_tcb_ptr = &tcbsAllPtr[next_tid];

    next_tcb_ptr->state = PT_RUNNING;

    __abort_to_stack(&tcb_ptr->stackPtr);

}



