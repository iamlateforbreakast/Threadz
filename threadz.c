/* Threadz.c */
#include <stdio.h>
#include <sys/timerfd.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>

struct Thread1Param
{
  int status;
};

struct Thread2Param
{
  int status;
};

struct MainParam
{
  pthread_cond_t cv;
  pthread_condattr_t cattr;
  pthread_t thread1Handle;
  pthread_t thread2Handle;
  struct Thread1Param thread1Param;
  struct Thread2Param thread2Param;
};

long double get_wall_time() 
{
  struct timeval time;
  if (gettimeofday(&time, NULL)) {
    //  Handle error
    return 0;
  }
  return (long double)time.tv_sec + (long double)time.tv_usec/1000000;
}

long double get_cpu_time() 
{
  return (long double)clock() / CLOCKS_PER_SEC;
}

void * thread1Body(void * p)
{
  struct Thread1Param * param = p;
  int tfd = timerfd_create(CLOCK_REALTIME,  0);
  //int tfd = timerfd_create(CLOCK_MONOTONIC,  0);
  long double t = 0.0;
  long double t0 = 0.0;
  long double t1 = 0.0;
  int cycle = -1;
  int duration = 0;
  int period = 100000; // 0.1 s
  int period_sec = period/1000000;
  int period_nsec = (period - (period_sec * 1000000)) * 1000;
 
  printf("Starting...\n");

  if(tfd > 0)
  {
    char dummybuf[8];
    struct itimerspec spec;

    /* Wait */
    fd_set rfds;
    int retval;

    spec.it_interval.tv_sec = period_sec;
    spec.it_interval.tv_nsec = period_nsec;
    spec.it_value.tv_sec = period_sec;
    spec.it_value.tv_nsec = period_nsec;

    timerfd_settime(tfd, 0, &spec, NULL);

    /* Watch timefd file descriptor */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    FD_SET(tfd, &rfds);
   
    /* Let's wait for initial timer expiration */
    retval = select(tfd+1, &rfds, NULL, NULL, NULL); /* Last parameter = NULL --> wait forever */
    t = get_cpu_time();
    t0 = get_wall_time();

    while (duration<5)
    {
      int retVal;

      /* Let's wait for initial timer expiration */
      retval = select(tfd+1, &rfds, NULL, NULL, NULL); /* Last parameter = NULL --> wait forever */
      t = get_cpu_time();
      t1 = get_wall_time();
      cycle = (cycle+1)%10;
      if (cycle==0) duration++;
      retVal = read(tfd, dummybuf, 8);
      printf("%Lf %Lf %d %d\n", t, t1, cycle, duration); 
    }
    param->status = 1;
    //getrusage(RUSAGE_THREAD, struct rusage *usage); // get thread CPU usage
    pthread_exit(&param->status);
  }
}

void * thread2Body(void * p)
{
  struct Thread2Param * param = p;
  printf("Thread 2 is active\n");
  param->status = 1;
  pthread_exit(&param->status);
}

int main()
{
  struct MainParam mainParam;

  int err1 = pthread_create(&mainParam.thread1Handle, NULL, &thread1Body, &mainParam.thread1Param);
  int err2 = pthread_create(&mainParam.thread2Handle, NULL, &thread2Body, &mainParam.thread2Param);

  //int pthread_setname_np(pthread_t thread, const char *name);

  pthread_detach(mainParam.thread1Handle);
  pthread_detach(mainParam.thread2Handle);
    
  sleep(10);
    
  pthread_join(mainParam.thread1Handle, NULL); 
  pthread_join(mainParam.thread2Handle, NULL); 

  return 0;
}
