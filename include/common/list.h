#ifndef COMMON_LIST_H
#define COMMON_LIST_H

#include <kernel/types.h>
#include <stddef.h>

/* Intrusive linked list (Linux kernel style) */
struct list_head {
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list) {
    list->next = list;
    list->prev = list;
}

static inline void __list_add(struct list_head *new_node,
                              struct list_head *prev,
                              struct list_head *next) {
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

static inline void list_add(struct list_head *new_node, struct list_head *head) {
    __list_add(new_node, head, head->next);
}

static inline void list_add_tail(struct list_head *new_node, struct list_head *head) {
    __list_add(new_node, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next) {
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

static inline int list_empty(const struct list_head *head) {
    return head->next == head;
}

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)

/* Generic node-based list */
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

list_t *list_create(void);
void list_destroy(list_t *list);
int list_append(list_t *list, void *data);
int list_prepend(list_t *list, void *data);
int list_remove(list_t *list, void *data);
void *list_get(list_t *list, u32 index);
u32 list_size(const list_t *list);

#endif
