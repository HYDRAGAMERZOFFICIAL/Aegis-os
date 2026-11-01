#ifndef FILESYSTEM_SNAPSHOTS_H
#define FILESYSTEM_SNAPSHOTS_H

#include <stdint.h>

int fs_create_snapshot(const char* name);
int fs_restore_snapshot(const char* name);

#endif
