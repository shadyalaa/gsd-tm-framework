#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <immintrin.h>
#include <rtmintrin.h>

#include "thread.h"

__attribute__((aligned(CACHE_LINE_SIZE))) padded_scalar_t fallback_in_use;
__attribute__((aligned(CACHE_LINE_SIZE))) padded_scalar_t exists_sw;
__attribute__((aligned(CACHE_LINE_SIZE))) __thread unsigned short read_only_htm;
__attribute__((aligned(CACHE_LINE_SIZE))) padded_statistics_t statistics_array[128];

void freeHTM(void* ptr) {
    free(ptr);
}

void abortHTM(norec_Thread* norec_Self) {
    _xabort(0xab);
}
intptr_t sharedReadHTM(norec_Thread* norec_Self, intptr_t* addr) {
    return *addr;
}
void sharedWriteHTM(norec_Thread* norec_Self, intptr_t* addr, intptr_t val) {
    *addr = val;
    read_only_htm = 0;
}

void freeSTM(void* ptr) {
}

void abortSTM(norec_Thread* norec_Self) {
    norec_STM_RESTART();
}
intptr_t sharedReadSTM(norec_Thread* norec_Self, intptr_t* addr) {
    return norec_STM_HYBRID_READ(addr);
}
void sharedWriteSTM(norec_Thread* norec_Self, intptr_t* addr, intptr_t val) {
    norec_STM_HYBRID_WRITE(addr, val);
}

__thread void (*freeFunPtr)(void* ptr);
__thread void (*abortFunPtr)(norec_Thread* norec_Self);
__thread intptr_t (*sharedReadFunPtr)(norec_Thread* norec_Self, intptr_t* addr);
__thread void (*sharedWriteFunPtr)(norec_Thread* norec_Self, intptr_t* addr, intptr_t val);


#ifndef REDUCED_TM_API

#include <assert.h>
#include <stdlib.h>
#include <sched.h>
#include "types.h"
#include "rapl.h"

static THREAD_LOCAL_T    global_threadId;
static long              global_numThread       = 1;
static THREAD_BARRIER_T* global_barrierPtr      = NULL;
static long*             global_threadIds       = NULL;
static THREAD_ATTR_T     global_threadAttr;
static THREAD_T*         global_threads         = NULL;
static void            (*global_funcPtr)(void*) = NULL;
static void*             global_argPtr          = NULL;
static volatile bool_t   global_doShutdown      = FALSE;


static void threadWait (void* argPtr)
{
    long threadId = *(long*)argPtr;
    
    THREAD_LOCAL_SET(global_threadId, (long)threadId);

    bindThread(threadId);

    while (1) {
        THREAD_BARRIER(global_barrierPtr, threadId); /* wait for start parallel */
        if (global_doShutdown) {
            break;
        }
        global_funcPtr(global_argPtr);
        THREAD_BARRIER(global_barrierPtr, threadId); /* wait for end parallel */
        if (threadId == 0) {
        	endEnergy();
            break;
        }
    }
}

void thread_startup (long numThread)
{
    long i;

    global_numThread = numThread;
    global_doShutdown = FALSE;

    /* Set up barrier */
    assert(global_barrierPtr == NULL);
    global_barrierPtr = THREAD_BARRIER_ALLOC(numThread);
    assert(global_barrierPtr);
    THREAD_BARRIER_INIT(global_barrierPtr, numThread);

    /* Set up ids */
    THREAD_LOCAL_INIT(global_threadId);
    assert(global_threadIds == NULL);
    global_threadIds = (long*)malloc(numThread * sizeof(long));
    assert(global_threadIds);
    for (i = 0; i < numThread; i++) {
        global_threadIds[i] = i;
    }

    /* Set up thread list */
    assert(global_threads == NULL);
    global_threads = (THREAD_T*)malloc(numThread * sizeof(THREAD_T));
    assert(global_threads);

	startEnergy();

    /* Set up pool */
    THREAD_ATTR_INIT(global_threadAttr);
    for (i = 1; i < numThread; i++) {
        THREAD_CREATE(global_threads[i],
                      global_threadAttr,
                      &threadWait,
                      &global_threadIds[i]);
    }

    /*
     * Wait for primary thread to call thread_start
     */
}


void thread_start (void (*funcPtr)(void*), void* argPtr)
{
    global_funcPtr = funcPtr;
    global_argPtr = argPtr;

    long threadId = 0; /* primary */
    threadWait((void*)&threadId);
}


void thread_shutdown ()
{
    /* Make secondary threads exit wait() */
    global_doShutdown = TRUE;
    THREAD_BARRIER(global_barrierPtr, 0);

    long numThread = global_numThread;

    long i;
    for (i = 1; i < numThread; i++) {
        THREAD_JOIN(global_threads[i]);
    }

    THREAD_BARRIER_FREE(global_barrierPtr);
    global_barrierPtr = NULL;

    free(global_threadIds);
    global_threadIds = NULL;

    free(global_threads);
    global_threads = NULL;

    global_numThread = 1;
}

barrier_t *barrier_alloc() {
    return (barrier_t *)malloc(sizeof(barrier_t));
}

void barrier_free(barrier_t *b) {
    free(b);
}

void barrier_init(barrier_t *b, int n) {
    pthread_cond_init(&b->complete, NULL);
    pthread_mutex_init(&b->mutex, NULL);
    b->count = n;
    b->crossing = 0;
}

void barrier_cross(barrier_t *b) {
    pthread_mutex_lock(&b->mutex);
    /* One more thread through */
    b->crossing++;
    /* If not all here, wait */
    if (b->crossing < b->count) {
        pthread_cond_wait(&b->complete, &b->mutex);
    } else {
        /* Reset for next time */
        b->crossing = 0;
        pthread_cond_broadcast(&b->complete);
    }
    pthread_mutex_unlock(&b->mutex);
}

void thread_barrier_wait()
{
    long threadId = thread_getId();
    THREAD_BARRIER(global_barrierPtr, threadId);
}


long thread_getId()
{
    return (long)THREAD_LOCAL_GET(global_threadId);
}


long thread_getNumThread()
{
    return global_numThread;
}

#endif
