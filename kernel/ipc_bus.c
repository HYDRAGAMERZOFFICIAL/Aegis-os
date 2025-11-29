#include <kernel/ipc_bus.h>
#include <common/list.h>
#include <common/rbtree.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct ipc_message_node {
    struct list_head list;
    ipc_message_t message;
} ipc_message_node_t;

typedef struct ipc_route {
    struct rb_node rb_node;
    int source_id;
    int dest_id;
    int priority;
    uint32_t message_count;
    uint32_t error_count;
} ipc_route_t;

typedef struct {
    struct list_head queue;
    int queue_size;
    int max_queue_size;
    uint32_t dropped_messages;
} ipc_message_queue_t;

static ipc_message_queue_t message_queues[MAX_IPC_ROUTES];
static struct rb_root route_tree;
static int initialized = 0;

void ipc_bus_init(void)
{
    if (initialized) return;
    
    for (int i = 0; i < MAX_IPC_ROUTES; i++) {
        INIT_LIST_HEAD(&message_queues[i].queue);
        message_queues[i].queue_size = 0;
        message_queues[i].max_queue_size = IPC_MAX_QUEUE_SIZE;
        message_queues[i].dropped_messages = 0;
    }
    
    route_tree = RB_ROOT;
    initialized = 1;
}

static int compare_routes(int a, int b)
{
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

int ipc_bus_register_route(int source_id, int dest_id, int priority)
{
    if (source_id < 0 || dest_id < 0 || source_id >= MAX_IPC_ROUTES || dest_id >= MAX_IPC_ROUTES) {
        return -1;
    }
    
    if (priority < 0 || priority > IPC_MAX_PRIORITY) {
        return -1;
    }
    
    ipc_route_t *new_route = (ipc_route_t *)malloc(sizeof(ipc_route_t));
    if (!new_route) return -1;
    
    new_route->source_id = source_id;
    new_route->dest_id = dest_id;
    new_route->priority = priority;
    new_route->message_count = 0;
    new_route->error_count = 0;
    
    struct rb_node **new_node = &(route_tree.rb_node);
    struct rb_node *parent = NULL;
    
    while (*new_node) {
        ipc_route_t *entry = container_of(*new_node, ipc_route_t, rb_node);
        parent = *new_node;
        
        int cmp = compare_routes((source_id << 16) | dest_id, 
                                (entry->source_id << 16) | entry->dest_id);
        if (cmp < 0) {
            new_node = &((*new_node)->rb_left);
        } else if (cmp > 0) {
            new_node = &((*new_node)->rb_right);
        } else {
            free(new_route);
            return -1;
        }
    }
    
    rb_link_node(&new_route->rb_node, parent, new_node);
    rb_insert_color(&new_route->rb_node, &route_tree);
    
    return 0;
}

int ipc_bus_send_message(const ipc_message_t *msg)
{
    if (!msg || msg->dest_id < 0 || msg->dest_id >= MAX_IPC_ROUTES) {
        return -1;
    }
    
    if (!initialized) {
        return -1;
    }
    
    ipc_message_queue_t *queue = &message_queues[msg->dest_id];
    
    if (queue->queue_size >= queue->max_queue_size) {
        queue->dropped_messages++;
        return -1;
    }
    
    ipc_message_node_t *node = (ipc_message_node_t *)malloc(sizeof(ipc_message_node_t));
    if (!node) {
        queue->dropped_messages++;
        return -1;
    }
    
    memcpy(&node->message, msg, sizeof(ipc_message_t));
    list_add_tail(&node->list, &queue->queue);
    queue->queue_size++;
    
    return 0;
}

int ipc_bus_receive_message(int dest_id, ipc_message_t *msg)
{
    if (dest_id < 0 || dest_id >= MAX_IPC_ROUTES || !msg) {
        return -1;
    }
    
    if (!initialized) {
        return -1;
    }
    
    ipc_message_queue_t *queue = &message_queues[dest_id];
    
    if (list_empty(&queue->queue)) {
        return 0;
    }
    
    ipc_message_node_t *node = list_first_entry(&queue->queue, ipc_message_node_t, list);
    memcpy(msg, &node->message, sizeof(ipc_message_t));
    
    list_del(&node->list);
    queue->queue_size--;
    free(node);
    
    return 1;
}

int ipc_bus_get_queue_size(int dest_id)
{
    if (dest_id < 0 || dest_id >= MAX_IPC_ROUTES) {
        return -1;
    }
    
    return message_queues[dest_id].queue_size;
}

uint32_t ipc_bus_get_dropped_messages(int dest_id)
{
    if (dest_id < 0 || dest_id >= MAX_IPC_ROUTES) {
        return 0;
    }
    
    return message_queues[dest_id].dropped_messages;
}

void ipc_bus_clear_queue(int dest_id)
{
    if (dest_id < 0 || dest_id >= MAX_IPC_ROUTES) {
        return;
    }
    
    ipc_message_queue_t *queue = &message_queues[dest_id];
    struct list_head *pos, *tmp;
    
    list_for_each_safe(pos, tmp, &queue->queue) {
        ipc_message_node_t *node = list_entry(pos, ipc_message_node_t, list);
        list_del(&node->list);
        free(node);
    }
    
    queue->queue_size = 0;
}

void ipc_bus_print_stats(void)
{
    printf("\n=== IPC Bus Statistics ===\n");
    printf("Route-ID | Queue-Size | Max-Size | Dropped\n");
    printf("---------|------------|----------|----------\n");
    
    for (int i = 0; i < MAX_IPC_ROUTES; i++) {
        ipc_message_queue_t *queue = &message_queues[i];
        if (queue->queue_size > 0 || queue->dropped_messages > 0) {
            printf("%-8d | %-10d | %-8d | %-8u\n",
                   i,
                   queue->queue_size,
                   queue->max_queue_size,
                   queue->dropped_messages);
        }
    }
}

int ipc_bus_is_route_available(int source_id, int dest_id)
{
    if (source_id < 0 || dest_id < 0) {
        return 0;
    }
    
    struct rb_node *node = route_tree.rb_node;
    
    while (node) {
        ipc_route_t *entry = container_of(node, ipc_route_t, rb_node);
        
        if (entry->source_id == source_id && entry->dest_id == dest_id) {
            return 1;
        }
        
        int cmp = compare_routes((source_id << 16) | dest_id,
                                (entry->source_id << 16) | entry->dest_id);
        if (cmp < 0) {
            node = node->rb_left;
        } else {
            node = node->rb_right;
        }
    }
    
    return 0;
}
