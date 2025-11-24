set(COMMON_SOURCES
    string.c
    math.c
    bitmap.c
    rbtree.c
    list.c
    crypto.c
)

add_library(common_lib STATIC ${COMMON_SOURCES})
target_include_directories(common_lib PUBLIC ${INCLUDE_DIR})
