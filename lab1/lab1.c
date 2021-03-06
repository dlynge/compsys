/* lab1.c
 * David Lynge
 * dlynge
 * ECE 2230 Fall 2018
 * MP1
 *
 * Purpose: This program is designed to simulate having to interact with a
 * sequential list. This list helps to manage RF bands by providing functions
 * to control items within the list with a specific channel number.
 *
 * Assumptions: We will assume that the print functions and input functions work
 * as expected since they were given. Also, we will assume the user exits the
 * program with the 'QUIT' command and not ctrl+c. This allows us to clean up
 * memory to avoid memory-leaks.
 *
 * Bugs: If the program is not ended by typing 'QUIT', then the allocated
 * memory is not free'd and programs like valgrid may detect a memory leak. This
 * is not a problem in this scope, however, since we are assuming the user does
 * not do this.
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "saslist.h"

#define MAXLINE 256

// function prototypes for lab1.c
void fill_sec_usr_record(struct secusr_info_t *new);
void print_sec_usr_rec(struct secusr_info_t *rec);
int invalid_ch(int channel_num);

int main(int argc, char *argv[])
{
    char line[MAXLINE];
    char command[MAXLINE];
    char junk[MAXLINE];
    int list_size = -1;
    int num_items;
    int input_1, input_2;
    int i;
    struct secusr_info_t *rec_ptr = NULL;
    struct saslist_t *sas_list = NULL;

    if (argc != 1)
    {
        printf("Usage: ./lab1\n");
        exit(1);
    }
    printf("Welcome to lab1. Possible commands:\n");
    printf("CREATE x\nADDSU\nLISTCH x\nDELSU x\nCLEARCH x\n");
    printf("CHANGECH old new\nSTATS\nPRINT\nQUIT\n");

    // remember fgets includes newline \n unless line too long
    while (fgets(line, MAXLINE, stdin) != NULL)
    {
        num_items = sscanf(line, "%s%d%d%s", command, &input_1, &input_2, junk);
        if (num_items == 2 && strcmp(command, "CREATE") == 0)
        {
            list_size = input_1;
            if (list_size < 1)
            {
                printf("Cannot create list: invalid size %d\n", list_size);
            }
            else
            {
                // clean up old list if necessary and create a new empty one
                if (sas_list != NULL)
                    saslist_destruct(sas_list);

                sas_list = saslist_construct(list_size);
                printf("New list with size: %d\n", list_size);
            }
        }
        else if (num_items == 1 && strcmp(command, "QUIT") == 0)
        {
            printf("Goodbye\n");
            saslist_destruct(sas_list);
            break;
        }
        else if (num_items == 1 && strcmp(command, "ADDSU") == 0)
        {
            rec_ptr = (struct secusr_info_t *) malloc(sizeof(struct secusr_info_t));
            fill_sec_usr_record(rec_ptr);

            // Add the new item and respond accordingly to the return value
            int added_return = saslist_add(sas_list, rec_ptr);

            if (added_return == 1)
            {
                printf("Inserted: %d\n", rec_ptr->su_id);
            }
            else if (added_return == 0)
            {
                printf("Updated: %d\n", rec_ptr->su_id);
            }
            else if (added_return == -1)
            {
                printf("Rejected: %d\n", rec_ptr->su_id);
            }
            else
            {
                printf("Error with return value! Fix your code.\n");
            }
            free(rec_ptr);
            rec_ptr = NULL;
        }
        else if (num_items == 2 && strcmp(command, "LISTCH") == 0)
        {
            // Get all of the items in the list with a certain channel
            int num_in_list = saslist_number_entries(sas_list);
            if (invalid_ch(input_1))
            {
                printf("Invalid channel number %d for LISTCH\n", input_1);
            }
            else if (num_in_list == 0)
            {
                printf("List empty no users on ch %d\n", input_1);
            }
            else
            {
                printf("List has %d records.  Looking for ch %d\n",
                       num_in_list, input_1);
                // print record of each user on channel
                for (i = 0; i < num_in_list; i++)
                {
                    rec_ptr = saslist_access(sas_list, i);
                    if (rec_ptr->channel == input_1)
                        print_sec_usr_rec(rec_ptr);
                }
            }
            rec_ptr = NULL;
        }
        else if (num_items == 2 && strcmp(command, "DELSU") == 0)
        {
            // Remove the item that corresponds with the ID given by the user
            rec_ptr = saslist_remove(sas_list, saslist_lookup(sas_list, input_1));

            if (rec_ptr == NULL)
            {
                printf("Did not remove: %d\n", input_1);
            }
            else
            {

                printf("Removed: %d\n", input_1);
                print_sec_usr_rec(rec_ptr);
            }
            free(rec_ptr);
            rec_ptr = NULL;
        }
        else if (num_items == 2 && strcmp(command, "CLEARCH") == 0)
        {
            // Loop through all the items and remove the ones where the channel matches
            int count_removed = 0;
            for (i = 0; i < saslist_number_entries(sas_list); i++)
                if (saslist_access(sas_list, i)->channel == input_1)
                {
                    free(saslist_remove(sas_list, i--));
                    count_removed++;
                }

            // remove records on matching channel.  After done print
            // message about number removed
            if (invalid_ch(input_1))
            {
                printf("Invalid channel number %d for CLEARCH\n", input_1);
            }
            else if (count_removed == 0)
            {
                printf("Did not find any users on channel %d\n", input_1);
            }
            else
            {
                printf("Removed %d from channel %d\n", count_removed, input_1);
            }
            rec_ptr = NULL;
        }
        else if (num_items == 3 && strcmp(command, "CHANGECH") == 0)
        {
            // Loop through the items and change the channels if they match
            int count_moved = 0;
            for (i = 0; i < saslist_number_entries(sas_list); i++)
                if (saslist_access(sas_list, i)->channel == input_1)
                {
                    saslist_access(sas_list, i)->channel = input_2;
                    count_moved++;
                }
            if (invalid_ch(input_1) || invalid_ch(input_2))
            {
                printf("Invalid old %d or new %d channel for CHANGECH\n",
                       input_1, input_2);
            }
            else
            {
                // move records on channel input_1 to channel input_2.
                // After done print message about number moved
                if (count_moved == 0)
                {
                    printf("Did not find any users on channel %d\n", input_1);
                }
                else
                {
                    printf("Moved %d users from channel %d to %d\n", count_moved,
                           input_1, input_2);
                }
            }
            rec_ptr = NULL;
        }
        else if (num_items == 1 && strcmp(command, "STATS") == 0)
        {
            // get the number in list and size of the list
            int num_in_list = saslist_number_entries(sas_list);
            int size_of_list = saslist_arraysize(sas_list);
            printf("Number records: %d, Array size: %d\n",
                   num_in_list, size_of_list);
        }
        else if (num_items == 1 && strcmp(command, "PRINT") == 0)
        {
            int num_in_list = saslist_number_entries(sas_list);
            if (num_in_list == 0)
            {
                printf("List empty\n");
            }
            else
            {
                printf("List has %d records\n", num_in_list);
                for (i = 0; i < num_in_list; i++)
                {
                    printf("%d: ", i+1);
                    rec_ptr = saslist_access(sas_list, i);
                    print_sec_usr_rec(rec_ptr);
                }
            }
            rec_ptr = NULL;
        }
        else
        {
            printf("# %s", line);
        }
    }
    exit(0);
}

/* Checks for a invalid channel number.  Channel numbers must be 1:10
 *
 * returns true if invalild channel
 *         false if valid channel
 */
int invalid_ch(int channel_num)
{
    if (channel_num < 1 || channel_num > 10) return 1;
    return 0;
}

/* Prompts user for secondary user record input starting with the ID number
 * assigned to the secondary uers by the SAS.
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
void fill_sec_usr_record(struct secusr_info_t *new)
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


    printf("Band (2.4|3.5|5.0):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%f", &new->band);

    printf("Channel:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->channel);

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
void print_sec_usr_rec(struct secusr_info_t *rec)
{
    const char *pri_str[] = {"none", "standard", "strong", "NSA"};
    assert(rec != NULL);
    printf("ID: %d,", rec->su_id);
    printf(" MIP: %d, AID: %d,", rec->ip_address,
           rec->access_point);
    printf(" Auth: %s,", rec->authenticated ? "T" : "F");
    printf(" Pri: %s, B: %g, C: %d,", pri_str[rec->privacy],
           rec->band, rec->channel);
    printf(" R: %g", rec->data_rate);
    printf(" Time: %d\n", rec->time_received);
}


/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
