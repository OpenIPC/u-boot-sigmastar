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
//TFTL:Tiny flash translation layer
#ifndef FWFS_TFTL_H
#define FWFS_TFTL_H

#include <stdint.h>
#include <stdbool.h>

#include "fwfs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// Version info ///

// Version of TFTL On-disk data structures
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define FWFS_TFTL_DISK_VERSION 0x00040000
#define FWFS_TFTL_DISK_VERSION_MAJOR (0xffff & (FWFS_TFTL_DISK_VERSION >> 16))
#define FWFS_TFTL_DISK_VERSION_MINOR (0xffff & (FWFS_TFTL_DISK_VERSION >>  0))

    /**
     * tftl header on start of every subblock
     *
     * Change its size need to increase FWFS_TFTL_DISK_VERSION_MAJOR
     * Backwards compatible changes need to increase FWFS_TFTL_DISK_VERSION_MINOR
     */
    typedef struct fwfs_tftl_header {
        uint32_t tag;
        uint32_t reserved;
    } fwfs_tftl_header_t;

#define FWFS_TFTL_HEADER_SIZE  ((fwfs_size_t)sizeof(fwfs_tftl_header_t))

    int fwfs_tftl_init(fwfs_t *fwfs);

    int fwfs_tftl_deinit(fwfs_t *fwfs);

    fwfs_size_t fwfs_tftl_header_size(fwfs_t *fwfs, fwfs_block_t block);

    int fwfs_tftl_header_fill(fwfs_t *fwfs, fwfs_tftl_header_t *header,
                              fwfs_block_t block, fwfs_off_t index,
                              uint32_t fid);

    /**
     * fwfs:
     * fwfs_t pointer
     *
     * block:
     * Logical erasable block (LEB)
     *
     *
     * return the PEB id encoded in LEB
     */
    fwfs_block_t fwfs_tftl_block_id(fwfs_block_t block);

    /**
     * fwfs:
     * fwfs_t pointer
     *
     * block:
     * Logical erasable block (LEB)
     *
     *
     * return block size of LEB
     */
    fwfs_size_t fwfs_tftl_block_size(fwfs_t *fwfs, fwfs_block_t block);

    /**
     * block:
     * when ctz is true, it is a in&out paramter,
     * it pass in the pointer of allocated sub-block(LEB),
     * and pass out PEB+LEB through it after return.
     *
     * when ctz is false, sub_block is a out paramter,
     * pass out allocated PEB through it after return.
     *
     * ctz:
     * the user is metdata pairs or CTZ skip-lists.
     *
     *
     * return 0 or err.
     */
    int fwfs_tftl_alloc(fwfs_t *fwfs, fwfs_block_t *block, bool ctz);

    /**
     * seed:
     * filesystem seed
     *
     */
    void fwfs_tftl_seed(uint32_t seed);

    int fwfs_tftl_fs_size(fwfs_t *fwfs, fwfs_size_t *size);

    // Read a region in a LEB block. Negative error codes are propogated
    // to the user.
    int fwfs_tftl_read (const struct fwfs_config *c, fwfs_block_t block,
                       fwfs_off_t off, void *buffer, fwfs_size_t size);

    // Program a region in a LEB block. The block must have previously
    // been erased. Negative error codes are propogated to the user.
    // May return FWFS_ERR_CORRUPT if the block should be considered bad.
    int fwfs_tftl_prog(const struct fwfs_config *c, fwfs_block_t block,
                      fwfs_off_t off, const void *buffer, fwfs_size_t size);

    // Erase a LEB block. a LEB block must be erased before being programmed.
    // The state of an erased block is undefined. Negative error codes
    // are propogated to the user.
    // May return FWFS_ERR_CORRUPT if the block should be considered bad.
    int fwfs_tftl_erase(const struct fwfs_config *c, fwfs_block_t block);

    // Sync the state of the underlying block device. Negative error codes
    // are propogated to the user.
    int fwfs_tftl_sync(const struct fwfs_config *c);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
