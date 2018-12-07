/* sas_support.c
 * David Lynge
 * ECE 2230 Fall 2018
 * MP2
 *
 * Propose: This file serves to handle records for secondary users that are in
 * a sorted and unsorted list.
 *
 * Assumptions: We assume that no dangling pointers are input into the functions
 * below. These functions should serve as the interface between a list and the
 * program that want to use that list.
 *
 * Bugs: Some functions will not prompt the user if the input is bad. (e.g. the 
 * user asks to see the stats but no list exists to see the stats for)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "datatypes.h"
#include "list.h"
#include "sas_support.h"

// Private functions used in this file only.  Do not include prototype
// in the header file
void sas_record_fill(su_info_t *rec);   // collect input from user
void sas_record_print(su_info_t *rec);  // print one record

/* sas_compare is required by the list ADT for sorted lists.
 *
 * This function returns
 *     1 if rec_a should be closer to the head than rec_b,
 *    -1 if rec_b is to be considered closer to the head, and
 *     0 if the records are equal.
 *
 * For the secondary user data we want to sort from lowest ID up, so
 * closer to the head means a smaller ID.
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int sas_compare(const su_info_t *record_a, const su_info_t *record_b)
{
    assert(record_a != NULL && record_b !=NULL);
    if (record_a->su_id < record_b->su_id)
        return 1;
    else if (record_a->su_id > record_b->su_id)
        return -1;
    else
        return 0;
}


/* print the secondary user record list
 *
 * This function provides an example on how to use the
 * Iterator pointers.
 */
void sas_print(ListPtr list_ptr, char *type_of_list)
{
    if (list_ptr == NULL)
        return;

    assert(strcmp(type_of_list, "Assigned List")==0
           || strcmp(type_of_list, "Waiting Queue")==0);
    IteratorPtr index;
    int num_in_list = list_size(list_ptr);
    int counter = 0;

    if (num_in_list == 0)
    {
        printf("%s empty\n", type_of_list);
    }
    else
    {
        printf("%s has %d records\n", type_of_list, num_in_list);
        index = list_iter_front(list_ptr);
        while (index != NULL)
        {
            printf("%d: ", ++counter);
            sas_record_print(list_access(list_ptr, index));
            index = list_iter_next(index);
        }
        assert(num_in_list == counter);
    }
    printf("\n");
}

/* This creates a list for storing secondary user records.  Based on
 * the type, the list can either store items in sorted or unsorted order.
 *
 * The only difference is if a comparison function pointer is installed
 * in the list header block.  Unsorted lists do not need a comparison
 * function, so the pointer is null.
 *
 * Notice this function has a list point as input.  If the pointer is
 * null, then this is the first time a list is created.  If it is not
 * null, then the old list should be destructed and a pointer to the
 * new list is returned.  This interface is similar to how the
 * recalloc() function works.
 */
ListPtr sas_create(ListPtr list_ptr, const char *type)
{
    int (*comp_function) (const su_info_t *, const su_info_t *);
    if (strcmp(type, "assignment list") == 0)
        comp_function = sas_compare;
    else
        comp_function = sas_compare;

    if (list_ptr != NULL)
    {
        printf("Replacing existing %s\n", type);
        list_destruct(list_ptr);
    }
    else
    {
        printf("New %s\n", type);
    }
    return list_construct(comp_function);
}

/* This function adds the secondary record to one of the lists.
 *
 * If the secondary user ID is found in the assigned list, then two cases
 * are possible.
 *    If the channel ID is the same, then simply update all the other
 *      information for the secondary user.
 *    If the channel ID is different, then remove the secondary user
 *      from the assigned list and place it at the tail of the waiting
 *      queue (and update all the secondary information).
 * If the secondary user is not in the assigned list, then determine
 *      if the secondary user is in the waiting queue, and if so update
 *      the secondary user information.
 * If the secondary user is not found in either the assigned list
 *      or waiting queue, then add the secondary user to the tail of
 *      the waiting queue
 */
