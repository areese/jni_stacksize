/* Copyright 2017 Yahoo Inc. */
/* Licensed under the terms of the 3-Clause BSD license. See LICENSE file in the project root for details. */

#define _GNU_SOURCE     /* To get pthread_getattr_np() declaration */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "y_sizer_jni_StackSizeAccess.h"

#define RUNTIME_EXCEPTION "java/lang/RuntimeException"

static void ThrowException(JNIEnv *jenv, const char *exceptionClass, const char *message)
{
  jclass clazz = 0;

  if (!jenv || !exceptionClass) {
    return;
  }

  jenv->ExceptionClear();

  clazz = jenv->FindClass(exceptionClass);

  if (0 == clazz) {
    fprintf(stderr, "Error, cannot find exception class: %s", exceptionClass);
    return;
  }

  jenv->ThrowNew(clazz, message);
}

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

  return 0;
}


/*
 * Class:     y_sizer_jni_StackSizeAccess
 * Method:    getStackSize
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_y_sizer_jni_StackSizeAccess_getStackSize
  (JNIEnv *jenv, jclass)
{
  pthread_attr_t tattr;
  size_t size = 0;

  memset(&tattr, 0, sizeof(tattr));
  int ret = pthread_attr_getstacksize(&tattr, &size);

  if (0 != ret) {
    char error[1024] = {0,};
    snprintf (error, sizeof(error), "Error %s", strerror(ret));
    ThrowException(jenv, RUNTIME_EXCEPTION, error);

    return -1;
  }

  return (jlong)size;
}

/*
 * Class:     y_sizer_jni_StackSizeAccess
 * Method:    getThreadInfo
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_y_sizer_jni_StackSizeAccess_getThreadInfo
  (JNIEnv *jenv, jclass)
{
  int ret = 0;
  pthread_attr_t tattr;

  memset(&tattr, 0, sizeof(tattr));

  /* pthread_getattr_np() is a non-standard GNU extension that
  retrieves the attributes of the thread specified in its
  first argument */
  ret = pthread_getattr_np(pthread_self(), &tattr);
  if (0 != ret) {
    char error[1024] = {0,};
    snprintf (error, sizeof(error), "Error %s", strerror(ret));
    ThrowException(jenv, RUNTIME_EXCEPTION, error);

    return 0;
  }

  size_t buffersize = 8192;
  char *buffer=(char*)calloc(buffersize + 1, sizeof(char));

  dump_pthread_attr(buffer, buffersize, &tattr);

  jstring stringRet = jenv->NewStringUTF(buffer);
  free(buffer);

  return stringRet;
}
