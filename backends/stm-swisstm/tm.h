#ifndef TM_H
#define TM_H 1

#  include <stdio.h>

#  define MAIN(argc, argv)              int main (int argc, char** argv)
#  define MAIN_RETURN(val)              return val

#  define GOTO_SIM()                    /* nothing */
#  define GOTO_REAL()                   /* nothing */
#  define IS_IN_SIM()                   (0)

#  define SIM_GET_NUM_CPU(var)          /* nothing */

#  define TM_PRINTF                     printf
#  define TM_PRINT0                     printf
#  define TM_PRINT1                     printf
#  define TM_PRINT2                     printf
#  define TM_PRINT3                     printf

#  define P_MEMORY_STARTUP(numThread)   /* nothing */
#  define P_MEMORY_SHUTDOWN()           /* nothing */

#  include <string.h>
#  include <stm.h>
#ifndef REDUCED_TM_API
#  include "thread.h"
#endif

#ifdef REDUCED_TM_API
#    define SPECIAL_THREAD_ID()         get_tid()
#else
#    define SPECIAL_THREAD_ID()         thread_getId()
#endif

#define TM_ARGDECL_ALONE                tx_desc* tx
#define TM_ARGDECL                      tx_desc* tx,
#define TM_ARG                          tx,
#define TM_ARG_ALONE                    tx
#    define TM_CALLABLE                   /* nothing */

# define SETUP_NUMBER_TASKS(p);
# define SETUP_NUMBER_THREADS(p);
# define PRINT_STATS()

#define TM_STARTUP(numThread,useless)     wlpdstm_global_init()

#      define TM_SHUTDOWN() { \
    wlpdstm_global_shutdown(); \
    unsigned long commits = 0; \
    unsigned long aborts = 0; \
    int i = 0; \
    for (; i < 128; i++) { \
       if (statistics_array[i].commits == 0) { break; } \
       commits += statistics_array[i].commits; \
       aborts += statistics_array[i].aborts; \
    } \
    printf("Total commits: %lu\nTotal aborts: %lu\n", commits, aborts); \
}

# ifdef REDUCED_TM_API
#define TM_THREAD_ENTER()         wlpdstm_thread_init();
#define TM_THREAD_EXIT()          wlpdstm_thread_shutdown();
#else
#define TM_THREAD_ENTER()         wlpdstm_thread_init(); \
                                  tx_desc *tx = wlpdstm_get_tx_desc(); \
                                  wlpdstm_start_thread_profiling_desc(tx)
#define TM_THREAD_EXIT()          wlpdstm_end_thread_profiling_desc(tx); \
                                  wlpdstm_thread_shutdown()
#endif

#define P_MALLOC(size)            malloc(size)
#define P_FREE(ptr)               free(ptr)
#define TM_MALLOC(size)           malloc(size)
#      define FAST_PATH_FREE(ptr)  
#      define SLOW_PATH_FREE(ptr)  

#ifdef REDUCED_TM_API

#define TM_BEGIN(b,m) \
	{ \
        sigsetjmp(*wlpdstm_get_long_jmp_buf(), 0); \
        wlpdstm_start_tx(); \
        statistics_array[SPECIAL_THREAD_ID()].aborts++;

#define TM_END() \
	    wlpdstm_commit_tx(); \
	    statistics_array[SPECIAL_THREAD_ID()].aborts--; \
        statistics_array[SPECIAL_THREAD_ID()].commits++; \
	} \

#else
#define TM_BEGIN(b,m) \
	{ \
        unsigned int counter_stm_executions = 0; \
        sigsetjmp(*wlpdstm_get_long_jmp_buf(), 0); \
        wlpdstm_start_tx_desc(tx); \
        statistics_array[SPECIAL_THREAD_ID()].aborts++;

#define TM_END()   \
        wlpdstm_commit_tx_desc(tx); \
        statistics_array[SPECIAL_THREAD_ID()].aborts--; \
        statistics_array[SPECIAL_THREAD_ID()].commits++; \
    } \

#define FAST_PATH_RESTART()              wlpdstm_restart_tx_desc(tx)
#define SLOW_PATH_RESTART() FAST_PATH_RESTART()
#endif

# define TM_BEGIN_EXT(b,mode,ro)     TM_BEGIN(b,mode)
#  define TM_EARLY_RELEASE(var)         /* nothing */

# ifdef REDUCED_TM_API
#  define FAST_PATH_SHARED_READ(var)      wlpdstm_read_word((Word *)(&var))
#  define FAST_PATH_SHARED_WRITE(var, val)     wlpdstm_write_word((Word *)(&var), (Word)(val))
#else
#  define FAST_PATH_SHARED_READ(var)      wlpdstm_read_word_desc(tx, (Word *)(&var))
#  define FAST_PATH_SHARED_READ_P(var)   (void *)wlpdstm_read_word_desc(tx, (Word *)(&var))
#  define FAST_PATH_SHARED_READ_D(var)   wlpdstm_read_double_desc(tx, (&var))
#  define FAST_PATH_SHARED_READ_F(var)   wlpdstm_read_float_desc(tx, (&var))
#  define FAST_PATH_SHARED_WRITE(var, val)     wlpdstm_write_word_desc(tx, (Word *)(&var), (Word)(val))
#  define FAST_PATH_SHARED_WRITE_P(var, val)   wlpdstm_write_word_desc(tx, (Word *)(&var), (Word)(val))
#  define FAST_PATH_SHARED_WRITE_D(var, val)   wlpdstm_write_double_desc(tx, (&var), (val))
#endif

# define SLOW_PATH_SHARED_READ(var)           FAST_PATH_SHARED_READ(var)
# define SLOW_PATH_SHARED_READ_P(var)         FAST_PATH_SHARED_READ_P(var)
# define SLOW_PATH_SHARED_READ_F(var)         FAST_PATH_SHARED_READ_F(var)
# define SLOW_PATH_SHARED_READ_D(var)         FAST_PATH_SHARED_READ_D(var)
# define SLOW_PATH_SHARED_WRITE(var, val)     FAST_PATH_SHARED_WRITE(var, val)
# define SLOW_PATH_SHARED_WRITE_P(var, val)   FAST_PATH_SHARED_WRITE_P(var, val)
# define SLOW_PATH_SHARED_WRITE_D(var, val)   FAST_PATH_SHARED_WRITE_D(var, val)

#define TM_LOCAL_WRITE(var, val)      ({var = val; var;})
#define TM_LOCAL_WRITE_P(var, val)    ({var = val; var;})
#define TM_LOCAL_WRITE_D(var, val)    ({var = val; var;})

#endif /* TM_H */

