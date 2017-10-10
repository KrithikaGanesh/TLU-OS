/* Pet Thread Library test driver
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 */

#include <stdio.h>
#include <stdlib.h>


#include "pet_thread.h"
#include "pet_log.h"

//func1, func2, func3 round robin

static int no_of_threads_RR = 2;
static int join = 5;
// To test Round Robin
void *
test_RR(void* arg)
{
    printf("--------------------");
    printf("Thread ID = %d\n",arg-1);

    fflush(stdout);
    //if(arg<no_of_threads_RR+){
        printf("Yielding To= %d\n",arg);
        fflush(stdout);
        pet_thread_yield_to(arg);
    //}
//    else
//    {
//        pet_thread_yield_to(1);
//    }

    printf("Completing Thread ID = %d\n",arg-1);
    return 0;
}

//TEST JOIN and Exit
void *
test_demojoin(void* arg)
{
    printf("JOINING on = %d\n",join);
    pet_thread_join(join,NULL);
    printf("JOINING THREAD = %d\n",arg);
    return 0;
}

//TEST JOIN and Exit
void *
test_demo(void* arg)
{
    printf("DEMO THREAD = %d\n",arg);
    return 0;
}

void *
test_demoexit(void* arg)
{
    printf("Exiting = %d\n",arg);
    pet_thread_exit((void*)0);
    printf("TEst = %d\n",arg);
    return 0;
}
int main(int argc, char ** argv)
{
    pet_thread_id_t test_thread[no_of_threads_RR];
    int ret = 0,ret1=0;

    ret = pet_thread_init();

    if (ret == -1) {
        ERROR("Could not initialize Pet Thread Library\n");
        return -1;
    }


    printf("Testing Pet Thread Library\n");
    //DEMO RR
    for(int i=0;i<no_of_threads_RR;i++)
    {
        ret = pet_thread_create(&test_thread[i], test_RR, (void *)i+2);
        if (ret == -1) {
            ERROR("Could not create test_thread1\n");
            return -1;
        }


    }

    //DEMO JOIN
    pet_thread_id_t test_threadj, test_threade,test_threade2 ;
    ret = pet_thread_create(&test_threadj, test_demojoin, (void *)3);
    if (ret == -1) {
        ERROR("Could not create test_thread1\n");
        return -1;
    }

    //DEMO JOIN
    ret = pet_thread_create(&test_threade, test_demo, (void *)12);
    if (ret == -1) {
        ERROR("Could not create test_thread1\n");
        return -1;
    }
    ret = pet_thread_create(&test_threade, test_demoexit, (void *)13);
    if (ret == -1) {
        ERROR("Could not create test_thread1\n");
        return -1;
    }

    ret = pet_thread_run();

    if (ret == -1) {
        ERROR("Error encountered while running pet threads (ret=%d)\n", ret);
        return -1;
    }

    return 0;

}
