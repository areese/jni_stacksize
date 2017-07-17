/* Copyright 2017 Yahoo Inc. */
/* Licensed under the terms of the 3-Clause BSD license. See LICENSE file in the project root for details. */

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