void sas_add(ListPtr assn_ptr, int size, ListPtr wait_ptr)
{
    int add_action = -2;
    IteratorPtr P_assn;
    IteratorPtr P_wait;
    su_info_t *rec_ptr;
    rec_ptr = (su_info_t *) calloc(1, sizeof(su_info_t));
    sas_record_fill(rec_ptr);

    // Make sure the list exists
    if (assn_ptr == NULL || wait_ptr == NULL)
        return;

    P_assn = list_elem_find(assn_ptr, rec_ptr);
    P_wait = list_elem_find(wait_ptr, rec_ptr);
    // figure out how to add and set add_action
    // If the ID is the same in the assigned list
    if (P_assn != NULL)
    {
        // If the channel is the same
        if (list_access(assn_ptr, P_assn)->channel == rec_ptr->channel)
        {
            *list_access(assn_ptr, P_assn) = *rec_ptr;
            add_action = 0;
            free(rec_ptr);
            rec_ptr = list_access(assn_ptr, P_assn);
        }
        else
        {
            list_remove(assn_ptr, P_assn);
            list_insert(wait_ptr, rec_ptr, NULL);
            add_action = 1;
        }
    }   // If the ID is the same in the wait list
    else if (P_wait != NULL)
    {
        *list_access(wait_ptr, P_wait) = *rec_ptr;
        add_action = 2;
        free(rec_ptr);
        rec_ptr = list_access(wait_ptr, P_wait);
    }
    else
    {
        // Add the user to the tail of the waiting queue
        list_insert(wait_ptr, rec_ptr, NULL);
        add_action = 3;
    }

    if (add_action == 3)
    {
        printf("Inserted new waiting SU: %d\n", rec_ptr->su_id);
    }
    else if (add_action == 0)
    {
        printf("Updated assigned SU: %d\n", rec_ptr->su_id);
    }
    else if (add_action == 1)
    {
        printf("Moved assigned SU to waiting: %d\n", rec_ptr->su_id);
    }
    else if (add_action == 2)
    {
        printf("Updated waiting SU: %d\n", rec_ptr->su_id);
    }
    else
    {
        printf("Error with return value!\n");
    }
    rec_ptr = NULL;
}

/* This function prints the records with the matching channel number in the
 * assigned sorted list.
 */
void sas_lookup(ListPtr list_ptr, int channel_no)
{
    IteratorPtr P = NULL;
    su_info_t *rec_ptr = NULL;
    int num_in_list;

    // Make sure the list exists
    if (list_ptr == NULL)
        return;

     num_in_list = list_size(list_ptr);

    if (num_in_list == 0)
    {
        printf("List is empty: no users on ch %d\n", channel_no);
    }
    else
    {
        printf("Assignment list has %d records.  Looking for SUs on ch %d\n",
               num_in_list, channel_no);

        P = list_iter_front(list_ptr);

        // print record of each user on channel
        while (P != NULL)
        {
            rec_ptr = list_access(list_ptr, P);
            if (rec_ptr->channel == channel_no)
                sas_record_print(rec_ptr);
            P = list_iter_next(P);
        }

    }
    rec_ptr = NULL;
}

/* This function removes the record from either the sorted assigned list
 * or the waiting queue.  There can only be one match.
 */
void sas_remove(ListPtr assn_list, ListPtr wait_q, int su_id)
{
    su_info_t *rec_ptr = NULL;
    int assigned_or_waiting = -1;
    IteratorPtr P = NULL;
    su_info_t comp_dummy;
    comp_dummy.su_id = su_id;

    // Make sure the list exists
    if (assn_list == NULL || wait_q == NULL)
        return;

    // use list_elem_find and list_remove
    P = list_elem_find(assn_list, &comp_dummy);
    if (P != NULL)
    {
        rec_ptr = list_remove(assn_list, P);
        assigned_or_waiting = 0;
    }
    else
    {
        P = list_elem_find(wait_q, &comp_dummy);
        if (P != NULL)
        {
            rec_ptr = list_remove(wait_q, P);
            assigned_or_waiting = 1;
        }
    }


    if (rec_ptr == NULL)
    {
        printf("Did not remove: %d\n", su_id);
    }
    else if (assigned_or_waiting == 0)
    {
        assert(rec_ptr->su_id == su_id);
        printf("Removed: %d from assigned list\n", su_id);
        sas_record_print(rec_ptr);
    }
    else if (assigned_or_waiting == 1)
    {
        assert(rec_ptr->su_id == su_id);
        printf("Removed: %d from waiting queue\n", su_id);
        sas_record_print(rec_ptr);
    }
    free(rec_ptr);
    rec_ptr = NULL;
}

/* This function removes the records from the sorted assigned list
 * that are assigned to 'channel' and appends them to the waiting queue.
 * The search of the assigned list must be by increasing order of the
 * secondary uers IDs.  A secondary user record removed from the assigned
 * list is inserted into the tail of the waiting queue.
 */
void sas_move(ListPtr assn_list, int channel, ListPtr wait_q)
{
    su_info_t *rec_ptr = NULL;
    int count_removed = 0;
    IteratorPtr P = NULL;
    IteratorPtr nextP = NULL;

    // Make sure the list exists
    if (assn_list == NULL || wait_q == NULL)
        return;

    P = list_iter_front(assn_list);

    while (P != NULL)
    {
        nextP = list_iter_next(P);
        rec_ptr = list_access(assn_list, P);
        if (rec_ptr->channel == channel)
        {
            list_remove(assn_list, P);
            list_insert(wait_q, rec_ptr, NULL);
            count_removed++;
        }
        P = nextP;
    }


    if (count_removed == 0)
    {
        printf("Did not find any users on channel %d\n", channel);
    }
    else
    {
        printf("Removed %d from channel %d\n", count_removed, channel);
    }
}

/* Change the channel number of any secondary user in the assigned
 * list that is assigned to the old_channel to the new_channel
 */
