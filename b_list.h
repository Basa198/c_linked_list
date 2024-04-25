#ifndef __B_LIST__
#define __B_LIST__

#include <stdlib.h>

typedef struct b_list_node {
    struct b_list_node *prev;
    struct b_list_node *next;
    void *data;
} b_list_node;

typedef struct {
    b_list_node *head;
    b_list_node *tail;
    unsigned int size;
    void (*free)(void *data);
} b_list;

typedef enum {
    LIST_BEGIN,
    LIST_END
} b_list_iterator_direction;

typedef struct {
    b_list_node *next;
    b_list_iterator_direction direction;
} b_list_iterator;

b_list_node* b_new_list_node(void *data);
b_list* b_new_list();
/**
 * Create a new list with custom free function
*/
b_list* b_new_list_with_func(void (*free)(void *));

b_list_node* b_list_rpush(b_list *list, b_list_node *node);
b_list_node* b_list_lpush(b_list *list, b_list_node *node);

b_list_node* b_list_rpop(b_list *list);
b_list_node* b_list_lpop(b_list *list);

b_list_node* b_list_fetch(b_list *list, int index);
b_list_node* b_list_find(b_list *list, int (*match)(void *a, void *b), void *data);

b_list_node* b_list_insert(b_list *list, b_list_node *new_node, int index);

void b_list_delete(b_list *list, b_list_node *node);
void b_list_delete_at(b_list *list, int index);

void b_free_list(b_list *list);

void b_list_sort(b_list *list, int (*compare)(void *a, void *b));

b_list_iterator* b_new_list_iterator(b_list *list, b_list_iterator_direction direction);
b_list_iterator* b_new_list_iterator_from_node(b_list_node *node, b_list_iterator_direction direction);
b_list_iterator* b_new_list_iterator_from_index(b_list *list, int index, b_list_iterator_direction direction); 
b_list_node* b_list_iterator_next(b_list_iterator *it); 
void b_free_list_iterator(b_list_iterator *it);

#endif