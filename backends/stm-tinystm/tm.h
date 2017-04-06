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
#  include <stm_tinystm.h>
#ifndef REDUCED_TM_API
#  include "thread.h"
#endif

#    define TM_ARG                        /* nothing */
#    define TM_ARG_ALONE                  /* nothing */
#    define TM_ARGDECL                    /* nothing */
#    define TM_ARGDECL_ALONE              /* nothing */
#    define TM_CALLABLE                   /* nothing */

#      include <mod_mem.h>
#      include <mod_stats.h>

#ifdef REDUCED_TM_API
#    define SPECIAL_THREAD_ID()         get_tid()
#else
#    define SPECIAL_THREAD_ID()         thread_getId()
#endif

# define SETUP_NUMBER_TASKS(p);
# define SETUP_NUMBER_THREADS(p);
# define PRINT_STATS()

#      define TM_STARTUP(numThread,useless)     if (sizeof(long) != sizeof(void *)) { \
                                          fprintf(stderr, "Error: unsupported long and pointer sizes\n"); \
                                          exit(1); \
                                        } \
                                        stm_init(); \
                                        mod_mem_init(0); \
                                        if (getenv("STM_STATS") != NULL) { \
                                          mod_stats_init(); \
                                        }
#      define TM_SHUTDOWN()             \
                                               if (getenv("STM_STATS") != NULL) { \
                                          unsigned long u; \
                                          if (stm_get_global_stats("global_nb_commits", &u) != 0) \
                                            printf("#commits    : %lu\n", u); \
                                          if (stm_get_global_stats("global_nb_aborts", &u) != 0) \
                                            printf("#aborts     : %lu\n", u); \
                                          if (stm_get_global_stats("global_max_retries", &u) != 0) \
                                            printf("Max retries : %lu\n", u); \
                                        } \
                                        stm_exit(); \
    unsigned long commits = 0; \
    unsigned long aborts = 0; \
    int ik = 0; \
    for (; ik < 128; ik++) { \
       if (statistics_array[ik].commits == 0) { break; } \
       commits += statistics_array[ik].commits; \
       aborts += statistics_array[ik].aborts; \
    } \
    printf("Total commits: %lu\nTotal aborts: %lu\n", commits, aborts); \


#      define TM_THREAD_ENTER()         stm_init_thread()
#      define TM_THREAD_EXIT()          stm_exit_thread()

#      define P_MALLOC(size)            malloc(size)
#      define P_FREE(ptr)               free(ptr)
#      define TM_MALLOC(size)           stm_malloc(size)
#      define FAST_PATH_FREE(ptr)        stm_free(ptr, sizeof(stm_word_t))
#      define SLOW_PATH_FREE(ptr)        FAST_PATH_FREE(ptr)


# define TM_BEGIN(b,mode)           TM_BEGIN_EXT(b,mode,0)
#    define TM_BEGIN_EXT(b,m,ro)   \
    { \
        sigjmp_buf buf; \
        sigsetjmp(buf, 0); \
        stm_tx_attr_t _a = {}; \
        _a.read_only = ro; \
        stm_start(_a, &buf); \
        sigsetjmp(buf, 0); \
        statistics_array[SPECIAL_THREAD_ID()].aborts++; \

#    define TM_END()   \
    stm_commit(); \
    statistics_array[SPECIAL_THREAD_ID()].aborts--; \
    statistics_array[SPECIAL_THREAD_ID()].commits++; \
}

#    define FAST_PATH_RESTART()                stm_abort(0)
#    define SLOW_PATH_RESTART()                FAST_PATH_RESTART()

#    define TM_EARLY_RELEASE(var)       /* nothing */

#  include <wrappers.h>


#  define FAST_PATH_SHARED_READ(var)     stm_load((volatile stm_word_t *)(void *)&(var))
#  define FAST_PATH_SHARED_READ_P(var)   stm_load_ptr((volatile void **)(void *)&(var))
#  define FAST_PATH_SHARED_READ_D(var)   stm_load_double((volatile double *)(void *)&(var))
#  define FAST_PATH_SHARED_READ_F(var)   stm_load_float((volatile float *)(void *)&(var))
#  define FAST_PATH_SHARED_WRITE(var, val)     stm_store((volatile stm_word_t *)(void *)&(var), (stm_word_t)val)
#  define FAST_PATH_SHARED_WRITE_P(var, val)   stm_store_ptr((volatile void **)(void *)&(var), val)
#  define FAST_PATH_SHARED_WRITE_D(var, val)   stm_store_double((volatile double *)(void *)&(var), val)

# define SLOW_PATH_SHARED_READ(var)           FAST_PATH_SHARED_READ(var)
# define SLOW_PATH_SHARED_READ_P(var)         FAST_PATH_SHARED_READ_P(var)
# define SLOW_PATH_SHARED_READ_F(var)         FAST_PATH_SHARED_READ_F(var)
# define SLOW_PATH_SHARED_READ_D(var)         FAST_PATH_SHARED_READ_D(var)
# define SLOW_PATH_SHARED_WRITE(var, val)     FAST_PATH_SHARED_WRITE(var, val)
# define SLOW_PATH_SHARED_WRITE_P(var, val)   FAST_PATH_SHARED_WRITE_P(var, val)
# define SLOW_PATH_SHARED_WRITE_D(var, val)   FAST_PATH_SHARED_WRITE_D(var, val)

#  define TM_LOCAL_WRITE(var, val)      ({var = val; var;})
#  define TM_LOCAL_WRITE_P(var, val)    ({var = val; var;})
#  define TM_LOCAL_WRITE_D(var, val)    ({var = val; var;})

#endif /* TM_H */

