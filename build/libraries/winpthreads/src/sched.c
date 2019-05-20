#include <windows.h>
#include <stdio.h>
#include "pthread.h"
#include "thread.h"

#include "misc.h"

int sched_get_priority_min(int pol)
{
  if (pol < SCHED_MIN || pol > SCHED_MAX) {
      errno = EINVAL;
      return -1;
  }

  return THREAD_PRIORITY_IDLE;
}

int sched_get_priority_max(int pol)
{
  if (pol < SCHED_MIN || pol > SCHED_MAX) {
      errno = EINVAL;
      return -1;
  }

  return THREAD_PRIORITY_TIME_CRITICAL;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *p)
{
    int r = 0;

    if (attr == NULL || p == NULL) {
        return EINVAL;
    }
    memcpy(&attr->param, p, sizeof (*p));
    return r;
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *p)
{
    int r = 0;

    if (attr == NULL || p == NULL) {
        return EINVAL;
    }
    memcpy(p, &attr->param, sizeof (*p));
    return r;
}

static int pthread_check(pthread_t t)
{
  DWORD dwFlags;
  if (!t.p)
    return ESRCH;
  else if (!(t.p->h) || t.p->h == INVALID_HANDLE_VALUE)
  {
  	if (t.p->ended == 0)
  	  return 0;
        return ESRCH;
  }
  else if ((!GetHandleInformation(t.p->h, &dwFlags) && t.p->ended))
        return ESRCH;
  return 0;
}

int pthread_getschedparam(pthread_t t, int *pol, struct sched_param *p)
{
    int r;
    //if (!t.p)
    //  t = pthread_self();

    if ((r = pthread_check(t)) != 0)
    {
        return r;
    }

    if (!p || !pol)
    {
        return EINVAL;
    }
    *pol = t.p->sched_pol;
    p->sched_priority = t.p->sched.sched_priority;

    return 0;
}

int pthread_setschedparam(pthread_t t, int pol,  const struct sched_param *p)
{
    int r, pr = 0;
    //if (!t.p) t = pthread_self();

    if ((r = pthread_check(t)) != 0)
        return r;

    if (pol < SCHED_MIN || pol > SCHED_MAX || p == NULL)
        return EINVAL;
    if (pol != SCHED_OTHER)
        return ENOTSUP;
    pr = p->sched_priority;
    if (pr < sched_get_priority_min(pol) || pr > sched_get_priority_max(pol))
      return EINVAL;

    /* See msdn: there are actually 7 priorities:
    THREAD_PRIORITY_IDLE    -      -15
    THREAD_PRIORITY_LOWEST          -2
    THREAD_PRIORITY_BELOW_NORMAL    -1
    THREAD_PRIORITY_NORMAL           0
    THREAD_PRIORITY_ABOVE_NORMAL     1
    THREAD_PRIORITY_HIGHEST          2
    THREAD_PRIORITY_TIME_CRITICAL   15
    */
    if (pr <= THREAD_PRIORITY_IDLE) {
        pr = THREAD_PRIORITY_IDLE;
    } else if (pr <= THREAD_PRIORITY_LOWEST) {
        pr = THREAD_PRIORITY_LOWEST;
    } else if (pr >= THREAD_PRIORITY_TIME_CRITICAL) {
        pr = THREAD_PRIORITY_TIME_CRITICAL;
    } else if (pr >= THREAD_PRIORITY_HIGHEST) {
        pr = THREAD_PRIORITY_HIGHEST;
    }

    if (SetThreadPriority(t.p->h, pr)) {
        t.p->sched_pol = pol;
        t.p->sched.sched_priority = p->sched_priority;
    } else
        r = EINVAL;
    return r;
}

int sched_getscheduler(pid_t pid)
{
  if (pid != 0)
  {
      HANDLE h = NULL;
      int selfPid = (int) GetCurrentProcessId ();

      if (pid != (pid_t) selfPid && (h = OpenProcess (PROCESS_QUERY_INFORMATION, 0, (DWORD) pid)) == NULL)
      {
	  errno = (GetLastError () == (0xFF & ERROR_ACCESS_DENIED)) ? EPERM : ESRCH;
	  return -1;
      }
      if (h)
	  CloseHandle (h);
  }
  return SCHED_OTHER;
}

int sched_setscheduler(pid_t pid, int pol)
{
  if (pid != 0)
  {
      HANDLE h = NULL;
      int selfPid = (int) GetCurrentProcessId ();

      if (pid != (pid_t) selfPid && (h = OpenProcess (PROCESS_SET_INFORMATION, 0, (DWORD) pid)) == NULL)
      {
	  errno = (GetLastError () == (0xFF & ERROR_ACCESS_DENIED)) ? EPERM : ESRCH;
	  return -1;
      }
      if (h)
          CloseHandle (h);
  }

  if (pol != SCHED_OTHER)
  {
      errno = ENOSYS;
      return -1;
  }
  return SCHED_OTHER;
}

int sched_yield(void)
{
  Sleep(0);
  return 0;
}
