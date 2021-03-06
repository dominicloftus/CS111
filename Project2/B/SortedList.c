
#include "SortedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
  if(list == NULL || element == NULL)
    return;

  SortedListElement_t* cur = list;

  //list empty
  if(cur->next == NULL){

    if(opt_yield & INSERT_YIELD)
      sched_yield();

    cur->next = element;
    cur->prev = element;
    element->prev = cur;
    element->next = NULL;
    return;
  }

  while(cur->next != NULL && strcmp(cur->next->key, element->key) < 0)
    cur = cur->next;

  if(opt_yield & INSERT_YIELD)
    sched_yield();

  //end of list
  if(cur->next == NULL){
    cur->next = element;
    list->prev = element;
    element->next = NULL;
    element->prev = cur;
  }
  //place alphabetically
  else{
    element->next = cur->next;
    element->prev = cur;
    cur->next->prev = element;
    cur->next = element;
  }
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element){
  if(element == NULL)
    return 1;

  if(opt_yield & DELETE_YIELD)
      sched_yield();
  
  if(element->prev != NULL){
    if(element->prev->next != element)
      return 1;
    else
      element->prev->next = element->next;
  }

  if(element->next != NULL){
    if(element->next->prev != element)
      return 1;
    else
      element->next->prev = element->prev;
  }
  return 0;
}


/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
  if(list == NULL)
    return NULL;

  SortedListElement_t* cur = list->next;

  while(cur != NULL){

    if(opt_yield & LOOKUP_YIELD)
      sched_yield();
    
    if(strcmp(cur->key, key) == 0)
      return cur;
    cur = cur->next;
  }

  return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list){
  if(list == NULL)
    return -1;

  SortedListElement_t* cur = list->next;
  int count = 0;

  while(cur != NULL){

    if(opt_yield & LOOKUP_YIELD)
      sched_yield();
    
    count++;
    cur = cur->next;
  }

  return count;
  
}

