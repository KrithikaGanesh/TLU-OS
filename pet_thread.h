/* Pet Thread Library
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 */

#ifndef __PET_THREAD_H__
#define __PET_THREAD_H__

#define _GNU_SOURCE

#include <stdint.h>



#define PET_INVALID_THREAD_ID (~0x0ULL)
#define PT_MASTER_ID  (0x0ULL)


#define  PT_STACK_SIZE (4096 * 32)
#define  PT_MAX_TCOUNT 5

typedef struct PARG {
    char *msg;
    int arg1;
    int arg2;
    int arg3;
}PT_ARG;


typedef uintptr_t PT_ID;

typedef void *(*PT_FNPTR)(PT_ARG *arg);


int pet_thread_create(PT_ID * id, PT_FNPTR func, PT_ARG * arg);

int  pet_thread_join(PT_ID thread_id, void ** ret_val);
void pet_thread_exit(void * ret_val);

int pet_thread_yield_to(PT_ID next_tid);
int pet_thread_schedule( void );

int pet_thread_init( void );
int pet_thread_run ( void );




#endif
