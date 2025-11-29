#include <kernel/event_system.h>
#include <common/list.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct event_subscriber {
    struct list_head list;
    int subscriber_id;
    event_callback_t callback;
    void *context;
    uint32_t subscribed_events;
} event_subscriber_t;

typedef struct {
    struct list_head subscribers;
    uint32_t event_count;
    uint32_t last_timestamp;
} event_registry_entry_t;

static event_registry_entry_t event_registry[MAX_EVENT_TYPES];
static int initialized = 0;

void event_system_init(void)
{
    if (initialized) return;
    
    for (int i = 0; i < MAX_EVENT_TYPES; i++) {
        INIT_LIST_HEAD(&event_registry[i].subscribers);
        event_registry[i].event_count = 0;
        event_registry[i].last_timestamp = 0;
    }
    
    initialized = 1;
}

int event_subscribe(int subscriber_id, uint32_t event_type, event_callback_t callback, void *context)
{
    if (event_type >= MAX_EVENT_TYPES || !callback) {
        return -1;
    }
    
    if (!initialized) {
        return -1;
    }
    
    event_subscriber_t *subscriber = (event_subscriber_t *)malloc(sizeof(event_subscriber_t));
    if (!subscriber) {
        return -1;
    }
    
    subscriber->subscriber_id = subscriber_id;
    subscriber->callback = callback;
    subscriber->context = context;
    subscriber->subscribed_events = event_type;
    
    list_add_tail(&subscriber->list, &event_registry[event_type].subscribers);
    
    return 0;
}

int event_unsubscribe(int subscriber_id, uint32_t event_type)
{
    if (event_type >= MAX_EVENT_TYPES) {
        return -1;
    }
    
    if (!initialized) {
        return -1;
    }
    
    event_registry_entry_t *entry = &event_registry[event_type];
    struct list_head *pos, *tmp;
    
    list_for_each_safe(pos, tmp, &entry->subscribers) {
        event_subscriber_t *sub = list_entry(pos, event_subscriber_t, list);
        
        if (sub->subscriber_id == subscriber_id) {
            list_del(&sub->list);
            free(sub);
            return 0;
        }
    }
    
    return -1;
}

int event_publish(const kernel_event_t *event)
{
    if (!event || event->event_type >= MAX_EVENT_TYPES) {
        return -1;
    }
    
    if (!initialized) {
        return -1;
    }
    
    event_registry_entry_t *entry = &event_registry[event->event_type];
    entry->event_count++;
    entry->last_timestamp = event->timestamp;
    
    struct list_head *pos;
    int handled = 0;
    
    list_for_each(pos, &entry->subscribers) {
        event_subscriber_t *sub = list_entry(pos, event_subscriber_t, list);
        
        if (sub->callback) {
            int result = sub->callback(event, sub->context);
            if (result == 0) {
                handled++;
            }
        }
    }
    
    return handled;
}

uint32_t event_get_count(uint32_t event_type)
{
    if (event_type >= MAX_EVENT_TYPES) {
        return 0;
    }
    
    return event_registry[event_type].event_count;
}

uint32_t event_get_last_timestamp(uint32_t event_type)
{
    if (event_type >= MAX_EVENT_TYPES) {
        return 0;
    }
    
    return event_registry[event_type].last_timestamp;
}

int event_get_subscriber_count(uint32_t event_type)
{
    if (event_type >= MAX_EVENT_TYPES) {
        return 0;
    }
    
    int count = 0;
    struct list_head *pos;
    
    list_for_each(pos, &event_registry[event_type].subscribers) {
        count++;
    }
    
    return count;
}

void event_print_registry(void)
{
    printf("\n=== Event Registry ===\n");
    printf("Event-Type | Event-Count | Subscribers\n");
    printf("-----------|-------------|------------\n");
    
    for (int i = 0; i < MAX_EVENT_TYPES; i++) {
        event_registry_entry_t *entry = &event_registry[i];
        
        if (entry->event_count > 0 || event_get_subscriber_count(i) > 0) {
            printf("%-10d | %-11u | %-11d\n",
                   i,
                   entry->event_count,
                   event_get_subscriber_count(i));
        }
    }
}

int event_clear_subscribers(uint32_t event_type)
{
    if (event_type >= MAX_EVENT_TYPES) {
        return -1;
    }
    
    event_registry_entry_t *entry = &event_registry[event_type];
    struct list_head *pos, *tmp;
    int removed = 0;
    
    list_for_each_safe(pos, tmp, &entry->subscribers) {
        event_subscriber_t *sub = list_entry(pos, event_subscriber_t, list);
        list_del(&sub->list);
        free(sub);
        removed++;
    }
    
    return removed;
}

int event_subscribe_all(int subscriber_id, event_callback_t callback, void *context)
{
    if (!callback) {
        return -1;
    }
    
    int subscribed = 0;
    
    for (int i = 0; i < MAX_EVENT_TYPES; i++) {
        if (event_subscribe(subscriber_id, i, callback, context) == 0) {
            subscribed++;
        }
    }
    
    return subscribed;
}

int event_unsubscribe_all(int subscriber_id)
{
    int unsubscribed = 0;
    
    for (int i = 0; i < MAX_EVENT_TYPES; i++) {
        if (event_unsubscribe(subscriber_id, i) == 0) {
            unsubscribed++;
        }
    }
    
    return unsubscribed;
}
