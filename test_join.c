#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

/** dump_pthread_attr was built from display_pthread_attr which was copied from http://man7.org/linux/man-pages/man3/pthread_attr_init.3.html
  * which says:
Copyright for pthread_attr_init.3:

Copyright (c) 2008 Linux Foundation, written by Michael Kerrisk
    <mtk.manpages@gmail.com>
License for pthread_attr_init.3:

Permission is granted to make and distribute verbatim copies of this
manual provided the copyright notice and this permission notice are
preserved on all copies.

Permission is granted to copy and distribute modified versions of this
manual under the conditions for verbatim copying, provided that the
entire resulting derived work is distributed under the terms of a
permission notice identical to this one.

Since the Linux kernel and libraries are constantly changing, this
manual page may be incorrect or out-of-date.  The author(s) assume no
responsibility for errors or omissions, or for damages resulting from
the use of the information contained herein.  The author(s) may not
have taken the same level of care in the production of this manual,
which is licensed free of charge, as they might when working
professionally.

Formatted or processed versions of this manual, if unaccompanied by
the source, must acknowledge the copyright and authors of this work.
**/

#define CHECKLEFT { \
    if (rc == -1 || rc >= left) { \
      /* truncation, bail */ \
      return -1; \
    } \
    bp += rc; \
    left -= rc; \
}

static int dump_pthread_attr(char *buffer, size_t size, pthread_attr_t *attr)
{
  int s = 0, i = 0;
  size_t v = 0;
  void *stkaddr = NULL;
  struct sched_param sp;

  int rc = 0;
  char *bp = buffer;
  size_t left = size;

  s = pthread_attr_getdetachstate(attr, &i);
  rc = snprintf(bp, left, "Detach state        = %s\n",
    (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :
    (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :
    "???");
  CHECKLEFT

  s = pthread_attr_getscope(attr, &i);
  rc = snprintf(bp, left, "Scope               = %s\n",
    (i == PTHREAD_SCOPE_SYSTEM)  ? "PTHREAD_SCOPE_SYSTEM" :
    (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :
    "???");
  CHECKLEFT

  s = pthread_attr_getinheritsched(attr, &i);
  rc = snprintf(bp, left, "Inherit scheduler   = %s\n",
    (i == PTHREAD_INHERIT_SCHED)  ? "PTHREAD_INHERIT_SCHED" :
    (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED" :
    "???");
  CHECKLEFT

  s = pthread_attr_getschedpolicy(attr, &i);
  rc = snprintf(bp, left, "Scheduling policy   = %s\n",
    (i == SCHED_OTHER) ? "SCHED_OTHER" :
    (i == SCHED_FIFO)  ? "SCHED_FIFO" :
    (i == SCHED_RR)    ? "SCHED_RR" :
    "???");
  CHECKLEFT

  s = pthread_attr_getschedparam(attr, &sp);
  rc = snprintf(bp, left, "Scheduling priority = %d\n", sp.sched_priority);
  CHECKLEFT

  s = pthread_attr_getguardsize(attr, &v);
  rc = snprintf(bp, left, "Guard size          = %d bytes\n", v);
  CHECKLEFT

  s = pthread_attr_getstack(attr, &stkaddr, &v);
  rc = snprintf(bp, left, "Stack address       = %p\n", stkaddr);
  CHECKLEFT

  rc = snprintf(bp, left, "Stack size          = 0x%zx bytes\n", v);
  CHECKLEFT

  rc = snprintf(bp, left, "Stack size          = %zd MB\n", (v / 1024 / 1024)) ;
  CHECKLEFT

  return 0;
}

char *getThreadAttr() {
  int ret = 0;
  pthread_attr_t tattr;

  memset(&tattr, 0, sizeof(tattr));

  /* pthread_getattr_np() is a non-standard GNU extension that
  retrieves the attributes of the thread specified in its
  first argument */
  ret = pthread_getattr_np(pthread_self(), &tattr);
  if (0 != ret) {
    fprintf (stderr, "Error %s", strerror(ret));

    return NULL;
  }

  size_t buffersize = 8192;
  char *buffer=(char*)calloc(buffersize + 1, sizeof(char));

  dump_pthread_attr(buffer, buffersize, &tattr);

  return buffer;
}

void *child(void *arg) {
  long *retVal = (long *)calloc(1, sizeof(long));
  *retVal = 0xDEADCAFE;

  char *buffer = getThreadAttr();
  fprintf(stderr, "child attr %s\n", buffer);
  free(buffer);

  fprintf(stderr, "child returning %p\n", retVal);

  return retVal;
}

int main(int argc, char **argv) {
  char *buffer = getThreadAttr();
  fprintf(stderr, "main attr %s\n", buffer);
  free(buffer);

  pthread_t child_thread;
  pthread_attr_t attr;
  int ret = 0;

  ret = pthread_attr_init(&attr);
  if (0 != ret) {
    fprintf(stderr, "pthread_attr_init failed: %s\n", strerror(ret));
    return -1;
  }

  ret = pthread_attr_setstacksize(&attr, 5 * 1024 * 1024);
  if (0 != ret) {
    fprintf(stderr, "pthread_attr_setstacksize failed: %s\n", strerror(ret));
    return -2;
  }

  ret = pthread_create (&child_thread, &attr, child, NULL);
  if (0 != ret) {
    fprintf(stderr, "pthread_create failed: %s\n", strerror(ret));
    return -3;
  }

  void *retVal = NULL;

  ret = pthread_join(child_thread, &retVal);
  if (0 != ret) {
    fprintf(stderr, "pthread_join failed: %s\n", strerror(ret));
    return -4;
  }

  if (NULL != retVal) {
    fprintf(stderr, "got %p containing 0x%zx\n", retVal, *((long*)retVal));
    free(retVal);
  }
}
