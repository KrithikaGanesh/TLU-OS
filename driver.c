//
// Created by gswaminatha2 on 02/10/17.
//
/* Pet Thread Library test driver
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 */

#include <stdio.h>
#include <stdlib.h>

#include "pet_thread.h"
// #include "pet_log.h"

//extern int pet_thread_init(void);
//extern int pet_thread_create(pet_thread_id_t * id, pet_thread_fn_t func, PET_THREAD_ARG * arg);

extern int nof_switches;

void test_func1(PET_THREAD_ARG *parg) {

    int arg1,arg2,arg3;

    do  {
        for (int i=0;i<10;i++) {
            arg1 = parg->arg1;
            arg2 = parg->arg2;
            arg3 = parg->arg3;

            // printf("           Inside Thread %s: %d, %d, %d \n", parg->msg, arg1,arg2,arg3);
            printf(".");
        }
        printf("\n");
        pet_thread_schedule();
    } while (1);

    printf("End of Infinite Loop \n");

}


int main(int argc, char ** argv) {
    int ret = 0;

    PET_THREAD_ARG pt_arg[PET_MAX_TCOUNT];



    pet_thread_id_t thread_ids[PET_MAX_TCOUNT];

    ret = pet_thread_init();

    if (ret == -1) {
        // ERROR("Could not initialize Pet Thread Library\n");
        return -1;
    }

    printf("Testing Pet Thread Library\n");



    for (int i=0 ; i<PET_MAX_TCOUNT; i++)
    {

        pt_arg[i].arg1 = i;
        pt_arg[i].arg2 = i+1;
        pt_arg[i].arg3 = i+2;
        pt_arg[i].msg = "......";

        ret = pet_thread_create(&thread_ids[i], (pet_thread_fn_t) test_func1, &pt_arg[i]);


        printf("THREAD ID = %d \n",(int) thread_ids[i]);

    }


    if (ret == -1) {
        // ERROR("Could not create test_thread1\n");
        return -1;
    }

    while(nof_switches < 100)
    {
        ret = pet_thread_run();
        printf("\n Back to Main - ");
        printf("No of switches =%d\n",nof_switches);
    }

    if (ret == -1) {
        // ERROR("Error encountered while running pet threads (ret=%d)\n", ret);
        return -1;
    }

    exit (0);

    // return 0;

}

