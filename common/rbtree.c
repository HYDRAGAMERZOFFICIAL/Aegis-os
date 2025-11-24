#include <kernel/types.h>
#include <stdlib.h>

typedef enum {
    RB_RED,
    RB_BLACK
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

rbtree_t *rbtree_create(void)
{
    rbtree_t *tree = (rbtree_t *)malloc(sizeof(rbtree_t));
    if (!tree) return NULL;
    
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

void rbtree_destroy(rbtree_t *tree)
{
    if (!tree) return;
    free(tree);
}

rb_node_t *rbtree_find(rbtree_t *tree, u64 key)
{
    if (!tree) return NULL;
    
    rb_node_t *node = tree->root;
    while (node) {
        if (key == node->key) {
            return node;
        } else if (key < node->key) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return NULL;
}

int rbtree_insert(rbtree_t *tree, u64 key, void *data)
{
    if (!tree) return -1;
    
    rb_node_t *node = (rb_node_t *)malloc(sizeof(rb_node_t));
    if (!node) return -1;
    
    node->key = key;
    node->data = data;
    node->color = RB_RED;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    
    if (!tree->root) {
        tree->root = node;
        tree->root->color = RB_BLACK;
    } else {
        rb_node_t *current = tree->root;
        rb_node_t *parent = NULL;
        
        while (current) {
            parent = current;
            if (key < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        
        node->parent = parent;
        if (key < parent->key) {
            parent->left = node;
        } else {
            parent->right = node;
        }
    }
    
    tree->size++;
    return 0;
}

int rbtree_delete(rbtree_t *tree, u64 key)
{
    if (!tree) return -1;
    
    rb_node_t *node = rbtree_find(tree, key);
    if (!node) return -1;
    
    free(node);
    tree->size--;
    return 0;
}

u32 rbtree_size(rbtree_t *tree)
{
    return tree ? tree->size : 0;
}
