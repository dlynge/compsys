/* list.c
 * David Lynge
 * dlynge
 * ECE 2230 Fall 2018
 * MP4
 *
 * Propose: This file holds all functions necessary for controlling a 2 way
 * linked list with an arbitrary datatype.
 *
 * Assumptions: This file knows nothing about the functionality of the program
 * that uses it. Also, we assume that no dangling pointers are inputted into
 * the functions.
 *
 * Bugs: None known.
 */ 

#include <stdlib.h>
#include <assert.h>

#include "datatypes.h"   // defines data_t 
#include "list.h"        // defines public functions for list ADT 

// definitions for private constants used in list.c only
#define SORTED_LIST   0xCACACAC
#define UNSORTED_LIST 0x3535353

// prototypes for private functions used in list.c only
void list_debug_validate(list_t *L);
void SelectionSort(list_t *A, list_node_t *m, list_node_t *n);
void IterativeSelectionSort(list_t *A, list_node_t *m, list_node_t *n);
void InsertionSort(list_t *A, list_node_t *m, list_node_t *n);
void SplitLR(list_t *A, list_t *L, list_t *R);
void MergeSort(list_t *A);
void MergeLR(list_t *A, list_t *L, list_t *R);
list_node_t *FindMax(list_t *A, list_node_t *m, list_node_t *n);

/* ----- below are the functions  ----- */

/* Allocates a new, empty list
 *
 * By convention, the list is initially assumed to be sorted.  The field sorted
 * can only take values SORTED_LIST or UNSORTED_LIST
 *
 * The inital empty list must have
 *  1. current_list_size = 0
 *  2. list_sorted_state = SORTED_LIST
 *
 * Use list_destruct to remove and deallocate all elements on a list
 * and the header block.
 */
list_t *list_construct(int (*fcomp)(const data_t *, const data_t *))
{
    list_t *L;

    L = (list_t *) malloc(sizeof(list_t));
    L->head = NULL;
    L->tail = NULL;
    L->current_list_size = 0;
    L->list_sorted_state = SORTED_LIST;
    L->comp_proc = fcomp;

    // the last line of this function must call validate
    //list_debug_validate(L);
    return L;
}

/* Purpose: return the count of number of elements in the list.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * Return: the number of elements stored in the list.
 */
int list_size(list_t *list_ptr)
{
    assert(NULL != list_ptr);
    assert(list_ptr->current_list_size >= 0);
    return list_ptr->current_list_size;
}

/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 *
 * Free all elements in the list and the header block.
 */
void list_destruct(list_t *list_ptr)
{
    // the first line must validate the list
    //list_debug_validate(list_ptr);
    // add your code after this line

    IteratorPtr temp;

    // Remove all data_t and list_node_t in the list
    while (list_ptr->head != NULL)
    {
        temp = list_ptr->head;
        list_ptr->head = list_ptr->head->next;
        free(temp->data_ptr);
        free(temp);
    }

    // Remove header block
    free(list_ptr);
    list_ptr = NULL;
}

/* Return an Iterator that points to the last list_node_t. If the list is empty
 * then the pointer that is returned is NULL.
 */
list_node_t * list_iter_back(list_t *list_ptr)
{
    assert(NULL != list_ptr);
    //list_debug_validate(list_ptr);
    return list_ptr->tail;
}

/* Return an Iterator that points to the first element in the list.  If the
 * list is empty the value that is returned in NULL.
 */
list_node_t * list_iter_front(list_t *list_ptr)
{
    assert(NULL != list_ptr);
    //list_debug_validate(list_ptr);
    return list_ptr->head;
}

/* Advance the Iterator to the next item in the list.
 * If the iterator points to the last item in the list, then
 * this function returns NULL to indicate there is no next item.
 *
 * It is a catastrophic error to call this function if the
 * iterator, idx_ptr, is null.
 */
list_node_t * list_iter_next(list_node_t * idx_ptr)
{
    assert(idx_ptr != NULL);
    return idx_ptr->next;
}

