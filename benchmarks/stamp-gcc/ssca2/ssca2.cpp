#include "rapl.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "computeGraph.h"
#include "defs.h"
#include "findSubGraphs.h"
#include "genScalData.h"
#include "getUserParameters.h"
#include "globals.h"
#include "timer.h"
#include "thread.h"

int main(int argc, char* argv[])
{
    /*
     * Tuple for Scalable Data Generation
     * stores startVertex, endVertex, long weight and other info
     */
    graphSDG* SDGdata;

    /*
     * The graph data structure for this benchmark - see defs.h
     */
    graph* G;

    double totalTime = 0.0;

    /* -------------------------------------------------------------------------
     * Preamble
     * -------------------------------------------------------------------------
     */

    /*
     * User Interface: Configurable parameters, and global program control
     */

    getUserParameters(argc, (char** const) argv);

    thread_startup(THREADS);

double time_total = 0.0;
double energy_total = 0.0;
int repeat = REPEATS;
for (; repeat > 0; --repeat) {

    SDGdata = (graphSDG*)malloc(sizeof(graphSDG));
    assert(SDGdata);

    genScalData_seq(SDGdata);

    G = (graph*)malloc(sizeof(graph));
    assert(G);

    computeGraph_arg_t computeGraphArgs;
    computeGraphArgs.GPtr       = G;
    computeGraphArgs.SDGdataPtr = SDGdata;

TIMER_T start;
    startEnergyIntel();
    TIMER_READ(start);

    thread_start(computeGraph, (void*)&computeGraphArgs);
TIMER_T stop;
    TIMER_READ(stop);
double time_tmp = TIMER_DIFF_SECONDS(start, stop);
double energy_tmp = endEnergyIntel();
printf("%lf\t%lf\n", time_tmp, energy_tmp);
time_total += time_tmp;
energy_total += energy_tmp;
}

totalTime += time_total;
printf("Energy = %0.6lf\n", energy_total);

    printf("\nTime taken for all is %9.6f sec.\n\n", totalTime);

    /* -------------------------------------------------------------------------
     * Cleanup
     * -------------------------------------------------------------------------
     */

    free(G->outDegree);
    free(G->outVertexIndex);
    free(G->outVertexList);
    free(G->paralEdgeIndex);
    free(G->inDegree);
    free(G->inVertexIndex);
    free(G->inVertexList);
    free(G->intWeight);
    free(G->strWeight);

    free(SOUGHT_STRING);
    free(G);
    free(SDGdata);

}
