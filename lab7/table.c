/* 
 * table.c
 * Lab7: Hash Tables 
 * David Lynge
 * dlynge
 * ECE 2230, Fall 2018
 * 
 * This file contains the functions that manage and maintain a hash table.
 * Look at table.h for a overview of the functions and definitions
 * 
 * ASSUMPTIONS
 * We assume that the keys 0 and 1 are reserved and will never be used.
 * 
 * BUGS
 * None known.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "table.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MOD(a,b) ((((a)%(b))+(b))%(b))


// Private funciton prototypes
table_entry_t *table_find(table_t *T, hashkey_t K);

/*  The empty table is created.  The table must be dynamically allocated and
 *  have a total size of table_size.  The maximum number of (K, I) entries
 *  that can be stored in the table is table_size-1.  For open addressing, 
 *  the table is filled with a special empty key distinct from all other 
 *  nonempty keys.  
 *
 *  the probe_type must be one of {LINEAR, DOUBLE, QUAD}
 *
 *  Do not "correct" the table_size or probe decrement if there is a chance
 *  that the combinaion of table size or probe decrement will not cover
 *  all entries in the table.  Instead we will experiment to determine under
 *  what conditions an incorrect choice of table size and probe decrement
 *  results in poor performance.
 */
table_t *table_construct(int table_size, int probe_type)
{
    table_t *T;

    if (table_size <= 1 || probe_type < 0 || probe_type > 2)
        return NULL;

    T = (table_t *)malloc(sizeof(table_t));
    T->table_ptr = (table_entry_t *)calloc(table_size, sizeof(table_entry_t));

    T->probeType = probe_type;
    T->numEntries = 0;
    T->size = table_size;
    T->numProbes = 0;

    return T;
}

/* Sequentially remove each table entry (K, I) and insert into a new
 * empty table with size new_table_size.  Free the memory for the old table
 * and return the pointer to the new table.  The probe type should remain
 * the same.
 *
 * Do not rehash the table during an insert or delete function call.  Instead
 * use drivers to verify under what conditions rehashing is required, and
 * call the rehash function in the driver to show how the performance
 * can be improved.
 */
table_t *table_rehash(table_t *T, int new_table_size)
{
    table_t *newT;
    int i;

    if (T == NULL)
        return NULL;

    newT = table_construct(new_table_size, T->probeType);
    
    for (i = 0; i < T->size; i++)
        table_insert(newT, T->table_ptr[i].key, T->table_ptr[i].data_ptr);

    free(T->table_ptr);
    free(T);

    return newT;
}

/* returns number of entries in the table */
int table_entries(table_t *T)
{
    if (T == NULL)
        return -1;

    return T->numEntries;
}

/* returns 1 if table is full and 0 if not full. */
int table_full(table_t *T)
{
    if (T == NULL)
        return -1;

    if (T->numEntries == T->size - 1)
        return 1;
    else
        return 0;
}

/* returns the number of table entries marked as deleted */
int table_deletekeys(table_t *T)
{
    int i;
    int numDeleted = 0;

    if (T == NULL)
        return -1;

    for (i = 0; i < T->size; i++)
        if (T->table_ptr[i].key == 1)
            numDeleted++;

    return numDeleted;
}
   
/* Insert a new table entry (K, I) into the table provided the table is not
 * already full.  
 * Return:
 *      0 if (K, I) is inserted, 
 *      1 if an older (K, I) is already in the table (update with the new I), or 
 *     -1 if the (K, I) pair cannot be inserted.
 */
int table_insert(table_t *T, hashkey_t K, data_t I)
{
    table_entry_t *location;

    if (T == NULL || K < 2)
        return -1;

    location = table_find(T, K);

    // Update to new I
    if (location->key == K)
    {
        free(location->data_ptr);
        location->data_ptr = I;
        return 1;
    }

    // Insert
    if (T->numEntries == T->size - 1)
    {
        return -1;
    }
    
    location->data_ptr = I;
    location->key = K;
    T->numEntries++;
    return 0;
}

