#ifndef AEGIS_FILE_INDEXING_H
#define AEGIS_FILE_INDEXING_H

#include <kernel/types.h>

typedef struct {
    u64 file_id;
    const char *filename;
    const char *file_path;
    u64 size;
    u64 modified_time;
    char **tags;
    u32 tag_count;
} indexed_file_t;

typedef struct {
    const char *query;
    u32 max_results;
    u64 min_size, max_size;
    u64 start_date, end_date;
} search_filter_t;

int indexing_init(void);
int indexing_add_file(const char *file_path);
int indexing_remove_file(const char *file_path);
int indexing_update_file(const char *file_path);
int indexing_rebuild_index(void);
indexed_file_t **indexing_search_by_name(const char *name_pattern, u32 *count);
indexed_file_t **indexing_search_by_tags(const char **tags, u32 tag_count, u32 *result_count);
indexed_file_t **indexing_search_advanced(search_filter_t *filter, u32 *count);
indexed_file_t **indexing_search_by_content(const char *content_pattern, u32 *count);
int indexing_add_tag(const char *file_path, const char *tag);
int indexing_remove_tag(const char *file_path, const char *tag);
int indexing_get_file_tags(const char *file_path, char ***tags, u32 *count);
int indexing_enable_real_time_indexing(void);
u64 indexing_get_index_size(void);

#endif
