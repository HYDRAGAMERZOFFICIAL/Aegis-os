#ifndef AEGIS_APP_STORE_H
#define AEGIS_APP_STORE_H

#include <kernel/types.h>

typedef enum {
    APP_RATING_VERIFIED,
    APP_RATING_UNVERIFIED,
    APP_RATING_SUSPICIOUS,
    APP_RATING_MALWARE
} app_rating_t;

typedef struct {
    u64 app_id;
    const char *app_name;
    const char *developer;
    const char *version;
    u64 size;
    app_rating_t rating;
    u32 download_count;
    const char *category;
    const char *description;
    u32 star_rating;
} app_listing_t;

typedef struct {
    const char *component_name;
    const char *status;
    bool passed;
} security_scan_result_t;

int app_store_init(void);
app_listing_t **app_store_search(const char *query, u32 *count);
app_listing_t **app_store_get_category(const char *category, u32 *count);
app_listing_t **app_store_get_featured_apps(u32 *count);
int app_store_download_app(u64 app_id);
int app_store_install_app(u64 app_id);
int app_store_uninstall_app(u64 app_id);
int app_store_rate_app(u64 app_id, u32 star_rating, const char *review);
security_scan_result_t **app_store_scan_app(u64 app_id, u32 *scan_count);
int app_store_verify_app_signature(u64 app_id);
app_listing_t *app_store_get_app_info(u64 app_id);
int app_store_check_permissions(u64 app_id);
int app_store_request_app_review(u64 app_id);
app_listing_t **app_store_get_updates(u32 *count);

#endif
