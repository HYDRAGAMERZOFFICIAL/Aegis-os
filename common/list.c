#include <kernel/types.h>
#include <stdlib.h>

typedef struct list_node_s {
    void *data;
    struct list_node_s *next;
    struct list_node_s *prev;
} list_node_t;

typedef struct {
    list_node_t *head;
    list_node_t *tail;
    u32 size;
} list_t;

list_t *list_create(void)
{
    list_t *list = (list_t *)malloc(sizeof(list_t));
    if (!list) return NULL;
    
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void list_destroy(list_t *list)
{
    if (!list) return;
    
    list_node_t *node = list->head;
    while (node) {
        list_node_t *next = node->next;
        free(node);
        node = next;
    }
    
    free(list);
}

int list_append(list_t *list, void *data)
{
    if (!list) return -1;
    
    list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));
    if (!node) return -1;
    
    node->data = data;
    node->next = NULL;
    node->prev = list->tail;
    
    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    
    list->tail = node;
    list->size++;
    return 0;
}

int list_prepend(list_t *list, void *data)
{
    if (!list) return -1;
    
    list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));
    if (!node) return -1;
    
    node->data = data;
    node->next = list->head;
    node->prev = NULL;
    
    if (list->head) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }
    
    list->head = node;
    list->size++;
    return 0;
}

int list_remove(list_t *list, void *data)
{
    if (!list) return -1;
    
    list_node_t *node = list->head;
    while (node) {
        if (node->data == data) {
            if (node->prev) {
                node->prev->next = node->next;
            } else {
                list->head = node->next;
            }
            
            if (node->next) {
                node->next->prev = node->prev;
            } else {
                list->tail = node->prev;
            }
            
            free(node);
            list->size--;
            return 0;
        }
        node = node->next;
    }
    
    return -1;
}

void *list_get(list_t *list, u32 index)
{
    if (!list || index >= list->size) return NULL;
    
    list_node_t *node = list->head;
    for (u32 i = 0; i < index; i++) {
        node = node->next;
    }
    
    return node ? node->data : NULL;
}

u32 list_size(list_t *list)
{
    return list ? list->size : 0;
}
