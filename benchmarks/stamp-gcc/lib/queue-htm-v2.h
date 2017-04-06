/* =============================================================================
 *
 * queue.h
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


#ifndef QUEUE_HTM_H
#define QUEUE_HTM_H 1

#include "queue-struct-v2.h"
#include "random.h"
#include "types.h"

namespace queue_htm_v2 {


/* =============================================================================
 * queue_alloc
 * =============================================================================
 */
queue_t*
queue_alloc (long initCapacity);


/* =============================================================================
 * Pqueue_alloc
 * =============================================================================
 */
__attribute__((transaction_safe)) queue_t*
Pqueue_alloc (long initCapacity);


/* =============================================================================
 * TMqueue_alloc
 * =============================================================================
 */
__attribute__((transaction_safe)) queue_t*
TMqueue_alloc (  long initCapacity);


/* =============================================================================
 * queue_free
 * =============================================================================
 */
void
queue_free (queue_t* queuePtr);


/* =============================================================================
 * Pqueue_free
 * =============================================================================
 */
__attribute__((transaction_safe)) void
Pqueue_free (queue_t* queuePtr);


/* =============================================================================
 * TMqueue_free
 * =============================================================================
 */
__attribute__((transaction_safe)) void
TMqueue_free (  queue_t* queuePtr);


/* =============================================================================
 * queue_isEmpty
 * =============================================================================
 */
__attribute__((transaction_safe)) bool_t
queue_isEmpty (queue_t* queuePtr);


/* =============================================================================
 * TMqueue_isEmpty
 * =============================================================================
 */

__attribute__((transaction_safe)) bool_t
TMqueue_isEmpty (  queue_t* queuePtr);


/* =============================================================================
 * queue_clear
 * =============================================================================
 */
__attribute__((transaction_safe)) void
queue_clear (queue_t* queuePtr);


/* =============================================================================
 * queue_shuffle
 * =============================================================================
 */
void
queue_shuffle (queue_t* queuePtr, random_t* randomPtr);


/* =============================================================================
 * queue_push
 * =============================================================================
 */
bool_t
queue_push (queue_t* queuePtr, void* dataPtr);


/* =============================================================================
 * Pqueue_push
 * =============================================================================
 */
__attribute__((transaction_safe)) bool_t
Pqueue_push (queue_t* queuePtr, void* dataPtr);


/* =============================================================================
 * TMqueue_push
 * =============================================================================
 */

__attribute__((transaction_safe)) bool_t
TMqueue_push (  queue_t* queuePtr, void* dataPtr);


/* =============================================================================
 * queue_pop
 * =============================================================================
 */
__attribute__((transaction_safe)) void*
queue_pop (queue_t* queuePtr);


/* =============================================================================
 * TMqueue_pop
 * =============================================================================
 */

__attribute__((transaction_safe)) void*
TMqueue_pop (  queue_t* queuePtr);


}

#endif /* QUEUE_H */


/* =============================================================================
 *
 * End of queue.h
 *
 * =============================================================================
 */

