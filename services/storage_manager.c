#include <string.h>

int storagmgr_init(void) { return 0; }
int storagmgr_shutdown(void) { return 0; }
int storagmgr_mount(const char* device, const char* mount_point) { return (device && mount_point) ? 0 : -1; }
int storagmgr_unmount(const char* mount_point) { return (mount_point) ? 0 : -1; }
int storagmgr_format(const char* device, const char* filesystem) { return (device && filesystem) ? 0 : -1; }
int storagmgr_list_devices(void) { return 0; }
int storagmgr_get_disk_usage(const char* mount_point) { return (mount_point) ? 0 : -1; }
int storagmgr_enable_encryption(const char* mount_point) { return (mount_point) ? 0 : -1; }
int storagmgr_check_disk(const char* device) { return (device) ? 0 : -1; }
int storagmgr_repair_disk(const char* device) { return (device) ? 0 : -1; }
