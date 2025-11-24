#ifndef AEGIS_KERNEL_FILESYSTEM_H
#define AEGIS_KERNEL_FILESYSTEM_H

#include <kernel/types.h>

typedef enum {
    INODE_TYPE_FILE,
    INODE_TYPE_DIR,
    INODE_TYPE_SYMLINK,
    INODE_TYPE_BLKDEV,
    INODE_TYPE_CHRDEV
} inode_type_t;

typedef struct {
    u64 ino;
    u64 size;
    u64 blocks;
    inode_type_t type;
    u32 mode;
    u32 uid, gid;
    u64 atime, mtime, ctime;
    u32 link_count;
    u64 *block_ptrs;
    bool encrypted;
    u8 hash[32];
} inode_t;

typedef struct {
    char name[256];
    u64 ino;
    inode_type_t type;
} dirent_t;

typedef struct {
    u64 txn_id;
    u64 timestamp;
    enum {TXN_BEGIN, TXN_COMMIT, TXN_ROLLBACK} state;
    u64 *blocks;
    u32 block_count;
} transaction_t;

typedef struct {
    u32 magic;
    u64 total_blocks;
    u64 free_blocks;
    u64 block_size;
    u64 journal_offset;
    u64 journal_size;
    u64 features;
} fs_superblock_t;

int aegisfs_init(void);
inode_t *aegisfs_lookup(const char *path);
inode_t *aegisfs_create_file(const char *path, u32 mode);
int aegisfs_delete_file(const char *path);
int aegisfs_write(inode_t *inode, u64 offset, const void *data, u64 size);
int aegisfs_read(inode_t *inode, u64 offset, void *data, u64 size);
transaction_t *aegisfs_begin_transaction(void);
int aegisfs_commit_transaction(transaction_t *txn);
int aegisfs_rollback_transaction(transaction_t *txn);
int aegisfs_enable_encryption(inode_t *inode);
int aegisfs_verify_integrity(inode_t *inode);
int aegisfs_get_tamper_proof_log(u64 offset, void *buffer, u64 size);

#endif
