/* =============================================================================
 *
 * list.h
 * -- Sorted singly linked list
 * -- Options: -DLIST_NO_DUPLICATES (default: allow duplicates)
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


#ifndef LIST_HTM_H
#define LIST_HTM_H 1

#include "list-struct-v2.h"
#include "types.h"


namespace list_htm_v2 {

/* =============================================================================
 * list_iter_reset
 * =============================================================================
 */
__attribute__((transaction_safe)) void
list_iter_reset (list_iter_t* itPtr, list_t* listPtr);


/* =============================================================================
 * TMlist_iter_reset
 * =============================================================================
 */

__attribute__((transaction_safe)) void
TMlist_iter_reset (  list_iter_t* itPtr, list_t* listPtr);


/* =============================================================================
 * list_iter_hasNext
 * =============================================================================
 */
__attribute__((transaction_safe)) bool_t
list_iter_hasNext (list_iter_t* itPtr, list_t* listPtr);


/* =============================================================================
 * TMlist_iter_hasNext
 * =============================================================================
 */

__attribute__((transaction_safe)) bool_t
TMlist_iter_hasNext (  list_iter_t* itPtr, list_t* listPtr);


/* =============================================================================
 * list_iter_next
 * =============================================================================
 */
__attribute__((transaction_safe)) void*
list_iter_next (list_iter_t* itPtr, list_t* listPtr);


/* =============================================================================
 * TMlist_iter_next
 * =============================================================================
 */

__attribute__((transaction_safe)) void*
TMlist_iter_next (  list_iter_t* itPtr, list_t* listPtr);


/* =============================================================================
 * list_alloc
 * -- If NULL passed for 'compare' function, will compare data pointer addresses
 * -- Returns NULL on failure
 * =============================================================================
 */
list_t*
list_alloc (long (*compare)(const void*, const void*));


/* =============================================================================
 * Plist_alloc
 * -- If NULL passed for 'compare' function, will compare data pointer addresses
 * -- Returns NULL on failure
 * =============================================================================
 */
__attribute__((transaction_safe)) list_t*
Plist_alloc (long (*compare)(const void*, const void*));


/* =============================================================================
 * TMlist_alloc
 * -- If NULL passed for 'compare' function, will compare data pointer addresses
 * -- Returns NULL on failure
 * =============================================================================
 */
__attribute__((transaction_safe)) list_t*
TMlist_alloc (  long (*compare)(const void*, const void*));


/* =============================================================================
 * list_free
 * =============================================================================
 */
void
list_free (list_t* listPtr);


/* =============================================================================
 * Plist_free
 * -- If NULL passed for 'compare' function, will compare data pointer addresses
 * -- Returns NULL on failure
 * =============================================================================
 */
void
Plist_free (list_t* listPtr);


/* =============================================================================
 * TMlist_free
 * -- If NULL passed for 'compare' function, will compare data pointer addresses
 * -- Returns NULL on failure
 * =============================================================================
 */
__attribute__((transaction_safe)) void
TMlist_free (  list_t* listPtr);



/* =============================================================================
 * list_isEmpty
 * -- Return TRUE if list is empty, else FALSE
 * =============================================================================
 */
bool_t
list_isEmpty (list_t* listPtr);


/* =============================================================================
 * TMlist_isEmpty
 * -- Return TRUE if list is empty, else FALSE
 * =============================================================================
 */

__attribute__((transaction_safe)) bool_t
TMlist_isEmpty (  list_t* listPtr);


/* =============================================================================
 * list_getSize
 * -- Returns size of list
 * =============================================================================
 */
__attribute__((transaction_safe)) long
list_getSize (list_t* listPtr);


/* =============================================================================
 * TMlist_getSize
 * -- Returns size of list
 * =============================================================================
 */

__attribute__((transaction_safe)) long
TMlist_getSize (  list_t* listPtr);


/* =============================================================================
 * list_find
 * -- Returns NULL if not found, else returns pointer to data
 * =============================================================================
 */
__attribute__((transaction_safe)) void*
list_find (list_t* listPtr, void* dataPtr);


/* =============================================================================
 * TMlist_find
 * -- Returns NULL if not found, else returns pointer to data
 * =============================================================================
 */

__attribute__((transaction_safe)) void*
TMlist_find (  list_t* listPtr, void* dataPtr);


/* =============================================================================
 * list_insert
 * -- Return TRUE on success, else FALSE
 * =============================================================================
 */
bool_t
list_insert (list_t* listPtr, void* dataPtr);


/* =============================================================================
 * Plist_insert
 * -- Return TRUE on success, else FALSE
 * =============================================================================
 */
__attribute__((transaction_safe)) bool_t
Plist_insert (list_t* listPtr, void* dataPtr);


/* =============================================================================
 * TMlist_insert
 * -- Return TRUE on success, else FALSE
 * =============================================================================
 */

__attribute__((transaction_safe)) bool_t
TMlist_insert (  list_t* listPtr, void* dataPtr);


/* =============================================================================
 * list_remove
 * -- Returns TRUE if successful, else FALSE
 * =============================================================================
 */
bool_t
list_remove (list_t* listPtr, void* dataPtr);


/* =============================================================================
 * Plist_remove
 * -- Returns TRUE if successful, else FALSE
 * =============================================================================
 */
bool_t
Plist_remove (list_t* listPtr, void* dataPtr);


/* =============================================================================
 * TMlist_remove
 * -- Returns TRUE if successful, else FALSE
 * =============================================================================
 */

__attribute__((transaction_safe)) bool_t
TMlist_remove (  list_t* listPtr, void* dataPtr);


/* =============================================================================
 * list_clear
 * -- Removes all elements
 * =============================================================================
 */
void
list_clear (list_t* listPtr);


/* =============================================================================
 * Plist_clear
 * -- Removes all elements
 * =============================================================================
 */
__attribute__((transaction_safe)) void
Plist_clear (list_t* listPtr);

}

#endif /* LIST_H */


/* =============================================================================
 *
 * End of list.h
 *
 * =============================================================================
 */