/* Pet Thread Library test driver
 *  (c) 2017, Jack Lange <jacklange@cs.pitt.edu>
 */

#include <stdio.h>
#include <stdlib.h>


#include "pet_thread.h"
#include "pet_log.h"

//func1, func2, func3 round robin


void *
test_func1(void* arg)
{
    printf("Test Function arg = %d\n",arg);
    pet_thread_yield_to(2);
    printf("End Test Function arg = %d\n",arg);
    return 0;
}

void *
test_func2(void* arg)
{
    printf("Test Function arg = %d\n",arg);
    pet_thread_yield_to(3);
    printf("End Test Function arg = %d\n",arg);
    return 0;
}

void *
test_func3(void* arg)
{
    printf("Test Function arg = %d\n",arg);
    pet_thread_yield_to(1);
    printf("End Test Function arg = %d\n",arg);
    return 0;
}
//demo exit (uncomment only func4 comment other
void *
test_func4(void* arg)
{
    printf("Exiting= %d\n",arg);
    pet_thread_exit((void*)arg);
    printf("Test Function arg = %d\n",arg);
    return 0;
}

void *
test_func5(void* arg)
{
    pet_thread_join(6,NULL);
    printf("Test Function arg = %d\n",arg);

    return 0;
}

void *
test_func6(void* arg)
{
    printf("Test Function arg = %d\n",arg);
    return 0;
}

int main(int argc, char ** argv)
{
    pet_thread_id_t test_thread1,test_thread2,test_thread3, test_thread4,test_thread5,test_thread6;
    int ret = 0,ret1=0;

    ret = pet_thread_init();

    if (ret == -1) {
	ERROR("Could not initialize Pet Thread Library\n");
	return -1;
    }

    
    printf("Testing Pet Thread Library\n");

    ret = pet_thread_create(&test_thread1, test_func1, (void *)1);
    ret = pet_thread_create(&test_thread2, test_func2, (void *)2);
    ret = pet_thread_create(&test_thread3, test_func3, (void *)3);
    ret = pet_thread_create(&test_thread4, test_func4, (void *)4);
    ret = pet_thread_create(&test_thread5, test_func5, (void *)5);
    ret = pet_thread_create(&test_thread6, test_func6, (void *)6);

    if (ret == -1) {
	ERROR("Could not create test_thread1\n");
	return -1;
    }
    if (ret1 == -1) {
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
