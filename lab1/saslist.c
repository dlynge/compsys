/* saslist.c
 * David Lynge
 * dlynge
 * ECE 2230 Fall 2018
 * MP1
 *
 * This file contains the functions to access and manipulate the SAS list type
 * that is provided in saslist.h 
 */

// The ONLY member you can access in a secusr_info_t block is
// su_id.  All other members in this structure cannot be used
// in this file.

#include <stdlib.h>

#include "saslist.h"

/* 
 * Initializes the saslist to a given size
 * 
 * The input size is a positive integer
 * 
 * The output is a pointer to the list
*/ 
struct saslist_t *saslist_construct(int size)
{
    struct saslist_t *saslist;

    saslist = (struct saslist_t *)malloc(sizeof(struct saslist_t));
    saslist->sas_list_size = size;
    saslist->sec_usrs_count = 0;
    saslist->sec_usrs_ptr = (struct secusr_info_t **)calloc(size, sizeof(struct secusr_info_t));

    return saslist;
}

/* 
 * Cleans up the memory of a list
 * 
 * The input is a pointer to the list
 * 
 * The output is void
*/ 
void saslist_destruct(struct saslist_t *ptr)
{
    int i;

    // Return -1 if the list has not been created
    if (ptr == NULL || ptr->sec_usrs_ptr == NULL)
        return;

    for (i = 0; i < ptr->sec_usrs_count; i++)
        free(ptr->sec_usrs_ptr[i]);

    free(ptr->sec_usrs_ptr);
    free(ptr);
}

/* 
 * Adds an item to the list in sequential order by ID.
 * 
 * The input is a pointer to the list and a pointer to the item you wish to add
 * 
 * The output is an integer where -1=Error/Full, 0=Overwrite, 1=Insert
*/ 
int saslist_add(struct saslist_t *list_ptr, struct secusr_info_t *rec_ptr)
{
    int i;

    // Return -1 if the list has not been created
    if (list_ptr == NULL || list_ptr->sec_usrs_ptr == NULL)
        return -1;

    // Check to see if the user id already exists and overwrite it
    for (i = 0; i < list_ptr->sec_usrs_count; i++)
        if (list_ptr->sec_usrs_ptr[i]->su_id == rec_ptr->su_id)
        {
            *list_ptr->sec_usrs_ptr[i] = *rec_ptr;
            return 0;
        }

    // Check to see if the list is full
    if (list_ptr->sas_list_size == list_ptr->sec_usrs_count)
        return -1;

    // Insert it into the list
    for (i = list_ptr->sec_usrs_count - 1; i >= 0 && list_ptr->sec_usrs_ptr[i]->su_id > rec_ptr->su_id; i--)
        list_ptr->sec_usrs_ptr[i+1] = list_ptr->sec_usrs_ptr[i];

    list_ptr->sec_usrs_ptr[++i] = (struct secusr_info_t *)malloc(sizeof(struct secusr_info_t));
    *list_ptr->sec_usrs_ptr[i] = *rec_ptr;
    list_ptr->sec_usrs_count++;

    return 1;
}

/* 
 * Searches the list for an id
 * 
 * The input is a pointer to the list and the id that you wish to search for
 * 
 * The output is the index at which that id appears in the list. Note that there
 * should only be one item with this id
*/ 
int saslist_lookup(struct saslist_t *list_ptr, int id)
{
    int i;

    // Return -1 if the list has not been created
    if (list_ptr == NULL || list_ptr->sec_usrs_ptr == NULL)
        return -1;

    // Loop through and search for id
    for (i = 0; i < list_ptr->sec_usrs_count; i++)
        if (list_ptr->sec_usrs_ptr[i]->su_id == id)
            return i;

    return -1;
}

/* 
 * Gets the user info at a specific index in the list
 * 
 * The input is a pointer to the list and the index you wish to get the info for
 * 
 * The output is a pointer to the user info at that specific index
*/ 
struct secusr_info_t *saslist_access(struct saslist_t *list_ptr, int index)
{
    // Return NULL if the list has not been created
    if (list_ptr == NULL || list_ptr->sec_usrs_ptr == NULL)
        return NULL;

    // Reuturn NULL if out of bounds
    if (index < 0 || index >= list_ptr->sec_usrs_count)
        return NULL;

    return list_ptr->sec_usrs_ptr[index];
}

/* 
 * Removes an item from the list and shifts everything to fill the gap
 * 
 * The input is a pointer to the list and the index you wish to remove
 * 
 * The output is a pointer to the item removed. Note it has to be free'd.
*/ 
struct secusr_info_t *saslist_remove(struct saslist_t *list_ptr, int index)
{
    struct secusr_info_t *old_ptr;
    int i;


    // Return NULL if the list has not been created
    if (list_ptr == NULL || list_ptr->sec_usrs_ptr == NULL)
        return NULL;

    // Reuturn NULL if out of bounds
    if (index < 0 || index >= list_ptr->sec_usrs_count)
        return NULL;

    old_ptr = list_ptr->sec_usrs_ptr[index];

    // Shift down
    for (i = index; i  < list_ptr->sec_usrs_count - 1; i++)
        list_ptr->sec_usrs_ptr[i] = list_ptr->sec_usrs_ptr[i+1];

    list_ptr->sec_usrs_ptr[i] = NULL;

    list_ptr->sec_usrs_count--;

    return old_ptr;
}

/* 
 * Gets the size of the list independantly of how many users are registered
 * 
 * The input is the list pointer
 * 
 * The output is the size of the list in an integer
*/ 
int saslist_arraysize(struct saslist_t *list_ptr)
{
    // Return 0 if the list has not been created
    if (list_ptr == NULL || list_ptr->sec_usrs_ptr == NULL)
        return 0;

    return list_ptr->sas_list_size;
}

/* 
 * Gets the number of users that are registered in the list
 * 
 * The input is a list pointer
 * 
 * The output is the number of users registered in the list as an integer
*/ 
int saslist_number_entries(struct saslist_t *list_ptr)
{
    // Return 0 if the list has not been created
    if (list_ptr == NULL || list_ptr->sec_usrs_ptr == NULL)
        return 0;

    return list_ptr->sec_usrs_count;
}

