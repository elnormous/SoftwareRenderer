//
//  SoftwareRenderer
//

#include <stdlib.h>
#include <string.h>
#ifdef __APPLE__
#include <sys/time.h>
#endif
#include "thread.h"

#if defined(_MSC_VER)
static const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
#endif

typedef struct ThreadData
{
    void(*function)(void*);
    void* argument;
    char* name;
}ThreadData;

#if defined(_MSC_VER)
static DWORD WINAPI threadFunction(LPVOID parameter)
#else
static void* threadFunction(void* parameter)
#endif
{
    ThreadData* threadData = (ThreadData*)parameter;

#ifdef __APPLE__
    if (threadData->name) pthread_setname_np(threadData->name);
#endif

    threadData->function(threadData->argument);

    if (threadData->name) free(threadData->name);
    free(threadData);

#if defined(_MSC_VER)
    return 0;
#else
    return NULL;
#endif
}

int gpThreadInit(GPThread* thread, void(*function)(void*), void* argument, const char* name)
{
    ThreadData* threadData = malloc(sizeof(ThreadData));
    threadData->function = function;
    threadData->argument = argument;
    if (name)
    {
        size_t size = strlen(name) + 1;
        threadData->name = malloc(size);
        memcpy(threadData->name, name, size);
    }
    else
        threadData->name = NULL;

#if defined(_MSC_VER)
    thread->handle = CreateThread(NULL, 0, threadFunction, threadData, 0, &thread->threadId);
    if (thread->handle == NULL) return 0;

    if (name)
    {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = thread->threadId;
        info.dwFlags = 0;

        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }

    return 1;
#else
    if (pthread_create(&thread->thread, NULL, threadFunction, threadData) != 0) return 0;

#ifndef __APPLE__
    if (name) pthread_setname_np(thread->thread, name);
#endif

    return 1;
#endif
}

int gpThreadDestroy(GPThread* thread)
{
#if defined(_MSC_VER)
    return CloseHandle(thread->handle);
#else
    return 1;
#endif
}

int gpThreadJoin(GPThread* thread)
{
#if defined(_MSC_VER)
    return WaitForSingleObject(thread->handle, INFINITE) != WAIT_FAILED;
#else
    return pthread_join(thread->thread, NULL) == 0;
#endif
}

int gpMutexInit(GPMutex* mutex)
{
#if defined(_MSC_VER)
    InitializeCriticalSection(&mutex->criticalSection);
    return 1;
#else
    return pthread_mutex_init(&mutex->mutex, NULL) == 0;
#endif
}

int gpMutexDestroy(GPMutex* mutex)
{
#if defined(_MSC_VER)
    DeleteCriticalSection(&mutex->criticalSection);
    return 1;
#else
    return pthread_mutex_destroy(&mutex->mutex) == 0;
#endif
}

int gpMutexLock(GPMutex* mutex)
{
#if defined(_MSC_VER)
    EnterCriticalSection(&mutex->criticalSection);
    return 1;
#else
    return pthread_mutex_lock(&mutex->mutex) == 0;
#endif
}

int gpMutexTryLock(GPMutex* mutex)
{
#if defined(_MSC_VER)
    return TryEnterCriticalSection(&mutex->criticalSection) != 0;
#else
    return pthread_mutex_trylock(&mutex->mutex) == 0;
#endif
}

int gpMutexUnlock(GPMutex* mutex)
{
#if defined(_MSC_VER)
    LeaveCriticalSection(&mutex->criticalSection);
    return 1;
#else
    return pthread_mutex_unlock(&mutex->mutex) == 0;
#endif
}

int gpConditionInit(GPCondition* condition)
{
#if defined(_MSC_VER)
    InitializeConditionVariable(&condition->conditionVariable);
    return 1;
#else
    return pthread_cond_init(&condition->condition, NULL) == 0;
#endif
}

int gpConditionDestroy(GPCondition* condition)
{
#if defined(_MSC_VER)
    return 1;
#else
    return pthread_cond_destroy(&condition->condition) == 0;
#endif
}

int gpConditionSignal(GPCondition* condition)
{
#if defined(_MSC_VER)
    WakeConditionVariable(&condition->conditionVariable);
    return 1;
#else
    return pthread_cond_signal(&condition->condition) == 0;
#endif
}

int gpConditionBroadcast(GPCondition* condition)
{
#if defined(_MSC_VER)
    WakeAllConditionVariable(&condition->conditionVariable);
    return 1;
#else
    return pthread_cond_broadcast(&condition->condition) == 0;
#endif
}

int gpConditionWait(GPCondition* condition, GPMutex* mutex)
{
#if defined(_MSC_VER)
    return SleepConditionVariableCS(&condition->conditionVariable, &mutex->criticalSection, INFINITE) != 0;
#else
    return pthread_cond_wait(&condition->condition, &mutex->mutex) == 0;
#endif
}

int gpConditionTimedWait(GPCondition* condition, GPMutex* mutex, uint64_t ns)
{
#if defined(_MSC_VER)
    return SleepConditionVariableCS(&condition->conditionVariable, &mutex->criticalSection, (DWORD)(ns / 1000000)) != 0;
#else
    static const long NANOSEC_PER_SEC = 1000000000L;
    struct timespec ts;

#ifdef __APPLE__ // OS X does not have clock_gettime, use gettimeofday
    struct timeval tv;
    gettimeofday(&tv, NULL);
    TIMEVAL_TO_TIMESPEC(&tv, &ts);
#else
    clock_gettime(CLOCK_REALTIME, &ts);
#endif

    ts.tv_nsec += ns;

    ts.tv_sec += (int32_t)(ts.tv_nsec / NANOSEC_PER_SEC);
    ts.tv_nsec %= NANOSEC_PER_SEC;

    return pthread_cond_timedwait(&condition->condition, &mutex->mutex, &ts) == 0;
#endif
}

int gpInterlockedCompareAndSwap(int32_t oldValue, int32_t newValue, int32_t* value)
{
#if defined(_MSC_VER)
    return InterlockedCompareExchange(value, newValue, oldValue);
#else
    return __sync_val_compare_and_swap(value, oldValue, newValue);
#endif
}
