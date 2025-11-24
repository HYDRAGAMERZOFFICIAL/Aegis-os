set(FILESYSTEM_SOURCES
    metadata_engine.c
    journaling.c
    file_encryption.c
    access_control.c
    file_indexing.c
    snapshot_versioning.c
    self_healing.c
)

add_library(aegis_filesystem STATIC ${FILESYSTEM_SOURCES})
target_include_directories(aegis_filesystem PUBLIC ${INCLUDE_DIR})
target_link_libraries(aegis_filesystem PUBLIC aegis_kernel aegis_common)
