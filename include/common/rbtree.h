#ifndef COMMON_RBTREE_H
#define COMMON_RBTREE_H

#include <kernel/types.h>
#include <stddef.h>

struct rb_node {
    unsigned long  __rb_parent_color;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
};

struct rb_root {
    struct rb_node *rb_node;
};

#define RB_ROOT (struct rb_root) { NULL, }

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

#ifndef rb_entry
#define rb_entry(ptr, type, member) container_of(ptr, type, member)
#endif

static inline void rb_link_node(struct rb_node *node, struct rb_node *parent,
                                struct rb_node **rb_link) {
    node->__rb_parent_color = (unsigned long)parent;
    node->rb_left = node->rb_right = NULL;
    *rb_link = node;
}

static inline void rb_insert_color(struct rb_node *node, struct rb_root *root) {
    (void)node;
    (void)root;
}

typedef enum {
    RB_RED = 0,
    RB_BLACK = 1
} rb_color_t;

typedef struct rb_node_s {
    u64 key;
    void *data;
    rb_color_t color;
    struct rb_node_s *left;
    struct rb_node_s *right;
    struct rb_node_s *parent;
} rb_node_t;

typedef struct {
    rb_node_t *root;
    u32 size;
} rbtree_t;

rbtree_t *rbtree_create(void);
void rbtree_destroy(rbtree_t *tree);
rb_node_t *rbtree_find(rbtree_t *tree, u64 key);
int rbtree_insert(rbtree_t *tree, u64 key, void *data);

#endif
