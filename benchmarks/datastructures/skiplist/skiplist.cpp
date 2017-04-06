#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "random.h"
#include "timer.h"

#include "rapl.h"

#define DEFAULT_DURATION                10000
#define DEFAULT_INITIAL                 256
#define DEFAULT_NB_THREADS              1
#define DEFAULT_RANGE                   0xFFFF
#define DEFAULT_SEED                    0
#define DEFAULT_UPDATE                  20

#define XSTR(s)                         STR(s)
#define STR(s)                          #s

/* ################################################################### *
 * GLOBALS
 * ################################################################### */

#include "tm.h"

#define VAL_MIN                         INT_MIN
#define VAL_MAX                         INT_MAX


unsigned int levelmax;
#define MAXLEVEL    32


typedef struct node {
  long val;
  int toplevel;
  struct node *next[1];
} node_t;

typedef struct intset {
  node_t *head;
} intset_t;

__thread random_t* randomPtr;;
intset_t* set;

static volatile int stop;

int get_rand_level() {
	int i, level = 1;
	for (i = 0; i < levelmax - 1; i++) {
		if ((random_generate(randomPtr) % 100) < 50)
			level++;
		else
			break;
	}
	/* 1 <= level <= levelmax */
	return level;
}

node_t* new_simple_node(long val, int toplevel) {
  node_t *node;
  node = (node_t *)TM_MALLOC(sizeof(node_t) + toplevel * sizeof(node_t *));
  node->val = val;
  node->toplevel = toplevel;

  return node;
}

node_t* seq_new_simple_node(long val, int toplevel) {
  node_t *node;
  node = (node_t *)malloc(sizeof(node_t) + toplevel * sizeof(node_t *));
  node->val = val;
  node->toplevel = toplevel;

  return node;
}

node_t* new_node(long val, node_t *next, int toplevel) {
  node_t *node;
  int i;

  node = seq_new_simple_node(val, toplevel);
  for (i = 0; i < levelmax; i++)
    node->next[i] = next;

  return node;
}

intset_t *set_new() {
	intset_t *set;
	node_t *min, *max;

	set = (intset_t *)malloc(sizeof(intset_t));
	max = new_node(VAL_MAX, NULL, levelmax);
	min = new_node(VAL_MIN, max, levelmax);
	set->head = min;
	return set;
}

int set_size(intset_t *set)
{
  unsigned long size = 0;
  node_t *node;

  node = set->head->next[0];
  while (node->next[0] != NULL) {
	  size++;
    node = node->next[0];
  }

  return size;
}

int set_seq_add(long val) {
        int i, l, result;
        node_t *node, *next;
        node_t *preds[MAXLEVEL], *succs[MAXLEVEL];

        node = set->head;
        for (i = node->toplevel-1; i >= 0; i--) {
                next = node->next[i];
                while (next->val < val) {
                        node = next;
                        next = node->next[i];
                }
                preds[i] = node;
                succs[i] = node->next[i];
        }
        node = node->next[0];
        if ((result = (node->val != val)) == 1) {
                l = get_rand_level();
                node = seq_new_simple_node(val, l);
                for (i = 0; i < l; i++) {
                        node->next[i] = succs[i];
                        preds[i]->next[i] = node;
                }
        }
        return result;
}



long set_add(TM_ARGDECL long val)
{
    int res = 0;


	int mode = 0;
	TM_BEGIN(0,mode);
	if (mode == 0) {
		int i, l;
		node_t *node, *next;
		node_t *preds[MAXLEVEL];
		long v;
		v = VAL_MIN;
		node = set->head;
		for (i = node->toplevel-1; i >= 0; i--) {
			next = FAST_PATH_SHARED_READ_P(node->next[i]);
			while ((v = FAST_PATH_SHARED_READ(next->val)) < val) {
				node = next;
				next = FAST_PATH_SHARED_READ_P(node->next[i]);
			}
			preds[i] = node;
		}
		if ((res = (v != val)) == 1) {
			l = get_rand_level();
			node = new_simple_node(val, l);
			for (i = 0; i < l; i++) {
				node->next[i] = FAST_PATH_SHARED_READ_P(preds[i]->next[i]);
				FAST_PATH_SHARED_WRITE_P(preds[i]->next[i], node);
			}
		}
	} else {
		int i, l;
		node_t *node, *next;
		node_t *preds[MAXLEVEL];
		long v;
		v = VAL_MIN;
		node = set->head;
		for (i = node->toplevel-1; i >= 0; i--) {
			next = SLOW_PATH_SHARED_READ_P(node->next[i]);
			while ((v = SLOW_PATH_SHARED_READ(next->val)) < val) {
				node = next;
				next = SLOW_PATH_SHARED_READ_P(node->next[i]);
			}
			preds[i] = node;
		}
		if ((res = (v != val)) == 1) {
			l = get_rand_level();
			node = new_simple_node(val, l);
			for (i = 0; i < l; i++) {
				node->next[i] = SLOW_PATH_SHARED_READ_P(preds[i]->next[i]);
				SLOW_PATH_SHARED_WRITE_P(preds[i]->next[i], node);
			}
		}
	}
	TM_END();

    return res;
}

