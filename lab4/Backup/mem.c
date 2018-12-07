/* mem.c A template
 * My Name is
 * My User ID is
 * Lab4: Dynamic Memory Allocation
 * ECE 2230, Fall 2018
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "mem.h"

// Global variables required in mem.c only
static chunk_t Dummy = {&Dummy, 0};
static chunk_t * Rover = &Dummy;
static int NumSbrkCalls = 0;
static int NumPages = 0;


// private function prototypes
void mem_validate(void);

/* function to request 1 or more pages from the operating system.
 *
 * new_bytes must be the number of bytes that are being requested from
 *           the OS with the sbrk command.  It must be an integer 
 *           multiple of the PAGESIZE
 *
 * returns a pointer to the new memory location.  If the request for
 * new memory fails this function simply returns NULL, and assumes some
 * calling function will handle the error condition.  Since the error
 * condition is catastrophic, nothing can be done but to terminate 
 * the program.
 */
chunk_t *morecore(int new_bytes) 
{
    char *cp;
    chunk_t *new_p;
    // preconditions
    assert(new_bytes % PAGESIZE == 0 && new_bytes > 0);
    assert(PAGESIZE % sizeof(chunk_t) == 0);
    cp = sbrk(new_bytes);
    if (cp == (char *) -1)  /* no space available */
        return NULL;
    new_p = (chunk_t *) cp;
    // You should add some code to count the number of calls
    // to sbrk, and the number of pages that have been requested
    NumSbrkCalls++; NumPages += new_bytes/PAGESIZE;
    return new_p;
}

/* deallocates the space pointed to by return_ptr; it does nothing if
 * return_ptr is NULL.  
 *
 * This function assumes that the Rover pointer has already been 
 * initialized and points to some memory block in the free list.
 */
void Mem_free(void *return_ptr)
{
    chunk_t *p;
    
    p = ((chunk_t *)return_ptr) - 1;
    
    // precondition
    assert(Rover != NULL && Rover->next != NULL);

    // Find insertion location
    if (Coalescing == TRUE)
    {
        Rover = &Dummy;
        while (Rover->next < p && Rover->next != &Dummy) Rover = Rover->next;
    }

    // Insert
    p->next = Rover->next;
    Rover->next = p;

    // Merge if possible
    if (Coalescing == TRUE)
    {
        // Merge chunk after
        if (p + p->size == p->next)
        {
            p->size += p->next->size;
            p->next = p->next->next;
        }
        // Merge chunk before
        if (Rover + Rover->size == Rover->next) 
        {
            Rover->size += Rover->next->size;
            Rover->next = Rover->next->next;
            
            // Need to set rover to before free'd block
            p = Rover;
            while (Rover->next != p) Rover = Rover->next;
        }
    }
}

/* returns a pointer to space for an object of size nbytes, or NULL if the
 * request cannot be satisfied.  The memory is uninitialized.
 *
 * This function assumes that there is a Rover pointer that points to
 * some item in the free list.  The first time the function is called,
 * Rover is null, and must be initialized with a dummy block whose size
 * is one, but set the size field to zero so this block can never be 
 * removed from the list.  After the first call, the Rover can never be null
 * again.
 */
void *Mem_alloc(const int nbytes)
{
    chunk_t *start_ptr, *prevRover, *p, *q, *bestRover, *bestPrevRover;
    chunk_t *mcRtn;
    int mcRequest, nChunks;

    // precondition
    assert(nbytes > 0);
    assert(Rover != NULL && Rover->next != NULL);


    // Insert your code here to find memory block
    start_ptr = Rover;
    p = q = NULL;

    nChunks = nbytes / sizeof(chunk_t) + 1;
    if (nbytes % sizeof(chunk_t))
        nChunks++;

    // Find available memory
    if (SearchPolicy == FIRST_FIT)
        do  
        {
            prevRover = Rover;
            Rover = Rover->next;

            if (Rover->size >= nChunks)
            {
                p = Rover;
                q = p + 1;
                break;
            }
        } while (Rover != start_ptr);
    else
    {
        bestPrevRover = NULL;
        bestRover = NULL;
        do  
        {
            prevRover = Rover;
            Rover = Rover->next;

            if (Rover->size >= nChunks && 
                (bestRover == NULL || Rover->size < bestRover->size))
            {
                bestRover = Rover;
                bestPrevRover = prevRover;
            }
        } while (Rover != start_ptr);

        if (bestRover != NULL)
        {
            Rover = bestRover;
            prevRover = bestPrevRover;
            p = bestRover;
            q = p + 1;
        }
    }

            
    if (p == NULL) // If we don't find a match
    {
        mcRequest = (nbytes + sizeof(chunk_t));
        if (mcRequest % PAGESIZE)
            mcRequest = (mcRequest / PAGESIZE) + PAGESIZE;

        
        mcRtn = morecore(mcRequest);
        
        if (mcRtn == NULL)
            return NULL;

        mcRtn->size = mcRequest / sizeof(chunk_t);
        
        Mem_free(mcRtn + 1);    // Memory is inserted after rover
        
        prevRover = Rover;
        Rover = Rover->next;
        p = Rover;
        q = p + 1;
    }
            
    // Split up the segment if it can make a new segment
    if (p->size > nChunks + 1)
    {
        (Rover + nChunks)->size = p->size - nChunks;
        (Rover + nChunks)->next = p->next;

        p->next = Rover + nChunks;
        p->size = nChunks;
    }

    prevRover->next = Rover = p->next;
    p->next = NULL;
    
    // here are possible post-conditions, depending on your design
    //
    // assume p is a pointer to memory block that will be given to the user
    // and q is the address given to the user
    assert(p + 1 == q);
    // the minus one in the next two tests accounts for the header
    assert((p->size - 1)*sizeof(chunk_t) >= nbytes);

    if (!((p->size - 1)*sizeof(chunk_t) < nbytes + 2 * sizeof(chunk_t)))
        printf("p size: %d\nnbytes: %d\n", (int)((p->size - 1)*sizeof(chunk_t)), (int)(nbytes + sizeof(chunk_t)));

    assert((p->size - 1)*sizeof(chunk_t) < nbytes + 2 * sizeof(chunk_t));
    assert(p->next == NULL);  // saftey first!
    return q;
}