/* Finds an element in a list and returns a pointer to it.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * elem_ptr: element against which other elements in the list are compared.
 *           Note: uses the comp_proc function pointer found in the list_t
 *           header block.
 *
 * The function returns an Iterator pointer to the list_node_t that contains
 * the first matching element if a match if found.  If a match is not found
 * the return value is NULL.
 *
 * Note: to get a pointer to the matching data_t memory block pass the return
 *       value from this function to the list_access function.
 */
list_node_t * list_elem_find(list_t *list_ptr, data_t *elem_ptr)
{
    //list_debug_validate(list_ptr);

    IteratorPtr P = list_ptr->head;

    // Iterate through the list, comparing each data_t
    while (P != NULL)
    {
        if (list_ptr->comp_proc(elem_ptr, P->data_ptr) == 0)
            return P;

        P = P->next;
    }

    return NULL;
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the comparison function, comp_proc.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use list_insert_sorted, the list preserves its sorted nature.
 *
 * If you use list_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call list_insert_sorted, this subroutine
 * must generate a system error and the program should immediately stop.
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type data_t.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the front of the list than B
 *   -1: B should be closer to the front of the list than A
 *    0: A and B are equal in rank
 *
 * Note: if the element to be inserted is equal in rank to an element already
 * in the list, the newly inserted element will be placed after all the
 * elements of equal rank that are already in the list.
 */
void list_insert_sorted(list_t *list_ptr, data_t *elem_ptr)
{
    assert(NULL != list_ptr);
    assert(SORTED_LIST == list_ptr->list_sorted_state);

    IteratorPtr P = list_ptr->head;

    // Create new node
    list_node_t *newNode = (list_node_t *)malloc(sizeof(list_node_t));
    newNode->data_ptr = elem_ptr;
    newNode->prev = NULL;
    newNode->next = NULL;


    if (P == NULL)  // Empty List
    {
        list_ptr->head = list_ptr->tail = newNode;
    }
    else if (list_ptr->comp_proc(elem_ptr, list_ptr->head->data_ptr) == 1) // Insert at head
    {
        list_ptr->head->prev = newNode;
        newNode->next = list_ptr->head;
        list_ptr->head = newNode;
    }
    else if (list_ptr->comp_proc(elem_ptr, list_ptr->tail->data_ptr) != 1) // Insert at tail
    {
        list_ptr->tail->next = newNode;
        newNode->prev= list_ptr->tail;
        list_ptr->tail = newNode;
    }
    else    // Insert somewhere inside list (not the tail or head)
    {
        while (P != NULL && list_ptr->comp_proc(elem_ptr, P->data_ptr) != 1)
            P = P->next;
        newNode->next = P;
        newNode->prev = P->prev;
        newNode->next->prev = newNode;
        newNode->prev->next = newNode;
    }
    list_ptr->current_list_size++;
    // the last line of this function must be the following
    //list_debug_validate(list_ptr);
}

/* Inserts the data element into the list in front of the iterator
 * position.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * elem_ptr: pointer to the memory block to be inserted into list.
 *
 * idx_ptr: pointer to a list_node_t.  The element is to be inserted as a
 *          member in the list at the position that is immediately in front
 *          of the position of the provided Iterator pointer.
 *
 *          If the idx_ptr pointer is NULL, the the new memory block is
 *          inserted after the last element in the list.  That is, a null
 *          iterator pointer means make the element the new tail.
 *
 * If idx_ptr is set using
 *    -- list_iter_front, then the new element becomes the first item in
 *       the list.
 *    -- list_iter_back, then the new element is inserted before the last item
 *       in the list.
 *    -- for any other idx_ptr, the new element is insert before the
 *       Iterator
 *
 * For example, to insert at the tail of the list do
 *      list_insert(mylist, myelem, NULL)
 * to insert at the front of the list do
 *      list_insert(mylist, myelem, list_iter_front(mylist))
 *
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the list_ptr->list_sorted_state must be equal
 * to UNSORTED_LIST.
 */
void list_insert(list_t *list_ptr, data_t *elem_ptr, list_node_t * idx_ptr)
{
    assert(NULL != list_ptr);

    // insert your code after this line

    // Create new node
    list_node_t *newNode = (list_node_t *)malloc(sizeof(list_node_t));
    newNode->data_ptr = elem_ptr;
    newNode->next = newNode->prev = NULL;

    // Check if they are inserting at end
    if (idx_ptr != NULL)
    {
        newNode->next = idx_ptr;
        newNode->prev = idx_ptr->prev;
        newNode->next->prev = newNode;
        if (idx_ptr == list_ptr->head)
            list_ptr->head = newNode;
        else
            newNode->prev->next = newNode;
    }
    else
    {
        newNode->prev = list_ptr->tail;
        if (list_ptr->current_list_size != 0)
            newNode->prev->next = newNode;
        else
            list_ptr->head = newNode;
        list_ptr->tail = newNode;
    }

    list_ptr->current_list_size++;
    // the last two lines of this function must be the following
    list_ptr->list_sorted_state = UNSORTED_LIST;
    //list_debug_validate(list_ptr);
}

/* Removes the element from the specified list that is found at the
 * iterator pointer.  A pointer to the data element is returned.
 *
 * list_ptr: pointer to list-of-interest.
 *
 * idx_ptr: pointer to position of the element to be accessed.  This pointer
 *          must be obtained from list_elem_find, list_iter_front, or
 *          list_iter_next, or list_iter_back.
 *
 *          If the idx_ptr is null, then assume that the first item
 *          at the head is to be removed.
 *
 * If the list is empty, then the function should return NULL.  Note: if
 *    the list is empty, then the iterator should be a NULL pointer.
 *
 * Note to remove the element at the front of the list use either
 *     list_remove(mylist, list_iter_front(mylist))
 *  or
 *     list_remove(mylist, NULL);
 *
 * Note: a significant danger with this function is that once
 * an element is removed from the list, the idx_ptr is dangling.  That
 * is, the idx_ptr now points to memory that is no longer valid.
 * You should not call list_iter_next on an iterator after there
 * has been a call to list_remove with the same iterator.
 *
 * When you remove the list_node_t in this function, you should null the next
 * and prev pointers before you free the memory block to avoid accidental use
 * of these now invalid pointers.
 */
data_t * list_remove(list_t *list_ptr, list_node_t * idx_ptr)
{
    assert(NULL != list_ptr);
    if (0 == list_ptr->current_list_size)
        assert(idx_ptr == NULL);

    if (idx_ptr == NULL)
        idx_ptr = list_ptr->head;

    data_t *rtn;

    // insert your code after this line
    // Rearrange pointers
    if (list_ptr->current_list_size == 1)   // list is only item
    {
        list_ptr->head = NULL;
        list_ptr->tail = NULL;
    }
    else if (list_ptr->head == idx_ptr)     // idx is at head
    {
        list_ptr->head = idx_ptr->next;
        idx_ptr->next->prev = NULL;
    }
    else if (list_ptr->tail == idx_ptr)     // idx is at tail
    {
        list_ptr->tail = idx_ptr->prev;
        idx_ptr->prev->next = NULL;
    }
    else                                    // idx is somewhere in middle
    {
        idx_ptr->prev->next = idx_ptr->next;
        idx_ptr->next->prev = idx_ptr->prev;
    }
    // Free memory
    rtn = idx_ptr->data_ptr;
    free(idx_ptr);

    list_ptr->current_list_size--;
    // the last line should verify the list is valid after the remove
    //list_debug_validate(list_ptr);
    return rtn;  // fix the return value
}

/* Return a pointer to an element stored in the list, at the Iterator position
 *
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from list_construct.
 *
 * idx_ptr: pointer to position of the element to be accessed.  This pointer
 *          must be obtained from list_elem_find, list_iter_front,
 *          list_iter_back, or list_iter_next.
 *
 * return value: pointer to the data_t element found in the list at the
 * iterator position. A value NULL is returned if
 *     a:  the idx_ptr is NULL.
 *     b:  the list is empty
 */
data_t * list_access(list_t *list_ptr, list_node_t * idx_ptr)
{
    assert(NULL != list_ptr);
    if (idx_ptr == NULL || list_ptr->current_list_size == 0)
        return NULL;
    // debugging function to verify that the structure of the list is valid
    //list_debug_validate(list_ptr);

    return idx_ptr->data_ptr;   // you have to fix the return value
}

// =====----- Sorting -----=====
/*
 * This function will sort a list based upon the order described by comp_proc.
 * The input is the list you wish to sort and the sort type listed below.
 * 1: Insertion Sort
 * 2: Selection Sort (Recursive)
 * 3: Selection Sort (Iterative)
 * 4: Merge Sort
 * Any other inputs will be ignored for sort_type
 * There is no output, as the list is sorted by reference 
 */
void list_sort(ListPtr list_ptr, int sort_type)
{
    // Make sure the number is within bounds
    if (sort_type < 1 || sort_type > 4)
        return;

    // Make sure the list has more than one item
    if (list_ptr == NULL || list_ptr->current_list_size < 2)
        return;

    // Select which sorting method to use
    if (sort_type == 2)
        SelectionSort(list_ptr, list_ptr->head, list_ptr->tail);
    else if (sort_type == 3)
        IterativeSelectionSort(list_ptr, list_ptr->head, list_ptr->tail);
    else if (sort_type == 1)
        InsertionSort(list_ptr, list_ptr->head, list_ptr->tail);
    else if (sort_type == 4)
        MergeSort(list_ptr);

    list_ptr->list_sorted_state = SORTED_LIST;
    list_debug_validate(list_ptr);
}

/* 
 * This function finds the "max" value in a list. the "max" value is the value
 * that comp_proc defines to be at the head of the list. 
 * 
 * The input for this function is the list you want to search, the first 
 * node pointer, and the last node pointer in the search.
 * 
 * The output is the pointer to the node with the max value
 */
list_node_t *FindMax(list_t *A, list_node_t *m, list_node_t *n) 
{
   list_node_t *i = m;
   list_node_t *j = m;     
            
    // Iterate i through the entire list          
   do {
      i = i->next;                      
      if (A->comp_proc(i->data_ptr, j->data_ptr) == 1) {          
         j = i;            
      }
   } while (i != n);           

   return j;                       
} 

/*
 * This function sorts a list based on selection sort (recursive). 
 * 
 * The input is the list you want to sort, the first and last nodes of the 
 * range within that list. We assime that m is before n in the list.
 * 
 * There is no output, as the list is passed by referance.
*/
void SelectionSort(list_t *A, list_node_t *m, list_node_t *n)
{
   list_node_t *MaxPosition;   
   data_t *temp;            
    

   if (m != n) {               
         MaxPosition = FindMax(A,m,n);
      
        // Exchange the two data pointers
         temp = m->data_ptr;
         m->data_ptr = MaxPosition->data_ptr;
         MaxPosition->data_ptr = temp;
      
      // Sort the rest of the list
         SelectionSort(A, m->next, n);
   } 
}

/*
 * This function sorts a list based on selection sort (iterative)
 * 
 * The input is the list you want to sort, the first and last nodes of the 
 * range within that list. We assime that m is before n in the list.
 * 
 * There is no output, as the list is passed by reference.
*/
void IterativeSelectionSort(list_t *A, list_node_t *m, list_node_t *n)
{
   list_node_t *MaxPosition;
   list_node_t *i;
   data_t *temp;


   while (m != n) {
   
      i = m; 
      MaxPosition = m;

    // Find the max position
      do {
         i = i->next;
         if (A->comp_proc(MaxPosition->data_ptr, i->data_ptr) == -1 ) MaxPosition = i;
      } while (i != n);
        // Swap the data pointers
      temp = m->data_ptr; m->data_ptr = MaxPosition->data_ptr; 
      MaxPosition->data_ptr = temp;
   
      m = m->next;
   }
}

/*
 * This function sorts a list based on insertion sort
 * 
 * The input is the list you want to sort, the first and last nodes of the 
 * range within that list. We assime that m is before n in the list.
 * 
 * There is no output, as the list is passed by reference.
*/
void InsertionSort(list_t *A, list_node_t *m, list_node_t *n)
{
    
    list_t temp;

    temp.head = temp.tail = NULL;
    temp.comp_proc = A->comp_proc;
    temp.current_list_size = 0;
    temp.list_sorted_state = SORTED_LIST;

    A->list_sorted_state = UNSORTED_LIST;
    while (m != NULL)
    {
        list_insert_sorted(&temp, list_remove(A, m));
        m = A->head;
    }
    *A = temp;
}

/*
 * This function splits a list into two seperate lists down the middle. If there
 * is an odd number of items, there will be one more item in the right list
 * 
 * The input is the list you wish to split and the left/right lists that store
 * the result.
 * 
 * There is no output, as each list is passed by reference.
 */
void SplitLR(list_t *A, list_t *L, list_t *R)
{
    int i;
    int size = A->current_list_size;
    for (i = 0; i < size / 2; i++)
        list_insert(L, list_remove(A, A->head), L->head);
    while (A->head != NULL)
        list_insert(R, list_remove(A, A->head), R->head);
}

/*
 * This function takes two lists, L & R, and combines them such that the resulting
 * list, A, is sorted accoring to comp_proc.
 * 
 * The input is the list you wish to split and the left/right lists that store
 * the result.
 * 
 * There is no output, as each list is passed by reference.
 */
void MergeLR(list_t *A, list_t *L, list_t *R)
{
    while (L->head != NULL && R->head != NULL)
        if (A->comp_proc(L->head->data_ptr, R->head->data_ptr) == 1)
            list_insert(A, list_remove(L, L->head), NULL);
        else
            list_insert(A, list_remove(R, R->head), NULL);
    while (L->head != NULL)
        list_insert(A, list_remove(L, L->head), NULL);
    while (R->head != NULL)
        list_insert(A, list_remove(R, R->head), NULL);
}

/*
 * This function sorts a list based on merge sort
 * 
 * The input is the list you want to sort, passed by reference.
 * 
 * There is no output, as the list is passed by reference.
*/
void MergeSort(list_t *A)
{
    list_t L;
    list_t R;


    if (A->current_list_size > 1)
    {
        L.comp_proc = A->comp_proc;
        L.head = L.tail = NULL;
        L.current_list_size = 0;
        R = L;
        SplitLR(A, &L, &R);
        MergeSort(&L);
        MergeSort(&R);
        MergeLR(A, &L, &R);
    }
}

/* This function verifies that the pointers for the two-way linked list are
 * valid. It checks if the list size equals the number of items in the list.
 *
 * No output is produced if the two-way linked list is correct.
 * The program terminates and prints a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive. An error may still exist in the list even
 * if these checks pass.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 */
void list_debug_validate(list_t *L)
{
    assert(NULL != L);
    assert(SORTED_LIST == L->list_sorted_state || UNSORTED_LIST == L->list_sorted_state);
    if (0 == L->current_list_size) assert(NULL == L->head && L->tail == NULL);
    if (NULL == L->tail)
        assert(NULL == L->head && 0 == L->current_list_size);
    else
        assert(NULL == L->tail->next);
    if (NULL == L->head)
        assert(NULL == L->tail && 0 == L->current_list_size);
    else
        assert(NULL == L->head->prev);
    if (NULL != L->tail && L->tail == L->head) assert(1 == L->current_list_size);
    if (1 == L->current_list_size)
    {
        assert(L->head == L->tail && NULL != L->tail);
        assert(NULL != L->tail->data_ptr);
        assert(NULL == L->tail->next && NULL == L->head->prev);
    }
    if (1 < L->current_list_size)
    {
        assert(L->head != L->tail && NULL != L->tail && NULL != L->head);
        list_node_t *R = L->head;
        int tally = 0;
        while (NULL != R)
        {
            if (NULL != R->next) assert(R->next->prev == R);
            else assert(R == L->tail);
            assert(NULL != R->data_ptr);
            ++tally;
            R = R->next;
        }
        assert(tally == L->current_list_size);
    }
    if (NULL != L->head && SORTED_LIST == L->list_sorted_state)
    {
        list_node_t *R = L->head;
        while (NULL != R->next)
        {
            assert(-1 != L->comp_proc(R->data_ptr, R->next->data_ptr));
            R = R->next;
        }
    }
}
/* commands for vim. ts: tabstop, sts: softtabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */

