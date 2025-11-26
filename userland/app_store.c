#include <userland/app_store.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    app_listing_t *apps;
    u32 app_count;
    u32 max_apps;
} app_store_state_t;

static app_store_state_t app_store_state = {0};

int app_store_init(void)
{
    app_store_state.max_apps = 8192;
    app_store_state.app_count = 0;

    app_store_state.apps = (app_listing_t *)calloc(app_store_state.max_apps, sizeof(app_listing_t));
    if (!app_store_state.apps) return -1;

    return 0;
}

app_listing_t **app_store_search(const char *query, u32 *count)
{
    if (!query || !count) return NULL;

    app_listing_t **result = (app_listing_t **)calloc(app_store_state.app_count, sizeof(app_listing_t *));
    if (!result) return NULL;

    u32 result_count = 0;
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (strstr(app_store_state.apps[i].app_name, query) != NULL) {
            result[result_count++] = &app_store_state.apps[i];
        }
    }

    *count = result_count;
    return result;
}

app_listing_t **app_store_get_category(const char *category, u32 *count)
{
    if (!category || !count) return NULL;

    app_listing_t **result = (app_listing_t **)calloc(app_store_state.app_count, sizeof(app_listing_t *));
    if (!result) return NULL;

    u32 result_count = 0;
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (strcmp(app_store_state.apps[i].category, category) == 0) {
            result[result_count++] = &app_store_state.apps[i];
        }
    }

    *count = result_count;
    return result;
}

app_listing_t **app_store_get_featured_apps(u32 *count)
{
    if (!count) return NULL;

    app_listing_t **result = (app_listing_t **)calloc(32, sizeof(app_listing_t *));
    if (!result) return NULL;

    u32 result_count = 0;
    for (u32 i = 0; i < app_store_state.app_count && result_count < 32; i++) {
        if (app_store_state.apps[i].rating == APP_RATING_VERIFIED) {
            result[result_count++] = &app_store_state.apps[i];
        }
    }

    *count = result_count;
    return result;
}

int app_store_download_app(u64 app_id)
{
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (app_store_state.apps[i].app_id == app_id) {
            return 0;
        }
    }

    return -1;
}

int app_store_install_app(u64 app_id)
{
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (app_store_state.apps[i].app_id == app_id) {
            return 0;
        }
    }

    return -1;
}

int app_store_uninstall_app(u64 app_id)
{
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (app_store_state.apps[i].app_id == app_id) {
            return 0;
        }
    }

    return -1;
}

int app_store_rate_app(u64 app_id, u32 star_rating, const char *review)
{
    if (star_rating > 5) return -1;

    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (app_store_state.apps[i].app_id == app_id) {
            app_store_state.apps[i].star_rating = star_rating;
            return 0;
        }
    }

    return -1;
}

security_scan_result_t **app_store_scan_app(u64 app_id, u32 *scan_count)
{
    if (!scan_count) return NULL;

    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (app_store_state.apps[i].app_id == app_id) {
            security_scan_result_t **results = (security_scan_result_t **)calloc(8, sizeof(security_scan_result_t *));
            if (!results) return NULL;

            *scan_count = 0;
            return results;
        }
    }

    return NULL;
}

int app_store_verify_app_signature(u64 app_id)
{
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (app_store_state.apps[i].app_id == app_id) {
            return 0;
        }
    }

    return -1;
}

app_listing_t *app_store_get_app_info(u64 app_id)
{
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (app_store_state.apps[i].app_id == app_id) {
            return &app_store_state.apps[i];
        }
    }

    return NULL;
}

int app_store_check_permissions(u64 app_id)
{
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (app_store_state.apps[i].app_id == app_id) {
            return 0;
        }
    }

    return -1;
}

int app_store_request_app_review(u64 app_id)
{
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        if (app_store_state.apps[i].app_id == app_id) {
            return 0;
        }
    }

    return -1;
}

app_listing_t **app_store_get_updates(u32 *count)
{
    if (!count) return NULL;

    app_listing_t **result = (app_listing_t **)calloc(app_store_state.app_count, sizeof(app_listing_t *));
    if (!result) return NULL;

    u32 result_count = 0;
    for (u32 i = 0; i < app_store_state.app_count; i++) {
        result[result_count++] = &app_store_state.apps[i];
    }

    *count = result_count;
    return result;
}