/* prints stats about the current free list
 *
 * -- number of items in the linked list including dummy item
 * -- min, max, and average size of each item (in bytes)
 * -- total memory in list (in bytes)
 * -- number of calls to sbrk and number of pages requested
 *
 * A message is printed if all the memory is in the free list
 */
void Mem_stats(void)
{
    chunk_t *start_ptr = Rover;
    int M = 0;
    
    do {
        M += Rover->size * sizeof(chunk_t);
        Rover = Rover->next;
    } while (Rover != start_ptr);
    // One of the stats you must collect is the total number
    // of pages that have been requested using sbrk.
    // Say, you call this NumPages.  You also must count M,
    // the total number of bytes found in the free list 
    // (including all bytes used for headers).  If it is the case
    // that M == NumPages * PAGESiZE then print
    if (M == NumPages * PAGESIZE)
        printf("all memory is in the heap -- no leaks are possible\n");
    else
        printf("M: %d\nNumPages: %d\n", M, NumPages * PAGESIZE);
}

/* print table of memory in free list 
 *
 * The print should include the dummy item in the list 
 */
void Mem_print(void)
{
    if (!(Rover != NULL && Rover->next != NULL))
        printf("bad");
    assert(Rover != NULL && Rover->next != NULL);
    chunk_t *p = Rover;
    chunk_t *start = p;
    do {
        // example format.  Modify for your design
        printf("p=%p, size=%d, end=%p, next=%p %s\n", 
                p, p->size, p + p->size, p->next, p->size!=0?"":"<-- dummy");
        p = p->next;
    } while (p != start);
    mem_validate();
}

/* This is an experimental function to attempt to validate the free
 * list when coalescing is used.  It is not clear that these tests
 * will be appropriate for all designs.  If your design utilizes a different
 * approach, that is fine.  You do not need to use this function and you
 * are not required to write your own validate function.
 */
void mem_validate(void)
{
    assert(Rover != NULL && Rover->next != NULL);
    assert(Rover->size >= 0);
    int wrapped = FALSE;
    int found_dummy = FALSE;
    int found_rover = FALSE;
    chunk_t *p, *largest, *smallest;

    // for validate begin at Dummy
    p = &Dummy;
    do {
        if (p->size == 0) {
            assert(found_dummy == FALSE);
            found_dummy = TRUE;
        } else {
            assert(p->size > 0);
        }
        if (p == Rover) {
            assert(found_rover == FALSE);
            found_rover = TRUE;
        }
        p = p->next;
    } while (p != &Dummy);
    assert(found_dummy == TRUE);
    assert(found_rover == TRUE);

    if (Coalescing) {
        do {
            if (p >= p->next) {
                // this is not good unless at the one wrap
                if (wrapped == TRUE) {
                    printf("validate: List is out of order, already found wrap\n");
                    printf("first largest %p, smallest %p\n", largest, smallest);
                    printf("second largest %p, smallest %p\n", p, p->next);
                    assert(0);   // stop and use gdb
                } else {
                    wrapped = TRUE;
                    largest = p;
                    smallest = p->next;
                }
            } else {
                assert(p + p->size < p->next);
            }
            p = p->next;
        } while (p != &Dummy);
        assert(wrapped == TRUE);
    }
}
/* vi:set ts=8 sts=4 sw=4 et: */

