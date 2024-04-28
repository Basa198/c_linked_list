#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "b_list.h"

#define test(fn) \
    puts("... \x1b[33m" # fn "\x1b[0m"); \
    test_##fn();

typedef struct {
    char* name;
    int age;
} User;

static int free_calls = 0;

void dummy_free(void *val) {
    ++free_calls;
    free(val);
}

static int User_match(void *u1, void *u2) {
    User *a = u1;
    User *b = u2;
    return strcmp(a->name, b->name) == 0 && a->age == b->age;
}

static void test_b_new_list_node() {
    User *user = malloc(sizeof(User));
    user->age = 20;
    user->name = "Name";
    b_list_node *node = b_new_list_node(user);
    assert(User_match(node->data, user)); 
    assert(node->prev == NULL);
    assert(node->next == NULL);

    free(user);
    free(node); 
} 

static void test_b_new_list() {
    b_list *list = b_new_list();
    assert(list->size == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->free == NULL);

    b_free_list(list);
}

static void test_b_new_list_with_func() {
    b_list *list = b_new_list_with_func(dummy_free);
    assert(list->size == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->free == dummy_free);

    b_free_list(list);
}

static void test_b_list_rpush() {
    char *n = "Name";
    b_list_node *node = b_new_list_node(n);
    b_list *list = b_new_list();
    b_list_rpush(list, node);
    assert(list->head == node);
    assert(list->tail == node);
    assert(list->size == 1);
    char *n2 = "Name2";
    b_list_node *node2 = b_new_list_node(n2);
    b_list_rpush(list, node2);
    assert(list->tail == node2);
    assert(list->head != node2);
    assert(list->head->next == list->tail);
    assert(list->tail->prev == list->head);

    b_free_list(list);
}

static void test_b_list_lpush() {
    char *n = "Name";
    b_list_node *node = b_new_list_node(n);
    b_list *list = b_new_list();
    b_list_lpush(list, node);
    assert(list->head == node);
    assert(list->tail == node);
    assert(list->size == 1);
    char *n2 = "Name2";
    b_list_node *node2 = b_new_list_node(n2);
    b_list_lpush(list, node2);
    assert(list->head == node2);
    assert(list->tail != node2);
    assert(list->head->next == list->tail);
    assert(list->tail->prev == list->head);

    b_free_list(list);
}

static void test_b_list_rpop() {
    b_list *list = b_new_list();
    b_list_rpop(list); // Must not crash
    char *n = "Name";
    b_list_node *node = b_new_list_node(n);
    b_list_rpush(list, node);
    b_list_rpop(list);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    b_list_node *node1 = b_new_list_node(n);
    b_list_node *node2 = b_new_list_node(n);
    b_list_rpush(list, node1);
    b_list_rpush(list, node2);
    b_list_rpop(list);
    assert(list->head == node1);
    assert(list->tail == node1);
    assert(list->size == 1);
    b_list_rpop(list);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);

    free(node);
    free(node1);
    free(node2);
    b_free_list(list);
}

static void test_b_list_lpop() {
    b_list *list = b_new_list();
    b_list_lpop(list); // Must not crash
    char *n = "Name";
    b_list_node *node = b_new_list_node(n);
    b_list_lpush(list, node);
    b_list_lpop(list);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);
    b_list_node *node1 = b_new_list_node(n);
    b_list_node *node2 = b_new_list_node(n);
    b_list_lpush(list, node1);
    b_list_lpush(list, node2);
    b_list_lpop(list);
    assert(list->head == node1);
    assert(list->tail == node1);
    assert(list->size == 1);
    b_list_lpop(list);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    assert(list->size == 0);

    free(node);
    free(node1);
    free(node2);
    b_free_list(list); 
}

static void test_b_list_fetch() {
    b_list *list = b_new_list();
    b_list_fetch(list, 2); // must not crash
    char *n = "Name";
    b_list_node *node = b_new_list_node(n);
    b_list_lpush(list, node);
    assert(b_list_fetch(list, 0) == node);
    b_list_node *node1 = b_new_list_node(n);
    b_list_lpush(list, node1);
    assert(b_list_fetch(list, 0) == node1);
    assert(b_list_fetch(list, 1) == node);
    b_list_node *node2 = b_new_list_node(n);
    b_list_rpush(list, node2);
    assert(b_list_fetch(list, 0) == node1);
    assert(b_list_fetch(list, 1) == node);
    assert(b_list_fetch(list, 2) == node2);

    b_free_list(list);
}

static void test_b_list_find() {
    b_list *list = b_new_list_with_func(dummy_free);
    User *us1 = malloc(sizeof(User));
    us1->name = "Name";
    us1->age = 20;
    User *us2 = malloc(sizeof(User));
    us2->name = "Name1";
    us2->age = 21;
    User *us3 = malloc(sizeof(User));
    us3->name = "Name";
    us3->age = 23;
    b_list_node *node = b_new_list_node(us1);
    b_list_node *node1 = b_new_list_node(us2);
    b_list_lpush(list, node);
    b_list_lpush(list, node1);
    assert(b_list_find(list, User_match, us1) == node);
    assert(b_list_find(list, User_match, us2) == node1);
    assert(b_list_find(list, User_match, us3) == NULL);

    b_free_list(list);
    free(us3);
}

