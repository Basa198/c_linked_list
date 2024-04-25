#include <stdlib.h>

#include "b_list.h"

void b_free_list(b_list *list) {
    if (!list) return;
    b_list_node *node = list->head;
    b_list_node *next;
    while (node) {
        if (list->free) list->free(node->data);
        next = node->next;
        free(node);
        node = next;
    } 
    free(list);
}

b_list_node* b_new_list_node(void *data) {
    b_list_node *node = malloc(sizeof(b_list_node));
    if (!node) return NULL;
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

b_list* b_new_list() {
    b_list *list = calloc(1, sizeof(b_list));
    if (!list) return NULL;
    return list;
}

b_list* b_new_list_with_func(void (*free)(void *data)) {
    b_list *list = malloc(sizeof(b_list));
    if (!list) return NULL;
    list->free = free;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

b_list_node* b_list_lpush(b_list *list, b_list_node *node) {
    if (!list || !node) return NULL;
    node->next = node->prev = NULL;
    if (!list->size) list->head = list->tail = node;
    else {
        list->head->prev = node;
        node->next = list->head;
        list->head = node;
    }
    list->size += 1;
    return node;
}

b_list_node* b_list_rpush(b_list *list, b_list_node *node) {
    if (!list || !node) return NULL;
    node->next = node->prev = NULL;
    if (!list->size) list->head = list->tail = node;
    else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    list->size += 1;
    return node;
}

b_list_node* b_list_rpop(b_list *list) {
    if (!list || !list->size) return NULL;
    b_list_node *node = list->tail;
    if (list->size-- == 1) list->head = list->tail = NULL;
    else {
        (list->tail = node->prev)->next = NULL;
        node->prev = node->next = NULL;
    }
    return node;
}

b_list_node* b_list_lpop(b_list *list) {
    if (!list || !list->size) return NULL;
    b_list_node *node = list->head;
    if (list->size-- == 1) list->head = list->tail = NULL;
    else {
        (list->head = node->next)->prev = NULL;
        node->prev = node->next = NULL;
    }
    return node;
}

b_list_node* b_list_fetch(b_list *list, int index) {
    if (!list || !list->size || index < 0 || index >= list->size) return NULL;
    b_list_node *node = list->head;
    while (--index >= 0) node = node->next;
    return node;
}

b_list_node* b_list_find(b_list *list, int (*match)(void *a, void *b), void *data) {
    if (!list) return NULL;
    b_list_node *node = list->head;
    while (node && !match(node->data, data)) node = node->next;
    return node;
}

b_list_node* b_list_insert(b_list *list, b_list_node *new_node, int index) {
    if (!list || !new_node || index < 0 || index > list->size) return NULL;
    new_node->next = new_node->prev = NULL;
    if (!list->size) list->head = list->tail = new_node;
    else if (index == 0) {
        new_node->next = list->head;
        list->head = list->head->prev = new_node;
    } else if (index == list->size) {
        new_node->prev = list->tail;
        list->tail = list->tail->next = new_node;
    } else {
        b_list_node *node = list->head;
        while (--index >= 0) node = node->next;
        new_node->next = node;
        new_node->prev = node->prev;
        node->prev = node->prev->next = new_node;
    }
    ++list->size;
    return new_node;
}

void b_list_delete(b_list *list, b_list_node *node) {
    if (!list || !node) return; 
    if (node->prev) node->prev->next = node->next;
    else list->head = node->next;
    if (node->next) node->next->prev = node->prev;
    else list->tail = node->prev;
    if (list->free) list->free(node->data);
    --list->size;
    free(node);
}

void b_list_delete_at(b_list *list, int index) {
    if (!list || index < 0 || index >= list->size) return;
    if (index == 0) b_list_delete(list, list->head);
    else if (index == list->size - 1) b_list_delete(list, list->tail);
    else {
        b_list_node* node = list->head;
        while (--index >= 0) node = node->next;
        node->prev->next = node->next;
        node->next->prev = node->prev;
        --list->size;
        free(node);
    }
}

void b_free_list_iterator(b_list_iterator *it) {
    if (!it) return;
    free(it);
}

b_list_iterator* b_new_list_iterator(b_list *list, b_list_iterator_direction direction) {
    if (!list) return NULL;
    b_list_iterator* it = malloc(sizeof(b_list_iterator));
    if (!it) return NULL;
    it->next = direction == LIST_BEGIN ? list->head : list->tail;
    it->direction = direction == LIST_BEGIN ? LIST_BEGIN : LIST_END;
    return it;
}

b_list_iterator* b_new_list_iterator_from_node(b_list_node *node, b_list_iterator_direction direction) {
    if (!node) return NULL;
    b_list_iterator* it = malloc(sizeof(b_list_iterator));
    if (!it) return NULL;
    it->next = node;
    it->direction = direction == LIST_BEGIN ? LIST_BEGIN : LIST_END;
    return it;
}

b_list_iterator* b_new_list_iterator_from_index(b_list *list, int index, b_list_iterator_direction direction) {
    if (!list || index < 0 || index >= list->size) return NULL;
    b_list_iterator *it = malloc(sizeof(b_list_iterator));
    if (!it) return NULL;
    it->direction = direction == LIST_BEGIN ? LIST_BEGIN : LIST_END;
    if (index == list->size) {
        it->next = list->tail;
    } else {
        b_list_node *node = list->head;
        while (--index >= 0) node = node->next;
        it->next = node;
    }
    return it;
}

b_list_node* b_list_iterator_next(b_list_iterator *it) {
    if (!it) return NULL;
    b_list_node* node = it->next;
    it->next = it->direction == LIST_BEGIN ? node->next : node->prev;
    return node;
}

static void merge(b_list_node **l1, b_list_node **l2, b_list_node **tail_ref, int (*compare)(void *a, void *b)) {
    b_list_node start;
    b_list_node *node = &start;
    b_list_node *left = *l1;
    b_list_node *right = *l2;
    if (!left) {
        *l1 = right;
        return;
    }
    if (!right) return;
    b_list_node *prev = NULL;
    while (left && right) {
        printf("1) %d 2) %d\n", *(int*)left->data, *(int*)right->data);
        int comp = compare(left->data, right->data);
        if (comp <= 0) {
            node->next = left;
            left = left->next;
        } else {
            node->next = right;
            right = right->next;
        }
        node = node->next;
        node->prev = prev;
        prev = node;
    }
    b_list_node *remainder = left ? left : right;
    while (remainder) {
        node->next = remainder;
        node = node->next;
        node->prev = prev;
        prev = node;
        remainder = remainder->next;
    }

    *tail_ref = node;
    *l1 = start.next;
}

static void merge_sort(b_list_node **head_ref, b_list_node **tail_ref, int (*compare)(void *a, void *b)) {
    b_list_node *head = *head_ref;
    if (!head || !head->next) return;
    b_list_node *l1 = head;
    b_list_node *l2 = head->next;
    while (l2 && l2->next) {
        l1 = l1->next;
        l2 = l2->next->next;
    }
    l2 = l1->next;
    l1->next = l2->prev = NULL;
    l1 = head;

    merge_sort(head_ref, tail_ref, compare);
    merge_sort(&l2, tail_ref, compare);

    merge(head_ref, &l2, tail_ref, compare);
}

void b_list_sort(b_list *list, int (*compare)(void *a, void *b)) {
    if (!list || list->size <= 1) return;
    
    merge_sort(&list->head, &list->tail, compare);
}
