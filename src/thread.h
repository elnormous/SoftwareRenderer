//
//  SoftwareRenderer
//

#pragma once

#include <stdint.h>
#if defined(_MSC_VER)
#include <Windows.h>
#else
#include <pthread.h>
#endif

typedef struct GPThread
{
#if defined(_MSC_VER)
    HANDLE handle;
    DWORD threadId;
#else
    pthread_t thread;
#endif
} GPThread;

typedef struct GPMutex
{
#if defined(_MSC_VER)
    CRITICAL_SECTION criticalSection;
#else
    pthread_mutex_t mutex;
#endif
} GPMutex;

typedef struct GPCondition
{
#if defined(_MSC_VER)
    CONDITION_VARIABLE conditionVariable;
#else
    pthread_cond_t condition;
#endif
} GPCondition;

int gpThreadInit(GPThread* thread, void(*function)(void*), void* argument, const char* name);
int gpThreadDestroy(GPThread* thread);
int gpThreadJoin(GPThread* thread);
int gpMutexInit(GPMutex* mutex);
int gpMutexDestroy(GPMutex* mutex);
int gpMutexLock(GPMutex* mutex);
int gpMutexTryLock(GPMutex* mutex);
int gpMutexUnlock(GPMutex* mutex);
int gpConditionInit(GPCondition* condition);
int gpConditionDestroy(GPCondition* condition);
int gpConditionSignal(GPCondition* condition);
int gpConditionBroadcast(GPCondition* condition);
int gpConditionWait(GPCondition* condition, GPMutex* mutex);
int gpConditionTimedWait(GPCondition* condition, GPMutex* mutex, uint64_t ns);
int gpInterlockedCompareAndSwap(int32_t oldValue, int32_t newValue, int32_t* value);
