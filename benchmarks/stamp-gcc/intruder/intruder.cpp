/* =============================================================================
 *
 * intruder.c
 *
 * =============================================================================
 *
 * Copyright (C) Stanford University, 2006.  All Rights Reserved.
 * Author: Chi Cao Minh
 *
 * =============================================================================
 *
 * For the license of bayes/sort.h and bayes/sort.c, please see the header
 * of the files.
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of kmeans, please see kmeans/LICENSE.kmeans
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of ssca2, please see ssca2/COPYRIGHT
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of lib/mt19937ar.c and lib/mt19937ar.h, please see the
 * header of the files.
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of lib/rbtree.h and lib/rbtree.c, please see
 * lib/LEGALNOTICE.rbtree and lib/LICENSE.rbtree
 * 
 * ------------------------------------------------------------------------
 * 
 * Unless otherwise noted, the following license applies to STAMP files:
 * 
 * Copyright (c) 2007, Stanford University
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 * 
 *     * Neither the name of Stanford University nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =============================================================================
 */

#include "rapl.h"
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "decoder-htm-v2.h"
#include "detector.h"
#include "dictionary.h"
#include "packet.h"
#include "stream-htm-v2.h"
#include "thread.h"
#include "timer.h"

using namespace decoder_htm_v2;
using namespace stream_htm_v2;

enum param_types {
    PARAM_ATTACK = (unsigned char)'a',
    PARAM_LENGTH = (unsigned char)'l',
    PARAM_NUM    = (unsigned char)'n',
    PARAM_SEED   = (unsigned char)'s',
    PARAM_THREAD = (unsigned char)'t',
    PARAM_REPEAT = (unsigned char)'r',
	PARAM_COMBINATION = (unsigned char)'c',
};

enum param_defaults {
    PARAM_DEFAULT_ATTACK = 10,
    PARAM_DEFAULT_LENGTH = 16,
    PARAM_DEFAULT_NUM    = 1 << 20,
    PARAM_DEFAULT_SEED   = 1,
    PARAM_DEFAULT_THREAD = 1,
    PARAM_DEFAULT_REPEAT = 1,
	PARAM_DEFAULT_COMBINATION = 0
};

long global_params[256]; /* 256 = ascii limit */

#define CHANGINGWORKLOAD
#ifdef CHANGINGWORKLOAD
std::vector<long> combinations;
#endif

static void
setDefaultParams( void )
{
    global_params[PARAM_ATTACK]    = PARAM_DEFAULT_ATTACK;
    global_params[PARAM_LENGTH]  = PARAM_DEFAULT_LENGTH;
    global_params[PARAM_NUM] = PARAM_DEFAULT_NUM;
    global_params[PARAM_COMBINATION] = PARAM_DEFAULT_COMBINATION;
    global_params[PARAM_SEED]  = PARAM_DEFAULT_SEED;
    global_params[PARAM_THREAD]  = PARAM_DEFAULT_THREAD;
}

typedef struct arg {
  /* input: */
    stream_t* streamPtr;
    decoder_t* decoderPtr;
  /* output: */
    vector_t** errorVectors;
} arg_t;


/* =============================================================================
 * displayUsage
 * =============================================================================
 */
static void
displayUsage (const char* appName)
{
    printf("Usage: %s [options]\n", appName);
    puts("\nOptions:                            (defaults)\n");
    printf("    a <UINT>   Percent [a]ttack     (%i)\n", PARAM_DEFAULT_ATTACK);
    printf("    l <UINT>   Max data [l]ength    (%i)\n", PARAM_DEFAULT_LENGTH);
    printf("    n <UINT>   [n]umber of flows    (%i)\n", PARAM_DEFAULT_NUM);
    printf("    s <UINT>   Random [s]eed        (%i)\n", PARAM_DEFAULT_SEED);
    printf("    t <UINT>   Number of [t]hreads  (%i)\n", PARAM_DEFAULT_THREAD);
    exit(1);
}


/* =============================================================================
 * parseArgs
 * =============================================================================
 */
