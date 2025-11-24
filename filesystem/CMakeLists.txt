set(FILESYSTEM_SOURCES
    metadata_engine.c
    journaling.c
    file_encryption.c
    access_control.c
    file_indexing.c
    snapshot_versioning.c
    self_healing.c
)

add_library(filesystem_lib STATIC ${FILESYSTEM_SOURCES})
target_include_directories(filesystem_lib PUBLIC ${INCLUDE_DIR})
target_link_libraries(filesystem_lib PUBLIC kernel_lib common_lib)