int set_remove(TM_ARGDECL long val)
{
    int res = 0;

	int mode = 0;
	TM_BEGIN(1,mode);
	if (mode == 0) {
		int i;
		node_t *node, *next = NULL;
		node_t *preds[MAXLEVEL], *succs[MAXLEVEL];
		long v;
		v = VAL_MIN;
		node = set->head;
		for (i = node->toplevel-1; i >= 0; i--) {
			next = FAST_PATH_SHARED_READ_P(node->next[i]);
			while ((v = FAST_PATH_SHARED_READ(next->val)) < val) {
				node = next;
				next = FAST_PATH_SHARED_READ_P(node->next[i]);
			}
			preds[i] = node;
			succs[i] = next;
		}
		if ((res = (next->val == val))) {
			for (i = 0; i < set->head->toplevel; i++) {
				if (succs[i]->val == val) {
					FAST_PATH_SHARED_WRITE_P(preds[i]->next[i], FAST_PATH_SHARED_READ_P(succs[i]->next[i]));
				}
			}
			FAST_PATH_FREE(next);
		}
	} else {
		int i;
		node_t *node, *next = NULL;
		node_t *preds[MAXLEVEL], *succs[MAXLEVEL];
		long v;
		v = VAL_MIN;
		node = set->head;
		for (i = node->toplevel-1; i >= 0; i--) {
			next = SLOW_PATH_SHARED_READ_P(node->next[i]);
			while ((v = SLOW_PATH_SHARED_READ(next->val)) < val) {
				node = next;
				next = SLOW_PATH_SHARED_READ_P(node->next[i]);
			}
			preds[i] = node;
			succs[i] = next;
		}
		if ((res = (next->val == val))) {
			for (i = 0; i < set->head->toplevel; i++) {
				if (succs[i]->val == val) {
					SLOW_PATH_SHARED_WRITE_P(preds[i]->next[i], SLOW_PATH_SHARED_READ_P(succs[i]->next[i]));
				}
			}
			SLOW_PATH_FREE(next);
		}
	}
	TM_END();

    return res;
}

long set_contains(TM_ARGDECL long  val)
{
    long res = 0;

	int mode = 0;
	TM_BEGIN_EXT(2,mode,1);
    if (mode == 0) {
    	int i;
    	node_t *node, *next;
    	long v = VAL_MIN;
        node = set->head;
        for (i = node->toplevel-1; i >= 0; i--) {
        	next = FAST_PATH_SHARED_READ_P(node->next[i]);
        	while ((v = FAST_PATH_SHARED_READ(next->val)) < val) {
        		node = next;
        		next = FAST_PATH_SHARED_READ_P(node->next[i]);
        	}
        }
        node = FAST_PATH_SHARED_READ_P(node->next[0]);
        res = (v == val);
    } else {
    	int i;
    	node_t *node, *next;
    	long v = VAL_MIN;
        node = set->head;
        for (i = node->toplevel-1; i >= 0; i--) {
        	next = SLOW_PATH_SHARED_READ_P(node->next[i]);
        	while ((v = SLOW_PATH_SHARED_READ(next->val)) < val) {
        		node = next;
        		next = SLOW_PATH_SHARED_READ_P(node->next[i]);
        	}
        }
        node = SLOW_PATH_SHARED_READ_P(node->next[0]);
        res = (v == val);
    }
    TM_END();

    return res;
}

#include <sched.h>
  long range;
  int update;
  unsigned long nb_add;
  unsigned long nb_remove;
  unsigned long nb_contains;
  unsigned long nb_found;
  unsigned long nb_aborts;
  unsigned int nb_threads;
  unsigned int seed;
  long operations;

void *test(void *data)
{

  TM_THREAD_ENTER();

  randomPtr = random_alloc();

  unsigned int mySeed = seed + sched_getcpu();

  long myOps = operations / nb_threads;
  long val = -1;
  int op;

  while (myOps > 0) {
    op = rand_r(&mySeed) % 100;
    if (op < update) {
      if (val == -1) {
        /* Add random value */  
        val = (rand_r(&mySeed) % range) + 1;
        if(set_add(TM_ARG val) == 0) {
          val = -1;
        }
      } else {
        /* Remove random value */
        int res = set_remove(TM_ARG  val);
        val = -1;
      }
    } else {
      /* Look for random value */
      long tmp = (rand_r(&mySeed) % range) + 1;
      set_contains(TM_ARG tmp);
    }

    myOps--;
  }

  TM_THREAD_EXIT();
  return NULL;
}