static void
parseArgs (long argc, char* const argv[])
{
    long i;
    long opt;

    opterr = 0;

    setDefaultParams();

    while ((opt = getopt(argc, argv, "a:l:n:s:t:f:p:r:c:")) != -1) {
        switch (opt) {
            case 'a':
#ifdef CHANGINGWORKLOAD
            	if(global_params[PARAM_COMBINATION] == 1) {
            		combinations.push_back(atol(optarg));
            		break;
            	}
#endif
            case 'l':
#ifdef CHANGINGWORKLOAD
            	if(global_params[PARAM_COMBINATION] == 1) {
            		combinations.push_back(atol(optarg));
            		break;
            	}
#endif
            case 'n':
#ifdef CHANGINGWORKLOAD
            	if(global_params[PARAM_COMBINATION] == 1) {
            		combinations.push_back(atol(optarg));
            		break;
            	}
#endif
            case 's':
            case 't':
            case 'f':
            case 'p':
            case 'r':
                global_params[(unsigned char)opt] = atol(optarg);
                break;
            case 'c':
#ifdef CHANGINGWORKLOAD
            	global_params[(unsigned char)opt] = 1;
            	combinations.push_back(atol(optarg));
            	break;
#endif
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
        displayUsage(argv[0]);
    }
}


/* =============================================================================
 * processPackets
 * =============================================================================
 */
void
processPackets (void* argPtr)
{
    long threadId = thread_getId();

    stream_t*   streamPtr    = ((arg_t*)argPtr)->streamPtr;
    decoder_t*  decoderPtr   = ((arg_t*)argPtr)->decoderPtr;
    vector_t**  errorVectors = ((arg_t*)argPtr)->errorVectors;

    detector_t* detectorPtr = PDETECTOR_ALLOC();
    assert(detectorPtr);
    PDETECTOR_ADDPREPROCESSOR(detectorPtr, &preprocessor_toLower);

    vector_t* errorVectorPtr = errorVectors[threadId];

    while (1) {

        char* bytes;
	int i;
	int lim = 10000;
        __transaction_atomic {
        	bytes = stream_htm_v2::TMSTREAM_GETPACKET(streamPtr);
        }
        if (!bytes) {
            break;
        }

        packet_t* packetPtr = (packet_t*)bytes;
        long flowId = packetPtr->flowId;

        error_t error;
        __transaction_atomic {
            error = decoder_htm_v2::TMDECODER_PROCESS(decoderPtr,
                                      bytes,
                                      (PACKET_HEADER_LENGTH + packetPtr->length));
        }
        if (error) {
            /*
             * Currently, stream_generate() does not create these errors.
             */
            assert(0);
            bool_t status = PVECTOR_PUSHBACK(errorVectorPtr, (void*)flowId);
            assert(status);
        }

        char* data;
        long decodedFlowId;
        __transaction_atomic {
        	data = decoder_htm_v2::TMDECODER_GETCOMPLETE(decoderPtr, &decodedFlowId);
        }
        if (data) {
            error_t error = PDETECTOR_PROCESS(detectorPtr, data);
            free(data);
            if (error) {
                bool_t status = PVECTOR_PUSHBACK(errorVectorPtr,
                                                 (void*)decodedFlowId);
                assert(status);
            }
        }

    }

    PDETECTOR_FREE(detectorPtr);

}


/* =============================================================================
 * main
 * =============================================================================
 */
int main(int argc, char* argv[])
{
    parseArgs(argc, (char** const)argv);
    long numThread = global_params[PARAM_THREAD];
    thread_startup(numThread);

    long percentAttack = global_params[PARAM_ATTACK];
    long maxDataLength = global_params[PARAM_LENGTH];
    long numFlow       = global_params[PARAM_NUM];
    long randomSeed    = global_params[PARAM_SEED];

#ifdef CHANGINGWORKLOAD
int currentCombination = 0;
#endif

double time_total = 0.0;
double energy_total = 0.0;
int repeats = global_params[PARAM_REPEAT];
int roundsLeft = 0;
for (; repeats > 0; --repeats) {

#ifdef CHANGINGWORKLOAD
    if(global_params[PARAM_COMBINATION] == 1) {
        if (roundsLeft == 0) {
            roundsLeft = combinations.at(currentCombination);
            percentAttack = combinations.at(currentCombination + 1);
            maxDataLength = combinations.at(currentCombination + 2);
            numFlow = combinations.at(currentCombination + 3);
            currentCombination += 4;

            if(currentCombination > combinations.size()) {
                printf("Wrong combinations, exceeded size! %d > %d\n", currentCombination, combinations.size());
            }

            printf("Going to use the next workload for %d rounds:\n", roundsLeft);
            printf("\tPercent attack  = %li\n", percentAttack);
            printf("\tMax data length = %li\n", maxDataLength);
            printf("\tNum flow        = %li\n", numFlow);
            printf("\tRandom seed     = %li\n", randomSeed);
        }
        roundsLeft--;
    }
#endif

    dictionary_t* dictionaryPtr = dictionary_alloc();
    assert(dictionaryPtr);
    stream_t* streamPtr = stream_alloc(percentAttack);
    assert(streamPtr);
    long numAttack = stream_generate(streamPtr,
                                     dictionaryPtr,
                                     numFlow,
                                     randomSeed,
                                     maxDataLength);
//    printf("Num attack      = %li\n", numAttack);

    decoder_t* decoderPtr = decoder_alloc();
    assert(decoderPtr);

    vector_t** errorVectors = (vector_t**)malloc(numThread * sizeof(vector_t*));
    assert(errorVectors);
    long i;
    for (i = 0; i < numThread; i++) {
        vector_t* errorVectorPtr = vector_alloc(numFlow);
        assert(errorVectorPtr);
        errorVectors[i] = errorVectorPtr;
    }

    arg_t arg;
    arg.streamPtr    = streamPtr;
    arg.decoderPtr   = decoderPtr;
    arg.errorVectors = errorVectors;

    /*
     * Run transactions
     */

    TIMER_T startTime;
    startEnergyIntel();
    TIMER_READ(startTime);
    thread_start(processPackets, (void*)&arg);
    TIMER_T stopTime;
    TIMER_READ(stopTime);
    double time_tmp = TIMER_DIFF_SECONDS(startTime, stopTime);
    double energy_tmp = endEnergyIntel();
    time_total += time_tmp;
    energy_total += energy_tmp;
    printf("%lf\t%lf\n", time_tmp, energy_tmp);

    /*
     * Check solution
     */

    long numFound = 0;
    for (i = 0; i < numThread; i++) {
        vector_t* errorVectorPtr = errorVectors[i];
        long e;
        long numError = vector_getSize(errorVectorPtr);
        numFound += numError;
        for (e = 0; e < numError; e++) {
            long flowId = (long)vector_at(errorVectorPtr, e);
            bool_t status = stream_isAttack(streamPtr, flowId);
            assert(status);
        }
    }
//    printf("Num found       = %li\n", numFound);
    assert(numFound == numAttack);

    /*
     * Clean up
     */

    for (i = 0; i < numThread; i++) {
        vector_free(errorVectors[i]);
    }
    free(errorVectors);
    decoder_free(decoderPtr);
    stream_free(streamPtr);
    dictionary_free(dictionaryPtr);

}

printf("Elapsed time    = %f seconds\n", time_total);
printf("Energy = %0.6lf\n", energy_total);

}


/* =============================================================================
 *
 * End of intruder.c
 *
 * =============================================================================
 */