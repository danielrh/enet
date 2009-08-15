/** 
 @file list.c
 @brief ENet linked list functions
*/
#define ENET_BUILDING_LIB 1
#include "enet/list.h"
#include <assert.h>
/** 
    @defgroup list ENet linked list utility functions
    @ingroup private
    @{
*/
void
enet_list_clear (ENetList * list)
{
   list -> sentinel.next = & list -> sentinel;
   list -> sentinel.previous = & list -> sentinel;
   list -> byte_size = 0;
}

static int
enet_same_list (ENetList *list, ENetListIterator query)
{
   ENetListIterator position;
   if (&list -> sentinel.next == query )
       return 1;
   for (position = enet_list_begin (list);
        position != enet_list_end (list);
        position = enet_list_next (position)) {
       if ( position == query )
           return 1;
   }    
   return 0;
}

ENetListIterator
enet_list_insert (ENetList* list, ENetListIterator position, void * data, unsigned short size)
{
   ENetListIterator result = (ENetListIterator) data;
   
   result -> byte_size = size;
   result -> previous = position -> previous;
   result -> next = position;

   result -> previous -> next = result;
   position -> previous = result;
   list -> byte_size += size;

   assert (enet_same_list(list, position));
   
   return result;
}

void *
enet_list_remove (ENetList* list, ENetListIterator position)
{
   assert (enet_same_list(list, position));
   list -> byte_size -= position -> byte_size;
   position -> previous -> next = position -> next;
   position -> next -> previous = position -> previous;
   return position;
}
size_t enet_list_byte_size(ENetList * list) {
   return list->byte_size;
}
size_t
enet_list_size (ENetList * list)
{
   size_t size = 0;
   ENetListIterator position;

   for (position = enet_list_begin (list);
        position != enet_list_end (list);
        position = enet_list_next (position))
     ++ size;
   
   return size;
}

/** @} */