static void test_b_list_insert() {
    b_list *list = b_new_list();
    char *n = "Name";
    b_list_node *node = b_new_list_node(n);
    b_list_node *node1 = b_new_list_node(n);
    b_list_node *node2 = b_new_list_node(n);
    b_list_node *node3 = b_new_list_node(n);
    b_list_insert(list, node, 10); // must not crash
    b_list_insert(list, node, 0);
    assert(list->head == node);
    assert(list->tail == node);
    assert(list->size == 1);
    b_list_insert(list, node1, 0);
    assert(list->head == node1);
    assert(list->tail == node);
    assert(list->head->next == node);
    assert(list->tail->prev == node1);
    assert(list->size == 2);
    b_list_insert(list, node2, 2);
    assert(list->tail == node2);
    assert(list->tail->prev == node);
    assert(list->size == 3);
    b_list_insert(list, node3, 1);
    assert(node3->prev == node1);
    assert(node3->next == node);
    assert(node1->next == node3);
    assert(node->prev == node3);

    b_free_list(list);
}

static void test_b_list_delete() {
    b_list *list = b_new_list();
    char *n = "Name";
    b_list_node *node = b_new_list_node(n);
    b_list_node *node1 = b_new_list_node(n);
    b_list_node *node2 = b_new_list_node(n);
    b_list_node *node3 = b_new_list_node(n);
    b_list_rpush(list, node);
    b_list_rpush(list, node1);
    b_list_rpush(list, node2);
    b_list_rpush(list, node3);
    b_list_delete(list, node); 
    assert(list->head == node1);
    assert(list->size == 3);
    assert(list->head->prev == NULL);
    b_list_delete(list, node2);
    assert(list->head == node1);
    assert(list->head->next == node3);
    assert(list->tail == node3);
    assert(node3->prev == node1);
    assert(list->size == 2);
    b_list_delete(list, node3);
    assert(list->tail == list->head);
    assert(list->head == node1);

    b_free_list(list);
}

static void test_b_list_delete_at() {
    b_list *list = b_new_list();
    b_list_node *node = b_new_list_node("a");
    b_list_node *node1 = b_new_list_node("b");
    b_list_node *node2 = b_new_list_node("c");
    b_list_node *node3 = b_new_list_node("d");
    b_list_rpush(list, node);
    b_list_rpush(list, node1);
    b_list_rpush(list, node2);
    b_list_rpush(list, node3);
    b_list_delete_at(list, 0);
    assert(list->head == node1);
    assert(list->head->prev == NULL);
    assert(list->size == 3);
    b_list_delete_at(list, 1);
    assert(list->head == node1);
    assert(list->head->next == list->tail);
    assert(list->tail == node3);
    assert(list->tail->prev == node1);
    assert(list->size == 2);
    b_list_delete_at(list, 1);
    assert(list->head == node1);
    assert(list->tail == node1);

    b_free_list(list);
}

static void test_b_new_list_iterator() {
    b_list *list = b_new_list();
    b_list_node *node = b_new_list_node("a");
    b_list_node *node1 = b_new_list_node("b");
    b_list_rpush(list, node);
    b_list_rpush(list, node1);
    b_list_iterator *it = b_new_list_iterator(list, LIST_BEGIN); 
    assert(it->next == node);
    assert(it->direction == LIST_BEGIN);
    b_list_iterator *it1 = b_new_list_iterator(list, LIST_END);
    assert(it1->next == node1);
    assert(it1->direction == LIST_END);

    b_free_list_iterator(it);
    b_free_list_iterator(it1);
    b_free_list(list);
}

static void test_b_new_list_iterator_from_node() {
    b_list *list = b_new_list();
    b_list_node *node = b_new_list_node("a");
    b_list_node *node1 = b_new_list_node("b");
    b_list_node *node2 = b_new_list_node("c");
    b_list_rpush(list, node);
    b_list_rpush(list, node1);
    b_list_rpush(list, node2);
    b_list_iterator *it = b_new_list_iterator_from_node(node1, LIST_BEGIN);
    assert(it->next == node1);
    assert(it->direction == LIST_BEGIN);
    b_list_iterator *it1 = b_new_list_iterator_from_node(node, LIST_BEGIN);
    assert(it1->next == node);
    b_list_iterator *it2 = b_new_list_iterator_from_node(node2, LIST_END);
    assert(it2->next == node2);
    assert(it2->direction == LIST_END); 

    b_free_list_iterator(it);
    b_free_list_iterator(it1);
    b_free_list_iterator(it2);
    b_free_list(list);
}

