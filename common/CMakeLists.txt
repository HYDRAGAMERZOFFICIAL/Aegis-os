set(COMMON_SOURCES
    string.c
    math.c
    bitmap.c
    rbtree.c
    list.c
    crypto.c
)

add_library(aegis_common STATIC ${COMMON_SOURCES})
target_include_directories(aegis_common PUBLIC ${INCLUDE_DIR})
