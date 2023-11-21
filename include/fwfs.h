/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
/*
 * The little filesystem
 *
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
//FWFS:firmware filesystem
#ifndef FWFS_H
#define FWFS_H

#include <linux/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef bool
typedef unsigned char       bool;
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

// FIXME:
// Because flash driver does DMA-based read/write with fs buffer directly,
// and DMA needs the address and size are cache line size aligned, so the
// filesystem needs to make sure the address of buffer is aligned right.

#define CACHE_LINE_SIZE (64)

#define CACHE_LINE_ALIGNED(addr)        (((uintptr_t)(addr) & ((uintptr_t)(CACHE_LINE_SIZE - 1))) == 0)
/// Version info ///

// Software library version
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define FWFS_VERSION 0x00030002
#define FWFS_VERSION_MAJOR (0xffff & (FWFS_VERSION >> 16))
#define FWFS_VERSION_MINOR (0xffff & (FWFS_VERSION >>  0))

// Version of On-disk data structures
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define FWFS_DISK_VERSION 0x00020000
#define FWFS_DISK_VERSION_MAJOR (0xffff & (FWFS_DISK_VERSION >> 16))
#define FWFS_DISK_VERSION_MINOR (0xffff & (FWFS_DISK_VERSION >>  0))


/// Definitions ///

// Type definitions
typedef uint32_t fwfs_size_t;
typedef uint32_t fwfs_off_t;

typedef int32_t  fwfs_ssize_t;
typedef int32_t  fwfs_soff_t;

typedef uint32_t fwfs_block_t;

// Maximum name size in bytes, may be redefined to reduce the size of the
// info struct. Limited to <= 1022. Stored in superblock and must be
// respected by other firmwarefs drivers.
#ifndef FWFS_NAME_MAX
#define FWFS_NAME_MAX 255
#endif

// Maximum size of a file in bytes, may be redefined to limit to support other
// drivers. Limited on disk to <= 4294967296. However, above 2147483647 the
// functions fwfs_file_seek, fwfs_file_size, and fwfs_file_tell will return
// incorrect values due to using signed integers. Stored in superblock and
// must be respected by other firmwarefs drivers.
#ifndef FWFS_FILE_MAX
#define FWFS_FILE_MAX 2147483647
#endif

// Maximum size of custom attributes in bytes, may be redefined, but there is
// no real benefit to using a smaller FWFS_ATTR_MAX. Limited to <= 1022.
#ifndef FWFS_ATTR_MAX
#define FWFS_ATTR_MAX 1022
#endif

// 'C' << 24 | 'O' << 16 | 'M' << 8 | 'P'
#define FWFS_COMPACT_MAGIC 0x434f4d50

// 'C' << 24 | 'M' << 16 | 'N' << 8 | 'T'
#define FWFS_COMPACT_ON_MOUNT_MAGIC 0x434d4e54

// Possible error codes, these are negative to allow
// valid positive return values
enum fwfs_error {
    FWFS_ERR_OK          = 0,    // No error
    FWFS_ERR_IO          = -5,   // Error during device operation
    FWFS_ERR_CORRUPT     = -84,  // Corrupted
    FWFS_ERR_NOENT       = -2,   // No directory entry
    FWFS_ERR_EXIST       = -17,  // Entry already exists
    FWFS_ERR_NOTDIR      = -20,  // Entry is not a dir
    FWFS_ERR_ISDIR       = -21,  // Entry is a dir
    FWFS_ERR_NOTEMPTY    = -39,  // Dir is not empty
    FWFS_ERR_BADF        = -9,   // Bad file number
    FWFS_ERR_FBIG        = -27,  // File too large
    FWFS_ERR_INVAL       = -22,  // Invalid parameter
    FWFS_ERR_NOSPC       = -28,  // No space left on device
    FWFS_ERR_NOMEM       = -12,  // No more memory available
    FWFS_ERR_NOATTR      = -61,  // No data/attr available
    FWFS_ERR_NAMETOOLONG = -36,  // File name too long
};

// File types
enum fwfs_type {
    // file types
    FWFS_TYPE_REG            = 0x001,
    FWFS_TYPE_DIR            = 0x002,

    // internally used types
    FWFS_TYPE_SPLICE         = 0x400,
    FWFS_TYPE_NAME           = 0x000,
    FWFS_TYPE_STRUCT         = 0x200,
    FWFS_TYPE_USERATTR       = 0x300,
    FWFS_TYPE_FROM           = 0x100,
    FWFS_TYPE_TAIL           = 0x600,
    FWFS_TYPE_GLOBALS        = 0x700,
    FWFS_TYPE_CRC            = 0x500,

    // internally used type specializations
    FWFS_TYPE_CREATE         = 0x401,
    FWFS_TYPE_DELETE         = 0x4ff,
    FWFS_TYPE_SUPERBLOCK     = 0x0ff,
    FWFS_TYPE_DIRSTRUCT      = 0x200,
    FWFS_TYPE_CTZSTRUCT      = 0x202,
    FWFS_TYPE_INLINESTRUCT   = 0x201,
    FWFS_TYPE_SOFTTAIL       = 0x600,
    FWFS_TYPE_HARDTAIL       = 0x601,
    FWFS_TYPE_MOVESTATE      = 0x7ff,

    // internal chip sources
    FWFS_FROM_NOOP           = 0x000,
    FWFS_FROM_MOVE           = 0x101,
    FWFS_FROM_USERATTRS      = 0x102,
};

// File open flags
enum fwfs_open_flags {
    // open flags
    FWFS_O_RDONLY = 1,         // Open a file as read only
    FWFS_O_WRONLY = 2,         // Open a file as write only
    FWFS_O_RDWR   = 3,         // Open a file as read and write
    FWFS_O_CREAT  = 0x0100,    // Create a file if it does not exist
    FWFS_O_EXCL   = 0x0200,    // Fail if a file already exists
    FWFS_O_TRUNC  = 0x0400,    // Truncate the existing file to zero size
    FWFS_O_APPEND = 0x0800,    // Move to end of file on every write

    // internally used flags
    FWFS_F_DIRTY   = 0x010000, // File does not match storage
    FWFS_F_WRITING = 0x020000, // File has been written since last flush
    FWFS_F_READING = 0x040000, // File has been read since last flush
    FWFS_F_ERRED   = 0x080000, // An error occured during write
    FWFS_F_INLINE  = 0x100000, // Currently inlined in directory entry
    FWFS_F_OPENED  = 0x200000, // File has been opened
};

// File seek flags
enum fwfs_whence_flags {
    FWFS_SEEK_SET = 0,   // Seek relative to an absolute position
    FWFS_SEEK_CUR = 1,   // Seek relative to the current file position
    FWFS_SEEK_END = 2,   // Seek relative to the end of the file
};

typedef struct fwfs_partition
{
    uint8_t  u8_UseExternBlockSize;
    uint16_t u16_StartBlk;
    uint16_t u16_BlkCnt;
    uint32_t u32_PageSize;
    uint32_t u32_BlkSize;
} fwfs_partition_t;

// Configuration provided during initialization of the firmwarefs
struct fwfs_config {
    // Opaque user provided context that can be used to pass
    // information to the block device operations
    void *context;

    // Read a region in a block. Negative error codes are propogated
    // to the user.
    int (*read)(const struct fwfs_config *c, fwfs_block_t block,
            fwfs_off_t off, void *buffer, fwfs_size_t size);

    // Program a region in a block. The block must have previously
    // been erased. Negative error codes are propogated to the user.
    // May return FWFS_ERR_CORRUPT if the block should be considered bad.
    int (*prog)(const struct fwfs_config *c, fwfs_block_t block,
            fwfs_off_t off, const void *buffer, fwfs_size_t size);

    // Erase a block. A block must be erased before being programmed.
    // The state of an erased block is undefined. Negative error codes
    // are propogated to the user.
    // May return FWFS_ERR_CORRUPT if the block should be considered bad.
    int (*erase)(const struct fwfs_config *c, fwfs_block_t block);

    // Sync the state of the underlying block device. Negative error codes
    // are propogated to the user.
    int (*sync)(const struct fwfs_config *c);

    int (*bbt)(const struct fwfs_config *c, fwfs_block_t block);

    // Debug level, 0 means disable debug, bigger value means more eager to
    // do error check and will influence performance heavily.
    //
    // Note the currently biggest allowed number is 2.
    uint32_t debug;

    // Size of subblock, must not be zero and must be a multiple of the read
    // and program sizes, and a factor of the block size.
    fwfs_size_t subblock_size;

    // Minimum size of a block read. All read operations will be a
    // multiple of this value.
    fwfs_size_t read_size;

    // Minimum size of a block program. All program operations will be a
    // multiple of this value.
    fwfs_size_t prog_size;

    // Size of an erasable block. This does not impact ram consumption and
    // may be larger than the physical erase size. However, non-inlined files
    // take up at minimum one block. Must be a multiple of the read
    // and program sizes.
    fwfs_size_t block_size;

    // Number of erasable blocks on the device.
    fwfs_size_t block_count;

    // Number of erase cycles before firmwarefs evicts metadata logs and moves
    // the metadata to another block. Suggested values are in the
    // range 100-1000, with large values having better performance at the cost
    // of less consistent wear distribution.
    //
    // Set to -1 to disable block-level wear-leveling.
    int32_t block_cycles;

    // Size of block caches. Each cache buffers a portion of a block in RAM.
    // The firmwarefs needs a read cache, a program cache for metadata I/O.
    // Larger caches can improve performance by storing more data and reducing
    // the number of disk accesses.
    // Must be a multiple of the read and program sizes, and a factor of the
    // block size.
    fwfs_size_t cache_size;

    // Size of subblock caches. Each cache buffers a portion of a subblock in
    // RAM.
    // The firmwarefs needs a file read cache, a file program cache and the
    // additional cache per file.
    // Must be a multiple of the read and program sizes, a factor of the
    // subblock size.
    fwfs_size_t file_cache_size;

    // Size of cache pool, the number of caches in rcache_pool in fwfs_t, its
    // value must be in range [1, MAX_CACHE_POOL_SIZE].
    fwfs_size_t cache_pool_size;

    // Size of the lookahead buffer in bytes. A larger lookahead buffer
    // increases the number of blocks found during an allocation pass. The
    // lookahead buffer is stored as a compact bitmap, so each byte of RAM
    // can track 8 blocks. Must be a multiple of 8.
    fwfs_size_t lookahead_size;

    // Dump blocks' states info when failing to allocate
    // free block/subblock，debug > 0 will enable it by
    // default.
    uint32_t dump_on_full;

    // WARNING: This option should be used by mkfwfs program only.
    uint32_t compact;

    // Set its value to FWFS_COMPACT_ON_MOUNT_MAGIC will force
    // metadata compaction when mounting the filesystem.
    uint32_t compact_on_mount;
    
    // Optional statically allocated read buffer. Must be (cache_size *
    // cache_pool_size).
    // By default fwfs_malloc is used to allocate this buffer.
    void *read_buffer;

    // Optional statically allocated program buffer. Must be cache_size.
    // By default fwfs_malloc is used to allocate this buffer.
    void *prog_buffer;

    // Optional statically allocated file read buffer. Must be
    // file_cache_size.
    // By default fwfs_malloc is used to allocate this buffer.
    void *file_read_buffer;

    // Optional statically allocated file prog buffer. Must be
    // file_cache_size.
    // By default fwfs_malloc is used to allocate this buffer.
    void *file_prog_buffer;

    // Optional statically allocated lookahead buffer. Must be lookahead_size
    // and aligned to a 32-bit boundary. By default fwfs_malloc is used to
    // allocate this buffer.
    void *lookahead_buffer;

    // Optional upper limit on length of file names in bytes. No downside for
    // larger names except the size of the info struct which is controlled by
    // the FWFS_NAME_MAX define. Defaults to FWFS_NAME_MAX when zero. Stored in
    // superblock and must be respected by other firmwarefs drivers.
    fwfs_size_t name_max;

    // Optional upper limit on files in bytes. No downside for larger files
    // but must be <= FWFS_FILE_MAX. Defaults to FWFS_FILE_MAX when zero. Stored
    // in superblock and must be respected by other firmwarefs drivers.
    fwfs_size_t file_max;

    // Optional upper limit on custom attributes in bytes. No downside for
    // larger attributes size but must be <= FWFS_ATTR_MAX. Defaults to
    // FWFS_ATTR_MAX when zero.
    fwfs_size_t attr_max;

    //partition block offset
    fwfs_size_t block_offset;
    //partition type
    uint16_t  partition_type;

    fwfs_size_t page_size;

    fwfs_partition_t fwfs_partition;
};

// File info structure
struct fwfs_info {
    // Type of the file, either FWFS_TYPE_REG or FWFS_TYPE_DIR
    uint8_t type;

    // Size of the file, only valid for REG files. Limited to 32-bits.
    fwfs_size_t size;

    // Name of the file stored as a null-terminated string. Limited to
    // FWFS_NAME_MAX+1, which can be changed by redefining FWFS_NAME_MAX to
    // reduce RAM. FWFS_NAME_MAX is stored in superblock and must be
    // respected by other firmwarefs drivers.
    char name[FWFS_NAME_MAX+1];
};

// Custom attribute structure, used to describe custom attributes
// committed atomically during file writes.
struct fwfs_attr {
    // 8-bit type of attribute, provided by user and used to
    // identify the attribute
    uint8_t type;

    // Pointer to buffer containing the attribute
    void *buffer;

    // Size of attribute in bytes, limited to FWFS_ATTR_MAX
    fwfs_size_t size;
};

// Optional configuration provided during fwfs_file_opencfg
struct fwfs_file_config {
    // Optional statically allocated file buffer. Must be file_cache_size.
    // By default fwfs_malloc is used to allocate this buffer.
    void *buffer;

    // Optional list of custom attributes related to the file. If the file
    // is opened with read access, these attributes will be read from disk
    // during the open call. If the file is opened with write access, the
    // attributes will be written to disk every file sync or close. This
    // write occurs atomically with update to the file's contents.
    //
    // Custom attributes are uniquely identified by an 8-bit type and limited
    // to FWFS_ATTR_MAX bytes. When read, if the stored attribute is smaller
    // than the buffer, it will be padded with zeros. If the stored attribute
    // is larger, then it will be silently truncated. If the attribute is not
    // found, it will be created implicitly.
    struct fwfs_attr *attrs;

    // Number of custom attributes in the list
    fwfs_size_t attr_count;
};


/// internal firmwarefs data structures ///
typedef struct fwfs_cache {
    uint32_t access_tick;
    fwfs_size_t cache_size;
    fwfs_block_t block;
    fwfs_off_t off;
    fwfs_size_t size;
    uint8_t *buffer;
} fwfs_cache_t;

#define MAX_CACHE_POOL_SIZE 8
typedef struct fwfs_cache_pool {
    uint32_t access_tick;
    uint8_t size;
    fwfs_cache_t *caches[MAX_CACHE_POOL_SIZE];
} fwfs_cache_pool_t;

typedef struct fwfs_mdir {
    fwfs_block_t pair[2];
    uint32_t rev;
    fwfs_off_t off;
    uint32_t etag;
    uint16_t count;
    bool erased;
    bool split;
    bool force_compact;
    fwfs_block_t tail[2];
} fwfs_mdir_t;

// firmwarefs directory type
typedef struct fwfs_dir {
    struct fwfs_dir *next;
    uint16_t id;
    uint8_t type;
    fwfs_mdir_t m;

    fwfs_off_t pos;
    fwfs_block_t head[2];
} fwfs_dir_t;

// firmwarefs file type
typedef struct fwfs_file {
    struct fwfs_file *next;
    uint16_t id;
    uint8_t type;
    fwfs_mdir_t m;

    struct fwfs_ctz {
        fwfs_block_t head;
        fwfs_size_t size;
        //file id
        uint32_t fid;
    } ctz;

    uint32_t flags;
    fwfs_off_t pos;
    fwfs_block_t block;
    fwfs_off_t off;
    fwfs_cache_t cache;

    const struct fwfs_file_config *cfg;
} fwfs_file_t;

typedef struct fwfs_superblock {
    uint32_t version;
    fwfs_size_t block_size;
    fwfs_size_t block_count;
    fwfs_size_t name_max;
    fwfs_size_t file_max;
    fwfs_size_t attr_max;
    uint32_t tftl_version;
    fwfs_size_t subblock_size;
} fwfs_superblock_t;

typedef struct fwfs_gstate {
    uint32_t tag;
    fwfs_block_t pair[2];
} fwfs_gstate_t;

// The firmwarefs filesystem type
typedef struct fwfs {
    fwfs_cache_pool_t rcache_pool;
    fwfs_cache_t pcache;

    fwfs_cache_t file_rcache;
    fwfs_cache_t file_pcache;

    fwfs_cache_t *debug_rcache;

    struct tftl_context {
        fwfs_size_t size;
        uint8_t *states;
        uint8_t *snapshot;
        bool checked;
        fwfs_size_t header_size;
    } tftl;

    fwfs_block_t b0; // block 0
    fwfs_block_t b1; // block 1
    fwfs_block_t bn; // block null
    fwfs_block_t bi; // block inline

    fwfs_block_t root[2];
    struct fwfs_mlist {
        struct fwfs_mlist *next;
        uint16_t id;
        uint8_t type;
        fwfs_mdir_t m;
    } *mlist;
    uint32_t seed;

    fwfs_gstate_t gstate;
    fwfs_gstate_t gdisk;
    fwfs_gstate_t gdelta;

    struct fwfs_free {
        fwfs_block_t off;
        fwfs_block_t size;
        fwfs_block_t i;
        fwfs_block_t ack;
        uint32_t *buffer;
    } free;

    const struct fwfs_config *cfg;
    fwfs_size_t name_max;
    fwfs_size_t file_max;
    fwfs_size_t attr_max;

#ifdef FWFS_MIGRATE
    struct fwfs1 *fwfs1;
#endif
} fwfs_t;


/// Filesystem functions ///

// Format a block device with the firmwarefs
//
// Requires a firmwarefs object and config struct. This clobbers the firmwarefs
// object, and does not leave the filesystem mounted. The config struct must
// be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int fwfs_format(fwfs_t *fwfs, const struct fwfs_config *config);

// Mounts a firmwarefs
//
// Requires a firmwarefs object and config struct. Multiple filesystems
// may be mounted simultaneously with multiple firmwarefs objects. Both
// fwfs and config must be allocated while mounted. The config struct must
// be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int fwfs_mount(fwfs_t *fwfs, const struct fwfs_config *config);

// Unmounts a firmwarefs
//
// Does nothing besides releasing any allocated resources.
// Returns a negative error code on failure.
int fwfs_unmount(fwfs_t *fwfs);

/// General operations ///

// Removes a file or directory
//
// If removing a directory, the directory must be empty.
// Returns a negative error code on failure.
int fwfs_remove(fwfs_t *fwfs, const char *path);

// Rename or move a file or directory
//
// If the destination exists, it must match the source in type.
// If the destination is a directory, the directory must be empty.
//
// Returns a negative error code on failure.
int fwfs_rename(fwfs_t *fwfs, const char *oldpath, const char *newpath);

// Find info about a file or directory
//
// Fills out the info structure, based on the specified file or directory.
// Returns a negative error code on failure.
int fwfs_stat(fwfs_t *fwfs, const char *path, struct fwfs_info *info);

// Get a custom attribute
//
// Custom attributes are uniquely identified by an 8-bit type and limited
// to FWFS_ATTR_MAX bytes. When read, if the stored attribute is smaller than
// the buffer, it will be padded with zeros. If the stored attribute is larger,
// then it will be silently truncated. If no attribute is found, the error
// FWFS_ERR_NOATTR is returned and the buffer is filled with zeros.
//
// Returns the size of the attribute, or a negative error code on failure.
// Note, the returned size is the size of the attribute on disk, irrespective
// of the size of the buffer. This can be used to dynamically allocate a buffer
// or check for existance.
fwfs_ssize_t fwfs_getattr(fwfs_t *fwfs, const char *path,
        uint8_t type, void *buffer, fwfs_size_t size);

// Set custom attributes
//
// Custom attributes are uniquely identified by an 8-bit type and limited
// to FWFS_ATTR_MAX bytes. If an attribute is not found, it will be
// implicitly created.
//
// Returns a negative error code on failure.
int fwfs_setattr(fwfs_t *fwfs, const char *path,
        uint8_t type, const void *buffer, fwfs_size_t size);

// Removes a custom attribute
//
// If an attribute is not found, nothing happens.
//
// Returns a negative error code on failure.
int fwfs_removeattr(fwfs_t *fwfs, const char *path, uint8_t type);


/// File operations ///

// Open a file
//
// The mode that the file is opened in is determined by the flags, which
// are values from the enum fwfs_open_flags that are bitwise-ored together.
//
// Returns a negative error code on failure.
int fwfs_file_open(fwfs_t *fwfs, fwfs_file_t *file,
        const char *path, int flags);

// Open a file with extra configuration
//
// The mode that the file is opened in is determined by the flags, which
// are values from the enum fwfs_open_flags that are bitwise-ored together.
//
// The config struct provides additional config options per file as described
// above. The config struct must be allocated while the file is open, and the
// config struct must be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int fwfs_file_opencfg(fwfs_t *fwfs, fwfs_file_t *file,
        const char *path, int flags,
        const struct fwfs_file_config *config);

// Close a file
//
// Any pending writes are written out to storage as though
// sync had been called and releases any allocated resources.
//
// Returns a negative error code on failure.
int fwfs_file_close(fwfs_t *fwfs, fwfs_file_t *file);

// Synchronize a file on storage
//
// Any pending writes are written out to storage.
// Returns a negative error code on failure.
int fwfs_file_sync(fwfs_t *fwfs, fwfs_file_t *file);

// Read data from file
//
// Takes a buffer and size indicating where to store the read data.
// Returns the number of bytes read, or a negative error code on failure.
fwfs_ssize_t fwfs_file_read(fwfs_t *fwfs, fwfs_file_t *file,
        void *buffer, fwfs_size_t size);

// Write data to file
//
// Takes a buffer and size indicating the data to write. The file will not
// actually be updated on the storage until either sync or close is called.
//
// Returns the number of bytes written, or a negative error code on failure.
fwfs_ssize_t fwfs_file_write(fwfs_t *fwfs, fwfs_file_t *file,
        const void *buffer, fwfs_size_t size);

// Change the position of the file
//
// The change in position is determined by the offset and whence flag.
// Returns the new position of the file, or a negative error code on failure.
fwfs_soff_t fwfs_file_seek(fwfs_t *fwfs, fwfs_file_t *file,
        fwfs_soff_t off, int whence);

// Truncates the size of the file to the specified size
//
// Returns a negative error code on failure.
int fwfs_file_truncate(fwfs_t *fwfs, fwfs_file_t *file, fwfs_off_t size);

// Return the position of the file
//
// Equivalent to fwfs_file_seek(fwfs, file, 0, FWFS_SEEK_CUR)
// Returns the position of the file, or a negative error code on failure.
fwfs_soff_t fwfs_file_tell(fwfs_t *fwfs, fwfs_file_t *file);

// Change the position of the file to the beginning of the file
//
// Equivalent to fwfs_file_seek(fwfs, file, 0, FWFS_SEEK_SET)
// Returns a negative error code on failure.
int fwfs_file_rewind(fwfs_t *fwfs, fwfs_file_t *file);

// Return the size of the file
//
// Similar to fwfs_file_seek(fwfs, file, 0, FWFS_SEEK_END)
// Returns the size of the file, or a negative error code on failure.
fwfs_soff_t fwfs_file_size(fwfs_t *fwfs, fwfs_file_t *file);


/// Directory operations ///

// Create a directory
//
// Returns a negative error code on failure.
int fwfs_mkdir(fwfs_t *fwfs, const char *path);

// Open a directory
//
// Once open a directory can be used with read to iterate over files.
// Returns a negative error code on failure.
int fwfs_dir_open(fwfs_t *fwfs, fwfs_dir_t *dir, const char *path);

// Close a directory
//
// Releases any allocated resources.
// Returns a negative error code on failure.
int fwfs_dir_close(fwfs_t *fwfs, fwfs_dir_t *dir);

// Read an entry in the directory
//
// Fills out the info structure, based on the specified file or directory.
// Returns a positive value on success, 0 at the end of directory,
// or a negative error code on failure.
int fwfs_dir_read(fwfs_t *fwfs, fwfs_dir_t *dir, struct fwfs_info *info);

// Change the position of the directory
//
// The new off must be a value previous returned from tell and specifies
// an absolute offset in the directory seek.
//
// Returns a negative error code on failure.
int fwfs_dir_seek(fwfs_t *fwfs, fwfs_dir_t *dir, fwfs_off_t off);

// Return the position of the directory
//
// The returned offset is only meant to be consumed by seek and may not make
// sense, but does indicate the current position in the directory iteration.
//
// Returns the position of the directory, or a negative error code on failure.
fwfs_soff_t fwfs_dir_tell(fwfs_t *fwfs, fwfs_dir_t *dir);

// Change the position of the directory to the beginning of the directory
//
// Returns a negative error code on failure.
int fwfs_dir_rewind(fwfs_t *fwfs, fwfs_dir_t *dir);


/// Filesystem-level filesystem operations

// Finds the current size of the filesystem
//
// Note: Result is best effort. If files share COW structures, the returned
// size may be larger than the filesystem actually is.
//
// Returns the number of allocated blocks, or a negative error code on failure.
fwfs_ssize_t fwfs_fs_size(fwfs_t *fwfs);

// Traverse through all blocks in use by the filesystem
//
// The provided callback will be called with each block address that is
// currently in use by the filesystem. This can be used to determine which
// blocks are in use or how much of the storage is available.
//
// Returns a negative error code on failure.
int fwfs_fs_traverse(fwfs_t *fwfs, int (*cb)(void*, fwfs_block_t, fwfs_off_t, uint32_t), void *data);

int fwfs_fs_traverseraw(fwfs_t *fwfs,
        int (*cb)(void *data, fwfs_block_t block, fwfs_off_t index,
                  uint32_t fid),
        void *data, bool includeorphans);

void fwfs_alloc_reset(fwfs_t *fwfs);    
// To reuse the fwfs->rcache, TFTL will use it to read data.
int fwfs_bd_read_ext(fwfs_t *fwfs, fwfs_size_t hint, fwfs_block_t block,
                    fwfs_off_t off, void *buffer, fwfs_size_t size);
#ifdef FWFS_MIGRATE
// Attempts to migrate a previous version of firmwarefs
//
// Behaves similarly to the fwfs_format function. Attempts to mount
// the previous version of firmwarefs and update the filesystem so it can be
// mounted with the current version of firmwarefs.
//
// Requires a firmwarefs object and config struct. This clobbers the firmwarefs
// object, and does not leave the filesystem mounted. The config struct must
// be zeroed for defaults and backwards compatibility.
//
// Returns a negative error code on failure.
int fwfs_migrate(fwfs_t *fwfs, const struct fwfs_config *cfg);
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