# define no_argument        0
# define required_argument  1
# define optional_argument  2

int floor_log_2(unsigned int n) {
  int pos = 0;
  if (n >= 1<<16) { n >>= 16; pos += 16; }
  if (n >= 1<< 8) { n >>=  8; pos +=  8; }
  if (n >= 1<< 4) { n >>=  4; pos +=  4; }
  if (n >= 1<< 2) { n >>=  2; pos +=  2; }
  if (n >= 1<< 1) {           pos +=  1; }
  return ((n == 0) ? (-1) : pos);
}

MAIN(argc, argv) {
    TIMER_T start;
    TIMER_T stop;


  struct option long_options[] = {
    // These options don't set a flag
    {"help",                      no_argument,       NULL, 'h'},
    {"duration",                  required_argument, NULL, 'd'},
    {"initial-size",              required_argument, NULL, 'i'},
    {"num-threads",               required_argument, NULL, 'n'},
    {"range",                     required_argument, NULL, 'r'},
    {"seed",                      required_argument, NULL, 's'},
    {"buckets",                   required_argument, NULL, 'b'},
    {"update-rate",               required_argument, NULL, 'u'},
    {NULL, 0, NULL, 0}
  };

  int i, c;
  long val;
  operations = DEFAULT_DURATION;
  unsigned int initial = DEFAULT_INITIAL;
  nb_threads = DEFAULT_NB_THREADS;
  range = DEFAULT_RANGE;
  update = DEFAULT_UPDATE;

  levelmax = 6; //floor_log_2((unsigned int) initial);

  while(1) {
    i = 0;
    c = getopt_long(argc, argv, "hd:i:n:b:r:s:u:", long_options, &i);

    if(c == -1)
      break;

    if(c == 0 && long_options[i].flag == 0)
      c = long_options[i].val;

    switch(c) {
     case 0:
       /* Flag is automatically set */
       break;
     case 'h':
       printf("intset -- STM stress test "
              "(linked list)\n"
              "\n"
              "Usage:\n"
              "  intset [options...]\n"
              "\n"
              "Options:\n"
              "  -h, --help\n"
              "        Print this message\n"
              "  -d, --duration <int>\n"
              "        Test duration in milliseconds (0=infinite, default=" XSTR(DEFAULT_DURATION) ")\n"
              "  -i, --initial-size <int>\n"
              "        Number of elements to insert before test (default=" XSTR(DEFAULT_INITIAL) ")\n"
              "  -n, --num-threads <int>\n"
              "        Number of threads (default=" XSTR(DEFAULT_NB_THREADS) ")\n"
              "  -r, --range <int>\n"
              "        Range of integer values inserted in set (default=" XSTR(DEFAULT_RANGE) ")\n"
              "  -s, --seed <int>\n"
              "        RNG seed (0=time-based, default=" XSTR(DEFAULT_SEED) ")\n"
              "  -u, --update-rate <int>\n"
              "        Percentage of update transactions (default=" XSTR(DEFAULT_UPDATE) ")\n"
         );
       exit(0);
     case 'd':
       operations = atoi(optarg);
       break;
     case 'i':
       initial = atoi(optarg);
       break;
     case 'n':
       nb_threads = atoi(optarg);
       break;
     case 'r':
       range = atoi(optarg);
       break;
     case 's':
       seed = atoi(optarg);
       break;
     case 'u':
       update = atoi(optarg);
       break;
     case '?':
       printf("Use -h or --help for help\n");
       exit(0);
     default:
       exit(1);
    }
  }

randomPtr = random_alloc();


  if (seed == 0)
    srand((int)time(0));
  else
    srand(seed);

  SIM_GET_NUM_CPU(nb_threads);
  TM_STARTUP(nb_threads,42);
  P_MEMORY_STARTUP(nb_threads);
  thread_startup(nb_threads);

  set = set_new();

  /* Populate set */
  printf("Adding %d entries to set\n", initial);
  for (i = 0; i < initial; i++) {
    val = (rand() % range) + 1;
    set_seq_add(val);
  }

printf("Initial size: %d\n",   set_size(set));

  seed = rand();
  TIMER_READ(start);
  GOTO_SIM();
startEnergyIntel();

  thread_start(test, NULL);

  GOTO_REAL();
  TIMER_READ(stop);

double energy = endEnergyIntel();
  puts("done.");
  printf("\nTime = %0.6lf\n", TIMER_DIFF_SECONDS(start, stop));
printf("Energy = %0.6lf\n", energy);
PRINT_STATS();
  fflush(stdout);

printf("Final size: %d\n",   set_size(set));

  TM_SHUTDOWN();
  P_MEMORY_SHUTDOWN();
  GOTO_SIM();
  thread_shutdown();
  MAIN_RETURN(0);
}