static void test_b_new_list_iterator_from_index() {
    b_list *list = b_new_list();
    b_list_node *node = b_new_list_node("a");
    b_list_node *node1 = b_new_list_node("b");
    b_list_node *node2 = b_new_list_node("c");
    b_list_rpush(list, node);
    b_list_rpush(list, node1);
    b_list_rpush(list, node2);
    b_list_iterator *it = b_new_list_iterator_from_index(list, 0, LIST_BEGIN);
    assert(it->next == node);
    assert(it->direction == LIST_BEGIN);
    b_list_iterator *it1 = b_new_list_iterator_from_index(list, 1, LIST_BEGIN);
    assert(it1->next == node1);
    b_list_iterator *it2 = b_new_list_iterator_from_index(list, 2, LIST_END);
    assert(it2->next == node2);
    assert(it2->direction == LIST_END);

    b_free_list_iterator(it);
    b_free_list_iterator(it1);
    b_free_list_iterator(it2);
    b_free_list(list);
}

static void test_b_list_iterator_next() {
    b_list *list = b_new_list();
    b_list_node *node = b_new_list_node("a");
    b_list_node *node1 = b_new_list_node("b");
    b_list_node *node2 = b_new_list_node("c");
    b_list_rpush(list, node);
    b_list_rpush(list, node1);
    b_list_rpush(list, node2);
    b_list_iterator *it = b_new_list_iterator(list, LIST_BEGIN);
    assert(b_list_iterator_next(it) == node); 
    assert(b_list_iterator_next(it) == node1); 
    assert(b_list_iterator_next(it) == node2); 
    b_list_iterator *it1 = b_new_list_iterator(list, LIST_END);
    assert(b_list_iterator_next(it1) == node2); 
    assert(b_list_iterator_next(it1) == node1); 
    assert(b_list_iterator_next(it1) == node); 

    b_free_list_iterator(it);
    b_free_list_iterator(it1);
    b_free_list(list);
}

int compare(void *a, void *b) {
    return *(int*)a - *(int*)b;
}

static void test_b_list_sort() {
    b_list *list = b_new_list();
    int arr[100] = {69, 55, 42, 72, 5, 41, 14, 37, 58, 46, 7, 84, 82, 34, 12, 94, 79, 27, 52, 76, 19, 71, 96, 69, 51, 35, 12, 45, 98, 16, 48, 50, 49, 22, 80, 48, 67, 73, 10, 79, 50, 69, 46, 98, 1, 34, 92, 0, 50, 94, 97, 58, 100, 36, 25, 43, 12, 8, 64, 9, 16, 39, 55, 84, 5, 13, 75, 85, 68, 33, 18, 97, 39, 42, 33, 45, 39, 79, 98, 39, 8, 21, 42, 36, 51, 99, 79, 0, 20, 70, 62, 50, 42, 22, 49, 98, 14, 49, 78, 20};
    for (int i = 0; i < 100; i++) { 
        b_list_rpush(list, b_new_list_node(&(arr[i])));
    }
    int sorted_arr[100] = {0, 0, 1, 5, 5, 7, 8, 8, 9, 10, 12, 12, 12, 13, 14, 14, 16, 16, 18, 19, 20, 20, 21, 22, 22, 25, 27, 33, 33, 34, 34, 35, 36, 36, 37, 39, 39, 39, 39, 41, 42, 42, 42, 42, 43, 45, 45, 46, 46, 48, 48, 49, 49, 49, 50, 50, 50, 50, 51, 51, 52, 55, 55, 58, 58, 62, 64, 67, 68, 69, 69, 69, 70, 71, 72, 73, 75, 76, 78, 79, 79, 79, 79, 80, 82, 84, 84, 85, 92, 94, 94, 96, 97, 97, 98, 98, 98, 98, 99, 100};    
    
    b_list_sort(list, compare);
    assert(*(int*)(list->head->data) == 0);
    assert(*(int*)(list->tail->data) == 100);
    assert(list->head->prev == NULL);
    assert(list->tail->next == NULL);
    b_list_iterator *it = b_new_list_iterator(list, LIST_BEGIN);
    b_list_iterator *it1 = b_new_list_iterator(list, LIST_END);
    int i = 0;
    while (i < 100) {
        assert(*(int*)(b_list_iterator_next(it)->data) == sorted_arr[i]);
        assert(*(int*)(b_list_iterator_next(it1)->data) == sorted_arr[99 - i]);
        i++;
    } 

    b_free_list_iterator(it);
    b_free_list_iterator(it1);
    b_free_list(list);
}

int main() {
    test(b_new_list_node);
    test(b_new_list);
    test(b_new_list_with_func);
    test(b_list_lpush);
    test(b_list_rpush);
    test(b_list_rpop);
    test(b_list_lpop);
    test(b_list_fetch);
    test(b_list_find);
    test(b_list_insert);
    test(b_list_delete);
    test(b_list_delete_at);
    test(b_new_list_iterator);
    test(b_new_list_iterator_from_node);
    test(b_new_list_iterator_from_index);
    test(b_list_iterator_next);
    test(b_list_sort);
}