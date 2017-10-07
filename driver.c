/* Pet Thread Library test driver
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 */

#include <stdio.h>
#include <stdlib.h>

#include "pet_thread.h"
// #include "pet_log.h"

extern int pet_thread_init(void);
extern int pet_thread_create(PT_ID * id, PT_FNPTR func, PT_ARG * arg);
extern int nof_switches;
extern void pt_busy_counter(char *string);

PT_ARG pt_arg[PT_MAX_TCOUNT];




void test_func1(PT_ARG *parg) {

    int arg1,arg2,arg3;

    printf("%s Arg1:%d Arg2:%d Arg3:%d \n",parg->msg, parg->arg1, parg->arg2, parg->arg3);

    while (1) {
        pt_busy_counter("t");
        pet_thread_run();
    }
}


int main(int argc, char ** argv) {
    int ret = 0;


    PT_ID thread_ids[PT_MAX_TCOUNT];

    ret = pet_thread_init();

    if (ret == -1) {
        // ERROR("Could not initialize Pet Thread Library\n");
        return -1;
    }

    printf("Testing Pet Thread Library\n");



    for (int i=0 ; i<PT_MAX_TCOUNT; i++)
    {

        pt_arg[i].arg1 = i+1;
        pt_arg[i].arg2 = i+1;
        pt_arg[i].arg3 = i+1;
        pt_arg[i].msg = "Hurray Inside Thread :-)";

        ret = pet_thread_create(&thread_ids[i], (PT_FNPTR) test_func1, &pt_arg[i]);


        printf("THREAD ID = %d \n",(int) thread_ids[i]);

    }

    if (ret == -1) {
        // ERROR("Could not create test_thread1\n");
        return -1;
    }


    while (1) {
        pt_busy_counter("M");
        pet_thread_run();
        if (nof_switches > 20)
            break;
    }


    return 0;

}
