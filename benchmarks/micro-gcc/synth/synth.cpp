#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "thread.h"
#include "timer.h"
#include "random.h"
#include <time.h>

#include "rapl.h"

enum param_types {
    PARAM_SIZE = (unsigned char)'s',
    PARAM_THREADS = (unsigned char)'t',
    PARAM_OPERATIONS = (unsigned char)'o',
    PARAM_INTERVAL = (unsigned char)'i',
    PARAM_CONTENTION = (unsigned char)'c',
    PARAM_UPDATES = (unsigned char)'u',
    PARAM_WORK = (unsigned char)'w',
};

#define PARAM_DEFAULT_SIZE (1024)
#define PARAM_DEFAULT_THREADS (2)
#define PARAM_DEFAULT_OPERATIONS (10000)
#define PARAM_DEFAULT_INTERVAL (1000)
#define PARAM_DEFAULT_CONTENTION (100)
#define PARAM_DEFAULT_WORK (1000)
#define PARAM_DEFAULT_UPDATES (20)

double global_params[256];

typedef struct
{
    volatile long value;
    long padding1;
    long padding2;
    long padding3;
    long padding4;
    long padding5;
    long padding6;
    long padding7;
} aligned_type_t ;

__attribute__((aligned(64))) volatile aligned_type_t* global_array;

static void setDefaultParams() {
    global_params[PARAM_SIZE] = PARAM_DEFAULT_SIZE;
    global_params[PARAM_THREADS] = PARAM_DEFAULT_THREADS;
    global_params[PARAM_OPERATIONS] = PARAM_DEFAULT_OPERATIONS;
    global_params[PARAM_INTERVAL] = PARAM_DEFAULT_INTERVAL;
    global_params[PARAM_CONTENTION] = PARAM_DEFAULT_CONTENTION;
    global_params[PARAM_WORK] = PARAM_DEFAULT_WORK;
    global_params[PARAM_UPDATES] = PARAM_DEFAULT_UPDATES;
}

static void parseArgs(long argc, char* const argv[]) {
    long i;
    long opt;
    opterr = 0;

    setDefaultParams();

    while ((opt = getopt(argc, argv, "s:t:o:i:c:w:u:")) != -1) {
        switch (opt) {
        case 's':
        case 'o':
        case 't':
        case 'u':
        case 'i':
        case 'c':
        case 'w':
            global_params[(unsigned char)opt] = atol(optarg);
            break;
        case '?':
        default:
            opterr++;
            break;
        }
    }

    for (i = optind; i < argc; i++) {
        fprintf(stderr, "Non-option argument: %s\n", argv[i]);
        opterr++;
    }

    if (opterr) {
        printf("Wrong usage\n");
        exit(1);
    }
}

#include <sched.h>

void client_run (void* argPtr) {
    random_t* randomPtr = random_alloc();
    random_seed(randomPtr, time(0) + sched_getcpu());

    long operations = (long)global_params[PARAM_OPERATIONS] / (long)global_params[PARAM_THREADS];
    long interval = (long)global_params[PARAM_INTERVAL];
    printf("operations: %ld \tinterval: %ld\n", operations, interval);

    long total = 0;
    long total2 = 0;

    int params_size = (int)global_params[PARAM_SIZE];
    int updates_size = (int)global_params[PARAM_UPDATES];
    int contention_size = (int)global_params[PARAM_CONTENTION];
    int work_size = (int)global_params[PARAM_WORK];

    long i = 0;
    for (; i < operations; i++) {
        __transaction_atomic {
            int repeat = 0;
            for (; repeat < contention_size; repeat++) {

                int random_number = random_generate(randomPtr) % params_size;
                int random_number2 = random_generate(randomPtr) % params_size;
                if (random_number == random_number2) {
                    random_number2 = (random_number2 + 1) % params_size;
                }
                long r1 = (long) global_array[random_number].value;
                long r2 = (long) global_array[random_number2].value;
                total2 += r1 + r2;

                if ((random_generate(randomPtr) % 100) < updates_size) {
                    r1 = r1 + 1;
                    r2 = r2 - 1;
                    global_array[random_number].value = r1;
                    global_array[random_number2].value = r2;
                }
            }
            int f = 1;
            int ii;
            for(ii = 1; ii <= work_size; ii++) {
                f *= ii;
            }
            total += f / 1000000;
        }

        long k = 0;
        for (;k < (long)global_params[PARAM_INTERVAL]; k++) {
            long ru = ((long) random_generate(randomPtr)) % 2;
            total += ru;
        }

    }

    printf("Control variables: %ld - %ld\n", total, total2);
}

int main(int argc, char* argv[]) {
    TIMER_T start;
    TIMER_T stop;

    parseArgs(argc, (char** const)argv);
    long sz = (long)global_params[PARAM_SIZE];
    global_array = (aligned_type_t*) malloc(sz * sizeof(aligned_type_t));

    int k = 0;
    for (; k < sz; k++) {
        global_array[k].value = 0;
    }

    long numThread = global_params[PARAM_THREADS];
    thread_startup(numThread);

    printf("Running clients... ");
    fflush(stdout);

    TIMER_READ(start);

    startEnergyIntel();

    thread_start(client_run, (void*)&numThread);

    TIMER_READ(stop);
    double energy = endEnergyIntel();
    puts("done.");
    printf("Time = %0.6lf\n", TIMER_DIFF_SECONDS(start, stop));
    printf("Energy = %0.6lf\n", energy);
    fflush(stdout);

    long i = 0;
    long sum = 0;
    for (;i < sz; i++) {
        sum += global_array[i].value;
    }
    if (sum != 0) {
        printf("Problem, sum was not zero!: %ld\n", sum);
    }

}