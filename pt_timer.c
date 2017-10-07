
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <limits.h>

int time_to_yield = 0;

void timer_handler (int signum)
{
    static int count = 0;
    time_to_yield = 1;
    printf (" TMR:%d ", ++count);
}

void set_timer_interval(unsigned long int interval)
{
    struct sigaction sa;
    struct itimerval timer;

    /* Install timer_handler as the signal handler for SIGVTALRM. */
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &timer_handler;
    sigaction (SIGVTALRM, &sa, NULL);

    /* Configure the timer to expire after 250 msec... */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = interval;
    /* ... and every 250 msec after that. */
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = interval;
    /* Start a virtual timer. It counts down whenever this process is
      executing. */
    setitimer (ITIMER_VIRTUAL, &timer, NULL);

}

/*
int main() {
    pt_set_timer_interval(250000);
    // Do busy work.
    IdleCounter();
}
*/



/*
int IdleCounter()
{
//ULONG_MAX
//UINT_MAX =  0xffffffff
//UNIT_MAX/256  = 0xffffff = 16777215)
    for (unsigned long int i=0xff;i>0;i--) {
        for (unsigned long int j=0xffffff;j>0;j--) {
        }
        printf("IC %p \n",i);
    }
}
*/