void sas_change(ListPtr list_ptr, int old_channel, int new_channel)
{
    int count_moved = 0;
    su_info_t *rec_ptr = NULL;
    IteratorPtr P;
    
    if (list_ptr == NULL)
        return;
    
    P = list_iter_front(list_ptr);

    while (P != NULL)
    {
        rec_ptr = list_access(list_ptr, P);
        if (rec_ptr->channel == old_channel)
        {
            rec_ptr->channel = new_channel;
            count_moved++;
        }
        P = list_iter_next(P);
    }

    if (count_moved == 0)
    {
        printf("Did not find any users on channel %d\n", old_channel);
    }
    else
    {
        printf("Moved %d users from channel %d to %d\n", count_moved,
               old_channel, new_channel);
    }
}

/* Assign a secondary user on the waiting list to a channel, if possible.
 *
 * First, if the waiting queue is empty, no move is possible
 *
 * Next, if the assigned list is full, no move is possible
 *
 * Finally, if neither case, then remove the secondary user at the head
 * of the waiting queue and insert into the assigned list, setting
 * the channel.
 */
void sas_assign(ListPtr assn_ptr, int size, ListPtr wait_q, int channel)
{
    su_info_t *rec_ptr = NULL;
    int assign_action = -2;

    // Check to make sure the lists exist
    if (assn_ptr == NULL || wait_q == NULL)
        return;

    if (list_size(wait_q) == 0)
        assign_action = 0;
    else if (list_size(assn_ptr) == size)
        assign_action = 1;
    else
    {
        rec_ptr = list_remove(wait_q, list_iter_front(wait_q));
        list_insert_sorted(assn_ptr, rec_ptr);
        rec_ptr->channel = channel;
        assign_action = 2;
    }

    if (assign_action == 0)
    {
        printf("No secondary users are waiting\n");
    }
    else if (assign_action == 1)
    {
        printf("User(s) waiting but the assigned list is full %d\n", size);
    }
    else if (assign_action == 2)
    {
        printf("Moved waiting SU %d to channel: %d\n", rec_ptr->su_id, channel);
    }
    else
    {
        printf("Error with assign action flag!\n");
    }
    rec_ptr = NULL;
}

/* print count of records in the assigned list and waiting queue
 */
void sas_stats(ListPtr sorted, int sorted_size, ListPtr unsorted)
{
    // Make sure the lists exist
    if (sorted == NULL || unsorted == NULL)
        return;

    // get the number in list and size of the list
    int num_in_list = list_size(sorted);   // fix!
    int num_in_queue = list_size(unsorted);
    printf("List records:  %d, Max list size: %d  ",
           num_in_list, sorted_size);
    printf("Queue records: %d\n", num_in_queue);
}

/* this function frees the memory for either a sorted or unsorted list.
 */
void sas_cleanup(ListPtr list_ptr)
{
    // Make sure the list exists
    if (list_ptr == NULL)
        return;

    list_destruct(list_ptr);
}

/* Checks for a invalid channel number.  Channel numbers must be 1:10
 *
 * returns true if invalild channel
 *         false if valid channel
 */
int sas_invalid_ch(int channel_num)
{
    if (channel_num < 1 || channel_num > 10) return 1;
    return 0;
}

/* Prompts user for secondary record input starting with the SU ID.
 * The input is not checked for errors but will default to an acceptable value
 * if the input is incorrect or missing.
 *
 * The input to the function assumes that the structure has already been
 * created.  The contents of the structure are filled in.
 *
 * There is no output.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void sas_record_fill(su_info_t *new)
{
    char line[MAXLINE];
    char str[MAXLINE];
    assert(new != NULL);

    printf("secondary user ID number:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->su_id);
    printf("IP address:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->ip_address);
    printf("Access point IP address:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->access_point);

    printf("Authenticated (T/F):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%s", str);
    if (strcmp(str, "T")==0 || strcmp(str, "t")==0)
        new->authenticated = 1;
    else
        new->authenticated = 0;

    printf("Privacy (none|standard|strong|NSA):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%s", str);
    if (strcmp(str, "standard")==0)
        new->privacy = 1;
    else if (strcmp(str, "strong")==0)
        new->privacy = 2;
    else if (strcmp(str, "NSA")==0)
        new->privacy = 3;
    else
        new->privacy = 0;

    printf("Band (2.4|5.0):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%f", &new->band);

    printf("Channel:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->channel);
    if (sas_invalid_ch(new->channel))
        new->channel = 10;

    printf("Data rate:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%f", &new->data_rate);

    printf("Time received (int):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->time_received);
    printf("\n");
}

/* print the information for a particular secondary user record
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void sas_record_print(su_info_t *rec)
{
    const char *pri_str[] = {"none", "standard", "strong", "NSA"};
    assert(rec != NULL);
    printf("ID: %d, C: %d,", rec->su_id, rec->channel);
    printf(" MIP: %d, AID: %d,", rec->ip_address,
           rec->access_point);
    printf(" Auth: %s,", rec->authenticated ? "T" : "F");
    printf(" Pri: %s, B: %g,", pri_str[rec->privacy], rec->band);
    printf(" R: %g", rec->data_rate);
    printf(" Time: %d\n", rec->time_received);
}


/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
