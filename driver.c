/* Pet Thread Library test driver
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 */

#include <stdio.h>
#include <stdlib.h>


#include "pet_thread.h"
#include "pet_log.h"





void *
test_func1(void* arg)
{

    printf("Test Function arg = %d",arg);
    pet_thread_yield_to(2);
    printf("Yield done = %d",arg);

    fflush(stdout);
    return 0;
}

void *
test_func2(void* arg)
{



    printf("Test Function arg = %d",arg);
    pet_thread_yield_to(3);
    printf("Yield done = %d",arg);

    fflush(stdout);
    return 0;
}

void *
test_func3(void* arg)
{



    printf("Test Function arg = %d",arg);
    fflush(stdout);
    return 0;
}


int main(int argc, char ** argv)
{
    pet_thread_id_t test_thread1,test_thread2;
    int ret = 0,ret1=0;

    ret = pet_thread_init();

    if (ret == -1) {
	ERROR("Could not initialize Pet Thread Library\n");
	return -1;
    }

    
    printf("Testing Pet Thread Library\n");


    ret = pet_thread_create(&test_thread1, test_func1, (void *)1);
    ret1 = pet_thread_create(&test_thread2, test_func2, (void *)2);
    ret1 = pet_thread_create(&test_thread2, test_func3, (void *)3);

    if (ret == -1) {
	ERROR("Could not create test_thread1\n");
	return -1;
    }
    if (ret1 == -1) {
        ERROR("Could not create test_thread1\n");
        return -1;
    }
    #ifdef ENABLE_TLS
	printf("TLS enabled");   
    #else
        printf("TLS not enabled"); 
    #endif

    ret = pet_thread_run();

    if (ret == -1) {
	ERROR("Error encountered while running pet threads (ret=%d)\n", ret);
	return -1;
    }
    
    return 0;

}
