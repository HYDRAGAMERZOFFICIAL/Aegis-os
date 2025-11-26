#include <filesystem/file_indexing.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    indexed_file_t *indexed_files;
    u32 file_count;
    u32 max_files;
    u32 real_time_indexing_enabled;
} indexing_state_t;

static indexing_state_t indexing_state = {0};

int indexing_init(void)
{
    indexing_state.max_files = 16384;
    indexing_state.file_count = 0;
    indexing_state.real_time_indexing_enabled = 0;

    indexing_state.indexed_files = (indexed_file_t *)calloc(indexing_state.max_files, sizeof(indexed_file_t));
    if (!indexing_state.indexed_files) return -1;

    return 0;
}

int indexing_add_file(const char *file_path)
{
    if (!file_path) return -1;

    if (indexing_state.file_count >= indexing_state.max_files) return -1;

    indexed_file_t *file = &indexing_state.indexed_files[indexing_state.file_count];
    file->file_id = indexing_state.file_count;
    file->filename = file_path;
    file->file_path = file_path;
    file->size = 0;
    file->modified_time = 0;
    file->tags = (char **)calloc(32, sizeof(char *));
    if (!file->tags) return -1;
    file->tag_count = 0;

    indexing_state.file_count++;

    return 0;
}

int indexing_remove_file(const char *file_path)
{
    if (!file_path) return -1;

    for (u32 i = 0; i < indexing_state.file_count; i++) {
        if (strcmp(indexing_state.indexed_files[i].file_path, file_path) == 0) {
            if (indexing_state.indexed_files[i].tags) {
                free(indexing_state.indexed_files[i].tags);
            }

            indexing_state.indexed_files[i] = indexing_state.indexed_files[indexing_state.file_count - 1];
            indexing_state.file_count--;

            return 0;
        }
    }

    return -1;
}

int indexing_update_file(const char *file_path)
{
    if (!file_path) return -1;

    for (u32 i = 0; i < indexing_state.file_count; i++) {
        if (strcmp(indexing_state.indexed_files[i].file_path, file_path) == 0) {
            indexing_state.indexed_files[i].modified_time = 0;
            return 0;
        }
    }

    return -1;
}

int indexing_rebuild_index(void)
{
    for (u32 i = 0; i < indexing_state.file_count; i++) {
        indexing_state.indexed_files[i].modified_time = 0;
    }

    return 0;
}

indexed_file_t **indexing_search_by_name(const char *name_pattern, u32 *count)
{
    if (!name_pattern || !count) return NULL;

    indexed_file_t **result = (indexed_file_t **)calloc(indexing_state.file_count, sizeof(indexed_file_t *));
    if (!result) return NULL;

    u32 result_count = 0;
    for (u32 i = 0; i < indexing_state.file_count; i++) {
        if (strstr(indexing_state.indexed_files[i].filename, name_pattern) != NULL) {
            result[result_count++] = &indexing_state.indexed_files[i];
        }
    }

    *count = result_count;
    return result;
}

indexed_file_t **indexing_search_by_tags(const char **tags, u32 tag_count, u32 *result_count)
{
    if (!tags || tag_count == 0 || !result_count) return NULL;

    indexed_file_t **result = (indexed_file_t **)calloc(indexing_state.file_count, sizeof(indexed_file_t *));
    if (!result) return NULL;

    u32 count = 0;
    for (u32 i = 0; i < indexing_state.file_count; i++) {
        for (u32 j = 0; j < tag_count; j++) {
            for (u32 k = 0; k < indexing_state.indexed_files[i].tag_count; k++) {
                if (strcmp(indexing_state.indexed_files[i].tags[k], tags[j]) == 0) {
                    result[count++] = &indexing_state.indexed_files[i];
                    break;
                }
            }
        }
    }

    *result_count = count;
    return result;
}

indexed_file_t **indexing_search_advanced(search_filter_t *filter, u32 *count)
{
    if (!filter || !count) return NULL;

    indexed_file_t **result = (indexed_file_t **)calloc(indexing_state.file_count, sizeof(indexed_file_t *));
    if (!result) return NULL;

    u32 result_count = 0;
    for (u32 i = 0; i < indexing_state.file_count; i++) {
        indexed_file_t *file = &indexing_state.indexed_files[i];

        if (filter->max_results > 0 && result_count >= filter->max_results) break;

        if (filter->min_size > 0 && file->size < filter->min_size) continue;
        if (filter->max_size > 0 && file->size > filter->max_size) continue;
        if (filter->start_date > 0 && file->modified_time < filter->start_date) continue;
        if (filter->end_date > 0 && file->modified_time > filter->end_date) continue;

        result[result_count++] = file;
    }

    *count = result_count;
    return result;
}

indexed_file_t **indexing_search_by_content(const char *content_pattern, u32 *count)
{
    if (!content_pattern || !count) return NULL;

    indexed_file_t **result = (indexed_file_t **)calloc(indexing_state.file_count, sizeof(indexed_file_t *));
    if (!result) return NULL;

    u32 result_count = 0;
    for (u32 i = 0; i < indexing_state.file_count; i++) {
        if (strstr(indexing_state.indexed_files[i].filename, content_pattern) != NULL) {
            result[result_count++] = &indexing_state.indexed_files[i];
        }
    }

    *count = result_count;
    return result;
}

int indexing_add_tag(const char *file_path, const char *tag)
{
    if (!file_path || !tag) return -1;

    for (u32 i = 0; i < indexing_state.file_count; i++) {
        if (strcmp(indexing_state.indexed_files[i].file_path, file_path) == 0) {
            indexed_file_t *file = &indexing_state.indexed_files[i];
            if (file->tag_count >= 32) return -1;

            file->tags[file->tag_count++] = (char *)tag;
            return 0;
        }
    }

    return -1;
}

int indexing_remove_tag(const char *file_path, const char *tag)
{
    if (!file_path || !tag) return -1;

    for (u32 i = 0; i < indexing_state.file_count; i++) {
        if (strcmp(indexing_state.indexed_files[i].file_path, file_path) == 0) {
            indexed_file_t *file = &indexing_state.indexed_files[i];

            for (u32 j = 0; j < file->tag_count; j++) {
                if (strcmp(file->tags[j], tag) == 0) {
                    file->tags[j] = file->tags[file->tag_count - 1];
                    file->tag_count--;
                    return 0;
                }
            }

            return -1;
        }
    }

    return -1;
}

int indexing_get_file_tags(const char *file_path, char ***tags, u32 *count)
{
    if (!file_path || !tags || !count) return -1;

    for (u32 i = 0; i < indexing_state.file_count; i++) {
        if (strcmp(indexing_state.indexed_files[i].file_path, file_path) == 0) {
            indexed_file_t *file = &indexing_state.indexed_files[i];
            *tags = file->tags;
            *count = file->tag_count;
            return 0;
        }
    }

    return -1;
}

int indexing_enable_real_time_indexing(void)
{
    indexing_state.real_time_indexing_enabled = 1;
    return 0;
}

u64 indexing_get_index_size(void)
{
    return indexing_state.file_count * sizeof(indexed_file_t);
}