/* Delete the table entry (K, I) from the table.  
 * Return:
 *     pointer to I, or
 *     null if (K, I) is not found in the table.  
 *
 * See the note on page 490 in Standish's book about marking table entries for
 * deletions when using open addressing.
 */
data_t table_delete(table_t *T, hashkey_t K)
{
    table_entry_t *location;
    data_t data;

    if (T == NULL || K < 2)
        return NULL;

    location = table_find(T, K);

    if (location->key != K)
        return NULL;
    
    T->numEntries--;
    data = location->data_ptr;
    location->data_ptr = NULL;
    location->key = 1;
    return data;
}

/* Given a key, K, retrieve the pointer to the information, I, from the table,
 * but do not remove (K, I) from the table.  Return NULL if the key is not
 * found.
 */
data_t table_retrieve(table_t *T, hashkey_t K)
{
    table_entry_t *location;

    if (T == NULL || K < 2)
        return NULL;

    location = table_find(T, K);

    if (location->key != K)
        return NULL;

    return location->data_ptr;
}

/* Free all information in the table, the table itself, and any additional
 * headers or other supporting data structures.  
 */
void table_destruct(table_t *T)
{
    int i;

    if (T == NULL)
        return;

    for (i = 0; i < T->size; i++)
        if (T->table_ptr[i].key > 1)
            free(T->table_ptr[i].data_ptr);

    free(T->table_ptr);
    free(T);
}

/* Return the table entry with the corresponding key. If no key is found,
 * return the first available position that the key could be inserted at.
 */
table_entry_t *table_find(table_t *T, hashkey_t K)
{
    int decrement = 0;
    int probes = 1;
    int size;
    int rover;
    int roverKey;
    int firstDeleted = -1;
    int probeType;

    if (T == NULL || K < 2)
        return NULL;

    size = T->size;
    probeType = T->probeType;

    if (probeType == LINEAR)
        decrement = 1;
    else if (probeType == DOUBLE)
        decrement = MAX(1, MOD((K / size), size));

    rover = MOD(K, size);
    roverKey = T->table_ptr[rover].key;

    // Probing loop
    while (roverKey != K && roverKey != 0 && probes < size)
    {
        probes++;

        if (firstDeleted == -1 && roverKey == 1)
            firstDeleted = rover;
        
        if (probeType == QUAD)
            decrement++;

        rover = MOD((rover - decrement), size);
        roverKey = T->table_ptr[rover].key;
    }

    // If we loop through too many values without finding an empty
    if (firstDeleted == -1 && roverKey != 0 && roverKey != K)
    {
        printf("Fatal Error: Probe sequence does not work with a table size of %d\n", T->size);
        exit(1);
    }

    T->numProbes = probes;

    if ((roverKey == 0 || probes == size) && firstDeleted != -1)    // Return first deletion
        return &(T->table_ptr[firstDeleted]);
    else                                                            // Return K location
        return &(T->table_ptr[rover]); 
}

/* The number of probes for the most recent call to table_retrieve,
 * table_insert, or table_delete 
 */
int table_stats(table_t *T)
{
    if (T == NULL)
        return -1;

    return T->numProbes;
}

/* This function is for testing purposes only.  Given an index position into
 * the hash table return the value of the key if data is stored in this 
 * index position.  If the index position does not contain data, then the
 * return value must be zero.  Make the first
 * lines of this function 
 *       assert(0 <= index && index < table_size); 
 */
hashkey_t table_peek(table_t *T, int index)
{
    assert(0 <= index && index < T->size);

    if (T == NULL)
        return -1;

    if (T->table_ptr[index].key >= 10)
        return T->table_ptr[index].key;
    
    return 0;
}

/* Print the table position and keys in a easily readable and compact format.
 * Only useful when the table is small.
 */
void table_debug_print(table_t *T)
{
    int i;

    if (T == NULL)
        return;

    printf("Table key positions\n");

    for (i = 0; i < T->size; i++)
    {
        if (T->table_ptr[i].key == 0)
            printf("[%3d] EMPTY\n", i);
        else if (T->table_ptr[i].key == 1)
            printf("[%3d] DELET\n", i);
        else
            printf("[%3d] %5d\n", i, T->table_ptr[i].key);
    }
}

