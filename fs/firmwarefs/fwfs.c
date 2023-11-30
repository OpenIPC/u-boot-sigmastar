/*
* fwfs.c- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/
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
#include "fwfs.h"
#include "fwfs_util.h"
#include "fwfs_tftl.h"

#define FWFS_BLOCK_NULL ((fwfs_block_t)-1)
#define FWFS_BLOCK_INLINE ((fwfs_block_t)-2)

#define POLICY_FILE     "folder_compress_policy"
#define POLICY_CONTENT  "Aggressive:This policy will reduce nand lifetime!!!!"

static int fwfs_alloc(fwfs_t *fwfs, fwfs_block_t *block);
static int fwfs_bd_read_nopool(fwfs_t *fwfs, const fwfs_cache_t *pcache,
                               fwfs_cache_t *rcache, fwfs_size_t hint,
                               fwfs_block_t block, fwfs_off_t off,
                               void *buffer, fwfs_size_t size);
static bool fwfs_folder_compress_policy(fwfs_t *fwfs, const char *dirname);

static fwfs_size_t fwfs_fs_path_dirname(fwfs_t *fwfs, const char *path,
                                        char *buffer, fwfs_size_t len);
static fwfs_size_t fwfs_fs_mdir_dirname(fwfs_t *fwfs, fwfs_block_t dir[2],
                                        char *buffer, fwfs_size_t buflen);

static inline bool fwfs_tftl_isenabled(const struct fwfs_config *c) {
    return (c->subblock_size > 0 && c->subblock_size != c->block_size);
}

static inline int fwfs_read_wrapper(const struct fwfs_config *c, fwfs_block_t block,
			       fwfs_off_t off, void *buffer, fwfs_size_t size) {
    if (fwfs_tftl_isenabled(c))
	return fwfs_tftl_read(c, block, off, buffer, size);
    else
	return c->read(c, block, off, buffer, size);
}

static int fwfs_prog_wrapper(fwfs_t *fwfs, fwfs_block_t block, fwfs_off_t off,
                             const void *buffer, fwfs_size_t size) {
    if (fwfs->cfg->debug >= 2) {
        int err;

        for (fwfs_off_t i = 0; i < size; i++) {
            uint8_t data;

            err = fwfs_bd_read_nopool(fwfs, NULL, fwfs->debug_rcache, size - i,
                                      block, off + i, &data, 1);
            if (err) {
                return err;
            }

            if (data != 0xff) {
                FWFS_ERROR("Value 0x%"PRIx32" at offset in block 0x%"PRIx32" isn't "
                           "0x%"PRIx32, (uint32_t)data, off + i, (uint32_t)0xff);
                FWFS_ASSERT(false);
            }
        }
    }

    if (fwfs_tftl_isenabled(fwfs->cfg))
	return fwfs_tftl_prog(fwfs->cfg, block, off, buffer, size);
    else
	return fwfs->cfg->prog(fwfs->cfg, block, off, buffer, size);
}

static inline int fwfs_erase_wrapper(const struct fwfs_config *c, fwfs_block_t block) {
    if (fwfs_tftl_isenabled(c))
	return fwfs_tftl_erase(c, block);
    else
	return c->erase(c, block);
}

static inline int fwfs_sync_wrapper(const struct fwfs_config *c) {
    if (fwfs_tftl_isenabled(c))
	return fwfs_tftl_sync(c);
    else
	return c->sync(c);
}

static inline int fwfs_alloc_wrapper(fwfs_t *fwfs, fwfs_block_t *block, bool ctz) {
    if (fwfs_tftl_isenabled(fwfs->cfg))
	return fwfs_tftl_alloc(fwfs, block, ctz);
    else
	return fwfs_alloc(fwfs, block);
}

static inline  fwfs_size_t fwfs_block_size_wrapper(fwfs_t *fwfs, fwfs_block_t block) {
    if (fwfs_tftl_isenabled(fwfs->cfg))
        return fwfs_tftl_block_size(fwfs, block);
    else
        return fwfs->cfg->block_size;
}

static inline  fwfs_size_t fwfs_tftl_header_size_wrapper(fwfs_t *fwfs, fwfs_block_t block) {
    if (fwfs_tftl_isenabled(fwfs->cfg))
        return fwfs_tftl_header_size(fwfs, block);
    else
        return 0;
}

/// Caching block device operations ///
static inline void fwfs_cache_drop(fwfs_t *fwfs, fwfs_cache_t *rcache) {
    // do not zero, cheaper if cache is readonly or only going to be
    // written with identical data (during relocates)
    (void)fwfs;
    rcache->block = fwfs->bn;
    rcache->access_tick = 0;
}

static inline void fwfs_cache_pool_drop(fwfs_t *fwfs,
                                       fwfs_cache_pool_t *rcache_pool) {
    // do not zero, cheaper if cache is readonly or only going to be
    // written with identical data (during relocates)
    (void)fwfs;
    for (int i = 0; i < rcache_pool->size; i++) {
        rcache_pool->caches[i]->block = fwfs->bn;
    }
}

static inline void fwfs_cache_zero(fwfs_t *fwfs, fwfs_cache_t *pcache) {
    // zero to avoid information leak
    memset(pcache->buffer, 0xff, pcache->cache_size);
    pcache->block = fwfs->bn;
    pcache->access_tick = 0;
}

static inline void fwfs_cache_pool_zero(fwfs_t *fwfs,
                                       fwfs_cache_pool_t *cache_pool) {
    (void)fwfs;
    for (int i = 0; i < cache_pool->size; i++) {
        fwfs_cache_zero(fwfs, cache_pool->caches[i]);
    }
}

static inline bool fwfs_block_check(fwfs_t *fwfs, fwfs_block_t block) {
    if (fwfs_tftl_block_id(block) >= fwfs->cfg->block_count)
        return false;
    return true;
}

static inline uint32_t fwfs_get_fid(fwfs_t *fwfs) {
    return fwfs->seed;
}

static int fwfs_bd_read(fwfs_t *fwfs,
        const fwfs_cache_t *pcache, fwfs_cache_pool_t *rcache_pool,
        fwfs_size_t hint, fwfs_block_t block, fwfs_off_t off,
        void *buffer, fwfs_size_t size) {
    uint8_t *data = buffer;
    fwfs_size_t block_size = fwfs_block_size_wrapper(fwfs, block);
    fwfs_size_t header_size = fwfs_tftl_header_size_wrapper(fwfs, block);
    fwfs_cache_t *rcache;
    fwfs_cache_t *tmp;

    FWFS_ASSERT(rcache_pool && rcache_pool->size > 0);

    for(uint8_t i = 0; i < rcache_pool->size; i++) {
        FWFS_ASSERT(rcache_pool->caches[i]->buffer);
    }

    if (fwfs->cfg->debug > 0) {
        for(uint8_t i = 1; i < rcache_pool->size; i++) {
            for(uint8_t j = 0; j < i; j++) {
                rcache = rcache_pool->caches[i];
                tmp = rcache_pool->caches[j];

                if (!(rcache->block == fwfs->bn ||
                      rcache->block != tmp->block ||
                      rcache->off >= tmp->off + tmp->size ||
                      tmp->off >= rcache->off + rcache->size)) {
                    FWFS_ERROR("rcache %d:block=0x%"PRIx32", off=0x%"PRIx32", "
                               "size=0x%"PRIx32", tick=0x%"PRIx32"; "
                               "tmp %d:block=0x%"PRIx32", off=0x%"PRIx32", "
                               "size=0x%"PRIx32", tick=0x%"PRIx32,
                               i, rcache->block, rcache->off, rcache->size,
                               rcache->access_tick,
                               j, tmp->block, tmp->off, tmp->size,
                               tmp->access_tick);
                    FWFS_ASSERT(false);
                }
            }
        }
    }

    if (!fwfs_block_check(fwfs, block) || off+size > block_size) {
        return FWFS_ERR_CORRUPT;
    }

 retry:
    while (size > 0) {
        fwfs_size_t diff = size;

        if (pcache && block == pcache->block &&
                off < pcache->off + pcache->size) {
            if (off >= pcache->off) {
                // is already in pcache?
                diff = fwfs_min(diff, pcache->size - (off-pcache->off));
                memcpy(data, &pcache->buffer[off-pcache->off], diff);

                data += diff;
                off += diff;
                size -= diff;
                continue;
            }

            // pcache takes priority
            diff = fwfs_min(diff, pcache->off-off);
            hint = fwfs_min(hint, pcache->off-off);
        }

        for (uint8_t i = 0; i < rcache_pool->size; i++) {
            rcache = rcache_pool->caches[i];

            if (block == rcache->block &&
                off < rcache->off + rcache->size) {
                if (off >= rcache->off) {
                    rcache->access_tick = ++fwfs->rcache_pool.access_tick;

                    // is already in rcache?
                    diff = fwfs_min(diff, rcache->size - (off-rcache->off));
                    memcpy(data, &rcache->buffer[off-rcache->off], diff);

                    data += diff;
                    off += diff;
                    size -= diff;
                    goto retry;
                }

                // rcache takes priority
                diff = fwfs_min(diff, rcache->off-off);
                hint = fwfs_min(hint, rcache->off-off);
            }
        }

        if (CACHE_LINE_ALIGNED(data) && size >= hint &&
            (off == header_size || off % fwfs->cfg->read_size == 0) &&
            size >= fwfs->cfg->read_size) {
            fwfs_size_t extra_size;

            FWFS_TRACE("fwfs_bd_read 0x%"PRIx32", bypass cache at off "
                       "0x%"PRIx32" with size 0x%"PRIx32", hint is 0x%"PRIx32,
                       block, off, diff, hint);
            if (off == header_size) {
                // memmove is slow, reduce the read size to size of smallest
                // I/O unit.
                extra_size = header_size;
                diff = fwfs->cfg->read_size;
            } else {
                extra_size = 0;
                diff = fwfs_aligndown(diff, fwfs->cfg->read_size);
            }

            // bypass cache?
            int err = fwfs_read_wrapper(fwfs->cfg, block, off - extra_size,
                                        data, diff);
            if (err > 0) {
                FWFS_ERROR("fwfs_read_wrapper return error %d:0x%x, 0x%x\n",
                           err, block, off - extra_size);
            }
            FWFS_ASSERT(err <= 0);
            if (err) {
                return err;
            }

            if (off == extra_size && extra_size > 0) {
                memmove(data, data + extra_size, diff - extra_size);
            }
            diff -= extra_size;

            data += diff;
            off += diff;
            size -= diff;
            continue;
        }

        //find next available rcache slot
        tmp = rcache_pool->caches[0];
        for(uint8_t i = 0; i < rcache_pool->size; i++) {
            rcache = rcache_pool->caches[i];

            if(rcache->block == fwfs->bn) {
                tmp = rcache;
                break;
            } else if(tmp->block != fwfs->bn &&
                      rcache->access_tick < tmp->access_tick) {
                tmp = rcache;
            }
        }
        rcache = tmp;

        // load to cache, first condition can no longer fail
        FWFS_ASSERT(fwfs_block_check(fwfs, block));
        rcache->block = block;
        rcache->off = fwfs_aligndown(off, fwfs->cfg->read_size);
        rcache->size = fwfs_min(
                fwfs_min(
                    fwfs_alignup(off+hint, fwfs->cfg->read_size),
                    block_size)
                - rcache->off,
                rcache->cache_size);
        int err = fwfs_read_wrapper(fwfs->cfg, rcache->block,
				   rcache->off, rcache->buffer, rcache->size);
        if (err > 0) {
            FWFS_ERROR("fwfs_read_wrapper return error %d:0x%x, 0x%x\n",
                       err, rcache->block, rcache->off);
        }
        FWFS_ASSERT(err <= 0);
        if (err) {
            return err;
        }
    }

    return 0;
}

static int fwfs_bd_read_nopool(fwfs_t *fwfs, const fwfs_cache_t *pcache,
                              fwfs_cache_t *rcache, fwfs_size_t hint,
                              fwfs_block_t block, fwfs_off_t off,
                              void *buffer, fwfs_size_t size) {
    fwfs_cache_pool_t cache_pool;

    cache_pool.access_tick = 0;
    cache_pool.size = 1;
    cache_pool.caches[0] = rcache;
    return fwfs_bd_read(fwfs, pcache, &cache_pool, hint, block, off,
                       buffer, size);
}

int fwfs_bd_read_ext(fwfs_t *fwfs, fwfs_size_t hint, fwfs_block_t block,
                    fwfs_off_t off, void *buffer, fwfs_size_t size) {
    return fwfs_bd_read(fwfs, &fwfs->pcache, &fwfs->rcache_pool, hint, block,
                       off, buffer, size);
}

enum {
    FWFS_CMP_EQ = 0,
    FWFS_CMP_LT = 1,
    FWFS_CMP_GT = 2,
};

static int fwfs_bd_cmp(fwfs_t *fwfs,
        const fwfs_cache_t *pcache, fwfs_cache_pool_t *rcache_pool,
        fwfs_size_t hint,
        fwfs_block_t block, fwfs_off_t off,
        const void *buffer, fwfs_size_t size) {
    const uint8_t *data = buffer;

    for (fwfs_off_t i = 0; i < size; i++) {
        uint8_t dat;
        int err = fwfs_bd_read(fwfs,
                pcache, rcache_pool, hint-i,
                block, off+i, &dat, 1);
        if (err) {
            return err;
        }

        if (dat != data[i]) {
            return (dat < data[i]) ? FWFS_CMP_LT : FWFS_CMP_GT;
        }
    }

    return FWFS_CMP_EQ;
}

static int fwfs_bd_flush(fwfs_t *fwfs,
        fwfs_cache_t *pcache, fwfs_cache_pool_t *rcache_pool, bool validate) {
    if (pcache->block != fwfs->bn && pcache->block != fwfs->bi) {
        FWFS_ASSERT(fwfs_block_check(fwfs, pcache->block));
        fwfs_size_t diff = fwfs_alignup(pcache->size, fwfs->cfg->prog_size);
        int err = fwfs_prog_wrapper(fwfs, pcache->block,
				   pcache->off, pcache->buffer, diff);
        if (err > 0) {
            FWFS_ERROR("fwfs_prog_wrapper return error %d:0x%x, 0x%x\n",
                       err, pcache->block, pcache->off);
        }
        FWFS_ASSERT(err <= 0);
        if (err) {
            return err;
        }

        if (validate) {
            // check data on disk
            fwfs_cache_pool_drop(fwfs, rcache_pool);
            int res = fwfs_bd_cmp(fwfs,
                    NULL, rcache_pool, diff,
                    pcache->block, pcache->off, pcache->buffer, diff);
            if (res < 0) {
                return res;
            }

            if (res != FWFS_CMP_EQ) {
                return FWFS_ERR_CORRUPT;
            }
        }

        fwfs_cache_zero(fwfs, pcache);
    }

    return 0;
}

static int fwfs_bd_flush_nopool(fwfs_t *fwfs,
        fwfs_cache_t *pcache, fwfs_cache_t *rcache, bool validate) {
    fwfs_cache_pool_t cache_pool;

    cache_pool.access_tick = 0;
    cache_pool.size = 1;
    cache_pool.caches[0] = rcache;
    return fwfs_bd_flush(fwfs,  pcache, &cache_pool, validate);
}

static int fwfs_bd_sync(fwfs_t *fwfs,
        fwfs_cache_t *pcache, fwfs_cache_pool_t *rcache_pool, bool validate) {
    fwfs_cache_pool_drop(fwfs, rcache_pool);

    int err = fwfs_bd_flush(fwfs, pcache, rcache_pool, validate);
    if (err) {
        return err;
    }

    err = fwfs_sync_wrapper(fwfs->cfg);
    if (err > 0) {
        FWFS_ERROR("fwfs_sync_wrapper return error %d\n", err);
    }
    FWFS_ASSERT(err <= 0);
    return err;
}

static int fwfs_bd_prog(fwfs_t *fwfs,
        fwfs_cache_t *pcache, fwfs_cache_pool_t *rcache_pool, bool validate,
        fwfs_block_t block, fwfs_off_t off,
        const void *buffer, fwfs_size_t size) {
    const uint8_t *data = buffer;
    fwfs_size_t block_size = fwfs_block_size_wrapper(fwfs, block);

    FWFS_ASSERT(block == fwfs->bi || fwfs_block_check(fwfs, block));
    FWFS_ASSERT(off + size <= block_size);

    while (size > 0) {
        if (block == pcache->block &&
                off >= pcache->off &&
                off < pcache->off + pcache->cache_size) {
            // already fits in pcache?
            fwfs_size_t diff = fwfs_min(size,
                    pcache->cache_size - (off-pcache->off));
            memcpy(&pcache->buffer[off-pcache->off], data, diff);

            data += diff;
            off += diff;
            size -= diff;

            pcache->size = fwfs_max(pcache->size, off - pcache->off);
            if (pcache->size == pcache->cache_size) {
                // eagerly flush out pcache if we fill up
                int err = fwfs_bd_flush(fwfs, pcache, rcache_pool, validate);
                if (err) {
                    return err;
                }
            }

            continue;
        }

        // pcache must have been flushed, either by programming and
        // entire block or manually flushing the pcache
        FWFS_ASSERT(pcache->block == fwfs->bn);

        // prepare pcache, first condition can no longer fail
        pcache->block = block;
        pcache->off = fwfs_aligndown(off, fwfs->cfg->prog_size);
        pcache->size = 0;
    }

    return 0;
}

static int fwfs_bd_prog_nopool(fwfs_t *fwfs,
        fwfs_cache_t *pcache, fwfs_cache_t *rcache, bool validate,
        fwfs_block_t block, fwfs_off_t off,
        const void *buffer, fwfs_size_t size) {
    fwfs_cache_pool_t cache_pool;

    cache_pool.access_tick = 0;
    cache_pool.size = 1;
    cache_pool.caches[0] = rcache;
    return fwfs_bd_prog(fwfs, pcache, &cache_pool, validate,
                        block, off, buffer, size);
}

static int fwfs_bd_erase(fwfs_t *fwfs, fwfs_block_t block) {
    FWFS_ASSERT(fwfs_block_check(fwfs, block));
    int err = fwfs_erase_wrapper(fwfs->cfg, block);
    if (err > 0) {
        FWFS_ERROR("fwfs_erase_wrapper return error %d:0x%x\n",
                   err, block);
    }
    FWFS_ASSERT(err <= 0);
    return err;
}


/// Small type-level utilities ///
// operations on block pairs
static inline void fwfs_pair_swap(fwfs_block_t pair[2]) {
    fwfs_block_t t = pair[0];
    pair[0] = pair[1];
    pair[1] = t;
}

static inline bool fwfs_pair_isnull(fwfs_t *fwfs, const fwfs_block_t pair[2]) {
    return pair[0] == fwfs->bn || pair[1] == fwfs->bn;
}

static inline int fwfs_pair_cmp(
        const fwfs_block_t paira[2],
        const fwfs_block_t pairb[2]) {
    return !(paira[0] == pairb[0] || paira[1] == pairb[1] ||
             paira[0] == pairb[1] || paira[1] == pairb[0]);
}

static inline bool fwfs_pair_sync(
        const fwfs_block_t paira[2],
        const fwfs_block_t pairb[2]) {
    return (paira[0] == pairb[0] && paira[1] == pairb[1]) ||
           (paira[0] == pairb[1] && paira[1] == pairb[0]);
}

static inline void fwfs_pair_fromle32(fwfs_block_t pair[2]) {
    pair[0] = fwfs_fromle32(pair[0]);
    pair[1] = fwfs_fromle32(pair[1]);
}

static inline void fwfs_pair_tole32(fwfs_block_t pair[2]) {
    pair[0] = fwfs_tole32(pair[0]);
    pair[1] = fwfs_tole32(pair[1]);
}

// operations on 32-bit entry tags
typedef uint32_t fwfs_tag_t;
typedef int32_t fwfs_stag_t;

#define FWFS_MKTAG(type, id, size) \
    (((fwfs_tag_t)(type) << 20) | ((fwfs_tag_t)(id) << 10) | (fwfs_tag_t)(size))

#define FWFS_MKTAG_IF(cond, type, id, size) \
    ((cond) ? FWFS_MKTAG(type, id, size) : FWFS_MKTAG(FWFS_FROM_NOOP, 0, 0))

#define FWFS_MKTAG_IF_ELSE(cond, type1, id1, size1, type2, id2, size2) \
    ((cond) ? FWFS_MKTAG(type1, id1, size1) : FWFS_MKTAG(type2, id2, size2))

static inline bool fwfs_tag_isvalid(fwfs_tag_t tag) {
    return !(tag & 0x80000000);
}

static inline bool fwfs_tag_isdelete(fwfs_tag_t tag) {
    return ((int32_t)(tag << 22) >> 22) == -1;
}

static inline uint16_t fwfs_tag_type1(fwfs_tag_t tag) {
    return (tag & 0x70000000) >> 20;
}

static inline uint16_t fwfs_tag_type3(fwfs_tag_t tag) {
    return (tag & 0x7ff00000) >> 20;
}

static inline uint8_t fwfs_tag_chunk(fwfs_tag_t tag) {
    return (tag & 0x0ff00000) >> 20;
}

static inline int8_t fwfs_tag_splice(fwfs_tag_t tag) {
    return (int8_t)fwfs_tag_chunk(tag);
}

static inline uint16_t fwfs_tag_id(fwfs_tag_t tag) {
    return (tag & 0x000ffc00) >> 10;
}

static inline fwfs_size_t fwfs_tag_size(fwfs_tag_t tag) {
    return tag & 0x000003ff;
}

static inline fwfs_size_t fwfs_tag_dsize(fwfs_tag_t tag) {
    return sizeof(tag) + fwfs_tag_size(tag + fwfs_tag_isdelete(tag));
}

// operations on attributes in attribute lists
struct fwfs_mattr {
    fwfs_tag_t tag;
    const void *buffer;
};

struct fwfs_diskoff {
    fwfs_block_t block;
    fwfs_off_t off;
};

#define FWFS_MKATTRS(...) \
    (struct fwfs_mattr[]){__VA_ARGS__}, \
    sizeof((struct fwfs_mattr[]){__VA_ARGS__}) / sizeof(struct fwfs_mattr)

// operations on global state
static inline void fwfs_gstate_xor(fwfs_gstate_t *a, const fwfs_gstate_t *b) {
    for (int i = 0; i < 3; i++) {
        ((uint32_t*)a)[i] ^= ((const uint32_t*)b)[i];
    }
}

static inline bool fwfs_gstate_iszero(const fwfs_gstate_t *a) {
    for (int i = 0; i < 3; i++) {
        if (((uint32_t*)a)[i] != 0) {
            return false;
        }
    }
    return true;
}

static inline bool fwfs_gstate_hasorphans(const fwfs_gstate_t *a) {
    return fwfs_tag_size(a->tag);
}

static inline uint8_t fwfs_gstate_getorphans(const fwfs_gstate_t *a) {
    return fwfs_tag_size(a->tag);
}

static inline bool fwfs_gstate_hasmove(const fwfs_gstate_t *a) {
    return fwfs_tag_type1(a->tag);
}

static inline bool fwfs_gstate_hasmovehere(const fwfs_gstate_t *a,
        const fwfs_block_t *pair) {
    return fwfs_tag_type1(a->tag) && fwfs_pair_cmp(a->pair, pair) == 0;
}

static inline void fwfs_gstate_fromle32(fwfs_gstate_t *a) {
    a->tag     = fwfs_fromle32(a->tag);
    a->pair[0] = fwfs_fromle32(a->pair[0]);
    a->pair[1] = fwfs_fromle32(a->pair[1]);
}

static inline void fwfs_gstate_tole32(fwfs_gstate_t *a) {
    a->tag     = fwfs_tole32(a->tag);
    a->pair[0] = fwfs_tole32(a->pair[0]);
    a->pair[1] = fwfs_tole32(a->pair[1]);
}

// other endianness operations
static void fwfs_ctz_fromle32(struct fwfs_ctz *ctz) {
    ctz->head = fwfs_fromle32(ctz->head);
    ctz->size = fwfs_fromle32(ctz->size);
    ctz->fid = fwfs_fromle32(ctz->fid);
}

static void fwfs_ctz_tole32(struct fwfs_ctz *ctz) {
    ctz->head = fwfs_tole32(ctz->head);
    ctz->size = fwfs_tole32(ctz->size);
    ctz->fid = fwfs_tole32(ctz->fid);
}

static inline void fwfs_superblock_fromle32(fwfs_superblock_t *superblock) {
    superblock->version     = fwfs_fromle32(superblock->version);
    superblock->block_size  = fwfs_fromle32(superblock->block_size);
    superblock->block_count = fwfs_fromle32(superblock->block_count);
    superblock->name_max    = fwfs_fromle32(superblock->name_max);
    superblock->file_max    = fwfs_fromle32(superblock->file_max);
    superblock->attr_max    = fwfs_fromle32(superblock->attr_max);
}

static inline void fwfs_superblock_tole32(fwfs_superblock_t *superblock) {
    superblock->version     = fwfs_tole32(superblock->version);
    superblock->block_size  = fwfs_tole32(superblock->block_size);
    superblock->block_count = fwfs_tole32(superblock->block_count);
    superblock->name_max    = fwfs_tole32(superblock->name_max);
    superblock->file_max    = fwfs_tole32(superblock->file_max);
    superblock->attr_max    = fwfs_tole32(superblock->attr_max);
}


/// Internal operations predeclared here ///
static int fwfs_dir_commit(fwfs_t *fwfs, fwfs_mdir_t *dir,
        const struct fwfs_mattr *attrs, int attrcount);
static int fwfs_dir_compact(fwfs_t *fwfs,
        fwfs_mdir_t *dir, const struct fwfs_mattr *attrs, int attrcount,
        fwfs_mdir_t *source, uint16_t begin, uint16_t end);
static int fwfs_file_outline(fwfs_t *fwfs, fwfs_file_t *file);
static int fwfs_file_flush(fwfs_t *fwfs, fwfs_file_t *file);
static void fwfs_fs_preporphans(fwfs_t *fwfs, int8_t orphans);
static void fwfs_fs_prepmove(fwfs_t *fwfs,
        uint16_t id, const fwfs_block_t pair[2]);
static int fwfs_fs_pred(fwfs_t *fwfs, const fwfs_block_t dir[2],
        fwfs_mdir_t *pdir);
static fwfs_stag_t fwfs_fs_parent(fwfs_t *fwfs, const fwfs_block_t dir[2],
        fwfs_mdir_t *parent);
static int fwfs_fs_relocate(fwfs_t *fwfs,
        const fwfs_block_t oldpair[2], fwfs_block_t newpair[2]);
static int fwfs_fs_forceconsistency(fwfs_t *fwfs);
static int fwfs_fs_forcecompact(fwfs_t *fwfs);
static int fwfs_deinit(fwfs_t *fwfs);
#ifdef FWFS_MIGRATE
static int fwfs1_traverse(fwfs_t *fwfs,
        int (*cb)(void*, fwfs_block_t), void *data);
#endif

/// Block allocator ///
static int fwfs_alloc_lookahead(void *p, fwfs_block_t block, fwfs_off_t index,
                                uint32_t fid) {
    (void)index;
    (void)fid;
    fwfs_t *fwfs = (fwfs_t*)p;
    fwfs_block_t off = ((block - fwfs->free.off)
            + fwfs->cfg->block_count) % fwfs->cfg->block_count;

    if (off < fwfs->free.size) {
        fwfs->free.buffer[off / 32] |= 1U << (off % 32);
    }

    return 0;
}

static void fwfs_alloc_ack(fwfs_t *fwfs) {
    fwfs->free.ack = fwfs->cfg->block_count;
}

// Invalidate the lookahead buffer. This is done during mounting and
// failed traversals
void fwfs_alloc_reset(fwfs_t *fwfs) {
    fwfs->free.off = fwfs->seed % fwfs->cfg->block_count;
    fwfs->free.size = 0;
    fwfs->free.i = 0;
    fwfs_alloc_ack(fwfs);
}

static int fwfs_alloc(fwfs_t *fwfs, fwfs_block_t *block) {
    while (true) {
        while (fwfs->free.i != fwfs->free.size) {
            fwfs_block_t off = fwfs->free.i;
            fwfs->free.i += 1;
            fwfs->free.ack -= 1;

            if (!(fwfs->free.buffer[off / 32] & (1U << (off % 32)))) {
                // found a free block
                *block = (fwfs->free.off + off) % fwfs->cfg->block_count;

                // eagerly find next off so an alloc ack can
                // discredit old lookahead blocks
                while (fwfs->free.i != fwfs->free.size &&
                        (fwfs->free.buffer[fwfs->free.i / 32]
                            & (1U << (fwfs->free.i % 32)))) {
                    fwfs->free.i += 1;
                    fwfs->free.ack -= 1;
                }

                return 0;
            }
        }

        // check if we have looked at all blocks since last ack
        if (fwfs->free.ack == 0) {
            FWFS_ERROR("No more free space %"PRIu32,
                    fwfs->free.i + fwfs->free.off);
            return FWFS_ERR_NOSPC;
        }

        fwfs->free.off = (fwfs->free.off + fwfs->free.size)
                % fwfs->cfg->block_count;
        fwfs->free.size = fwfs_min(8*fwfs->cfg->lookahead_size, fwfs->free.ack);
        fwfs->free.i = 0;

        // find mask of free blocks from tree
        memset(fwfs->free.buffer, 0, fwfs->cfg->lookahead_size);
        int err = fwfs_fs_traverseraw(fwfs, fwfs_alloc_lookahead, fwfs, true);
        if (err) {
            fwfs_alloc_reset(fwfs);
            return err;
        }
    }
}

/// Metadata pair and directory operations ///
static fwfs_stag_t fwfs_dir_getslice(fwfs_t *fwfs, const fwfs_mdir_t *dir,
        fwfs_tag_t gmask, fwfs_tag_t gtag,
        fwfs_off_t goff, void *gbuffer, fwfs_size_t gsize) {
    fwfs_off_t off = dir->off;
    fwfs_tag_t ntag = dir->etag;
    fwfs_stag_t gdiff = 0;

    if (fwfs_gstate_hasmovehere(&fwfs->gdisk, dir->pair) &&
            fwfs_tag_id(gmask) != 0 &&
            fwfs_tag_id(fwfs->gdisk.tag) <= fwfs_tag_id(gtag)) {
        // synthetic moves
        gdiff -= FWFS_MKTAG(0, 1, 0);
    }

    // iterate over dir block backwards (for faster lookups)
    while (off >= sizeof(fwfs_tag_t) + fwfs_tag_dsize(ntag)) {
        off -= fwfs_tag_dsize(ntag);
        fwfs_tag_t tag = ntag;
        int err = fwfs_bd_read(fwfs,
                NULL, &fwfs->rcache_pool, sizeof(ntag),
                dir->pair[0], off, &ntag, sizeof(ntag));
        if (err) {
            return err;
        }

        ntag = (fwfs_frombe32(ntag) ^ tag) & 0x7fffffff;

        if (fwfs_tag_id(gmask) != 0 &&
                fwfs_tag_type1(tag) == FWFS_TYPE_SPLICE &&
                fwfs_tag_id(tag) <= fwfs_tag_id(gtag - gdiff)) {
            if (tag == (FWFS_MKTAG(FWFS_TYPE_CREATE, 0, 0) |
                    (FWFS_MKTAG(0, 0x3ff, 0) & (gtag - gdiff)))) {
                // found where we were created
                return FWFS_ERR_NOENT;
            }

            // move around splices
            gdiff += FWFS_MKTAG(0, fwfs_tag_splice(tag), 0);
        }

        if ((gmask & tag) == (gmask & (gtag - gdiff))) {
            if (fwfs_tag_isdelete(tag)) {
                return FWFS_ERR_NOENT;
            }

            fwfs_size_t diff = fwfs_min(fwfs_tag_size(tag), gsize);
            err = fwfs_bd_read(fwfs,
                    NULL, &fwfs->rcache_pool, diff,
                    dir->pair[0], off+sizeof(tag)+goff, gbuffer, diff);
            if (err) {
                return err;
            }

            memset((uint8_t*)gbuffer + diff, 0, gsize - diff);

            return tag + gdiff;
        }
    }

    return FWFS_ERR_NOENT;
}

static fwfs_stag_t fwfs_dir_get(fwfs_t *fwfs, const fwfs_mdir_t *dir,
        fwfs_tag_t gmask, fwfs_tag_t gtag, void *buffer) {
    return fwfs_dir_getslice(fwfs, dir,
            gmask, gtag,
            0, buffer, fwfs_tag_size(gtag));
}

static int fwfs_dir_getread(fwfs_t *fwfs, const fwfs_mdir_t *dir,
        const fwfs_cache_t *pcache, fwfs_cache_t *rcache, fwfs_size_t hint,
        fwfs_tag_t gmask, fwfs_tag_t gtag,
        fwfs_off_t off, void *buffer, fwfs_size_t size) {
    uint8_t *data = buffer;
    fwfs_size_t block_size = fwfs_block_size_wrapper(fwfs, dir->pair[0]);

    if (off+size > block_size) {
        return FWFS_ERR_CORRUPT;
    }

    while (size > 0) {
        fwfs_size_t diff = size;

        if (pcache && pcache->block == fwfs->bi &&
                off < pcache->off + pcache->size) {
            if (off >= pcache->off) {
                // is already in pcache?
                diff = fwfs_min(diff, pcache->size - (off-pcache->off));
                memcpy(data, &pcache->buffer[off-pcache->off], diff);

                data += diff;
                off += diff;
                size -= diff;
                continue;
            }

            // pcache takes priority
            diff = fwfs_min(diff, pcache->off-off);
        }

        if (rcache->block == fwfs->bi &&
                off < rcache->off + rcache->size) {
            if (off >= rcache->off) {
                // is already in rcache?
                diff = fwfs_min(diff, rcache->size - (off-rcache->off));
                memcpy(data, &rcache->buffer[off-rcache->off], diff);

                data += diff;
                off += diff;
                size -= diff;
                continue;
            }

            // rcache takes priority
            diff = fwfs_min(diff, rcache->off-off);
        }

        // load to cache, first condition can no longer fail
        rcache->block = fwfs->bi;
        rcache->off = fwfs_aligndown(off, fwfs->cfg->read_size);
        rcache->size = fwfs_min(fwfs_alignup(off+hint, fwfs->cfg->read_size),
                rcache->cache_size);
        int err = fwfs_dir_getslice(fwfs, dir, gmask, gtag,
                rcache->off, rcache->buffer, rcache->size);
        if (err < 0) {
            return err;
        }
    }

    return 0;
}

static int fwfs_dir_traverse_filter(void *p,
        fwfs_tag_t tag, const void *buffer) {
    fwfs_tag_t *filtertag = p;
    (void)buffer;

    // which mask depends on unique bit in tag structure
    uint32_t mask = (tag & FWFS_MKTAG(0x100, 0, 0))
            ? FWFS_MKTAG(0x7ff, 0x3ff, 0)
            : FWFS_MKTAG(0x700, 0x3ff, 0);

    // check for redundancy
    if ((mask & tag) == (mask & *filtertag) ||
            fwfs_tag_isdelete(*filtertag) ||
            (FWFS_MKTAG(0x7ff, 0x3ff, 0) & tag) == (
                FWFS_MKTAG(FWFS_TYPE_DELETE, 0, 0) |
                    (FWFS_MKTAG(0, 0x3ff, 0) & *filtertag))) {
        return true;
    }

    // check if we need to adjust for created/deleted tags
    if (fwfs_tag_type1(tag) == FWFS_TYPE_SPLICE &&
            fwfs_tag_id(tag) <= fwfs_tag_id(*filtertag)) {
        *filtertag += FWFS_MKTAG(0, fwfs_tag_splice(tag), 0);
    }

    return false;
}

static int fwfs_dir_traverse(fwfs_t *fwfs,
        const fwfs_mdir_t *dir, fwfs_off_t off, fwfs_tag_t ptag,
        const struct fwfs_mattr *attrs, int attrcount,
        fwfs_tag_t tmask, fwfs_tag_t ttag,
        uint16_t begin, uint16_t end, int16_t diff,
        int (*cb)(void *data, fwfs_tag_t tag, const void *buffer), void *data) {
    // iterate over directory and attrs
    while (true) {
        fwfs_tag_t tag;
        const void *buffer;
        struct fwfs_diskoff disk;
        if (off+fwfs_tag_dsize(ptag) < dir->off) {
            off += fwfs_tag_dsize(ptag);
            int err = fwfs_bd_read(fwfs,
                    NULL, &fwfs->rcache_pool, sizeof(tag),
                    dir->pair[0], off, &tag, sizeof(tag));
            if (err) {
                return err;
            }

            tag = (fwfs_frombe32(tag) ^ ptag) | 0x80000000;
            disk.block = dir->pair[0];
            disk.off = off+sizeof(fwfs_tag_t);
            buffer = &disk;
            ptag = tag;
        } else if (attrcount > 0) {
            tag = attrs[0].tag;
            buffer = attrs[0].buffer;
            attrs += 1;
            attrcount -= 1;
        } else {
            return 0;
        }

        fwfs_tag_t mask = FWFS_MKTAG(0x7ff, 0, 0);
        if ((mask & tmask & tag) != (mask & tmask & ttag)) {
            continue;
        }

        // do we need to filter? inlining the filtering logic here allows
        // for some minor optimizations
        if (fwfs_tag_id(tmask) != 0) {
            // scan for duplicates and update tag based on creates/deletes
            int filter = fwfs_dir_traverse(fwfs,
                    dir, off, ptag, attrs, attrcount,
                    0, 0, 0, 0, 0,
                    fwfs_dir_traverse_filter, &tag);
            if (filter < 0) {
                return filter;
            }

            if (filter) {
                continue;
            }

            // in filter range?
            if (!(fwfs_tag_id(tag) >= begin && fwfs_tag_id(tag) < end)) {
                continue;
            }
        }

        // handle special cases for mcu-side operations
        if (fwfs_tag_type3(tag) == FWFS_FROM_NOOP) {
            // do nothing
        } else if (fwfs_tag_type3(tag) == FWFS_FROM_MOVE) {
            uint16_t fromid = fwfs_tag_size(tag);
            uint16_t toid = fwfs_tag_id(tag);
            int err = fwfs_dir_traverse(fwfs,
                    buffer, 0, 0xffffffff, NULL, 0,
                    FWFS_MKTAG(0x600, 0x3ff, 0),
                    FWFS_MKTAG(FWFS_TYPE_STRUCT, 0, 0),
                    fromid, fromid+1, toid-fromid+diff,
                    cb, data);
            if (err) {
                return err;
            }
        } else if (fwfs_tag_type3(tag) == FWFS_FROM_USERATTRS) {
            for (unsigned i = 0; i < fwfs_tag_size(tag); i++) {
                const struct fwfs_attr *a = buffer;
                int err = cb(data, FWFS_MKTAG(FWFS_TYPE_USERATTR + a[i].type,
                        fwfs_tag_id(tag) + diff, a[i].size), a[i].buffer);
                if (err) {
                    return err;
                }
            }
        } else {
            int err = cb(data, tag + FWFS_MKTAG(0, diff, 0), buffer);
            if (err) {
                return err;
            }
        }
    }
}

static bool fwfs_dir_need_compact(fwfs_t *fwfs, fwfs_mdir_t *dir,
                                  fwfs_size_t size) {
    fwfs_size_t pages = size / fwfs->cfg->prog_size;
    fwfs_off_t off;

    if (size % fwfs->cfg->prog_size)
        pages += 1;

    off = pages * fwfs->cfg->prog_size;

    //Need compaction if it could reduce at least one page
    off += fwfs->cfg->prog_size;

    return off <= dir->off;
}

static fwfs_stag_t fwfs_dir_fetchmatchraw(fwfs_t *fwfs,
        fwfs_mdir_t *dir, const fwfs_block_t pair[2],
        fwfs_tag_t fmask, fwfs_tag_t ftag, uint16_t *id,
        int (*cb)(void *data, fwfs_tag_t tag, const void *buffer),
        void *data, bool *need_compact) {
    // we can find tag very efficiently during a fetch, since we're already
    // scanning the entire directory
    fwfs_stag_t besttag = -1;
    fwfs_size_t block_size;
    fwfs_size_t tags_size;

    if (need_compact) {
        *need_compact = false;
    }

    // if either block address is invalid we return FWFS_ERR_CORRUPT here,
    // otherwise later writes to the pair could fail
    if (!fwfs_block_check(fwfs, pair[0]) || !fwfs_block_check(fwfs, pair[1])) {
        return FWFS_ERR_CORRUPT;
    }

    // find the block with the most recent revision
    uint32_t revs[2] = {0, 0};
    int r = 0;
    for (int i = 0; i < 2; i++) {
        int err = fwfs_bd_read(fwfs,
                NULL, &fwfs->rcache_pool, sizeof(revs[i]),
                pair[i], 0, &revs[i], sizeof(revs[i]));
        revs[i] = fwfs_fromle32(revs[i]);
        if (err && err != FWFS_ERR_CORRUPT) {
            return err;
        }

        if (err != FWFS_ERR_CORRUPT &&
                fwfs_scmp(revs[i], revs[(i+1)%2]) > 0) {
            r = i;
        }
    }

    dir->pair[0] = pair[(r+0)%2];
    dir->pair[1] = pair[(r+1)%2];
    dir->rev = revs[(r+0)%2];
    dir->off = 0; // nonzero = found some commits

    // now scan tags to fetch the actual dir and find possible match
    for (int i = 0; i < 2; i++) {
        fwfs_off_t off = 0;
        fwfs_tag_t ptag = 0xffffffff;

        uint16_t tempcount = 0;
        fwfs_block_t temptail[2] = {fwfs->bn, fwfs->bn};
        bool tempsplit = false;
        fwfs_stag_t tempbesttag = besttag;

        dir->rev = fwfs_tole32(dir->rev);
        uint32_t crc = fwfs_crc(0xffffffff, &dir->rev, sizeof(dir->rev));
        dir->rev = fwfs_fromle32(dir->rev);

        tags_size = 0;
        block_size = fwfs_block_size_wrapper(fwfs, dir->pair[0]);
        while (true) {
            // extract next tag
            fwfs_tag_t tag;
            bool bit31;
            fwfs_size_t tag_dsize;

            tag_dsize = fwfs_tag_dsize(ptag);
            off += tag_dsize;
            tags_size += tag_dsize;

            int err = fwfs_bd_read(fwfs,
                    NULL, &fwfs->rcache_pool, block_size,
                    dir->pair[0], off, &tag, sizeof(tag));
            if (err) {
                if (err == FWFS_ERR_CORRUPT) {
                    // can't continue?
                    dir->erased = false;
                    break;
                }
                return err;
            }
            bit31 = fwfs_frombe32(tag) >> 31;

            crc = fwfs_crc(crc, &tag, sizeof(tag));
            tag = fwfs_frombe32(tag) ^ ptag;

            // next commit not yet programmed or we're not in valid range
            if (!fwfs_tag_isvalid(tag)) {
                dir->erased = (fwfs_tag_type1(ptag) == FWFS_TYPE_CRC &&
                               dir->off % fwfs->cfg->prog_size == 0 &&
                               bit31);
                break;
            } else if (off + fwfs_tag_dsize(tag) > block_size) {
                dir->erased = false;
                break;
            }

            ptag = tag;

            if (fwfs_tag_type1(tag) == FWFS_TYPE_CRC) {
                // check the crc attr
                uint32_t dcrc;
                err = fwfs_bd_read(fwfs,
                        NULL, &fwfs->rcache_pool, block_size,
                        dir->pair[0], off+sizeof(tag), &dcrc, sizeof(dcrc));
                if (err) {
                    if (err == FWFS_ERR_CORRUPT) {
                        dir->erased = false;
                        break;
                    }
                    return err;
                }
                dcrc = fwfs_fromle32(dcrc);

                if (crc != dcrc) {
                    dir->erased = false;
                    break;
                }

                // reset the next bit if we need to
                ptag ^= (fwfs_tag_t)(fwfs_tag_chunk(tag) & 1U) << 31;

                // toss our crc into the filesystem seed for
                // pseudorandom numbers
                fwfs->seed ^= crc;

                // update with what's found so far
                besttag = tempbesttag;
                dir->off = off + fwfs_tag_dsize(tag);
                dir->etag = ptag;
                dir->count = tempcount;
                dir->tail[0] = temptail[0];
                dir->tail[1] = temptail[1];
                dir->split = tempsplit;

                // reset crc
                crc = 0xffffffff;

                // subtract the padding
                tags_size -= fwfs_tag_size(tag) - sizeof(tag);
                continue;
            }

            // crc the entry first, hopefully leaving it in the cache
            for (fwfs_off_t j = sizeof(tag); j < fwfs_tag_dsize(tag); j++) {
                uint8_t dat;
                err = fwfs_bd_read(fwfs,
                        NULL, &fwfs->rcache_pool, block_size,
                        dir->pair[0], off+j, &dat, 1);
                if (err) {
                    if (err == FWFS_ERR_CORRUPT) {
                        dir->erased = false;
                        break;
                    }
                    return err;
                }

                crc = fwfs_crc(crc, &dat, 1);
            }

            // directory modification tags?
            if (fwfs_tag_type1(tag) == FWFS_TYPE_NAME) {
                // increase count of files if necessary
                if (fwfs_tag_id(tag) >= tempcount) {
                    tempcount = fwfs_tag_id(tag) + 1;
                }
            } else if (fwfs_tag_type1(tag) == FWFS_TYPE_SPLICE) {
                tempcount += fwfs_tag_splice(tag);

                if (tag == (FWFS_MKTAG(FWFS_TYPE_DELETE, 0, 0) |
                        (FWFS_MKTAG(0, 0x3ff, 0) & tempbesttag))) {
                    tempbesttag |= 0x80000000;
                } else if (tempbesttag != -1 &&
                        fwfs_tag_id(tag) <= fwfs_tag_id(tempbesttag)) {
                    tempbesttag += FWFS_MKTAG(0, fwfs_tag_splice(tag), 0);
                }
            } else if (fwfs_tag_type1(tag) == FWFS_TYPE_TAIL) {
                tempsplit = (fwfs_tag_chunk(tag) & 1);

                err = fwfs_bd_read(fwfs,
                        NULL, &fwfs->rcache_pool, block_size,
                        dir->pair[0], off+sizeof(tag), &temptail, 8);
                if (err) {
                    if (err == FWFS_ERR_CORRUPT) {
                        dir->erased = false;
                        break;
                    }
                }
                fwfs_pair_fromle32(temptail);
            }

            // found a match for our fetcher?
            if ((fmask & tag) == (fmask & ftag)) {
                int res = cb(data, tag, &(struct fwfs_diskoff){
                        dir->pair[0], off+sizeof(tag)});
                if (res < 0) {
                    if (res == FWFS_ERR_CORRUPT) {
                        dir->erased = false;
                        break;
                    }
                    return res;
                }

                if (res == FWFS_CMP_EQ) {
                    // found a match
                    tempbesttag = tag;
                } else if ((FWFS_MKTAG(0x7ff, 0x3ff, 0) & tag) ==
                        (FWFS_MKTAG(0x7ff, 0x3ff, 0) & tempbesttag)) {
                    // found an identical tag, but contents didn't match
                    // this must mean that our besttag has been overwritten
                    tempbesttag = -1;
                } else if (res == FWFS_CMP_GT &&
                        fwfs_tag_id(tag) <= fwfs_tag_id(tempbesttag)) {
                    // found a greater match, keep track to keep things sorted
                    tempbesttag = tag | 0x80000000;
                }
            }
        }

        // consider what we have good enough
        if (dir->off > 0) {
            // synthetic move
            if (fwfs_gstate_hasmovehere(&fwfs->gdisk, dir->pair)) {
                if (fwfs_tag_id(fwfs->gdisk.tag) == fwfs_tag_id(besttag)) {
                    besttag |= 0x80000000;
                } else if (besttag != -1 &&
                        fwfs_tag_id(fwfs->gdisk.tag) < fwfs_tag_id(besttag)) {
                    besttag -= FWFS_MKTAG(0, 1, 0);
                }
            }

            // found tag? or found best id?
            if (id) {
                *id = fwfs_min(fwfs_tag_id(besttag), dir->count);
            }

            if (need_compact) {
                *need_compact = fwfs_dir_need_compact(fwfs, dir, tags_size);
            }

            if (fwfs_tag_isvalid(besttag)) {
                return besttag;
            } else if (fwfs_tag_id(besttag) < dir->count) {
                return FWFS_ERR_NOENT;
            } else {
                return 0;
            }
        }

        // failed, try the other block?
        fwfs_pair_swap(dir->pair);
        dir->rev = revs[(r+1)%2];
    }

    FWFS_ERROR("Corrupted dir pair at {0x%"PRIx32", 0x%"PRIx32"}",
            dir->pair[0], dir->pair[1]);
    return FWFS_ERR_CORRUPT;
}

static inline fwfs_stag_t fwfs_dir_fetchmatch(fwfs_t *fwfs,
        fwfs_mdir_t *dir, const fwfs_block_t pair[2],
        fwfs_tag_t fmask, fwfs_tag_t ftag, uint16_t *id,
        int (*cb)(void *data, fwfs_tag_t tag, const void *buffer),
        void *data) {
    return fwfs_dir_fetchmatchraw(fwfs, dir, pair, fmask, ftag,
                                  id, cb, data, NULL);
}

static fwfs_size_t fwfs_fs_path_dirname(fwfs_t *fwfs, const char *path,
                                        char *buffer, fwfs_size_t buflen) {
    //+1 for termination character '\0'
    char dirname[FWFS_NAME_MAX+1];
    fwfs_size_t index = 0;
    const char *name = path;
    (void)fwfs;

    if (!buffer || buflen < (FWFS_NAME_MAX + 1))
        return FWFS_ERR_INVAL;

    memset(dirname, 0, sizeof(dirname));
    while (true) {
nextname:
        // skip slashes
        name += strspn(name, "/");
        fwfs_size_t namelen = strcspn(name, "/");

        // skip '.' and root '..'
        if ((namelen == 1 && memcmp(name, ".", 1) == 0) ||
            (namelen == 2 && memcmp(name, "..", 2) == 0)) {
            name += namelen;
            goto nextname;
        }

        // skip if matched by '..' in name
        const char *suffix = name + namelen;
        fwfs_size_t sufflen;
        int depth = 1;
        while (true) {
            suffix += strspn(suffix, "/");
            sufflen = strcspn(suffix, "/");
            if (sufflen == 0) {
                break;
            }

            if (sufflen == 2 && memcmp(suffix, "..", 2) == 0) {
                depth -= 1;
                if (depth == 0) {
                    name = suffix + sufflen;
                    goto nextname;
                }
            } else {
                depth += 1;
            }

            suffix += sufflen;
        }

        if (name[0] == '\0')
            break;

        // (... + 1) to reserve space for prefix '/'
        if ((index + namelen + 1) > FWFS_NAME_MAX) {
            namelen = FWFS_NAME_MAX - (index + 1);
        }

        dirname[index] = '/';
        index += 1;

        memcpy(dirname + index, name, namelen);
        index += namelen;

        dirname[index] = '\0';

        if (index >= FWFS_NAME_MAX)
            break;

        // to next name
        name += namelen;
    }

    char *p = strrchr(dirname, '/');
    if (p && (p != dirname)) {
        *p = '\0';
        return snprintf(buffer, buflen, "%s", dirname);
    } else {
        return snprintf(buffer, buflen, "/");
    }
}

static bool fwfs_folder_compress_policy(fwfs_t *fwfs, const char *dirname) {
    fwfs_file_t file;
    int err;
    char path[FWFS_NAME_MAX+1];

    if (dirname)
        snprintf(path, sizeof(path), "%s/folder_compress_policy", dirname);
    else
        snprintf(path, sizeof(path), "folder_compress_policy");

    err = fwfs_file_open(fwfs, &file, path, FWFS_O_RDONLY);
    if (err)
        return false;

    memset(path, 0, sizeof(path));
    err = fwfs_file_read(fwfs, &file, path, sizeof(path));
    if (err <= 0) {
        fwfs_file_close(fwfs, &file);
        return false;
    }
    fwfs_file_close(fwfs, &file);

    if (strncmp(path, POLICY_CONTENT, strlen(POLICY_CONTENT)) == 0)
        return true;
    else
        return false;
}

static int fwfs_dir_fetch(fwfs_t *fwfs,
        fwfs_mdir_t *dir, const fwfs_block_t pair[2]) {
    // note, mask=-1, tag=-1 can never match a tag since this
    // pattern has the invalid bit set
    return (int)fwfs_dir_fetchmatch(fwfs, dir, pair,
            (fwfs_tag_t)-1, (fwfs_tag_t)-1, NULL, NULL, NULL);
}

static int fwfs_dir_getgstate(fwfs_t *fwfs, const fwfs_mdir_t *dir,
        fwfs_gstate_t *gstate) {
    fwfs_gstate_t temp;
    fwfs_stag_t res = fwfs_dir_get(fwfs, dir, FWFS_MKTAG(0x7ff, 0, 0),
            FWFS_MKTAG(FWFS_TYPE_MOVESTATE, 0, sizeof(temp)), &temp);
    if (res < 0 && res != FWFS_ERR_NOENT) {
        return res;
    }

    if (res != FWFS_ERR_NOENT) {
        // xor together to find resulting gstate
        fwfs_gstate_fromle32(&temp);
        fwfs_gstate_xor(gstate, &temp);
    }

    return 0;
}

static int fwfs_dir_getinfo(fwfs_t *fwfs, fwfs_mdir_t *dir,
        uint16_t id, struct fwfs_info *info) {
    if (id == 0x3ff) {
        // special case for root
        strcpy(info->name, "/");
        info->type = FWFS_TYPE_DIR;
        return 0;
    }

    fwfs_stag_t tag = fwfs_dir_get(fwfs, dir, FWFS_MKTAG(0x780, 0x3ff, 0),
            FWFS_MKTAG(FWFS_TYPE_NAME, id, fwfs->name_max+1), info->name);
    if (tag < 0) {
        return (int)tag;
    }

    info->type = fwfs_tag_type3(tag);

    struct fwfs_ctz ctz;
    tag = fwfs_dir_get(fwfs, dir, FWFS_MKTAG(0x700, 0x3ff, 0),
            FWFS_MKTAG(FWFS_TYPE_STRUCT, id, sizeof(ctz)), &ctz);
    if (tag < 0) {
        return (int)tag;
    }
    fwfs_ctz_fromle32(&ctz);

    if (fwfs_tag_type3(tag) == FWFS_TYPE_CTZSTRUCT) {
        info->size = ctz.size;
    } else if (fwfs_tag_type3(tag) == FWFS_TYPE_INLINESTRUCT) {
        info->size = fwfs_tag_size(tag);
    }

    return 0;
}

struct fwfs_dir_find_match {
    fwfs_t *fwfs;
    const void *name;
    fwfs_size_t size;
};

static int fwfs_dir_find_match(void *data,
        fwfs_tag_t tag, const void *buffer) {
    struct fwfs_dir_find_match *name = data;
    fwfs_t *fwfs = name->fwfs;
    const struct fwfs_diskoff *disk = buffer;

    // compare with disk
    fwfs_size_t diff = fwfs_min(name->size, fwfs_tag_size(tag));
    int res = fwfs_bd_cmp(fwfs,
            NULL, &fwfs->rcache_pool, diff,
            disk->block, disk->off, name->name, diff);
    if (res != FWFS_CMP_EQ) {
        return res;
    }

    // only equal if our size is still the same
    if (name->size != fwfs_tag_size(tag)) {
        return (name->size < fwfs_tag_size(tag)) ? FWFS_CMP_LT : FWFS_CMP_GT;
    }

    // found a match!
    return FWFS_CMP_EQ;
}

static fwfs_stag_t fwfs_dir_find(fwfs_t *fwfs, fwfs_mdir_t *dir,
        const char **path, uint16_t *id) {
    // we reduce path to a single name if we can find it
    const char *name = *path;
    if (id) {
        *id = 0x3ff;
    }

    // default to root dir
    fwfs_stag_t tag = FWFS_MKTAG(FWFS_TYPE_DIR, 0x3ff, 0);
    dir->tail[0] = fwfs->root[0];
    dir->tail[1] = fwfs->root[1];

    while (true) {
nextname:
        // skip slashes
        name += strspn(name, "/");
        fwfs_size_t namelen = strcspn(name, "/");

        // skip '.' and root '..'
        if ((namelen == 1 && memcmp(name, ".", 1) == 0) ||
            (namelen == 2 && memcmp(name, "..", 2) == 0)) {
            name += namelen;
            goto nextname;
        }

        // skip if matched by '..' in name
        const char *suffix = name + namelen;
        fwfs_size_t sufflen;
        int depth = 1;
        while (true) {
            suffix += strspn(suffix, "/");
            sufflen = strcspn(suffix, "/");
            if (sufflen == 0) {
                break;
            }

            if (sufflen == 2 && memcmp(suffix, "..", 2) == 0) {
                depth -= 1;
                if (depth == 0) {
                    name = suffix + sufflen;
                    goto nextname;
                }
            } else {
                depth += 1;
            }

            suffix += sufflen;
        }

        // found path
        if (name[0] == '\0') {
            return tag;
        }

        // update what we've found so far
        *path = name;

        // only continue if we hit a directory
        if (fwfs_tag_type3(tag) != FWFS_TYPE_DIR) {
            return FWFS_ERR_NOTDIR;
        }

        // grab the entry data
        if (fwfs_tag_id(tag) != 0x3ff) {
            fwfs_stag_t res = fwfs_dir_get(fwfs, dir, FWFS_MKTAG(0x700, 0x3ff, 0),
                    FWFS_MKTAG(FWFS_TYPE_STRUCT, fwfs_tag_id(tag), 8), dir->tail);
            if (res < 0) {
                return res;
            }
            fwfs_pair_fromle32(dir->tail);
        }

        // find entry matching name
        while (true) {
            tag = fwfs_dir_fetchmatch(fwfs, dir, dir->tail,
                    FWFS_MKTAG(0x780, 0, 0),
                    FWFS_MKTAG(FWFS_TYPE_NAME, 0, namelen),
                     // are we last name?
                    (strchr(name, '/') == NULL) ? id : NULL,
                    fwfs_dir_find_match, &(struct fwfs_dir_find_match){
                        fwfs, name, namelen});
            if (tag < 0) {
                return tag;
            }

            if (tag) {
                break;
            }

            if (!dir->split) {
                return FWFS_ERR_NOENT;
            }
        }

        // to next name
        name += namelen;
    }
}

// commit logic
struct fwfs_commit {
    fwfs_block_t block;
    fwfs_off_t off;
    fwfs_tag_t ptag;
    uint32_t crc;

    fwfs_off_t begin;
    fwfs_off_t end;
};

static int fwfs_dir_commitprog(fwfs_t *fwfs, struct fwfs_commit *commit,
        const void *buffer, fwfs_size_t size) {
    int err = fwfs_bd_prog(fwfs,
            &fwfs->pcache, &fwfs->rcache_pool, false,
            commit->block, commit->off ,
            (const uint8_t*)buffer, size);
    if (err) {
        return err;
    }

    commit->crc = fwfs_crc(commit->crc, buffer, size);
    commit->off += size;
    return 0;
}

static int fwfs_dir_commitattr(fwfs_t *fwfs, struct fwfs_commit *commit,
        fwfs_tag_t tag, const void *buffer) {
    // check if we fit
    fwfs_size_t dsize = fwfs_tag_dsize(tag);
    if (commit->off + dsize > commit->end) {
        return FWFS_ERR_NOSPC;
    }

    // write out tag
    fwfs_tag_t ntag = fwfs_tobe32((tag & 0x7fffffff) ^ commit->ptag);
    int err = fwfs_dir_commitprog(fwfs, commit, &ntag, sizeof(ntag));
    if (err) {
        return err;
    }

    if (!(tag & 0x80000000)) {
        // from memory
        err = fwfs_dir_commitprog(fwfs, commit, buffer, dsize-sizeof(tag));
        if (err) {
            return err;
        }
    } else {
        // from disk
        const struct fwfs_diskoff *disk = buffer;
        for (fwfs_off_t i = 0; i < dsize-sizeof(tag); i++) {
            // rely on caching to make this efficient
            uint8_t dat;
            err = fwfs_bd_read(fwfs,
                    NULL, &fwfs->rcache_pool, dsize-sizeof(tag)-i,
                    disk->block, disk->off+i, &dat, 1);
            if (err) {
                return err;
            }

            err = fwfs_dir_commitprog(fwfs, commit, &dat, 1);
            if (err) {
                return err;
            }
        }
    }

    commit->ptag = tag & 0x7fffffff;
    return 0;
}

static int fwfs_dir_commitcrc(fwfs_t *fwfs, struct fwfs_commit *commit) {
    const fwfs_off_t off1 = commit->off;
    const uint32_t crc1 = commit->crc;
    // align to program units
    const fwfs_off_t end = fwfs_alignup(off1 + 2*sizeof(uint32_t),
            fwfs->cfg->prog_size);

    // create crc tags to fill up remainder of commit, note that
    // padding is not crced, which lets fetches skip padding but
    // makes committing a bit more complicated
    while (commit->off < end) {
        fwfs_off_t off = commit->off + sizeof(fwfs_tag_t);
        fwfs_off_t noff = fwfs_min(end - off, 0x3fe) + off;
        if (noff < end) {
            noff = fwfs_min(noff, end - 2*sizeof(uint32_t));
        }

        // read erased state from next program unit
        fwfs_tag_t tag = 0xffffffff;
        int err = fwfs_bd_read(fwfs,
                NULL, &fwfs->rcache_pool, sizeof(tag),
                commit->block, noff, &tag, sizeof(tag));
        if (err && err != FWFS_ERR_CORRUPT) {
            return err;
        }

        // build crc tag
        bool reset = ~fwfs_frombe32(tag) >> 31;
        tag = FWFS_MKTAG(FWFS_TYPE_CRC + reset, 0x3ff, noff - off);

        // write out crc
        uint32_t footer[2];
        footer[0] = fwfs_tobe32(tag ^ commit->ptag);
        commit->crc = fwfs_crc(commit->crc, &footer[0], sizeof(footer[0]));
        footer[1] = fwfs_tole32(commit->crc);
        err = fwfs_bd_prog(fwfs,
                &fwfs->pcache, &fwfs->rcache_pool, false,
                commit->block, commit->off, &footer, sizeof(footer));
        if (err) {
            return err;
        }

        commit->off += sizeof(tag)+fwfs_tag_size(tag);
        commit->ptag = tag ^ ((fwfs_tag_t)reset << 31);
        commit->crc = 0xffffffff; // reset crc for next "commit"
    }

    // flush buffers
    int err = fwfs_bd_sync(fwfs, &fwfs->pcache, &fwfs->rcache_pool, false);
    if (err) {
        return err;
    }

    // successful commit, check checksums to make sure
    fwfs_off_t off = commit->begin;
    fwfs_off_t noff = off1 + sizeof(uint32_t);
    while (off < end) {
        uint32_t crc = 0xffffffff;
        for (fwfs_off_t i = off; i < noff+sizeof(uint32_t); i++) {
            // check against written crc, may catch blocks that
            // become readonly and match our commit size exactly
            if (i == off1 && crc != crc1) {
                return FWFS_ERR_CORRUPT;
            }

            // leave it up to caching to make this efficient
            uint8_t dat;
            err = fwfs_bd_read(fwfs,
                    NULL, &fwfs->rcache_pool, noff+sizeof(uint32_t)-i,
                    commit->block, i, &dat, 1);
            if (err) {
                return err;
            }

            crc = fwfs_crc(crc, &dat, 1);
        }

        // detected write error?
        if (crc != 0) {
            return FWFS_ERR_CORRUPT;
        }

        // skip padding
        off = fwfs_min(end - noff, 0x3fe) + noff;
        if (off < end) {
            off = fwfs_min(off, end - 2*sizeof(uint32_t));
        }
        noff = off + sizeof(uint32_t);
    }

    return 0;
}

static int fwfs_dir_alloc(fwfs_t *fwfs, fwfs_mdir_t *dir) {
    // allocate pair of dir blocks (backwards, so we write block 1 first)
    for (int i = 0; i < 2; i++) {
        int err = fwfs_alloc_wrapper(fwfs, &dir->pair[(i+1)%2], false);
        if (err) {
            return err;
        }
    }

    // zero for reproducability in case initial block is unreadable
    dir->rev = 0;

    // rather than clobbering one of the blocks we just pretend
    // the revision may be valid
    int err = fwfs_bd_read(fwfs,
            NULL, &fwfs->rcache_pool, sizeof(dir->rev),
            dir->pair[0], 0, &dir->rev, sizeof(dir->rev));
    dir->rev = fwfs_fromle32(dir->rev);
    if (err && err != FWFS_ERR_CORRUPT) {
        return err;
    }

    // make sure we don't immediately evict
    dir->rev += dir->rev & 1;

    // set defaults
    dir->off = sizeof(dir->rev);
    dir->etag = 0xffffffff;
    dir->count = 0;
    dir->tail[0] = fwfs->bn;
    dir->tail[1] = fwfs->bn;
    dir->erased = false;
    dir->split = false;

    // don't write out yet, let caller take care of that
    return 0;
}

static int fwfs_dir_drop(fwfs_t *fwfs, fwfs_mdir_t *dir, fwfs_mdir_t *tail) {
    // steal state
    int err = fwfs_dir_getgstate(fwfs, tail, &fwfs->gdelta);
    if (err) {
        return err;
    }

    // steal tail
    fwfs_pair_tole32(tail->tail);
    err = fwfs_dir_commit(fwfs, dir, FWFS_MKATTRS(
            {FWFS_MKTAG(FWFS_TYPE_TAIL + tail->split, 0x3ff, 8), tail->tail}));
    fwfs_pair_fromle32(tail->tail);
    if (err) {
        return err;
    }

    return 0;
}

static int fwfs_dir_split(fwfs_t *fwfs,
        fwfs_mdir_t *dir, const struct fwfs_mattr *attrs, int attrcount,
        fwfs_mdir_t *source, uint16_t split, uint16_t end) {
    // create tail directory
    fwfs_alloc_ack(fwfs);
    fwfs_mdir_t tail;
    int err = fwfs_dir_alloc(fwfs, &tail);
    if (err) {
        return err;
    }

    tail.split = dir->split;
    tail.tail[0] = dir->tail[0];
    tail.tail[1] = dir->tail[1];

    err = fwfs_dir_compact(fwfs, &tail, attrs, attrcount, source, split, end);
    if (err) {
        return err;
    }

    dir->tail[0] = tail.pair[0];
    dir->tail[1] = tail.pair[1];
    dir->split = true;

    // update root if needed
    if (fwfs_pair_cmp(dir->pair, fwfs->root) == 0 && split == 0) {
        fwfs->root[0] = tail.pair[0];
        fwfs->root[1] = tail.pair[1];
    }

    return 0;
}

static int fwfs_dir_commit_size(void *p, fwfs_tag_t tag, const void *buffer) {
    fwfs_size_t *size = p;
    (void)buffer;

    *size += fwfs_tag_dsize(tag);
    return 0;
}

struct fwfs_dir_commit_commit {
    fwfs_t *fwfs;
    struct fwfs_commit *commit;
};

static int fwfs_dir_commit_commit(void *p, fwfs_tag_t tag, const void *buffer) {
    struct fwfs_dir_commit_commit *commit = p;
    return fwfs_dir_commitattr(commit->fwfs, commit->commit, tag, buffer);
}

static int fwfs_dir_compact(fwfs_t *fwfs,
        fwfs_mdir_t *dir, const struct fwfs_mattr *attrs, int attrcount,
        fwfs_mdir_t *source, uint16_t begin, uint16_t end) {
    // save some state in case block is bad
    const fwfs_block_t oldpair[2] = {dir->pair[0], dir->pair[1]};
    bool relocated = false;
    bool tired = false;
    fwfs_size_t block_size = fwfs_block_size_wrapper(fwfs, dir->pair[0]);

    // should we split?
    while (end - begin > 1) {
        // find size
        fwfs_size_t size = 0;
        int err = fwfs_dir_traverse(fwfs,
                source, 0, 0xffffffff, attrs, attrcount,
                FWFS_MKTAG(0x400, 0x3ff, 0),
                FWFS_MKTAG(FWFS_TYPE_NAME, 0, 0),
                begin, end, -begin,
                fwfs_dir_commit_size, &size);
        if (err) {
            return err;
        }

        // space is complicated, we need room for tail, crc, gstate,
        // cleanup delete, and we cap at half a block to give room
        // for metadata updates.
        if (end - begin < 0xff &&
                size <= fwfs_min(block_size - 36,
                    fwfs_alignup(block_size/2,
                        fwfs->cfg->prog_size))) {
            break;
        }

        // can't fit, need to split, we should really be finding the
        // largest size that fits with a small binary search, but right now
        // it's not worth the code size
        uint16_t split = (end - begin) / 2;
        err = fwfs_dir_split(fwfs, dir, attrs, attrcount,
                source, begin+split, end);
        if (err) {
            // if we fail to split, we may be able to overcompact, unless
            // we're too big for even the full block, in which case our
            // only option is to error
            if (err == FWFS_ERR_NOSPC && size <= block_size - 36) {
                break;
            }
            return err;
        }

        end = begin + split;
    }

    // increment revision count
    dir->rev += 1;
    // If our revision count == n * block_cycles, we should force a relocation,
    // this is how firmwarefs wear-levels at the metadata-pair level. Note that we
    // actually use (block_cycles+1)|1, this is to avoid two corner cases:
    // 1. block_cycles = 1, which would prevent relocations from terminating
    // 2. block_cycles = 2n, which, due to aliasing, would only ever relocate
    //    one metadata block in the pair, effectively making this useless
    if (fwfs->cfg->block_cycles > 0 &&
            (dir->rev % ((fwfs->cfg->block_cycles+1)|1) == 0)) {
        if (fwfs_pair_cmp(dir->pair, (const fwfs_block_t[2]){fwfs->b0, fwfs->b1}) == 0) {
            // oh no! we're writing too much to the superblock,
            // should we expand?
            fwfs_ssize_t res = fwfs_fs_size(fwfs);
            if (res < 0) {
                return res;
            }

            // do we have extra space? firmwarefs can't reclaim this space
            // by itself, so expand cautiously
            if ((fwfs_size_t)res < fwfs->cfg->block_count/2) {
                FWFS_DEBUG("Expanding superblock at rev %"PRIu32, dir->rev);
                int err = fwfs_dir_split(fwfs, dir, attrs, attrcount,
                        source, begin, end);
                if (err && err != FWFS_ERR_NOSPC) {
                    return err;
                }

                // welp, we tried, if we ran out of space there's not much
                // we can do, we'll error later if we've become frozen
                if (!err) {
                    end = begin;
                }
            }
#ifdef FWFS_MIGRATE
        } else if (fwfs->fwfs1) {
            // do not proactively relocate blocks during migrations, this
            // can cause a number of failure states such: clobbering the
            // v1 superblock if we relocate root, and invalidating directory
            // pointers if we relocate the head of a directory. On top of
            // this, relocations increase the overall complexity of
            // fwfs_migration, which is already a delicate operation.
#endif
        } else {
            // we're writing too much, time to relocate
            tired = true;
            goto relocate;
        }
    }

    // begin loop to commit compaction to blocks until a compact sticks
    while (true) {
        {
            // setup commit state
            struct fwfs_commit commit = {
                .block = dir->pair[1],
                .off = 0,
                .ptag = 0xffffffff,
                .crc = 0xffffffff,

                .begin = 0,
                .end = block_size - 8,
            };

            // erase block to write to
            int err = fwfs_bd_erase(fwfs, dir->pair[1]);
            if (err) {
                if (err == FWFS_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            // write out header
            dir->rev = fwfs_tole32(dir->rev);
            err = fwfs_dir_commitprog(fwfs, &commit,
                    &dir->rev, sizeof(dir->rev));
            dir->rev = fwfs_fromle32(dir->rev);
            if (err) {
                if (err == FWFS_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            // traverse the directory, this time writing out all unique tags
            err = fwfs_dir_traverse(fwfs,
                    source, 0, 0xffffffff, attrs, attrcount,
                    FWFS_MKTAG(0x400, 0x3ff, 0),
                    FWFS_MKTAG(FWFS_TYPE_NAME, 0, 0),
                    begin, end, -begin,
                    fwfs_dir_commit_commit, &(struct fwfs_dir_commit_commit){
                        fwfs, &commit});
            if (err) {
                if (err == FWFS_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            // commit tail, which may be new after last size check
            if (!fwfs_pair_isnull(fwfs, dir->tail)) {
                fwfs_pair_tole32(dir->tail);
                err = fwfs_dir_commitattr(fwfs, &commit,
                        FWFS_MKTAG(FWFS_TYPE_TAIL + dir->split, 0x3ff, 8),
                        dir->tail);
                fwfs_pair_fromle32(dir->tail);
                if (err) {
                    if (err == FWFS_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }
            }

            // bring over gstate?
            fwfs_gstate_t delta = {0};
            if (!relocated) {
                fwfs_gstate_xor(&delta, &fwfs->gdisk);
                fwfs_gstate_xor(&delta, &fwfs->gstate);
            }
            fwfs_gstate_xor(&delta, &fwfs->gdelta);
            delta.tag &= ~FWFS_MKTAG(0, 0, 0x3ff);

            err = fwfs_dir_getgstate(fwfs, dir, &delta);
            if (err) {
                return err;
            }

            if (!fwfs_gstate_iszero(&delta)) {
                fwfs_gstate_tole32(&delta);
                err = fwfs_dir_commitattr(fwfs, &commit,
                        FWFS_MKTAG(FWFS_TYPE_MOVESTATE, 0x3ff,
                            sizeof(delta)), &delta);
                if (err) {
                    if (err == FWFS_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }
            }

            // complete commit with crc
            err = fwfs_dir_commitcrc(fwfs, &commit);
            if (err) {
                if (err == FWFS_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            // successful compaction, swap dir pair to indicate most recent
            FWFS_ASSERT(commit.off % fwfs->cfg->prog_size == 0);
            fwfs_pair_swap(dir->pair);
            dir->count = end - begin;
            dir->off = commit.off;
            dir->etag = commit.ptag;
            dir->erased = dir->off % fwfs->cfg->prog_size == 0;
            // update gstate
            fwfs->gdelta = (fwfs_gstate_t){0};
            if (!relocated) {
                fwfs->gdisk = fwfs->gstate;
            }
        }
        break;

relocate:
        // commit was corrupted, drop caches and prepare to relocate block
        relocated = true;
        fwfs_cache_drop(fwfs, &fwfs->pcache);
        if (!tired) {
            FWFS_DEBUG("Bad block at 0x%"PRIx32, dir->pair[1]);
        }

        // can't relocate superblock, filesystem is now frozen
        if (fwfs_pair_cmp(dir->pair, (const fwfs_block_t[2]){fwfs->b0, fwfs->b1}) == 0) {
            FWFS_WARN("Superblock 0x%"PRIx32" has become unwritable",
                    dir->pair[1]);
            return FWFS_ERR_NOSPC;
        }

        // relocate half of pair
        int err = fwfs_alloc_wrapper(fwfs, &dir->pair[1], false);
        if (err && (err != FWFS_ERR_NOSPC || !tired)) {
            return err;
        }

        tired = false;
        continue;
    }

    if (relocated) {
        // update references if we relocated
        FWFS_DEBUG("Relocating {0x%"PRIx32", 0x%"PRIx32"} "
                    "-> {0x%"PRIx32", 0x%"PRIx32"}",
                oldpair[0], oldpair[1], dir->pair[0], dir->pair[1]);
        int err = fwfs_fs_relocate(fwfs, oldpair, dir->pair);
        if (err) {
            return err;
        }
    }

    return 0;
}

static bool fwfs_force_compact(fwfs_t *fwfs) {
    return fwfs->cfg->compact == FWFS_COMPACT_MAGIC;
}

static int fwfs_dir_commit(fwfs_t *fwfs, fwfs_mdir_t *dir,
        const struct fwfs_mattr *attrs, int attrcount) {
    // check for any inline files that aren't RAM backed and
    // forcefully evict them, needed for filesystem consistency
    for (fwfs_file_t *f = (fwfs_file_t*)fwfs->mlist; f; f = f->next) {
        if (dir != &f->m && fwfs_pair_cmp(f->m.pair, dir->pair) == 0 &&
                f->type == FWFS_TYPE_REG && (f->flags & FWFS_F_INLINE) &&
                f->ctz.size > fwfs->pcache.cache_size) {
            int err = fwfs_file_outline(fwfs, f);
            if (err) {
                return err;
            }

            err = fwfs_file_flush(fwfs, f);
            if (err) {
                return err;
            }
        }
    }

    // calculate changes to the directory
    fwfs_mdir_t olddir = *dir;
    bool hasdelete = false;
    for (int i = 0; i < attrcount; i++) {
        if (fwfs_tag_type3(attrs[i].tag) == FWFS_TYPE_CREATE) {
            dir->count += 1;
        } else if (fwfs_tag_type3(attrs[i].tag) == FWFS_TYPE_DELETE) {
            FWFS_ASSERT(dir->count > 0);
            dir->count -= 1;
            hasdelete = true;
        } else if (fwfs_tag_type1(attrs[i].tag) == FWFS_TYPE_TAIL) {
            dir->tail[0] = ((fwfs_block_t*)attrs[i].buffer)[0];
            dir->tail[1] = ((fwfs_block_t*)attrs[i].buffer)[1];
            dir->split = (fwfs_tag_chunk(attrs[i].tag) & 1);
            fwfs_pair_fromle32(dir->tail);
        }
    }

    // should we actually drop the directory block?
    if (hasdelete && dir->count == 0) {
        fwfs_mdir_t pdir;
        char dirname[FWFS_NAME_MAX+1];
        int err = fwfs_fs_pred(fwfs, dir->pair, &pdir);
        if (err && err != FWFS_ERR_NOENT) {
            *dir = olddir;
            return err;
        }

        err = fwfs_fs_mdir_dirname(fwfs, pdir.pair, dirname,
                                  sizeof(dirname));
        if (err <= 0) {
            return err;
        }
        pdir.force_compact = fwfs_folder_compress_policy(fwfs,dirname);

        if (err != FWFS_ERR_NOENT && pdir.split) {
            err = fwfs_dir_drop(fwfs, &pdir, dir);
            if (err) {
                *dir = olddir;
                return err;
            }
        }
    }

    if (!(dir->force_compact || fwfs_force_compact(fwfs)) &&
        (dir->erased || dir->count >= 0xff)) {
        fwfs_size_t block_size = fwfs_block_size_wrapper(fwfs, dir->pair[0]);

        // try to commit
        struct fwfs_commit commit = {
            .block = dir->pair[0],
            .off = dir->off,
            .ptag = dir->etag,
            .crc = 0xffffffff,

            .begin = dir->off,
            .end = block_size - 8,
        };

        // traverse attrs that need to be written out
        fwfs_pair_tole32(dir->tail);
        int err = fwfs_dir_traverse(fwfs,
                dir, dir->off, dir->etag, attrs, attrcount,
                0, 0, 0, 0, 0,
                fwfs_dir_commit_commit, &(struct fwfs_dir_commit_commit){
                    fwfs, &commit});
        fwfs_pair_fromle32(dir->tail);
        if (err) {
            if (err == FWFS_ERR_NOSPC || err == FWFS_ERR_CORRUPT) {
                goto compact;
            }
            *dir = olddir;
            return err;
        }

        // commit any global diffs if we have any
        fwfs_gstate_t delta = {0};
        fwfs_gstate_xor(&delta, &fwfs->gstate);
        fwfs_gstate_xor(&delta, &fwfs->gdisk);
        fwfs_gstate_xor(&delta, &fwfs->gdelta);
        delta.tag &= ~FWFS_MKTAG(0, 0, 0x3ff);
        if (!fwfs_gstate_iszero(&delta)) {
            err = fwfs_dir_getgstate(fwfs, dir, &delta);
            if (err) {
                *dir = olddir;
                return err;
            }

            fwfs_gstate_tole32(&delta);
            err = fwfs_dir_commitattr(fwfs, &commit,
                    FWFS_MKTAG(FWFS_TYPE_MOVESTATE, 0x3ff,
                        sizeof(delta)), &delta);
            if (err) {
                if (err == FWFS_ERR_NOSPC || err == FWFS_ERR_CORRUPT) {
                    goto compact;
                }
                *dir = olddir;
                return err;
            }
        }

        // finalize commit with the crc
        err = fwfs_dir_commitcrc(fwfs, &commit);
        if (err) {
            if (err == FWFS_ERR_NOSPC || err == FWFS_ERR_CORRUPT) {
                goto compact;
            }
            *dir = olddir;
            return err;
        }

        // successful commit, update dir
        FWFS_ASSERT(commit.off % fwfs->cfg->prog_size == 0);
        dir->off = commit.off;
        dir->etag = commit.ptag;
        // and update gstate
        fwfs->gdisk = fwfs->gstate;
        fwfs->gdelta = (fwfs_gstate_t){0};
    } else {
compact:
        // fall back to compaction
        fwfs_cache_drop(fwfs, &fwfs->pcache);

        int err = fwfs_dir_compact(fwfs, dir, attrs, attrcount,
                dir, 0, dir->count);
        if (err) {
            *dir = olddir;
            return err;
        }
    }

    // this complicated bit of logic is for fixing up any active
    // metadata-pairs that we may have affected
    //
    // note we have to make two passes since the mdir passed to
    // fwfs_dir_commit could also be in this list, and even then
    // we need to copy the pair so they don't get clobbered if we refetch
    // our mdir.
    for (struct fwfs_mlist *d = fwfs->mlist; d; d = d->next) {
        if (&d->m != dir && fwfs_pair_cmp(d->m.pair, olddir.pair) == 0) {
            d->m = *dir;
            for (int i = 0; i < attrcount; i++) {
                if (fwfs_tag_type3(attrs[i].tag) == FWFS_TYPE_DELETE &&
                        d->id == fwfs_tag_id(attrs[i].tag)) {
                    d->m.pair[0] = fwfs->bn;
                    d->m.pair[1] = fwfs->bn;
                } else if (fwfs_tag_type3(attrs[i].tag) == FWFS_TYPE_DELETE &&
                        d->id > fwfs_tag_id(attrs[i].tag)) {
                    d->id -= 1;
                    if (d->type == FWFS_TYPE_DIR) {
                        ((fwfs_dir_t*)d)->pos -= 1;
                    }
                } else if (fwfs_tag_type3(attrs[i].tag) == FWFS_TYPE_CREATE &&
                        d->id >= fwfs_tag_id(attrs[i].tag)) {
                    d->id += 1;
                    if (d->type == FWFS_TYPE_DIR) {
                        ((fwfs_dir_t*)d)->pos += 1;
                    }
                }
            }
        }
    }

    for (struct fwfs_mlist *d = fwfs->mlist; d; d = d->next) {
        if (fwfs_pair_cmp(d->m.pair, olddir.pair) == 0) {
            while (d->id >= d->m.count && d->m.split) {
                // we split and id is on tail now
                d->id -= d->m.count;
                int err = fwfs_dir_fetch(fwfs, &d->m, d->m.tail);
                if (err) {
                    return err;
                }
            }
        }
    }

    return 0;
}


/// Top level directory operations ///
int fwfs_mkdir(fwfs_t *fwfs, const char *path) {
    FWFS_TRACE("fwfs_mkdir(%p, \"%s\")", (void*)fwfs, path);
    // deorphan if we haven't yet, needed at most once after poweron
    int err = fwfs_fs_forceconsistency(fwfs);
    if (err) {
        FWFS_TRACE("fwfs_mkdir -> %d", err);
        return err;
    }
    char dirname[FWFS_NAME_MAX+1];
    bool force_compact = false;
    err = fwfs_fs_path_dirname(fwfs, path, dirname, sizeof(dirname));
    if (err <= 0) {
        FWFS_TRACE("fwfs_mkdir -> %d", err);
        return err;
    }
    force_compact = fwfs_folder_compress_policy(fwfs, dirname);

    struct fwfs_mlist cwd;
    cwd.next = fwfs->mlist;
    uint16_t id;
    err = fwfs_dir_find(fwfs, &cwd.m, &path, &id);
    if (!(err == FWFS_ERR_NOENT && id != 0x3ff)) {
        FWFS_TRACE("fwfs_mkdir -> %d", (err < 0) ? err : FWFS_ERR_EXIST);
        return (err < 0) ? err : FWFS_ERR_EXIST;
    }
    cwd.m.force_compact = force_compact;

    // check that name fits
    fwfs_size_t nlen = strlen(path);
    if (nlen > fwfs->name_max) {
        FWFS_TRACE("fwfs_mkdir -> %d", FWFS_ERR_NAMETOOLONG);
        return FWFS_ERR_NAMETOOLONG;
    }

    // build up new directory
    fwfs_alloc_ack(fwfs);
    fwfs_mdir_t dir;
    err = fwfs_dir_alloc(fwfs, &dir);
    if (err) {
        FWFS_TRACE("fwfs_mkdir -> %d", err);
        return err;
    }

    // find end of list
    fwfs_mdir_t pred = cwd.m;
    while (pred.split) {
        err = fwfs_dir_fetch(fwfs, &pred, pred.tail);
        if (err) {
            FWFS_TRACE("fwfs_mkdir -> %d", err);
            return err;
        }
    }

    // setup dir
    fwfs_pair_tole32(pred.tail);
    err = fwfs_dir_commit(fwfs, &dir, FWFS_MKATTRS(
            {FWFS_MKTAG(FWFS_TYPE_SOFTTAIL, 0x3ff, 8), pred.tail}));
    fwfs_pair_fromle32(pred.tail);
    if (err) {
        FWFS_TRACE("fwfs_mkdir -> %d", err);
        return err;
    }

    // current block end of list?
    if (cwd.m.split) {
        // update tails, this creates a desync
        fwfs_fs_preporphans(fwfs, +1);

        // it's possible our predecessor has to be relocated, and if
        // our parent is our predecessor's predecessor, this could have
        // caused our parent to go out of date, fortunately we can hook
        // ourselves into firmwarefs to catch this
        cwd.type = 0;
        cwd.id = 0;
        fwfs->mlist = &cwd;

        fwfs_pair_tole32(dir.pair);
        err = fwfs_dir_commit(fwfs, &pred, FWFS_MKATTRS(
                {FWFS_MKTAG(FWFS_TYPE_SOFTTAIL, 0x3ff, 8), dir.pair}));
        fwfs_pair_fromle32(dir.pair);
        if (err) {
            fwfs->mlist = cwd.next;
            FWFS_TRACE("fwfs_mkdir -> %d", err);
            return err;
        }

        fwfs->mlist = cwd.next;
        fwfs_fs_preporphans(fwfs, -1);
    }

    // now insert into our parent block
    fwfs_pair_tole32(dir.pair);
    err = fwfs_dir_commit(fwfs, &cwd.m, FWFS_MKATTRS(
            {FWFS_MKTAG(FWFS_TYPE_CREATE, id, 0), NULL},
            {FWFS_MKTAG(FWFS_TYPE_DIR, id, nlen), path},
            {FWFS_MKTAG(FWFS_TYPE_DIRSTRUCT, id, 8), dir.pair},
            {FWFS_MKTAG_IF(!cwd.m.split,
                FWFS_TYPE_SOFTTAIL, 0x3ff, 8), dir.pair}));
    fwfs_pair_fromle32(dir.pair);
    if (err) {
        FWFS_TRACE("fwfs_mkdir -> %d", err);
        return err;
    }

    FWFS_TRACE("fwfs_mkdir -> %d", 0);
    return 0;
}

int fwfs_dir_open(fwfs_t *fwfs, fwfs_dir_t *dir, const char *path) {
    FWFS_TRACE("fwfs_dir_open(%p, %p, \"%s\")", (void*)fwfs, (void*)dir, path);
    fwfs_stag_t tag = fwfs_dir_find(fwfs, &dir->m, &path, NULL);
    if (tag < 0) {
        FWFS_TRACE("fwfs_dir_open -> %"PRId32, tag);
        return tag;
    }

    if (fwfs_tag_type3(tag) != FWFS_TYPE_DIR) {
        FWFS_TRACE("fwfs_dir_open -> %d", FWFS_ERR_NOTDIR);
        return FWFS_ERR_NOTDIR;
    }

    fwfs_block_t pair[2];
    if (fwfs_tag_id(tag) == 0x3ff) {
        // handle root dir separately
        pair[0] = fwfs->root[0];
        pair[1] = fwfs->root[1];
    } else {
        // get dir pair from parent
        fwfs_stag_t res = fwfs_dir_get(fwfs, &dir->m, FWFS_MKTAG(0x700, 0x3ff, 0),
                FWFS_MKTAG(FWFS_TYPE_STRUCT, fwfs_tag_id(tag), 8), pair);
        if (res < 0) {
            FWFS_TRACE("fwfs_dir_open -> %"PRId32, res);
            return res;
        }
        fwfs_pair_fromle32(pair);
    }

    // fetch first pair
    int err = fwfs_dir_fetch(fwfs, &dir->m, pair);
    if (err) {
        FWFS_TRACE("fwfs_dir_open -> %d", err);
        return err;
    }

    // setup entry
    dir->head[0] = dir->m.pair[0];
    dir->head[1] = dir->m.pair[1];
    dir->id = 0;
    dir->pos = 0;

    // add to list of mdirs
    dir->type = FWFS_TYPE_DIR;
    dir->next = (fwfs_dir_t*)fwfs->mlist;
    fwfs->mlist = (struct fwfs_mlist*)dir;

    FWFS_TRACE("fwfs_dir_open -> %d", 0);
    return 0;
}

int fwfs_dir_close(fwfs_t *fwfs, fwfs_dir_t *dir) {
    FWFS_TRACE("fwfs_dir_close(%p, %p)", (void*)fwfs, (void*)dir);
    // remove from list of mdirs
    for (struct fwfs_mlist **p = &fwfs->mlist; *p; p = &(*p)->next) {
        if (*p == (struct fwfs_mlist*)dir) {
            *p = (*p)->next;
            break;
        }
    }

    FWFS_TRACE("fwfs_dir_close -> %d", 0);
    return 0;
}

int fwfs_dir_read(fwfs_t *fwfs, fwfs_dir_t *dir, struct fwfs_info *info) {
    FWFS_TRACE("fwfs_dir_read(%p, %p, %p)",
            (void*)fwfs, (void*)dir, (void*)info);
    memset(info, 0, sizeof(*info));

    // special offset for '.' and '..'
    if (dir->pos == 0) {
        info->type = FWFS_TYPE_DIR;
        strcpy(info->name, ".");
        dir->pos += 1;
        FWFS_TRACE("fwfs_dir_read -> %d", true);
        return true;
    } else if (dir->pos == 1) {
        info->type = FWFS_TYPE_DIR;
        strcpy(info->name, "..");
        dir->pos += 1;
        FWFS_TRACE("fwfs_dir_read -> %d", true);
        return true;
    }

    while (true) {
        if (dir->id == dir->m.count) {
            if (!dir->m.split) {
                FWFS_TRACE("fwfs_dir_read -> %d", false);
                return false;
            }

            int err = fwfs_dir_fetch(fwfs, &dir->m, dir->m.tail);
            if (err) {
                FWFS_TRACE("fwfs_dir_read -> %d", err);
                return err;
            }

            dir->id = 0;
        }

        int err = fwfs_dir_getinfo(fwfs, &dir->m, dir->id, info);
        if (err && err != FWFS_ERR_NOENT) {
            FWFS_TRACE("fwfs_dir_read -> %d", err);
            return err;
        }

        dir->id += 1;
        if (err != FWFS_ERR_NOENT) {
            break;
        }
    }

    dir->pos += 1;
    FWFS_TRACE("fwfs_dir_read -> %d", true);
    return true;
}

int fwfs_dir_seek(fwfs_t *fwfs, fwfs_dir_t *dir, fwfs_off_t off) {
    FWFS_TRACE("fwfs_dir_seek(%p, %p, %"PRIu32")",
            (void*)fwfs, (void*)dir, off);
    // simply walk from head dir
    int err = fwfs_dir_rewind(fwfs, dir);
    if (err) {
        FWFS_TRACE("fwfs_dir_seek -> %d", err);
        return err;
    }

    // first two for ./..
    dir->pos = fwfs_min(2, off);
    off -= dir->pos;

    // skip superblock entry
    dir->id = (off > 0 && fwfs_pair_cmp(dir->head, fwfs->root) == 0);

    while (off > 0) {
        int diff = fwfs_min(dir->m.count - dir->id, off);
        dir->id += diff;
        dir->pos += diff;
        off -= diff;

        if (dir->id == dir->m.count) {
            if (!dir->m.split) {
                FWFS_TRACE("fwfs_dir_seek -> %d", FWFS_ERR_INVAL);
                return FWFS_ERR_INVAL;
            }

            err = fwfs_dir_fetch(fwfs, &dir->m, dir->m.tail);
            if (err) {
                FWFS_TRACE("fwfs_dir_seek -> %d", err);
                return err;
            }

            dir->id = 0;
        }
    }

    FWFS_TRACE("fwfs_dir_seek -> %d", 0);
    return 0;
}

fwfs_soff_t fwfs_dir_tell(fwfs_t *fwfs, fwfs_dir_t *dir) {
    FWFS_TRACE("fwfs_dir_tell(%p, %p)", (void*)fwfs, (void*)dir);
    (void)fwfs;
    FWFS_TRACE("fwfs_dir_tell -> %"PRId32, dir->pos);
    return dir->pos;
}

int fwfs_dir_rewind(fwfs_t *fwfs, fwfs_dir_t *dir) {
    FWFS_TRACE("fwfs_dir_rewind(%p, %p)", (void*)fwfs, (void*)dir);
    // reload the head dir
    int err = fwfs_dir_fetch(fwfs, &dir->m, dir->head);
    if (err) {
        FWFS_TRACE("fwfs_dir_rewind -> %d", err);
        return err;
    }

    dir->id = 0;
    dir->pos = 0;
    FWFS_TRACE("fwfs_dir_rewind -> %d", 0);
    return 0;
}


/// File index list operations ///
static int fwfs_ctz_index(fwfs_t *fwfs, fwfs_off_t *off) {
    fwfs_off_t size = *off;
    fwfs_size_t block_size = fwfs_tftl_isenabled(fwfs->cfg) ?
        fwfs->cfg->subblock_size - fwfs->tftl.header_size: fwfs->cfg->block_size;
    fwfs_off_t b = block_size - 2*4;
    fwfs_off_t i = size / b;
    if (i == 0) {
        *off += fwfs->tftl.header_size;
        return 0;
    }

    i = (size - 4*(fwfs_popc(i-1)+2)) / b;
    *off = size - b*i - 4*fwfs_popc(i);
    *off += fwfs->tftl.header_size;
    return i;
}

static int fwfs_ctz_find(fwfs_t *fwfs,
        const fwfs_cache_t *pcache, fwfs_cache_t *rcache,
        fwfs_block_t head, fwfs_size_t size,
        fwfs_size_t pos, fwfs_block_t *block, fwfs_off_t *off) {
    if (size == 0) {
        *block = fwfs->bn;
        *off = 0;
        return 0;
    }

    fwfs_off_t current = fwfs_ctz_index(fwfs, &(fwfs_off_t){size-1});
    fwfs_off_t target = fwfs_ctz_index(fwfs, &pos);

    while (current > target) {
        fwfs_size_t skip = fwfs_min(
                fwfs_npw2(current-target+1) - 1,
                fwfs_ctz(current));


        int err = fwfs_bd_read_nopool(fwfs,
                pcache, rcache, sizeof(head),
                head, 4*skip + fwfs->tftl.header_size, &head, sizeof(head));
        head = fwfs_fromle32(head);
        if (err) {
            return err;
        }

        current -= 1 << skip;
    }

    *block = head;
    *off = pos;
    return 0;
}

static int fwfs_ctz_extend(fwfs_t *fwfs,
        fwfs_cache_t *pcache, fwfs_cache_t *rcache,
        fwfs_block_t head, fwfs_size_t size, uint32_t fid,
        fwfs_block_t *block, fwfs_off_t *off) {
    FWFS_TRACE("fwfs_ctz_extend(%p, %p, %p, "
               "%"PRIu32", %"PRIu32", %p, %p)",
               (void*)fwfs, (void*)pcache, (void*)rcache,
               head, size,
               (void*)block, (void*)off);
    fwfs_size_t block_size = fwfs_tftl_isenabled(fwfs->cfg) ?
        fwfs->cfg->subblock_size : fwfs->cfg->block_size;

    while (true) {
        // go ahead and grab a block
        fwfs_block_t nblock;
        int err = fwfs_alloc_wrapper(fwfs, &nblock, true);
        if (err) {
            return err;
        }

        {
            err = fwfs_bd_erase(fwfs, nblock);
            if (err) {
                if (err == FWFS_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            if (size == 0) {
                if (fwfs_tftl_isenabled(fwfs->cfg)) {
                    fwfs_tftl_header_t header = {0, };

                    fwfs_tftl_header_fill(fwfs, &header, nblock, 0, fid);
                    fwfs_bd_prog_nopool(fwfs, pcache, rcache, true,
                                        nblock, 0, &header, sizeof(header));
                }

                *block = nblock;
                *off = 0 + fwfs->tftl.header_size;;
                return 0;
            }

            fwfs_size_t noff = size - 1;
            fwfs_off_t index = fwfs_ctz_index(fwfs, &noff);
            noff = noff + 1;

            // just copy out the last block if it is incomplete
            if (noff != block_size) {
                for (fwfs_off_t i = 0; i < noff; i++) {
                    uint8_t data;
                    err = fwfs_bd_read_nopool(fwfs,
                            NULL, rcache, noff-i,
                            head, i, &data, 1);
                    if (err) {
                        return err;
                    }

                    err = fwfs_bd_prog_nopool(fwfs,
                            pcache, rcache, true,
                            nblock, i, &data, 1);
                    if (err) {
                        if (err == FWFS_ERR_CORRUPT) {
                            goto relocate;
                        }
                        return err;
                    }
                }

                *block = nblock;
                *off = noff;
                return 0;
            }

            // append block
            index += 1;
            fwfs_size_t skips = fwfs_ctz(index) + 1;
            fwfs_block_t nhead = head;
            fwfs_block_t ohead __attribute__((unused)) = nhead;

            if (fwfs_tftl_isenabled(fwfs->cfg)) {
                fwfs_tftl_header_t header = {0, };

                fwfs_tftl_header_fill(fwfs, &header, nblock, index, fid);
                fwfs_bd_prog_nopool(fwfs, pcache, rcache, true,
                                    nblock, 0, &header, sizeof(header));
            }

            for (fwfs_off_t i = 0; i < skips; i++) {
                nhead = fwfs_tole32(nhead);
                FWFS_TRACE("CTZ prog %"PRIu32":block=0x%"PRIx32", off=0x%"PRIx32", "
                           "nhead=0x%"PRIx32"",
                           i, nblock, 4*i + fwfs->tftl.header_size, nhead);
                err = fwfs_bd_prog_nopool(fwfs, pcache, rcache, true,
                        nblock, 4*i + fwfs->tftl.header_size, &nhead, 4);
                nhead = fwfs_fromle32(nhead);
                if (err) {
                    if (err == FWFS_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }

                if (i != skips-1) {
                    ohead = nhead;
                    err = fwfs_bd_read_nopool(fwfs,
                            NULL, rcache, sizeof(nhead),
                            nhead, 4*i + fwfs->tftl.header_size, &nhead,
                            sizeof(nhead));
                    nhead = fwfs_fromle32(nhead);
                    FWFS_TRACE("CTZ read %"PRIu32":block=0x%"PRIx32", off=0x%"PRIx32", "
                               "nhead=0x%"PRIx32"",
                               i, ohead, 4*i + fwfs->tftl.header_size, nhead);
                    if (err) {
                        return err;
                    }
                }
            }

            *block = nblock;
            *off = 4*skips + fwfs->tftl.header_size;
            return 0;
        }

relocate:
        FWFS_DEBUG("Bad block at 0x%"PRIx32, nblock);

        // just clear cache and try a new block
        fwfs_cache_drop(fwfs, pcache);
    }
}

static int fwfs_ctz_traverse(fwfs_t *fwfs,
        const fwfs_cache_t *pcache, fwfs_cache_pool_t *rcache_pool,
        fwfs_block_t head, fwfs_size_t size, uint32_t fid,
        int (*cb)(void*, fwfs_block_t, fwfs_off_t, uint32_t), void *data) {
    if (size == 0) {
        return 0;
    }

    fwfs_off_t index = fwfs_ctz_index(fwfs, &(fwfs_off_t){size-1});
    while (true) {
        int err = cb(data, head, index, fid);
        if (err) {
            return err;
        }

        if (index == 0) {
            return 0;
        }

        fwfs_block_t heads[2];
        int count = 2 - (index & 1);
        err = fwfs_bd_read(fwfs,
                pcache, rcache_pool, count*sizeof(head),
                head, 0 + fwfs->tftl.header_size, &heads, count*sizeof(head));
        heads[0] = fwfs_fromle32(heads[0]);
        heads[1] = fwfs_fromle32(heads[1]);
        if (err) {
            return err;
        }

        for (int i = 0; i < count-1; i++) {
            err = cb(data, heads[i], index - 1, fid);
            if (err) {
                return err;
            }
        }

        head = heads[count-1];
        index -= count;
    }
}


/// Top level file operations ///
int fwfs_file_opencfg(fwfs_t *fwfs, fwfs_file_t *file,
        const char *path, int flags,
        const struct fwfs_file_config *cfg) {
    FWFS_TRACE("fwfs_file_opencfg(%p, %p, \"%s\", %x, %p {"
                 ".buffer=%p, .attrs=%p, .attr_count=%"PRIu32"})",
            (void*)fwfs, (void*)file, path, flags,
            (void*)cfg, cfg->buffer, (void*)cfg->attrs, cfg->attr_count);
    bool force_compact = false;

    // deorphan if we haven't yet, needed at most once after poweron
    if ((flags & 3) != FWFS_O_RDONLY) {
        char dirname[FWFS_NAME_MAX+1];
        int err = fwfs_fs_forceconsistency(fwfs);
        if (err) {
            FWFS_TRACE("fwfs_file_opencfg -> %d", err);
            return err;
        }
        err = fwfs_fs_path_dirname(fwfs, path, dirname, sizeof(dirname));
        if (err <= 0) {
            FWFS_TRACE("fwfs_file_opencfg -> %d", err);
            return err;
        }
        force_compact = fwfs_folder_compress_policy(fwfs, dirname);
    }

    // setup simple file details
    int err;
    file->cfg = cfg;
    file->flags = flags | FWFS_F_OPENED;
    file->pos = 0;
    file->off = 0;
    file->cache.buffer = NULL;

    // allocate entry for file if it doesn't exist
    fwfs_stag_t tag = fwfs_dir_find(fwfs, &file->m, &path, &file->id);
    if (tag < 0 && !(tag == FWFS_ERR_NOENT && file->id != 0x3ff)) {
        err = tag;
        goto cleanup;
    }
    file->m.force_compact = force_compact;

    // get id, add to list of mdirs to catch update changes
    file->type = FWFS_TYPE_REG;
    file->next = (fwfs_file_t*)fwfs->mlist;
    fwfs->mlist = (struct fwfs_mlist*)file;

    if (tag == FWFS_ERR_NOENT) {
        if (!(flags & FWFS_O_CREAT)) {
            err = FWFS_ERR_NOENT;
            goto cleanup;
        }

        // check that name fits
        fwfs_size_t nlen = strlen(path);
        if (nlen > fwfs->name_max) {
            err = FWFS_ERR_NAMETOOLONG;
            goto cleanup;
        }

        // get next slot and create entry to remember name
        err = fwfs_dir_commit(fwfs, &file->m, FWFS_MKATTRS(
                {FWFS_MKTAG(FWFS_TYPE_CREATE, file->id, 0), NULL},
                {FWFS_MKTAG(FWFS_TYPE_REG, file->id, nlen), path},
                {FWFS_MKTAG(FWFS_TYPE_INLINESTRUCT, file->id, 0), NULL}));
        if (err) {
            err = FWFS_ERR_NAMETOOLONG;
            goto cleanup;
        }

        tag = FWFS_MKTAG(FWFS_TYPE_INLINESTRUCT, 0, 0);
        file->ctz.fid = fwfs_get_fid(fwfs);
        FWFS_TRACE("fwfs_file_opencfg assigns fid 0x%"PRIx32" to %s\n",
                   file->ctz.fid, path);
    } else if (flags & FWFS_O_EXCL) {
        err = FWFS_ERR_EXIST;
        goto cleanup;
    } else if (fwfs_tag_type3(tag) != FWFS_TYPE_REG) {
        err = FWFS_ERR_ISDIR;
        goto cleanup;
    } else if (flags & FWFS_O_TRUNC) {
        // truncate if requested
        tag = FWFS_MKTAG(FWFS_TYPE_INLINESTRUCT, file->id, 0);
        file->flags |= FWFS_F_DIRTY;
    } else {
        // try to load what's on disk, if it's inlined we'll fix it later
        tag = fwfs_dir_get(fwfs, &file->m, FWFS_MKTAG(0x700, 0x3ff, 0),
                FWFS_MKTAG(FWFS_TYPE_STRUCT, file->id, sizeof(file->ctz)), &file->ctz);
        if (tag < 0) {
            err = tag;
            goto cleanup;
        }
        fwfs_ctz_fromle32(&file->ctz);

        if (fwfs_tag_type3(tag) == FWFS_TYPE_INLINESTRUCT) {
            file->ctz.fid = fwfs_get_fid(fwfs);
        }
    }

    // fetch attrs
    for (unsigned i = 0; i < file->cfg->attr_count; i++) {
        if ((file->flags & 3) != FWFS_O_WRONLY) {
            fwfs_stag_t res = fwfs_dir_get(fwfs, &file->m,
                    FWFS_MKTAG(0x7ff, 0x3ff, 0),
                    FWFS_MKTAG(FWFS_TYPE_USERATTR + file->cfg->attrs[i].type,
                        file->id, file->cfg->attrs[i].size),
                        file->cfg->attrs[i].buffer);
            if (res < 0 && res != FWFS_ERR_NOENT) {
                err = res;
                goto cleanup;
            }
        }

        if ((file->flags & 3) != FWFS_O_RDONLY) {
            if (file->cfg->attrs[i].size > fwfs->attr_max) {
                err = FWFS_ERR_NOSPC;
                goto cleanup;
            }

            file->flags |= FWFS_F_DIRTY;
        }
    }

    // allocate buffer if needed
    file->cache.cache_size = fwfs->cfg->file_cache_size;
    if (file->cfg->buffer) {
        file->cache.buffer = file->cfg->buffer;
    } else {
        file->cache.buffer = fwfs_malloc(file->cache.cache_size);
        if (!file->cache.buffer) {
            err = FWFS_ERR_NOMEM;
            goto cleanup;
        }
    }

    // zero to avoid information leak
    fwfs_cache_zero(fwfs, &file->cache);

    if (fwfs_tag_type3(tag) == FWFS_TYPE_INLINESTRUCT) {
        // load inline files
        file->ctz.head = fwfs->bi;
        file->ctz.size = fwfs_tag_size(tag);
        file->flags |= FWFS_F_INLINE;
        file->cache.block = file->ctz.head;
        file->cache.off = 0;
        file->cache.size = fwfs->cfg->file_cache_size;

        // don't always read (may be new/trunc file)
        if (file->ctz.size > 0) {
            fwfs_stag_t res = fwfs_dir_get(fwfs, &file->m,
                    FWFS_MKTAG(0x700, 0x3ff, 0),
                    FWFS_MKTAG(FWFS_TYPE_STRUCT, file->id,
                        fwfs_min(file->cache.size, 0x3fe)),
                    file->cache.buffer);
            if (res < 0) {
                err = res;
                goto cleanup;
            }
        }
    }

    FWFS_TRACE("fwfs_file_opencfg -> %d", 0);
    return 0;

cleanup:
    // clean up lingering resources
    file->flags |= FWFS_F_ERRED;
    fwfs_file_close(fwfs, file);
    FWFS_TRACE("fwfs_file_opencfg -> %d", err);
    return err;
}

int fwfs_file_open(fwfs_t *fwfs, fwfs_file_t *file,
        const char *path, int flags) {
    FWFS_TRACE("fwfs_file_open(%p, %p, \"%s\", %x)",
            (void*)fwfs, (void*)file, path, flags);
    static const struct fwfs_file_config defaults = {0};
    int err = fwfs_file_opencfg(fwfs, file, path, flags, &defaults);
    FWFS_TRACE("fwfs_file_open -> %d", err);
    return err;
}

int fwfs_file_close(fwfs_t *fwfs, fwfs_file_t *file) {
    FWFS_TRACE("fwfs_file_close(%p, %p)", (void*)fwfs, (void*)file);
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);

    int err = fwfs_file_sync(fwfs, file);

    // remove from list of mdirs
    for (struct fwfs_mlist **p = &fwfs->mlist; *p; p = &(*p)->next) {
        if (*p == (struct fwfs_mlist*)file) {
            *p = (*p)->next;
            break;
        }
    }

    // clean up memory
    if (!file->cfg->buffer) {
        fwfs_free(file->cache.buffer);
    }

    file->flags &= ~FWFS_F_OPENED;
    FWFS_TRACE("fwfs_file_close -> %d", err);
    return err;
}

static int fwfs_file_relocate(fwfs_t *fwfs, fwfs_file_t *file) {
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);

    while (true) {
        // just relocate what exists into new block
        fwfs_block_t nblock;
        int err = fwfs_alloc_wrapper(fwfs, &nblock, true);
        if (err) {
            return err;
        }

        err = fwfs_bd_erase(fwfs, nblock);
        if (err) {
            if (err == FWFS_ERR_CORRUPT) {
                goto relocate;
            }
            return err;
        }

        if (fwfs_tftl_isenabled(fwfs->cfg)) {
            if (file->flags & FWFS_F_INLINE) {
                fwfs_tftl_header_t header = {0, };

                fwfs_tftl_header_fill(fwfs, &header, nblock, 0, file->ctz.fid);
                fwfs_bd_prog_nopool(fwfs,
                                    &fwfs->file_pcache, &fwfs->file_rcache,
                                    true, nblock, 0, &header, sizeof(header));
            }
        }

        // either read from dirty cache or disk
        for (fwfs_off_t i = 0; i < file->off; i++) {
            uint8_t data;
            if (file->flags & FWFS_F_INLINE) {
                err = fwfs_dir_getread(fwfs, &file->m,
                        // note we evict inline files before they can be dirty
                        NULL, &file->cache, file->off-i,
                        FWFS_MKTAG(0xfff, 0x1ff, 0),
                        FWFS_MKTAG(FWFS_TYPE_INLINESTRUCT, file->id, 0),
                        i, &data, 1);
                if (err) {
                    return err;
                }
            } else {
                err = fwfs_bd_read_nopool(fwfs,
                        &file->cache, &fwfs->file_rcache, file->off-i,
                        file->block, i, &data, 1);
                if (err) {
                    return err;
                }
            }

            if (file->flags & FWFS_F_INLINE) {
                err = fwfs_bd_prog_nopool(fwfs,
                                  &fwfs->file_pcache, &fwfs->file_rcache, true,
                                  nblock, i + fwfs->tftl.header_size, &data, 1);
            } else {
                err = fwfs_bd_prog_nopool(fwfs,
                                  &fwfs->file_pcache, &fwfs->file_rcache, true,
                                  nblock, i, &data, 1);
            }
            if (err) {
                if (err == FWFS_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }
        }

        // copy over new state of file
        memcpy(file->cache.buffer, fwfs->file_pcache.buffer,
               fwfs->file_pcache.cache_size);
        file->cache.block = fwfs->file_pcache.block;
        file->cache.off = fwfs->file_pcache.off;
        file->cache.size = fwfs->file_pcache.size;
        fwfs_cache_zero(fwfs, &fwfs->file_pcache);

        file->block = nblock;
        file->flags |= FWFS_F_WRITING;
        return 0;

relocate:
        FWFS_DEBUG("Bad block at 0x%"PRIx32, nblock);

        // just clear cache and try a new block
        fwfs_cache_drop(fwfs, &fwfs->file_pcache);
    }
}

static int fwfs_file_outline(fwfs_t *fwfs, fwfs_file_t *file) {
    file->off = file->pos;
    fwfs_alloc_ack(fwfs);
    int err = fwfs_file_relocate(fwfs, file);
    if (err) {
        return err;
    }

    file->flags &= ~FWFS_F_INLINE;
    file->off += fwfs->tftl.header_size;
    return 0;
}

static int fwfs_file_flush(fwfs_t *fwfs, fwfs_file_t *file) {
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);

    if (file->flags & FWFS_F_READING) {
        if (!(file->flags & FWFS_F_INLINE)) {
            fwfs_cache_drop(fwfs, &file->cache);
        }
        file->flags &= ~FWFS_F_READING;
    }

    if (file->flags & FWFS_F_WRITING) {
        fwfs_off_t pos = file->pos;

        if (!(file->flags & FWFS_F_INLINE)) {
            // copy over anything after current branch
            fwfs_file_t orig = {
                .ctz.head = file->ctz.head,
                .ctz.size = file->ctz.size,
                .flags = FWFS_O_RDONLY | FWFS_F_OPENED,
                .pos = file->pos,
                // reuse the file_cache as the cache of temporary file
                .cache = fwfs->file_rcache,
            };
            fwfs_cache_drop(fwfs, &fwfs->file_rcache);

            while (file->pos < file->ctz.size) {
                // copy over a byte at a time, leave it up to caching
                // to make this efficient
                uint8_t data;
                fwfs_ssize_t res = fwfs_file_read(fwfs, &orig, &data, 1);
                if (res < 0) {
                    return res;
                }

                res = fwfs_file_write(fwfs, file, &data, 1);
                if (res < 0) {
                    return res;
                }

                // keep our reference to the file_cache in sync
                if (fwfs->file_rcache.block != fwfs->bn) {
                    fwfs_cache_drop(fwfs, &orig.cache);
                    fwfs_cache_drop(fwfs, &fwfs->file_rcache);
                }
            }

            // write out what we have
            while (true) {
                int err = fwfs_bd_flush_nopool(fwfs, &file->cache, &fwfs->file_rcache, true);
                if (err) {
                    if (err == FWFS_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }

                break;

relocate:
                FWFS_DEBUG("Bad block at 0x%"PRIx32, file->block);
                err = fwfs_file_relocate(fwfs, file);
                if (err) {
                    return err;
                }
            }
        } else {
            file->pos = fwfs_max(file->pos, file->ctz.size);
        }

        // actual file updates
        file->ctz.head = file->block;
        file->ctz.size = file->pos;
        file->flags &= ~FWFS_F_WRITING;
        file->flags |= FWFS_F_DIRTY;

        file->pos = pos;
    }

    return 0;
}

int fwfs_file_sync(fwfs_t *fwfs, fwfs_file_t *file) {
    FWFS_TRACE("fwfs_file_sync(%p, %p)", (void*)fwfs, (void*)file);
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);

    if (file->flags & FWFS_F_ERRED) {
        // it's not safe to do anything if our file errored
        FWFS_TRACE("fwfs_file_sync -> %d", 0);
        return 0;
    }

    int err = fwfs_file_flush(fwfs, file);
    if (err) {
        file->flags |= FWFS_F_ERRED;
        FWFS_TRACE("fwfs_file_sync -> %d", err);
        return err;
    }

    if ((file->flags & FWFS_F_DIRTY) &&
            !fwfs_pair_isnull(fwfs, file->m.pair)) {
        // update dir entry
        uint16_t type;
        const void *buffer;
        fwfs_size_t size;
        struct fwfs_ctz ctz;
        if (file->flags & FWFS_F_INLINE) {
            // inline the whole file
            type = FWFS_TYPE_INLINESTRUCT;
            buffer = file->cache.buffer;
            size = file->ctz.size;
        } else {
            // update the ctz reference
            type = FWFS_TYPE_CTZSTRUCT;
            // copy ctz so alloc will work during a relocate
            ctz = file->ctz;
            fwfs_ctz_tole32(&ctz);
            buffer = &ctz;
            size = sizeof(ctz);
        }

        // commit file data and attributes
        err = fwfs_dir_commit(fwfs, &file->m, FWFS_MKATTRS(
                {FWFS_MKTAG(type, file->id, size), buffer},
                {FWFS_MKTAG(FWFS_FROM_USERATTRS, file->id,
                    file->cfg->attr_count), file->cfg->attrs}));
        if (err) {
            file->flags |= FWFS_F_ERRED;
            FWFS_TRACE("fwfs_file_sync -> %d", err);
            return err;
        }

        file->flags &= ~FWFS_F_DIRTY;
    }

    FWFS_TRACE("fwfs_file_sync -> %d", 0);
    return 0;
}

fwfs_ssize_t fwfs_file_read(fwfs_t *fwfs, fwfs_file_t *file,
        void *buffer, fwfs_size_t size) {
    FWFS_TRACE("fwfs_file_read(%p, %p, %p, %"PRIu32")",
            (void*)fwfs, (void*)file, buffer, size);
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);
    FWFS_ASSERT((file->flags & 3) != FWFS_O_WRONLY);

    uint8_t *data = buffer;
    fwfs_size_t nsize = size;
    fwfs_size_t block_size = fwfs_tftl_isenabled(fwfs->cfg) ?
        fwfs->cfg->subblock_size : fwfs->cfg->block_size;

    if (file->flags & FWFS_F_WRITING) {
        // flush out any writes
        int err = fwfs_file_flush(fwfs, file);
        if (err) {
            FWFS_TRACE("fwfs_file_read -> %d", err);
            return err;
        }
    }

    if (file->pos >= file->ctz.size) {
        // eof if past end
        FWFS_TRACE("fwfs_file_read -> %d", 0);
        return 0;
    }

    size = fwfs_min(size, file->ctz.size - file->pos);
    nsize = size;
    while (nsize > 0) {
        // check if we need a new block
        if (!(file->flags & FWFS_F_READING) ||
                file->off == block_size) {
            if (!(file->flags & FWFS_F_INLINE)) {
                int err = fwfs_ctz_find(fwfs, NULL, &file->cache,
                        file->ctz.head, file->ctz.size,
                        file->pos, &file->block, &file->off);
                if (err) {
                    FWFS_TRACE("fwfs_file_read -> %d", err);
                    return err;
                }
            } else {
                file->block = fwfs->bi;
                file->off = file->pos;
            }

            file->flags |= FWFS_F_READING;
        }

        // read as much as we can in current block
        fwfs_size_t diff = fwfs_min(nsize, block_size - file->off);
        if (file->flags & FWFS_F_INLINE) {
            int err = fwfs_dir_getread(fwfs, &file->m,
                    NULL, &file->cache, block_size,
                    FWFS_MKTAG(0xfff, 0x1ff, 0),
                    FWFS_MKTAG(FWFS_TYPE_INLINESTRUCT, file->id, 0),
                    file->off, data, diff);
            if (err) {
                FWFS_TRACE("fwfs_file_read -> %d", err);
                return err;
            }
        } else {
            int err = fwfs_bd_read_nopool(fwfs,
                    NULL, &file->cache, fwfs->cfg->read_size,
                    file->block, file->off, data, diff);
            if (err) {
                FWFS_TRACE("fwfs_file_read -> %d", err);
                return err;
            }
        }

        file->pos += diff;
        file->off += diff;
        data += diff;
        nsize -= diff;
    }

    FWFS_TRACE("fwfs_file_read -> %"PRId32, size);
    return size;
}

fwfs_ssize_t fwfs_file_write(fwfs_t *fwfs, fwfs_file_t *file,
        const void *buffer, fwfs_size_t size) {
    FWFS_TRACE("fwfs_file_write(%p, %p, %p, %"PRIu32")",
            (void*)fwfs, (void*)file, buffer, size);
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);
    FWFS_ASSERT((file->flags & 3) != FWFS_O_RDONLY);

    const uint8_t *data = buffer;
    fwfs_size_t nsize = size;
    fwfs_size_t block_size = fwfs_tftl_isenabled(fwfs->cfg) ?
        fwfs->cfg->subblock_size : fwfs->cfg->block_size;

    if (file->flags & FWFS_F_READING) {
        // drop any reads
        int err = fwfs_file_flush(fwfs, file);
        if (err) {
            FWFS_TRACE("fwfs_file_write -> %d", err);
            return err;
        }
    }

    if ((file->flags & FWFS_O_APPEND) && file->pos < file->ctz.size) {
        file->pos = file->ctz.size;
    }

    if (file->pos + size > fwfs->file_max) {
        // Larger than file limit?
        FWFS_TRACE("fwfs_file_write -> %d", FWFS_ERR_FBIG);
        return FWFS_ERR_FBIG;
    }

    if (!(file->flags & FWFS_F_WRITING) && file->pos > file->ctz.size) {
        // fill with zeros
        fwfs_off_t pos = file->pos;
        file->pos = file->ctz.size;

        while (file->pos < pos) {
            fwfs_ssize_t res = fwfs_file_write(fwfs, file, &(uint8_t){0}, 1);
            if (res < 0) {
                FWFS_TRACE("fwfs_file_write -> %"PRId32, res);
                return res;
            }
        }
    }

    if ((file->flags & FWFS_F_INLINE) &&
            fwfs_max(file->pos+nsize, file->ctz.size) >
            fwfs_min(0x3fe, fwfs_min(
                fwfs->pcache.cache_size, block_size/8))) {
        // inline file doesn't fit anymore
        int err = fwfs_file_outline(fwfs, file);
        if (err) {
            file->flags |= FWFS_F_ERRED;
            FWFS_TRACE("fwfs_file_write -> %d", err);
            return err;
        }
    }

    while (nsize > 0) {
        // check if we need a new block
        if (!(file->flags & FWFS_F_WRITING) ||
                file->off == block_size) {
            if (!(file->flags & FWFS_F_INLINE)) {
                if (!(file->flags & FWFS_F_WRITING) && file->pos > 0) {
                    // find out which block we're extending from
                    int err = fwfs_ctz_find(fwfs, NULL, &file->cache,
                            file->ctz.head, file->ctz.size,
                            file->pos-1, &file->block, &file->off);
                    if (err) {
                        file->flags |= FWFS_F_ERRED;
                        FWFS_TRACE("fwfs_file_write -> %d", err);
                        return err;
                    }

                    // mark cache as dirty since we may have read data into it
                    fwfs_cache_zero(fwfs, &file->cache);
                }

                // extend file with new blocks
                fwfs_alloc_ack(fwfs);
                int err = fwfs_ctz_extend(fwfs,
                        &file->cache, &fwfs->file_rcache,
                        file->block, file->pos, file->ctz.fid,
                        &file->block, &file->off);
                if (err) {
                    file->flags |= FWFS_F_ERRED;
                    FWFS_TRACE("fwfs_file_write -> %d", err);
                    return err;
                }
            } else {
                file->block = fwfs->bi;
                file->off = file->pos;
            }

            file->flags |= FWFS_F_WRITING;
        }

        // program as much as we can in current block
        fwfs_size_t diff = fwfs_min(nsize, block_size - file->off);
        while (true) {
            int err = fwfs_bd_prog_nopool(fwfs, &file->cache,
                                          &fwfs->file_rcache, true,
                                          file->block, file->off, data, diff);
            if (err) {
                if (err == FWFS_ERR_CORRUPT) {
                    goto relocate;
                }
                file->flags |= FWFS_F_ERRED;
                FWFS_TRACE("fwfs_file_write -> %d", err);
                return err;
            }

            break;
relocate:
            err = fwfs_file_relocate(fwfs, file);
            if (err) {
                file->flags |= FWFS_F_ERRED;
                FWFS_TRACE("fwfs_file_write -> %d", err);
                return err;
            }
        }

        file->pos += diff;
        file->off += diff;
        data += diff;
        nsize -= diff;

        fwfs_alloc_ack(fwfs);
    }

    file->flags &= ~FWFS_F_ERRED;
    FWFS_TRACE("fwfs_file_write -> %"PRId32, size);
    return size;
}

fwfs_soff_t fwfs_file_seek(fwfs_t *fwfs, fwfs_file_t *file,
        fwfs_soff_t off, int whence) {
    FWFS_TRACE("fwfs_file_seek(%p, %p, %"PRId32", %d)",
            (void*)fwfs, (void*)file, off, whence);
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);

    // write out everything beforehand, may be noop if rdonly
    int err = fwfs_file_flush(fwfs, file);
    if (err) {
        FWFS_TRACE("fwfs_file_seek -> %d", err);
        return err;
    }

    // find new pos
    fwfs_off_t npos = file->pos;
    if (whence == FWFS_SEEK_SET) {
        npos = off;
    } else if (whence == FWFS_SEEK_CUR) {
        npos = file->pos + off;
    } else if (whence == FWFS_SEEK_END) {
        npos = file->ctz.size + off;
    }

    if (npos > fwfs->file_max) {
        // file position out of range
        FWFS_TRACE("fwfs_file_seek -> %d", FWFS_ERR_INVAL);
        return FWFS_ERR_INVAL;
    }

    // update pos
    file->pos = npos;
    FWFS_TRACE("fwfs_file_seek -> %"PRId32, npos);
    return npos;
}

int fwfs_file_truncate(fwfs_t *fwfs, fwfs_file_t *file, fwfs_off_t size) {
    FWFS_TRACE("fwfs_file_truncate(%p, %p, %"PRIu32")",
            (void*)fwfs, (void*)file, size);
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);
    FWFS_ASSERT((file->flags & 3) != FWFS_O_RDONLY);

    if (size > FWFS_FILE_MAX) {
        FWFS_TRACE("fwfs_file_truncate -> %d", FWFS_ERR_INVAL);
        return FWFS_ERR_INVAL;
    }

    fwfs_off_t pos = file->pos;
    fwfs_off_t oldsize = fwfs_file_size(fwfs, file);
    if (size < oldsize) {
        // need to flush since directly changing metadata
        int err = fwfs_file_flush(fwfs, file);
        if (err) {
            FWFS_TRACE("fwfs_file_truncate -> %d", err);
            return err;
        }

        // lookup new head in ctz skip list
        err = fwfs_ctz_find(fwfs, NULL, &file->cache,
                file->ctz.head, file->ctz.size,
                size > 0 ? size - 1 : 0,
                &file->block, &file->off);
        if (err) {
            FWFS_TRACE("fwfs_file_truncate -> %d", err);
            return err;
        }

        file->ctz.head = file->block;
        file->ctz.size = size;
        file->flags |= FWFS_F_DIRTY | FWFS_F_READING;
    } else if (size > oldsize) {
        // flush+seek if not already at end
        if (file->pos != oldsize) {
            fwfs_soff_t res = fwfs_file_seek(fwfs, file, 0, FWFS_SEEK_END);
            if (res < 0) {
                FWFS_TRACE("fwfs_file_truncate -> %"PRId32, res);
                return (int)res;
            }
        }

        // fill with zeros
        while (file->pos < size) {
            fwfs_ssize_t res = fwfs_file_write(fwfs, file, &(uint8_t){0}, 1);
            if (res < 0) {
                FWFS_TRACE("fwfs_file_truncate -> %"PRId32, res);
                return (int)res;
            }
        }
    }

    // restore pos
    fwfs_soff_t res = fwfs_file_seek(fwfs, file, pos, FWFS_SEEK_SET);
    if (res < 0) {
      FWFS_TRACE("fwfs_file_truncate -> %"PRId32, res);
      return (int)res;
    }

    FWFS_TRACE("fwfs_file_truncate -> %d", 0);
    return 0;
}

fwfs_soff_t fwfs_file_tell(fwfs_t *fwfs, fwfs_file_t *file) {
    FWFS_TRACE("fwfs_file_tell(%p, %p)", (void*)fwfs, (void*)file);
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);
    (void)fwfs;
    FWFS_TRACE("fwfs_file_tell -> %"PRId32, file->pos);
    return file->pos;
}

int fwfs_file_rewind(fwfs_t *fwfs, fwfs_file_t *file) {
    FWFS_TRACE("fwfs_file_rewind(%p, %p)", (void*)fwfs, (void*)file);
    fwfs_soff_t res = fwfs_file_seek(fwfs, file, 0, FWFS_SEEK_SET);
    if (res < 0) {
        FWFS_TRACE("fwfs_file_rewind -> %"PRId32, res);
        return (int)res;
    }

    FWFS_TRACE("fwfs_file_rewind -> %d", 0);
    return 0;
}

fwfs_soff_t fwfs_file_size(fwfs_t *fwfs, fwfs_file_t *file) {
    FWFS_TRACE("fwfs_file_size(%p, %p)", (void*)fwfs, (void*)file);
    FWFS_ASSERT(file->flags & FWFS_F_OPENED);
    (void)fwfs;
    if (file->flags & FWFS_F_WRITING) {
        FWFS_TRACE("fwfs_file_size -> %"PRId32,
                fwfs_max(file->pos, file->ctz.size));
        return fwfs_max(file->pos, file->ctz.size);
    } else {
        FWFS_TRACE("fwfs_file_size -> %"PRId32, file->ctz.size);
        return file->ctz.size;
    }
}


/// General fs operations ///
int fwfs_stat(fwfs_t *fwfs, const char *path, struct fwfs_info *info) {
    FWFS_TRACE("fwfs_stat(%p, \"%s\", %p)", (void*)fwfs, path, (void*)info);
    fwfs_mdir_t cwd;
    fwfs_stag_t tag = fwfs_dir_find(fwfs, &cwd, &path, NULL);
    if (tag < 0) {
        FWFS_TRACE("fwfs_stat -> %"PRId32, tag);
        return (int)tag;
    }

    int err = fwfs_dir_getinfo(fwfs, &cwd, fwfs_tag_id(tag), info);
    FWFS_TRACE("fwfs_stat -> %d", err);
    return err;
}

int fwfs_remove(fwfs_t *fwfs, const char *path) {
    FWFS_TRACE("fwfs_remove(%p, \"%s\")", (void*)fwfs, path);
    // deorphan if we haven't yet, needed at most once after poweron
    int err = fwfs_fs_forceconsistency(fwfs);
    if (err) {
        FWFS_TRACE("fwfs_remove -> %d", err);
        return err;
    }

    char dirname[FWFS_NAME_MAX+1];
    bool force_compact = false;
    err =  fwfs_fs_path_dirname(fwfs, path, dirname, sizeof(dirname));
    if (err <= 0) {
        FWFS_TRACE("fwfs_remove -> %d", err);
        return err;
    }
    force_compact = fwfs_folder_compress_policy(fwfs, dirname);

    fwfs_mdir_t cwd;
    fwfs_stag_t tag = fwfs_dir_find(fwfs, &cwd, &path, NULL);
    if (tag < 0 || fwfs_tag_id(tag) == 0x3ff) {
        FWFS_TRACE("fwfs_remove -> %"PRId32, (tag < 0) ? tag : FWFS_ERR_INVAL);
        return (tag < 0) ? (int)tag : FWFS_ERR_INVAL;
    }
    cwd.force_compact = force_compact;

    struct fwfs_mlist dir;
    dir.next = fwfs->mlist;
    if (fwfs_tag_type3(tag) == FWFS_TYPE_DIR) {
        // must be empty before removal
        fwfs_block_t pair[2];
        fwfs_stag_t res = fwfs_dir_get(fwfs, &cwd, FWFS_MKTAG(0x700, 0x3ff, 0),
                FWFS_MKTAG(FWFS_TYPE_STRUCT, fwfs_tag_id(tag), 8), pair);
        if (res < 0) {
            FWFS_TRACE("fwfs_remove -> %"PRId32, res);
            return (int)res;
        }
        fwfs_pair_fromle32(pair);

        err = fwfs_dir_fetch(fwfs, &dir.m, pair);
        if (err) {
            FWFS_TRACE("fwfs_remove -> %d", err);
            return err;
        }

        if (dir.m.count > 0 || dir.m.split) {
            FWFS_TRACE("fwfs_remove -> %d", FWFS_ERR_NOTEMPTY);
            return FWFS_ERR_NOTEMPTY;
        }

        // mark fs as orphaned
        fwfs_fs_preporphans(fwfs, +1);

        // I know it's crazy but yes, dir can be changed by our parent's
        // commit (if predecessor is child)
        dir.type = 0;
        dir.id = 0;
        fwfs->mlist = &dir;
    }

    // delete the entry
    err = fwfs_dir_commit(fwfs, &cwd, FWFS_MKATTRS(
            {FWFS_MKTAG(FWFS_TYPE_DELETE, fwfs_tag_id(tag), 0), NULL}));
    if (err) {
        fwfs->mlist = dir.next;
        FWFS_TRACE("fwfs_remove -> %d", err);
        return err;
    }

    fwfs->mlist = dir.next;
    if (fwfs_tag_type3(tag) == FWFS_TYPE_DIR) {
        // fix orphan
        fwfs_fs_preporphans(fwfs, -1);

        err = fwfs_fs_pred(fwfs, dir.m.pair, &cwd);
        if (err) {
            FWFS_TRACE("fwfs_remove -> %d", err);
            return err;
        }

        err = fwfs_fs_mdir_dirname(fwfs, cwd.pair, dirname,
                                  sizeof(dirname));
        if (err <= 0) {
            return err;
        }
        cwd.force_compact = fwfs_folder_compress_policy(fwfs,dirname);

        err = fwfs_dir_drop(fwfs, &cwd, &dir.m);
        if (err) {
            FWFS_TRACE("fwfs_remove -> %d", err);
            return err;
        }
    }

    FWFS_TRACE("fwfs_remove -> %d", 0);
    return 0;
}

int fwfs_rename(fwfs_t *fwfs, const char *oldpath, const char *newpath) {
    FWFS_TRACE("fwfs_rename(%p, \"%s\", \"%s\")", (void*)fwfs, oldpath, newpath);

    // deorphan if we haven't yet, needed at most once after poweron
    int err = fwfs_fs_forceconsistency(fwfs);
    if (err) {
        FWFS_TRACE("fwfs_rename -> %d", err);
        return err;
    }

    char dirname[FWFS_NAME_MAX+1];
    bool force_compact = false;
    err = fwfs_fs_path_dirname(fwfs, oldpath, dirname, sizeof(dirname));
    if (err <= 0) {
        FWFS_TRACE("fwfs_rename -> %d", err);
        return err;
    }
    force_compact = fwfs_folder_compress_policy(fwfs, dirname);

    // find old entry
    fwfs_mdir_t oldcwd;
    fwfs_stag_t oldtag = fwfs_dir_find(fwfs, &oldcwd, &oldpath, NULL);
    if (oldtag < 0 || fwfs_tag_id(oldtag) == 0x3ff) {
        FWFS_TRACE("fwfs_rename -> %"PRId32,
                (oldtag < 0) ? oldtag : FWFS_ERR_INVAL);
        return (oldtag < 0) ? (int)oldtag : FWFS_ERR_INVAL;
    }
    oldcwd.force_compact = force_compact;

    force_compact = false;
    err = fwfs_fs_path_dirname(fwfs, newpath, dirname, sizeof(dirname));
    if (err <= 0) {
        FWFS_TRACE("fwfs_rename -> %d", err);
        return err;
    }
    force_compact = fwfs_folder_compress_policy(fwfs, dirname);

    // find new entry
    fwfs_mdir_t newcwd;
    uint16_t newid;
    fwfs_stag_t prevtag = fwfs_dir_find(fwfs, &newcwd, &newpath, &newid);
    if ((prevtag < 0 || fwfs_tag_id(prevtag) == 0x3ff) &&
            !(prevtag == FWFS_ERR_NOENT && newid != 0x3ff)) {
        FWFS_TRACE("fwfs_rename -> %"PRId32,
            (prevtag < 0) ? prevtag : FWFS_ERR_INVAL);
        return (prevtag < 0) ? (int)prevtag : FWFS_ERR_INVAL;
    }
    newcwd.force_compact = force_compact;

    // if we're in the same pair there's a few special cases...
    bool samepair = (fwfs_pair_cmp(oldcwd.pair, newcwd.pair) == 0);
    uint16_t newoldid = fwfs_tag_id(oldtag);

    struct fwfs_mlist prevdir;
    prevdir.next = fwfs->mlist;
    if (prevtag == FWFS_ERR_NOENT) {
        // check that name fits
        fwfs_size_t nlen = strlen(newpath);
        if (nlen > fwfs->name_max) {
            FWFS_TRACE("fwfs_rename -> %d", FWFS_ERR_NAMETOOLONG);
            return FWFS_ERR_NAMETOOLONG;
        }

        // there is a small chance we are being renamed in the same
        // directory/ to an id less than our old id, the global update
        // to handle this is a bit messy
        if (samepair && newid <= newoldid) {
            newoldid += 1;
        }
    } else if (fwfs_tag_type3(prevtag) != fwfs_tag_type3(oldtag)) {
        FWFS_TRACE("fwfs_rename -> %d", FWFS_ERR_ISDIR);
        return FWFS_ERR_ISDIR;
    } else if (samepair && newid == newoldid) {
        // we're renaming to ourselves??
        FWFS_TRACE("fwfs_rename -> %d", 0);
        return 0;
    } else if (fwfs_tag_type3(prevtag) == FWFS_TYPE_DIR) {
        // must be empty before removal
        fwfs_block_t prevpair[2];
        fwfs_stag_t res = fwfs_dir_get(fwfs, &newcwd, FWFS_MKTAG(0x700, 0x3ff, 0),
                FWFS_MKTAG(FWFS_TYPE_STRUCT, newid, 8), prevpair);
        if (res < 0) {
            FWFS_TRACE("fwfs_rename -> %"PRId32, res);
            return (int)res;
        }
        fwfs_pair_fromle32(prevpair);

        // must be empty before removal
        err = fwfs_dir_fetch(fwfs, &prevdir.m, prevpair);
        if (err) {
            FWFS_TRACE("fwfs_rename -> %d", err);
            return err;
        }

        if (prevdir.m.count > 0 || prevdir.m.split) {
            FWFS_TRACE("fwfs_rename -> %d", FWFS_ERR_NOTEMPTY);
            return FWFS_ERR_NOTEMPTY;
        }

        // mark fs as orphaned
        fwfs_fs_preporphans(fwfs, +1);

        // I know it's crazy but yes, dir can be changed by our parent's
        // commit (if predecessor is child)
        prevdir.type = 0;
        prevdir.id = 0;
        fwfs->mlist = &prevdir;
    }

    if (!samepair) {
        fwfs_fs_prepmove(fwfs, newoldid, oldcwd.pair);
    }

    // move over all attributes
    err = fwfs_dir_commit(fwfs, &newcwd, FWFS_MKATTRS(
            {FWFS_MKTAG_IF(prevtag != FWFS_ERR_NOENT,
                FWFS_TYPE_DELETE, newid, 0), NULL},
            {FWFS_MKTAG(FWFS_TYPE_CREATE, newid, 0), NULL},
            {FWFS_MKTAG(fwfs_tag_type3(oldtag), newid, strlen(newpath)), newpath},
            {FWFS_MKTAG(FWFS_FROM_MOVE, newid, fwfs_tag_id(oldtag)), &oldcwd},
            {FWFS_MKTAG_IF(samepair,
                FWFS_TYPE_DELETE, newoldid, 0), NULL}));
    if (err) {
        fwfs->mlist = prevdir.next;
        FWFS_TRACE("fwfs_rename -> %d", err);
        return err;
    }

    // let commit clean up after move (if we're different! otherwise move
    // logic already fixed it for us)
    if (!samepair && fwfs_gstate_hasmove(&fwfs->gstate)) {
        // prep gstate and delete move id
        fwfs_fs_prepmove(fwfs, 0x3ff, NULL);
        err = fwfs_dir_commit(fwfs, &oldcwd, FWFS_MKATTRS(
                {FWFS_MKTAG(FWFS_TYPE_DELETE, fwfs_tag_id(oldtag), 0), NULL}));
        if (err) {
            fwfs->mlist = prevdir.next;
            FWFS_TRACE("fwfs_rename -> %d", err);
            return err;
        }
    }

    fwfs->mlist = prevdir.next;
    if (prevtag != FWFS_ERR_NOENT && fwfs_tag_type3(prevtag) == FWFS_TYPE_DIR) {
        // fix orphan
        fwfs_fs_preporphans(fwfs, -1);

        err = fwfs_fs_pred(fwfs, prevdir.m.pair, &newcwd);
        if (err) {
            FWFS_TRACE("fwfs_rename -> %d", err);
            return err;
        }

        err = fwfs_fs_mdir_dirname(fwfs, newcwd.pair, dirname,
                                  sizeof(dirname));
        if (err <= 0) {
            FWFS_TRACE("fwfs_rename -> %d", err);
            return err;
        }
        newcwd.force_compact = fwfs_folder_compress_policy(fwfs,dirname);

        err = fwfs_dir_drop(fwfs, &newcwd, &prevdir.m);
        if (err) {
            FWFS_TRACE("fwfs_rename -> %d", err);
            return err;
        }
    }

    FWFS_TRACE("fwfs_rename -> %d", 0);
    return 0;
}

fwfs_ssize_t fwfs_getattr(fwfs_t *fwfs, const char *path,
        uint8_t type, void *buffer, fwfs_size_t size) {
    FWFS_TRACE("fwfs_getattr(%p, \"%s\", %"PRIu8", %p, %"PRIu32")",
            (void*)fwfs, path, type, buffer, size);
    fwfs_mdir_t cwd;
    fwfs_stag_t tag = fwfs_dir_find(fwfs, &cwd, &path, NULL);
    if (tag < 0) {
        FWFS_TRACE("fwfs_getattr -> %"PRId32, tag);
        return tag;
    }

    uint16_t id = fwfs_tag_id(tag);
    if (id == 0x3ff) {
        // special case for root
        id = 0;
        int err = fwfs_dir_fetch(fwfs, &cwd, fwfs->root);
        if (err) {
            FWFS_TRACE("fwfs_getattr -> %d", err);
            return err;
        }
    }

    tag = fwfs_dir_get(fwfs, &cwd, FWFS_MKTAG(0x7ff, 0x3ff, 0),
            FWFS_MKTAG(FWFS_TYPE_USERATTR + type,
                id, fwfs_min(size, fwfs->attr_max)),
            buffer);
    if (tag < 0) {
        if (tag == FWFS_ERR_NOENT) {
            FWFS_TRACE("fwfs_getattr -> %d", FWFS_ERR_NOATTR);
            return FWFS_ERR_NOATTR;
        }

        FWFS_TRACE("fwfs_getattr -> %"PRId32, tag);
        return tag;
    }

    size = fwfs_tag_size(tag);
    FWFS_TRACE("fwfs_getattr -> %"PRId32, size);
    return size;
}

static int fwfs_commitattr(fwfs_t *fwfs, const char *path,
        uint8_t type, const void *buffer, fwfs_size_t size) {
    fwfs_mdir_t cwd;
    fwfs_stag_t tag;
    char dirname[FWFS_NAME_MAX+1];
    bool force_compact = false;

    int err = fwfs_fs_path_dirname(fwfs, path, dirname, sizeof(dirname));
    if (err <= 0) {
        return err;
    }
    force_compact = fwfs_folder_compress_policy(fwfs, dirname);

    tag = fwfs_dir_find(fwfs, &cwd, &path, NULL);
    if (tag < 0) {
        return tag;
    }
    cwd.force_compact = force_compact;

    uint16_t id = fwfs_tag_id(tag);
    if (id == 0x3ff) {
        // special case for root
        id = 0;
        err = fwfs_dir_fetch(fwfs, &cwd, fwfs->root);
        if (err) {
            return err;
        }
    }

    return fwfs_dir_commit(fwfs, &cwd, FWFS_MKATTRS(
            {FWFS_MKTAG(FWFS_TYPE_USERATTR + type, id, size), buffer}));
}

int fwfs_setattr(fwfs_t *fwfs, const char *path,
        uint8_t type, const void *buffer, fwfs_size_t size) {
    FWFS_TRACE("fwfs_setattr(%p, \"%s\", %"PRIu8", %p, %"PRIu32")",
            (void*)fwfs, path, type, buffer, size);
    if (size > fwfs->attr_max) {
        FWFS_TRACE("fwfs_setattr -> %d", FWFS_ERR_NOSPC);
        return FWFS_ERR_NOSPC;
    }

    int err = fwfs_commitattr(fwfs, path, type, buffer, size);
    FWFS_TRACE("fwfs_setattr -> %d", err);
    return err;
}

int fwfs_removeattr(fwfs_t *fwfs, const char *path, uint8_t type) {
    FWFS_TRACE("fwfs_removeattr(%p, \"%s\", %"PRIu8")", (void*)fwfs, path, type);
    int err = fwfs_commitattr(fwfs, path, type, NULL, 0x3ff);
    FWFS_TRACE("fwfs_removeattr -> %d", err);
    return err;
}


/// Filesystem operations ///
static int fwfs_init(fwfs_t *fwfs, const struct fwfs_config *cfg) {
    fwfs->cfg = cfg;
    int err = 0;
    fwfs_cache_t *rcache;
    void *rcache_buffer;

    // validate that the fwfs-cfg sizes were initiated properly before
    // performing any arithmetic logics with them
    FWFS_ASSERT(fwfs->cfg->read_size != 0);
    FWFS_ASSERT(fwfs->cfg->prog_size != 0);
    FWFS_ASSERT(fwfs->cfg->cache_size != 0);
    FWFS_ASSERT(fwfs->cfg->block_size != 0);
    FWFS_ASSERT(fwfs->cfg->subblock_size != 0);

    // check that block size is a multiple of cache size is a multiple
    // of prog and read sizes
    FWFS_ASSERT(fwfs->cfg->cache_size % fwfs->cfg->read_size == 0);
    FWFS_ASSERT(fwfs->cfg->cache_size % fwfs->cfg->prog_size == 0);
    FWFS_ASSERT(fwfs->cfg->block_size % fwfs->cfg->cache_size == 0);

    FWFS_ASSERT(fwfs->cfg->file_cache_size % fwfs->cfg->read_size == 0);
    FWFS_ASSERT(fwfs->cfg->file_cache_size % fwfs->cfg->prog_size == 0);
    FWFS_ASSERT(fwfs->cfg->subblock_size % fwfs->cfg->file_cache_size == 0);

    // check that the block size is large enough to fit ctz pointers
    if (fwfs_tftl_isenabled(fwfs->cfg))
        FWFS_ASSERT(4*fwfs_npw2(0xffffffff / (fwfs->cfg->subblock_size-2*4))
                   <= fwfs->cfg->subblock_size);
    else
        FWFS_ASSERT(4*fwfs_npw2(0xffffffff / (fwfs->cfg->block_size-2*4))
                   <= fwfs->cfg->block_size);

    // block_cycles = 0 is no longer supported.
    //
    // block_cycles is the number of erase cycles before firmwarefs evicts
    // metadata logs as a part of wear leveling. Suggested values are in the
    // range of 100-1000, or set block_cycles to -1 to disable block-level
    // wear-leveling.
    FWFS_ASSERT(fwfs->cfg->block_cycles != 0);

    FWFS_ASSERT(fwfs->cfg->cache_pool_size  > 0 &&
               fwfs->cfg->cache_pool_size <= MAX_CACHE_POOL_SIZE);

    fwfs->b0 = 0;
    fwfs->b1 = 1;
    fwfs->bn = FWFS_BLOCK_NULL;
    fwfs->bi = FWFS_BLOCK_INLINE;

    if (fwfs_tftl_isenabled(fwfs->cfg)) {
        err = fwfs_tftl_init(fwfs);
        if (err)
            goto cleanup;
    } else {
        memset((void*)&fwfs->tftl, 0, sizeof(fwfs->tftl));
    }
    FWFS_ASSERT(fwfs->tftl.header_size < fwfs->cfg->read_size);
    FWFS_ASSERT(fwfs->tftl.header_size < fwfs->cfg->prog_size);

    // setup read cache
    memset(&fwfs->rcache_pool, 0, sizeof(fwfs->rcache_pool));
    if (fwfs->cfg->cache_pool_size > 0)
        fwfs->rcache_pool.size = fwfs->cfg->cache_pool_size;
    else
        fwfs->rcache_pool.size = 1;
    if (fwfs->cfg->read_buffer) {
        rcache_buffer = fwfs->cfg->read_buffer;
    } else {
        fwfs_size_t total = fwfs->cfg->cache_size * fwfs->cfg->cache_pool_size;

        rcache_buffer = (void*)fwfs_malloc(total);
        if (!rcache_buffer) {
            err = FWFS_ERR_NOMEM;
            goto cleanup;
        }
    }

    for (uint8_t i = 0; i < fwfs->rcache_pool.size; i++) {
        rcache = (fwfs_cache_t*)fwfs_malloc(sizeof(fwfs_cache_t));
        if (!rcache) {
            err = FWFS_ERR_NOMEM;
            goto cleanup;
        }
        rcache->cache_size = fwfs->cfg->cache_size;
        rcache->buffer = (uint8_t*)rcache_buffer + i * rcache->cache_size;
        fwfs->rcache_pool.caches[i] = rcache;
    }

    // setup program cache
    fwfs->pcache.cache_size = fwfs->cfg->cache_size;
    if (fwfs->cfg->prog_buffer) {
        fwfs->pcache.buffer = fwfs->cfg->prog_buffer;
    } else {
        fwfs->pcache.buffer = fwfs_malloc(fwfs->pcache.cache_size);
        if (!fwfs->pcache.buffer) {
            err = FWFS_ERR_NOMEM;
            goto cleanup;
        }
    }

    fwfs->file_rcache.cache_size = fwfs->cfg->file_cache_size;
    if (fwfs->cfg->file_read_buffer) {
        fwfs->file_rcache.buffer = fwfs->cfg->file_read_buffer;
    } else {
        fwfs->file_rcache.buffer =
            fwfs_malloc(fwfs->file_rcache.cache_size);
        if (!fwfs->file_rcache.buffer) {
            err = FWFS_ERR_NOMEM;
            goto cleanup;
        }
    }

    fwfs->file_pcache.cache_size = fwfs->cfg->file_cache_size;
    if (fwfs->cfg->file_prog_buffer) {
        fwfs->file_pcache.buffer = fwfs->cfg->file_prog_buffer;
    } else {
        fwfs->file_pcache.buffer =
            fwfs_malloc(fwfs->file_pcache.cache_size);
        if (!fwfs->file_pcache.buffer) {
            err = FWFS_ERR_NOMEM;
            goto cleanup;
        }
    }

    if (fwfs->cfg->debug >= 2) {
        fwfs->debug_rcache = (fwfs_cache_t *)fwfs_malloc(sizeof(fwfs_cache_t));
        if (!fwfs->debug_rcache) {
            err = FWFS_ERR_NOMEM;
            goto cleanup;
        }
        memset(fwfs->debug_rcache, 0, sizeof(fwfs_cache_t));

        fwfs->debug_rcache->cache_size = fwfs->cfg->file_cache_size;
        fwfs->debug_rcache->buffer =
            fwfs_malloc(fwfs->debug_rcache->cache_size);
        if (!fwfs->debug_rcache->buffer) {
            err = FWFS_ERR_NOMEM;
            goto cleanup;
        }

        fwfs_cache_zero(fwfs, fwfs->debug_rcache);
    } else {
        fwfs->debug_rcache = NULL;
    }

    // zero to avoid information leaks
    fwfs_cache_pool_zero(fwfs, &fwfs->rcache_pool);
    fwfs_cache_zero(fwfs, &fwfs->pcache);
    fwfs_cache_zero(fwfs, &fwfs->file_rcache);
    fwfs_cache_zero(fwfs, &fwfs->file_pcache);

    // setup lookahead, must be multiple of 64-bits, 32-bit aligned
    FWFS_ASSERT(fwfs->cfg->lookahead_size > 0);
    FWFS_ASSERT(fwfs->cfg->lookahead_size % 8 == 0 &&
            (uintptr_t)fwfs->cfg->lookahead_buffer % 4 == 0);
    if (fwfs->cfg->lookahead_buffer) {
        fwfs->free.buffer = fwfs->cfg->lookahead_buffer;
    } else {
        fwfs->free.buffer = fwfs_malloc(fwfs->cfg->lookahead_size);
        if (!fwfs->free.buffer) {
            err = FWFS_ERR_NOMEM;
            goto cleanup;
        }
    }

    // check that the size limits are sane
    FWFS_ASSERT(fwfs->cfg->name_max <= FWFS_NAME_MAX);
    fwfs->name_max = fwfs->cfg->name_max;
    if (!fwfs->name_max) {
        fwfs->name_max = FWFS_NAME_MAX;
    }

    FWFS_ASSERT(fwfs->cfg->file_max <= FWFS_FILE_MAX);
    fwfs->file_max = fwfs->cfg->file_max;
    if (!fwfs->file_max) {
        fwfs->file_max = FWFS_FILE_MAX;
    }

    FWFS_ASSERT(fwfs->cfg->attr_max <= FWFS_ATTR_MAX);
    fwfs->attr_max = fwfs->cfg->attr_max;
    if (!fwfs->attr_max) {
        fwfs->attr_max = FWFS_ATTR_MAX;
    }

    // setup default state
    fwfs->root[0] = fwfs->bn;
    fwfs->root[1] = fwfs->bn;
    fwfs->mlist = NULL;
    fwfs->seed = 0;
    fwfs->gdisk = (fwfs_gstate_t){0};
    fwfs->gstate = (fwfs_gstate_t){0};
    fwfs->gdelta = (fwfs_gstate_t){0};
#ifdef FWFS_MIGRATE
    fwfs->fwfs1 = NULL;
#endif

    return 0;

cleanup:
    fwfs_deinit(fwfs);
    return err;
}

static int fwfs_deinit(fwfs_t *fwfs) {
    // free allocated memory

    if (fwfs->debug_rcache) {
        if (fwfs->debug_rcache->buffer)
            fwfs_free(fwfs->debug_rcache->buffer);
        fwfs_free(fwfs->debug_rcache);
    }

    if (!fwfs->cfg->read_buffer) {
        fwfs_free(fwfs->rcache_pool.caches[0]->buffer);
        for (uint8_t i = 0; i < fwfs->rcache_pool.size; i++)
            fwfs_free(fwfs->rcache_pool.caches[i]);
    }

    if (!fwfs->cfg->file_read_buffer) {
        fwfs_free(fwfs->file_rcache.buffer);
    }

    if (!fwfs->cfg->file_prog_buffer) {
        fwfs_free(fwfs->file_pcache.buffer);
    }

    if (!fwfs->cfg->prog_buffer) {
        fwfs_free(fwfs->pcache.buffer);
    }

    if (!fwfs->cfg->lookahead_buffer) {
        fwfs_free(fwfs->free.buffer);
    }

    if (fwfs_tftl_isenabled(fwfs->cfg)) {
        fwfs_tftl_deinit(fwfs);
    }
    return 0;
}

int fwfs_format(fwfs_t *fwfs, const struct fwfs_config *cfg) {
    FWFS_TRACE("fwfs_format(%p, %p {.context=%p, "
                ".read=%p, .prog=%p, .erase=%p, .sync=%p, "
                ".read_size=%"PRIu32", .prog_size=%"PRIu32", "
                ".block_size=%"PRIu32", .subblock_size=%"PRIu32", "
                ".block_count=%"PRIu32", .block_cycles=%"PRIu32", "
                ".cache_size=%"PRIu32", .file_cache_size=%"PRIu32", "
                ".lookahead_size=%"PRIu32", .read_buffer=%p, "
                ".prog_buffer=%p, .lookahead_buffer=%p, "
                ".name_max=%"PRIu32", .file_max=%"PRIu32", "
                ".attr_max=%"PRIu32"})",
            (void*)fwfs, (void*)cfg, cfg->context,
            (void*)(uintptr_t)cfg->read, (void*)(uintptr_t)cfg->prog,
            (void*)(uintptr_t)cfg->erase, (void*)(uintptr_t)cfg->sync,
            cfg->read_size, cfg->prog_size, cfg->block_size, cfg->subblock_size,
            cfg->block_count, cfg->block_cycles, cfg->cache_size,
            cfg->file_cache_size, cfg->lookahead_size, cfg->read_buffer,
            cfg->prog_buffer, cfg->lookahead_buffer, cfg->name_max,
            cfg->file_max, cfg->attr_max);
    int err = 0;
    {
        err = fwfs_init(fwfs, cfg);
        if (err) {
            FWFS_TRACE("fwfs_format -> %d", err);
            return err;
        }

        // create free lookahead
        memset(fwfs->free.buffer, 0, fwfs->cfg->lookahead_size);
        fwfs->free.off = 0;
        fwfs->free.size = fwfs_min(8*fwfs->cfg->lookahead_size,
                fwfs->cfg->block_count);
        fwfs->free.i = 0;
        fwfs_alloc_ack(fwfs);

        // create root dir
        fwfs_mdir_t root;
        err = fwfs_dir_alloc(fwfs, &root);
        if (err) {
            goto cleanup;
        }

        // write one superblock
        fwfs_superblock_t superblock = {
            .version     = FWFS_DISK_VERSION,
            .block_size  = fwfs->cfg->block_size,
            .block_count = fwfs->cfg->block_count,
            .name_max    = fwfs->name_max,
            .file_max    = fwfs->file_max,
            .attr_max    = fwfs->attr_max,
            .tftl_version     = FWFS_TFTL_DISK_VERSION,
            .subblock_size    = fwfs->cfg->subblock_size,
        };

        fwfs_superblock_tole32(&superblock);
        err = fwfs_dir_commit(fwfs, &root, FWFS_MKATTRS(
                {FWFS_MKTAG(FWFS_TYPE_CREATE, 0, 0), NULL},
                {FWFS_MKTAG(FWFS_TYPE_SUPERBLOCK, 0, 10), "firmwarefs"},
                {FWFS_MKTAG(FWFS_TYPE_INLINESTRUCT, 0, sizeof(superblock)),
                    &superblock}));
        if (err) {
            goto cleanup;
        }

        // sanity check that fetch works
        err = fwfs_dir_fetch(fwfs, &root,
                             (const fwfs_block_t[2]){fwfs->b0, fwfs->b1});
        if (err) {
            goto cleanup;
        }

        // force compaction to prevent accidentally mounting any
        // older version of firmwarefs that may live on disk
        root.erased = false;
        err = fwfs_dir_commit(fwfs, &root, NULL, 0);
        if (err) {
            goto cleanup;
        }
    }

cleanup:
    fwfs_deinit(fwfs);
    FWFS_TRACE("fwfs_format -> %d", err);
    return err;
}

static inline bool fwfs_is_forcecompact_enabled(fwfs_t *fwfs) {
    return fwfs->cfg->compact_on_mount == FWFS_COMPACT_ON_MOUNT_MAGIC;
}

int fwfs_mount(fwfs_t *fwfs, const struct fwfs_config *cfg) {
    FWFS_TRACE("fwfs_mount(%p, %p {.context=%p, "
                ".read=%p, .prog=%p, .erase=%p, .sync=%p, "
                ".read_size=%"PRIu32", .prog_size=%"PRIu32", "
                ".block_size=%"PRIu32", .subblock_size=%"PRIu32", "
                ".block_count=%"PRIu32", .block_cycles=%"PRIu32", "
                ".cache_size=%"PRIu32", .file_cache_size=%"PRIu32", "
                ".lookahead_size=%"PRIu32", .read_buffer=%p, "
                ".prog_buffer=%p, .lookahead_buffer=%p, "
                ".name_max=%"PRIu32", .file_max=%"PRIu32", "
                ".attr_max=%"PRIu32"})",
            (void*)fwfs, (void*)cfg, cfg->context,
            (void*)(uintptr_t)cfg->read, (void*)(uintptr_t)cfg->prog,
            (void*)(uintptr_t)cfg->erase, (void*)(uintptr_t)cfg->sync,
            cfg->read_size, cfg->prog_size, cfg->block_size, cfg->subblock_size,
            cfg->block_count, cfg->block_cycles, cfg->cache_size,
            cfg->file_cache_size, cfg->lookahead_size, cfg->read_buffer,
            cfg->prog_buffer, cfg->lookahead_buffer, cfg->name_max,
            cfg->file_max, cfg->attr_max);
    int err = fwfs_init(fwfs, cfg);
    if (err) {
        FWFS_TRACE("fwfs_mount -> %d", err);
        return err;
    }

    // scan directory blocks for superblock and any global updates
    fwfs_mdir_t dir = {.tail = {fwfs->b0, fwfs->b1}};
    fwfs_block_t cycle = 0;
    bool need_compact = false;

    while (!fwfs_pair_isnull(fwfs, dir.tail)) {
        if (cycle >= fwfs->cfg->block_count/2) {
            // loop detected
            err = FWFS_ERR_CORRUPT;
            goto cleanup;
        }
        cycle += 1;

        // fetch next block in tail list
        fwfs_stag_t tag = fwfs_dir_fetchmatchraw(fwfs, &dir, dir.tail,
                FWFS_MKTAG(0x7ff, 0x3ff, 0),
                FWFS_MKTAG(FWFS_TYPE_SUPERBLOCK, 0, 10),
                NULL,
                fwfs_dir_find_match, &(struct fwfs_dir_find_match){
                    fwfs, "firmwarefs", 10},
                    !need_compact ? &need_compact : NULL);
        if (tag < 0) {
            err = tag;
            goto cleanup;
        }

        // has superblock?
        if (tag && !fwfs_tag_isdelete(tag)) {
            // update root
            fwfs->root[0] = dir.pair[0];
            fwfs->root[1] = dir.pair[1];

            // grab superblock
            fwfs_superblock_t superblock;
            tag = fwfs_dir_get(fwfs, &dir, FWFS_MKTAG(0x7ff, 0x3ff, 0),
                    FWFS_MKTAG(FWFS_TYPE_INLINESTRUCT, 0, sizeof(superblock)),
                    &superblock);
            if (tag < 0) {
                err = tag;
                goto cleanup;
            }
            fwfs_superblock_fromle32(&superblock);

            // check version
            uint16_t major_version = (0xffff & (superblock.version >> 16));
            uint16_t minor_version = (0xffff & (superblock.version >>  0));
            if ((major_version != FWFS_DISK_VERSION_MAJOR ||
                 minor_version > FWFS_DISK_VERSION_MINOR)) {
                FWFS_ERROR("Invalid version v%"PRIu16".%"PRIu16,
                        major_version, minor_version);
                err = FWFS_ERR_INVAL;
                goto cleanup;
            }

            // check superblock configuration
            if (superblock.name_max) {
                if (superblock.name_max > fwfs->name_max) {
                    FWFS_ERROR("Unsupported name_max (%"PRIu32" > %"PRIu32")",
                            superblock.name_max, fwfs->name_max);
                    err = FWFS_ERR_INVAL;
                    goto cleanup;
                }

                fwfs->name_max = superblock.name_max;
            }

            if (superblock.file_max) {
                if (superblock.file_max > fwfs->file_max) {
                    FWFS_ERROR("Unsupported file_max (%"PRIu32" > %"PRIu32")",
                            superblock.file_max, fwfs->file_max);
                    err = FWFS_ERR_INVAL;
                    goto cleanup;
                }

                fwfs->file_max = superblock.file_max;
            }

            if (superblock.attr_max) {
                if (superblock.attr_max > fwfs->attr_max) {
                    FWFS_ERROR("Unsupported attr_max (%"PRIu32" > %"PRIu32")",
                            superblock.attr_max, fwfs->attr_max);
                    err = FWFS_ERR_INVAL;
                    goto cleanup;
                }

                fwfs->attr_max = superblock.attr_max;
            }

            // check tftl version
            major_version = (0xffff & (superblock.tftl_version >> 16));
            minor_version = (0xffff & (superblock.tftl_version >>  0));
            if ((major_version != FWFS_TFTL_DISK_VERSION_MAJOR ||
                 minor_version > FWFS_TFTL_DISK_VERSION_MINOR)) {
                FWFS_ERROR("Invalid tftl version v%"PRIu16".%"PRIu16,
                          major_version, minor_version);
                err = FWFS_ERR_INVAL;
                goto cleanup;
            }

            if (superblock.subblock_size != fwfs->cfg->subblock_size) {
                FWFS_ERROR("Unsupported subblock_size (%"PRIu32" != %"PRIu32")",
                          superblock.subblock_size, fwfs->cfg->subblock_size);
                err = FWFS_ERR_INVAL;
                goto cleanup;
            }
        }

        // has gstate?
        err = fwfs_dir_getgstate(fwfs, &dir, &fwfs->gstate);
        if (err) {
            goto cleanup;
        }
    }

    // found superblock?
    if (fwfs_pair_isnull(fwfs, fwfs->root)) {
        err = FWFS_ERR_INVAL;
        goto cleanup;
    }

    // update firmwarefs with gstate
    if (!fwfs_gstate_iszero(&fwfs->gstate)) {
        FWFS_DEBUG("Found pending gstate 0x%08"PRIx32"%08"PRIx32"%08"PRIx32,
                fwfs->gstate.tag,
                fwfs->gstate.pair[0],
                fwfs->gstate.pair[1]);
    }
    fwfs->gstate.tag += !fwfs_tag_isvalid(fwfs->gstate.tag);
    fwfs->gdisk = fwfs->gstate;

    // setup free lookahead
    fwfs_alloc_reset(fwfs);

    if (fwfs_is_forcecompact_enabled(fwfs) && need_compact) {
        err = fwfs_fs_forceconsistency(fwfs);
        if (err) {
            goto cleanup;
        }

        err = fwfs_fs_forcecompact(fwfs);
        if (err) {
            goto cleanup;
        }
    }
    FWFS_TRACE("fwfs_mount -> %d", 0);
    return 0;

cleanup:
    fwfs_unmount(fwfs);
    FWFS_TRACE("fwfs_mount -> %d", err);
    return err;
}

int fwfs_unmount(fwfs_t *fwfs) {
    FWFS_TRACE("fwfs_unmount(%p)", (void*)fwfs);
    int err = fwfs_deinit(fwfs);
    FWFS_TRACE("fwfs_unmount -> %d", err);
    return err;
}


/// Filesystem filesystem operations ///
int fwfs_fs_traverseraw(fwfs_t *fwfs,
        int (*cb)(void *data, fwfs_block_t block, fwfs_off_t index,
                  uint32_t fid),
        void *data, bool includeorphans) {
    // iterate over metadata pairs
    fwfs_mdir_t dir = {.tail = {fwfs->b0, fwfs->b1}};;

#ifdef FWFS_MIGRATE
    // also consider v1 blocks during migration
    if (fwfs->fwfs1) {
        int err = fwfs1_traverse(fwfs, cb, data);
        if (err) {
            return err;
        }

        dir.tail[0] = fwfs->root[0];
        dir.tail[1] = fwfs->root[1];
    }
#endif

    fwfs_block_t cycle = 0;
    while (!fwfs_pair_isnull(fwfs, dir.tail)) {
        if (cycle >= fwfs->cfg->block_count/2) {
            // loop detected
            return FWFS_ERR_CORRUPT;
        }
        cycle += 1;

        for (int i = 0; i < 2; i++) {
            int err = cb(data, dir.tail[i], FWFS_INDEX_NULL, FWFS_FID_NULL);
            if (err) {
                return err;
            }
        }

        // iterate through ids in directory
        int err = fwfs_dir_fetch(fwfs, &dir, dir.tail);
        if (err) {
            return err;
        }

        for (uint16_t id = 0; id < dir.count; id++) {
            struct fwfs_ctz ctz;
            fwfs_stag_t tag = fwfs_dir_get(fwfs, &dir, FWFS_MKTAG(0x700, 0x3ff, 0),
                    FWFS_MKTAG(FWFS_TYPE_STRUCT, id, sizeof(ctz)), &ctz);
            if (tag < 0) {
                if (tag == FWFS_ERR_NOENT) {
                    continue;
                }
                return tag;
            }
            fwfs_ctz_fromle32(&ctz);

            if (fwfs_tag_type3(tag) == FWFS_TYPE_CTZSTRUCT) {
                err = fwfs_ctz_traverse(fwfs, NULL, &fwfs->rcache_pool,
                        ctz.head, ctz.size, ctz.fid, cb, data);
                if (err) {
                    return err;
                }
            } else if (includeorphans &&
                    fwfs_tag_type3(tag) == FWFS_TYPE_DIRSTRUCT) {
                for (int i = 0; i < 2; i++) {
                    err = cb(data, (&ctz.head)[i], FWFS_INDEX_NULL,
                             FWFS_FID_NULL);
                    if (err) {
                        return err;
                    }
                }
            }
        }
    }

    // iterate over any open files
    for (fwfs_file_t *f = (fwfs_file_t*)fwfs->mlist; f; f = f->next) {
        if (f->type != FWFS_TYPE_REG) {
            continue;
        }

        if ((f->flags & FWFS_F_DIRTY) && !(f->flags & FWFS_F_INLINE)) {
            int err = fwfs_ctz_traverse(fwfs, &f->cache, &fwfs->rcache_pool,
                    f->ctz.head, f->ctz.size, f->ctz.fid, cb, data);
            if (err) {
                return err;
            }
        }

        if ((f->flags & FWFS_F_WRITING) && !(f->flags & FWFS_F_INLINE)) {
            int err = fwfs_ctz_traverse(fwfs, &f->cache, &fwfs->rcache_pool,
                    f->block, f->pos, f->ctz.fid, cb, data);
            if (err) {
                return err;
            }
        }
    }

    return 0;
}

int fwfs_fs_traverse(fwfs_t *fwfs,
        int (*cb)(void *data, fwfs_block_t block, fwfs_off_t index, uint32_t fid), void *data) {
    FWFS_TRACE("fwfs_fs_traverse(%p, %p, %p)",
            (void*)fwfs, (void*)(uintptr_t)cb, data);
    int err = fwfs_fs_traverseraw(fwfs, cb, data, true);
    FWFS_TRACE("fwfs_fs_traverse -> %d", 0);
    return err;
}

static int fwfs_fs_pred(fwfs_t *fwfs,
        const fwfs_block_t pair[2], fwfs_mdir_t *pdir) {
    // iterate over all directory directory entries
    pdir->tail[0] = fwfs->b0;
    pdir->tail[1] = fwfs->b1;
    fwfs_block_t cycle = 0;
    while (!fwfs_pair_isnull(fwfs, pdir->tail)) {
        if (cycle >= fwfs->cfg->block_count/2) {
            // loop detected
            return FWFS_ERR_CORRUPT;
        }
        cycle += 1;

        if (fwfs_pair_cmp(pdir->tail, pair) == 0) {
            return 0;
        }

        int err = fwfs_dir_fetch(fwfs, pdir, pdir->tail);
        if (err) {
            return err;
        }
    }

    return FWFS_ERR_NOENT;
}

struct fwfs_fs_parent_match {
    fwfs_t *fwfs;
    const fwfs_block_t pair[2];
};

static int fwfs_fs_parent_match(void *data,
        fwfs_tag_t tag, const void *buffer) {
    struct fwfs_fs_parent_match *find = data;
    fwfs_t *fwfs = find->fwfs;
    const struct fwfs_diskoff *disk = buffer;
    (void)tag;
    fwfs_size_t block_size = fwfs_block_size_wrapper(fwfs, disk->block);

    fwfs_block_t child[2];
    int err = fwfs_bd_read(fwfs,
            &fwfs->pcache, &fwfs->rcache_pool, block_size,
            disk->block, disk->off, &child, sizeof(child));
    if (err) {
        return err;
    }

    fwfs_pair_fromle32(child);
    return (fwfs_pair_cmp(child, find->pair) == 0) ? FWFS_CMP_EQ : FWFS_CMP_LT;
}

static fwfs_stag_t fwfs_fs_parent(fwfs_t *fwfs, const fwfs_block_t pair[2],
        fwfs_mdir_t *parent) {
    // use fetchmatch with callback to find pairs
    parent->tail[0] = fwfs->b0;
    parent->tail[1] = fwfs->b1;
    fwfs_block_t cycle = 0;
    while (!fwfs_pair_isnull(fwfs, parent->tail)) {
        if (cycle >= fwfs->cfg->block_count/2) {
            // loop detected
            return FWFS_ERR_CORRUPT;
        }
        cycle += 1;

        fwfs_stag_t tag = fwfs_dir_fetchmatch(fwfs, parent, parent->tail,
                FWFS_MKTAG(0x7ff, 0, 0x3ff),
                FWFS_MKTAG(FWFS_TYPE_DIRSTRUCT, 0, 8),
                NULL,
                fwfs_fs_parent_match, &(struct fwfs_fs_parent_match){
                    fwfs, {pair[0], pair[1]}});
        if (tag && tag != FWFS_ERR_NOENT) {
            return tag;
        }
    }

    return FWFS_ERR_NOENT;
}

static fwfs_size_t fwfs_fs_mdir_dirname(fwfs_t *fwfs, fwfs_block_t pair[2],
                                        char *buffer, fwfs_size_t buflen) {
    fwfs_mdir_t parent;
    fwfs_size_t index = 0;
    fwfs_stag_t tag;
    fwfs_stag_t ntag;
    char name[FWFS_NAME_MAX+1];
    fwfs_block_t cycle;

    if (!buffer || buflen < (FWFS_NAME_MAX + 1))
        return FWFS_ERR_INVAL;

    //reserve space for '\0'
    buflen -= 1;
    while (true) {
        // use fetchmatch with callback to find pairs
        parent.tail[0] = fwfs->b0;
        parent.tail[1] = fwfs->b1;
        cycle = 0;

        if (fwfs_pair_cmp(parent.tail, pair) == 0) {
            if (index == 0) {
                snprintf(buffer, buflen, "/");
                index = 1;
            }
            break;
        }

        while (!fwfs_pair_isnull(fwfs, parent.tail)) {
            if (cycle >= fwfs->cfg->block_count/2) {
                // loop detected
                return FWFS_ERR_CORRUPT;
            }
            cycle += 1;

            tag = fwfs_dir_fetchmatch(fwfs, &parent, parent.tail,
                                      FWFS_MKTAG(0x7ff, 0, 0x3ff),
                                      FWFS_MKTAG(FWFS_TYPE_DIRSTRUCT,
                                                 0, 8),
                                      NULL,
                                      fwfs_fs_parent_match,
                                      &(struct fwfs_fs_parent_match){
                                          fwfs, {pair[0], pair[1]}});

            if (tag && tag != FWFS_ERR_NOENT) {
                ntag = fwfs_dir_get(fwfs, &parent, FWFS_MKTAG(0x780, 0x3ff, 0),
                                    FWFS_MKTAG(FWFS_TYPE_NAME, fwfs_tag_id(tag),
                                               fwfs->name_max+1), name);
                if (ntag && ntag != FWFS_ERR_NOENT) {
                    fwfs_size_t len = strlen(name);

                    // (... + 1) to reserve space for prefix '/'
                    if ((index + len + 1) > buflen) {
                        len = buflen - (index + 1);
                    }

                    memmove(buffer + len + 1, buffer, index);
                    index += len + 1;
                    buffer[index] = '\0';

                    buffer[0] = '/';
                    memcpy(buffer + 1, name, len);

                    pair[0] = parent.pair[0];
                    pair[1] = parent.pair[1];
                    break;
                }
                return FWFS_ERR_CORRUPT;
            }

            fwfs_block_t head[2] = {fwfs->bn, fwfs->bn};

            tag = fwfs_dir_get(fwfs, &parent,
                               FWFS_MKTAG(0x7ff, 0x3ff, 0x3ff),
                               FWFS_MKTAG(FWFS_TYPE_TAIL + 1, 0x3ff, 8),
                               &head);
            if (tag && tag != FWFS_ERR_NOENT) {
                fwfs_pair_fromle32(head);
                if (fwfs_pair_cmp(head, pair) == 0) {
                    pair[0] = head[0];
                    pair[1] = head[1];
                    break;
                }
            }
        }

        if (index >= buflen) {
            break;
        }
    }

    return index;
}

static int fwfs_fs_relocate(fwfs_t *fwfs,
        const fwfs_block_t oldpair[2], fwfs_block_t newpair[2]) {
    // update internal root
    if (fwfs_pair_cmp(oldpair, fwfs->root) == 0) {
        fwfs->root[0] = newpair[0];
        fwfs->root[1] = newpair[1];
    }

    // update internally tracked dirs
    for (struct fwfs_mlist *d = fwfs->mlist; d; d = d->next) {
        if (fwfs_pair_cmp(oldpair, d->m.pair) == 0) {
            d->m.pair[0] = newpair[0];
            d->m.pair[1] = newpair[1];
        }

        if (d->type == FWFS_TYPE_DIR &&
                fwfs_pair_cmp(oldpair, ((fwfs_dir_t*)d)->head) == 0) {
            ((fwfs_dir_t*)d)->head[0] = newpair[0];
            ((fwfs_dir_t*)d)->head[1] = newpair[1];
        }
    }

    // find parent
    fwfs_mdir_t parent;
    fwfs_stag_t tag = fwfs_fs_parent(fwfs, oldpair, &parent);
    if (tag < 0 && tag != FWFS_ERR_NOENT) {
        return tag;
    }

    if (tag != FWFS_ERR_NOENT) {
        // update disk, this creates a desync
        fwfs_fs_preporphans(fwfs, +1);

        // fix pending move in this pair? this looks like an optimization but
        // is in fact _required_ since relocating may outdate the move.
        uint16_t moveid = 0x3ff;
        if (fwfs_gstate_hasmovehere(&fwfs->gstate, parent.pair)) {
            moveid = fwfs_tag_id(fwfs->gstate.tag);
            FWFS_DEBUG("Fixing move while relocating "
                    "{0x%"PRIx32", 0x%"PRIx32"} 0x%"PRIx16"\n",
                    parent.pair[0], parent.pair[1], moveid);
            fwfs_fs_prepmove(fwfs, 0x3ff, NULL);
            if (moveid < fwfs_tag_id(tag)) {
                tag -= FWFS_MKTAG(0, 1, 0);
            }
        }

        fwfs_pair_tole32(newpair);
        int err = fwfs_dir_commit(fwfs, &parent, FWFS_MKATTRS(
                {FWFS_MKTAG_IF(moveid != 0x3ff,
                    FWFS_TYPE_DELETE, moveid, 0), NULL},
                {tag, newpair}));
        fwfs_pair_fromle32(newpair);
        if (err) {
            return err;
        }

        // next step, clean up orphans
        fwfs_fs_preporphans(fwfs, -1);
    }

    // find pred
    int err = fwfs_fs_pred(fwfs, oldpair, &parent);
    if (err && err != FWFS_ERR_NOENT) {
        return err;
    }

    // if we can't find dir, it must be new
    if (err != FWFS_ERR_NOENT) {
        char dirname[FWFS_NAME_MAX+1];

        err = fwfs_fs_mdir_dirname(fwfs, parent.pair, dirname,
                                  sizeof(dirname));
        if (err <= 0) {
            return err;
        }
        parent.force_compact = fwfs_folder_compress_policy(fwfs,dirname);

        // fix pending move in this pair? this looks like an optimization but
        // is in fact _required_ since relocating may outdate the move.
        uint16_t moveid = 0x3ff;
        if (fwfs_gstate_hasmovehere(&fwfs->gstate, parent.pair)) {
            moveid = fwfs_tag_id(fwfs->gstate.tag);
            FWFS_DEBUG("Fixing move while relocating "
                    "{0x%"PRIx32", 0x%"PRIx32"} 0x%"PRIx16"\n",
                    parent.pair[0], parent.pair[1], moveid);
            fwfs_fs_prepmove(fwfs, 0x3ff, NULL);
        }

        // replace bad pair, either we clean up desync, or no desync occured
        fwfs_pair_tole32(newpair);
        err = fwfs_dir_commit(fwfs, &parent, FWFS_MKATTRS(
                {FWFS_MKTAG_IF(moveid != 0x3ff,
                    FWFS_TYPE_DELETE, moveid, 0), NULL},
                {FWFS_MKTAG(FWFS_TYPE_TAIL + parent.split, 0x3ff, 8), newpair}));
        fwfs_pair_fromle32(newpair);
        if (err) {
            return err;
        }
    }

    return 0;
}

static void fwfs_fs_preporphans(fwfs_t *fwfs, int8_t orphans) {
    FWFS_ASSERT(fwfs_tag_size(fwfs->gstate.tag) > 0 || orphans >= 0);
    fwfs->gstate.tag += orphans;
    fwfs->gstate.tag = ((fwfs->gstate.tag & ~FWFS_MKTAG(0x800, 0, 0)) |
            ((uint32_t)fwfs_gstate_hasorphans(&fwfs->gstate) << 31));
}

static void fwfs_fs_prepmove(fwfs_t *fwfs,
        uint16_t id, const fwfs_block_t pair[2]) {
    fwfs->gstate.tag = ((fwfs->gstate.tag & ~FWFS_MKTAG(0x7ff, 0x3ff, 0)) |
            ((id != 0x3ff) ? FWFS_MKTAG(FWFS_TYPE_DELETE, id, 0) : 0));
    fwfs->gstate.pair[0] = (id != 0x3ff) ? pair[0] : 0;
    fwfs->gstate.pair[1] = (id != 0x3ff) ? pair[1] : 0;
}

static int fwfs_fs_demove(fwfs_t *fwfs) {
    if (!fwfs_gstate_hasmove(&fwfs->gdisk)) {
        return 0;
    }

    // Fix bad moves
    FWFS_DEBUG("Fixing move {0x%"PRIx32", 0x%"PRIx32"} 0x%"PRIx16,
            fwfs->gdisk.pair[0],
            fwfs->gdisk.pair[1],
            fwfs_tag_id(fwfs->gdisk.tag));

    // fetch and delete the moved entry
    fwfs_mdir_t movedir;
    int err = fwfs_dir_fetch(fwfs, &movedir, fwfs->gdisk.pair);
    if (err) {
        return err;
    }

    // prep gstate and delete move id
    uint16_t moveid = fwfs_tag_id(fwfs->gdisk.tag);
    fwfs_fs_prepmove(fwfs, 0x3ff, NULL);
    err = fwfs_dir_commit(fwfs, &movedir, FWFS_MKATTRS(
            {FWFS_MKTAG(FWFS_TYPE_DELETE, moveid, 0), NULL}));
    if (err) {
        return err;
    }

    return 0;
}

static int fwfs_fs_deorphan(fwfs_t *fwfs) {
    if (!fwfs_gstate_hasorphans(&fwfs->gstate)) {
        return 0;
    }

    // Fix any orphans
    fwfs_mdir_t pdir = {.split = true, .tail = {fwfs->b0, fwfs->b1}};
    fwfs_mdir_t dir;

    // iterate over all directory directory entries
    while (!fwfs_pair_isnull(fwfs, pdir.tail)) {
        int err = fwfs_dir_fetch(fwfs, &dir, pdir.tail);
        if (err) {
            return err;
        }

        // check head blocks for orphans
        if (!pdir.split) {
            // check if we have a parent
            fwfs_mdir_t parent;
            fwfs_stag_t tag = fwfs_fs_parent(fwfs, pdir.tail, &parent);
            if (tag < 0 && tag != FWFS_ERR_NOENT) {
                return tag;
            }

            if (tag == FWFS_ERR_NOENT) {
                // we are an orphan
                FWFS_DEBUG("Fixing orphan {0x%"PRIx32", 0x%"PRIx32"}",
                        pdir.tail[0], pdir.tail[1]);

                err = fwfs_dir_drop(fwfs, &pdir, &dir);
                if (err) {
                    return err;
                }

                // refetch tail
                continue;
            }

            fwfs_block_t pair[2];
            fwfs_stag_t res = fwfs_dir_get(fwfs, &parent,
                    FWFS_MKTAG(0x7ff, 0x3ff, 0), tag, pair);
            if (res < 0) {
                return res;
            }
            fwfs_pair_fromle32(pair);

            if (!fwfs_pair_sync(pair, pdir.tail)) {
                // we have desynced
                FWFS_DEBUG("Fixing half-orphan {0x%"PRIx32", 0x%"PRIx32"} "
                            "-> {0x%"PRIx32", 0x%"PRIx32"}",
                        pdir.tail[0], pdir.tail[1], pair[0], pair[1]);

                fwfs_pair_tole32(pair);
                err = fwfs_dir_commit(fwfs, &pdir, FWFS_MKATTRS(
                        {FWFS_MKTAG(FWFS_TYPE_SOFTTAIL, 0x3ff, 8), pair}));
                fwfs_pair_fromle32(pair);
                if (err) {
                    return err;
                }

                // refetch tail
                continue;
            }
        }

        pdir = dir;
    }

    // mark orphans as fixed
    fwfs_fs_preporphans(fwfs, -fwfs_gstate_getorphans(&fwfs->gstate));
    return 0;
}

static int fwfs_fs_forceconsistency(fwfs_t *fwfs) {
    int err = fwfs_fs_demove(fwfs);
    if (err) {
        return err;
    }

    err = fwfs_fs_deorphan(fwfs);
    if (err) {
        return err;
    }

    return 0;
}

static int fwfs_fs_forcecompact(fwfs_t *fwfs) {
    fwfs_mdir_t dir = {.tail = {fwfs->b0, fwfs->b1}};
    fwfs_block_t cycle = 0;
    int err;
    bool need_compact;

    while (!fwfs_pair_isnull(fwfs, dir.tail)) {
        if (cycle >= fwfs->cfg->block_count/2) {
            // loop detected
            err = FWFS_ERR_CORRUPT;
            return err;
        }
        cycle += 1;

        err = (int)fwfs_dir_fetchmatchraw(fwfs, &dir, dir.tail,
                                          (fwfs_tag_t)-1, (fwfs_tag_t)-1,
                                          NULL, NULL, NULL, &need_compact);
        if (err) {
            return err;
        }

        if (!need_compact)
            continue;

        err = fwfs_dir_compact(fwfs, &dir, NULL, 0, &dir, 0, dir.count);
        if (err)
            return err;
    }

    return 0;
}

static int fwfs_fs_size_count(void *p, fwfs_block_t block, fwfs_off_t index,
                              uint32_t fid) {
    (void)block;
    (void)index;
    (void)fid;
    fwfs_size_t *size = p;
    *size += 1;
    return 0;
}

fwfs_ssize_t fwfs_fs_size(fwfs_t *fwfs) {
    FWFS_TRACE("fwfs_fs_size(%p)", (void*)fwfs);
    fwfs_size_t size = 0;
    int err;

    if (fwfs_tftl_isenabled(fwfs->cfg)) {
        err = fwfs_tftl_fs_size(fwfs, &size);
    } else {

        err = fwfs_fs_traverseraw(fwfs, fwfs_fs_size_count, &size, false);
        if (err) {
            FWFS_TRACE("fwfs_fs_size -> %d", err);
            return err;
        }
    }
    FWFS_TRACE("fwfs_fs_size -> %d", err);
    return size;
}

#ifdef FWFS_MIGRATE
////// Migration from littefwfs v1 below this //////

/// Version info ///

// Software library version
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define FWFS1_VERSION 0x00010007
#define FWFS1_VERSION_MAJOR (0xffff & (FWFS1_VERSION >> 16))
#define FWFS1_VERSION_MINOR (0xffff & (FWFS1_VERSION >>  0))

// Version of On-disk data structures
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define FWFS1_DISK_VERSION 0x00010001
#define FWFS1_DISK_VERSION_MAJOR (0xffff & (FWFS1_DISK_VERSION >> 16))
#define FWFS1_DISK_VERSION_MINOR (0xffff & (FWFS1_DISK_VERSION >>  0))


/// v1 Definitions ///

// File types
enum fwfs1_type {
    FWFS1_TYPE_REG        = 0x11,
    FWFS1_TYPE_DIR        = 0x22,
    FWFS1_TYPE_SUPERBLOCK = 0x2e,
};

typedef struct fwfs1 {
    fwfs_block_t root[2];
} fwfs1_t;

typedef struct fwfs1_entry {
    fwfs_off_t off;

    struct fwfs1_disk_entry {
        uint8_t type;
        uint8_t elen;
        uint8_t alen;
        uint8_t nlen;
        union {
            struct {
                fwfs_block_t head;
                fwfs_size_t size;
            } file;
            fwfs_block_t dir[2];
        } u;
    } d;
} fwfs1_entry_t;

typedef struct fwfs1_dir {
    struct fwfs1_dir *next;
    fwfs_block_t pair[2];
    fwfs_off_t off;

    fwfs_block_t head[2];
    fwfs_off_t pos;

    struct fwfs1_disk_dir {
        uint32_t rev;
        fwfs_size_t size;
        fwfs_block_t tail[2];
    } d;
} fwfs1_dir_t;

typedef struct fwfs1_superblock {
    fwfs_off_t off;

    struct fwfs1_disk_superblock {
        uint8_t type;
        uint8_t elen;
        uint8_t alen;
        uint8_t nlen;
        fwfs_block_t root[2];
        uint32_t block_size;
        uint32_t block_count;
        uint32_t version;
        char magic[8];
    } d;
} fwfs1_superblock_t;


/// Low-level wrappers v1->v2 ///
static void fwfs1_crc(uint32_t *crc, const void *buffer, size_t size) {
    *crc = fwfs_crc(*crc, buffer, size);
}

static int fwfs1_bd_read(fwfs_t *fwfs, fwfs_block_t block,
        fwfs_off_t off, void *buffer, fwfs_size_t size) {
    // if we ever do more than writes to alternating pairs,
    // this may need to consider pcache
    return fwfs_bd_read(fwfs, &fwfs->pcache, &fwfs->rcache_pool, size,
            block, off, buffer, size);
}

static int fwfs1_bd_crc(fwfs_t *fwfs, fwfs_block_t block,
        fwfs_off_t off, fwfs_size_t size, uint32_t *crc) {
    for (fwfs_off_t i = 0; i < size; i++) {
        uint8_t c;
        int err = fwfs1_bd_read(fwfs, block, off+i, &c, 1);
        if (err) {
            return err;
        }

        fwfs1_crc(crc, &c, 1);
    }

    return 0;
}


/// Endian swapping functions ///
static void fwfs1_dir_fromle32(struct fwfs1_disk_dir *d) {
    d->rev     = fwfs_fromle32(d->rev);
    d->size    = fwfs_fromle32(d->size);
    d->tail[0] = fwfs_fromle32(d->tail[0]);
    d->tail[1] = fwfs_fromle32(d->tail[1]);
}

static void fwfs1_dir_tole32(struct fwfs1_disk_dir *d) {
    d->rev     = fwfs_tole32(d->rev);
    d->size    = fwfs_tole32(d->size);
    d->tail[0] = fwfs_tole32(d->tail[0]);
    d->tail[1] = fwfs_tole32(d->tail[1]);
}

static void fwfs1_entry_fromle32(struct fwfs1_disk_entry *d) {
    d->u.dir[0] = fwfs_fromle32(d->u.dir[0]);
    d->u.dir[1] = fwfs_fromle32(d->u.dir[1]);
}

static void fwfs1_entry_tole32(struct fwfs1_disk_entry *d) {
    d->u.dir[0] = fwfs_tole32(d->u.dir[0]);
    d->u.dir[1] = fwfs_tole32(d->u.dir[1]);
}

static void fwfs1_superblock_fromle32(struct fwfs1_disk_superblock *d) {
    d->root[0]     = fwfs_fromle32(d->root[0]);
    d->root[1]     = fwfs_fromle32(d->root[1]);
    d->block_size  = fwfs_fromle32(d->block_size);
    d->block_count = fwfs_fromle32(d->block_count);
    d->version     = fwfs_fromle32(d->version);
}


///// Metadata pair and directory operations ///
static inline fwfs_size_t fwfs1_entry_size(const fwfs1_entry_t *entry) {
    return 4 + entry->d.elen + entry->d.alen + entry->d.nlen;
}

static int fwfs1_dir_fetch(fwfs_t *fwfs,
        fwfs1_dir_t *dir, const fwfs_block_t pair[2]) {
    // copy out pair, otherwise may be aliasing dir
    const fwfs_block_t tpair[2] = {pair[0], pair[1]};
    bool valid = false;

    // check both blocks for the most recent revision
    for (int i = 0; i < 2; i++) {
        struct fwfs1_disk_dir test;
        int err = fwfs1_bd_read(fwfs, tpair[i], 0, &test, sizeof(test));
        fwfs1_dir_fromle32(&test);
        if (err) {
            if (err == FWFS_ERR_CORRUPT) {
                continue;
            }
            return err;
        }

        if (valid && fwfs_scmp(test.rev, dir->d.rev) < 0) {
            continue;
        }

        if ((0x7fffffff & test.size) < sizeof(test)+4 ||
            (0x7fffffff & test.size) > fwfs->cfg->block_size) {
            continue;
        }

        uint32_t crc = 0xffffffff;
        fwfs1_dir_tole32(&test);
        fwfs1_crc(&crc, &test, sizeof(test));
        fwfs1_dir_fromle32(&test);
        err = fwfs1_bd_crc(fwfs, tpair[i], sizeof(test),
                (0x7fffffff & test.size) - sizeof(test), &crc);
        if (err) {
            if (err == FWFS_ERR_CORRUPT) {
                continue;
            }
            return err;
        }

        if (crc != 0) {
            continue;
        }

        valid = true;

        // setup dir in case it's valid
        dir->pair[0] = tpair[(i+0) % 2];
        dir->pair[1] = tpair[(i+1) % 2];
        dir->off = sizeof(dir->d);
        dir->d = test;
    }

    if (!valid) {
        FWFS_ERROR("Corrupted dir pair at {0x%"PRIx32", 0x%"PRIx32"}",
                tpair[0], tpair[1]);
        return FWFS_ERR_CORRUPT;
    }

    return 0;
}

static int fwfs1_dir_next(fwfs_t *fwfs, fwfs1_dir_t *dir, fwfs1_entry_t *entry) {
    while (dir->off + sizeof(entry->d) > (0x7fffffff & dir->d.size)-4) {
        if (!(0x80000000 & dir->d.size)) {
            entry->off = dir->off;
            return FWFS_ERR_NOENT;
        }

        int err = fwfs1_dir_fetch(fwfs, dir, dir->d.tail);
        if (err) {
            return err;
        }

        dir->off = sizeof(dir->d);
        dir->pos += sizeof(dir->d) + 4;
    }

    int err = fwfs1_bd_read(fwfs, dir->pair[0], dir->off,
            &entry->d, sizeof(entry->d));
    fwfs1_entry_fromle32(&entry->d);
    if (err) {
        return err;
    }

    entry->off = dir->off;
    dir->off += fwfs1_entry_size(entry);
    dir->pos += fwfs1_entry_size(entry);
    return 0;
}

/// firmwarefs v1 specific operations ///
int fwfs1_traverse(fwfs_t *fwfs, int (*cb)(void*, fwfs_block_t), void *data) {
    if (fwfs_pair_isnull(fwfs, fwfs->fwfs1->root)) {
        return 0;
    }

    // iterate over metadata pairs
    fwfs1_dir_t dir;
    fwfs1_entry_t entry;
    fwfs_block_t cwd[2] = {fwfs->b0, fwfs->b1};

    while (true) {
        for (int i = 0; i < 2; i++) {
            int err = cb(data, cwd[i]);
            if (err) {
                return err;
            }
        }

        int err = fwfs1_dir_fetch(fwfs, &dir, cwd);
        if (err) {
            return err;
        }

        // iterate over contents
        while (dir.off + sizeof(entry.d) <= (0x7fffffff & dir.d.size)-4) {
            err = fwfs1_bd_read(fwfs, dir.pair[0], dir.off,
                    &entry.d, sizeof(entry.d));
            fwfs1_entry_fromle32(&entry.d);
            if (err) {
                return err;
            }

            dir.off += fwfs1_entry_size(&entry);
            if ((0x70 & entry.d.type) == (0x70 & FWFS1_TYPE_REG)) {
                err = fwfs_ctz_traverse(fwfs, NULL, &fwfs->rcache_pool,
                        entry.d.u.file.head, entry.d.u.file.size, cb, data);
                if (err) {
                    return err;
                }
            }
        }

        // we also need to check if we contain a threaded v2 directory
        fwfs_mdir_t dir2 = {.split=true, .tail={cwd[0], cwd[1]}};
        while (dir2.split) {
            err = fwfs_dir_fetch(fwfs, &dir2, dir2.tail);
            if (err) {
                break;
            }

            for (int i = 0; i < 2; i++) {
                err = cb(data, dir2.pair[i]);
                if (err) {
                    return err;
                }
            }
        }

        cwd[0] = dir.d.tail[0];
        cwd[1] = dir.d.tail[1];

        if (fwfs_pair_isnull(fwfs, cwd)) {
            break;
        }
    }

    return 0;
}

static int fwfs1_moved(fwfs_t *fwfs, const void *e) {
    if (fwfs_pair_isnull(fwfs, fwfs->fwfs1->root)) {
        return 0;
    }

    // skip superblock
    fwfs1_dir_t cwd;
    int err = fwfs1_dir_fetch(fwfs, &cwd, (const fwfs_block_t[2]){fwfs->b0, fwfs->b1});
    if (err) {
        return err;
    }

    // iterate over all directory directory entries
    fwfs1_entry_t entry;
    while (!fwfs_pair_isnull(fwfs, cwd.d.tail)) {
        err = fwfs1_dir_fetch(fwfs, &cwd, cwd.d.tail);
        if (err) {
            return err;
        }

        while (true) {
            err = fwfs1_dir_next(fwfs, &cwd, &entry);
            if (err && err != FWFS_ERR_NOENT) {
                return err;
            }

            if (err == FWFS_ERR_NOENT) {
                break;
            }

            if (!(0x80 & entry.d.type) &&
                 memcmp(&entry.d.u, e, sizeof(entry.d.u)) == 0) {
                return true;
            }
        }
    }

    return false;
}

/// Filesystem operations ///
static int fwfs1_mount(fwfs_t *fwfs, struct fwfs1 *fwfs1,
        const struct fwfs_config *cfg) {
    int err = 0;
    {
        err = fwfs_init(fwfs, cfg);
        if (err) {
            return err;
        }

        fwfs->fwfs1 = fwfs1;
        fwfs->fwfs1->root[0] = fwfs->bn;
        fwfs->fwfs1->root[1] = fwfs->bn;

        // setup free lookahead
        fwfs->free.off = 0;
        fwfs->free.size = 0;
        fwfs->free.i = 0;
        fwfs_alloc_ack(fwfs);

        // load superblock
        fwfs1_dir_t dir;
        fwfs1_superblock_t superblock;
        err = fwfs1_dir_fetch(fwfs, &dir, (const fwfs_block_t[2]){fwfs->b0, fwfs->b1});
        if (err && err != FWFS_ERR_CORRUPT) {
            goto cleanup;
        }

        if (!err) {
            err = fwfs1_bd_read(fwfs, dir.pair[0], sizeof(dir.d),
                    &superblock.d, sizeof(superblock.d));
            fwfs1_superblock_fromle32(&superblock.d);
            if (err) {
                goto cleanup;
            }

            fwfs->fwfs1->root[0] = superblock.d.root[0];
            fwfs->fwfs1->root[1] = superblock.d.root[1];
        }

        if (err || memcmp(superblock.d.magic, "firmwarefs", 10) != 0) {
            FWFS_ERROR("Invalid superblock at {0x%"PRIx32", 0x%"PRIx32"}",
                    0, 1);
            err = FWFS_ERR_CORRUPT;
            goto cleanup;
        }

        uint16_t major_version = (0xffff & (superblock.d.version >> 16));
        uint16_t minor_version = (0xffff & (superblock.d.version >>  0));
        if ((major_version != FWFS1_DISK_VERSION_MAJOR ||
             minor_version > FWFS1_DISK_VERSION_MINOR)) {
            FWFS_ERROR("Invalid version v%d.%d", major_version, minor_version);
            err = FWFS_ERR_INVAL;
            goto cleanup;
        }

        return 0;
    }

cleanup:
    fwfs_deinit(fwfs);
    return err;
}

static int fwfs1_unmount(fwfs_t *fwfs) {
    return fwfs_deinit(fwfs);
}

/// v1 migration ///
int fwfs_migrate(fwfs_t *fwfs, const struct fwfs_config *cfg) {
    FWFS_TRACE("fwfs_migrate(%p, %p {.context=%p, "
                ".read=%p, .prog=%p, .erase=%p, .sync=%p, "
                ".read_size=%"PRIu32", .prog_size=%"PRIu32", "
                ".block_size=%"PRIu32", .subblock_size=%"PRIu32", "
                ".block_count=%"PRIu32", .block_cycles=%"PRIu32", "
                ".cache_size=%"PRIu32", .file_cache_size=%"PRIu32", "
                ".lookahead_size=%"PRIu32", .read_buffer=%p, "
                ".prog_buffer=%p, .lookahead_buffer=%p, "
                ".name_max=%"PRIu32", .file_max=%"PRIu32", "
                ".attr_max=%"PRIu32"})",
            (void*)fwfs, (void*)cfg, cfg->context,
            (void*)(uintptr_t)cfg->read, (void*)(uintptr_t)cfg->prog,
            (void*)(uintptr_t)cfg->erase, (void*)(uintptr_t)cfg->sync,
            cfg->read_size, cfg->prog_size, cfg->block_size, cfg->subblock_size,
            cfg->block_count, cfg->block_cycles, cfg->cache_size,
            cfg->file_cache_size, cfg->lookahead_size, cfg->read_buffer,
            cfg->prog_buffer, cfg->lookahead_buffer, cfg->name_max,
            cfg->file_max, cfg->attr_max);
    struct fwfs1 fwfs1;
    int err = fwfs1_mount(fwfs, &fwfs1, cfg);
    if (err) {
        FWFS_TRACE("fwfs_migrate -> %d", err);
        return err;
    }

    {
        // iterate through each directory, copying over entries
        // into new directory
        fwfs1_dir_t dir1;
        fwfs_mdir_t dir2;
        dir1.d.tail[0] = fwfs->fwfs1->root[0];
        dir1.d.tail[1] = fwfs->fwfs1->root[1];
        while (!fwfs_pair_isnull(fwfs, dir1.d.tail)) {
            // iterate old dir
            err = fwfs1_dir_fetch(fwfs, &dir1, dir1.d.tail);
            if (err) {
                goto cleanup;
            }

            // create new dir and bind as temporary pretend root
            err = fwfs_dir_alloc(fwfs, &dir2);
            if (err) {
                goto cleanup;
            }

            dir2.rev = dir1.d.rev;
            dir1.head[0] = dir1.pair[0];
            dir1.head[1] = dir1.pair[1];
            fwfs->root[0] = dir2.pair[0];
            fwfs->root[1] = dir2.pair[1];

            err = fwfs_dir_commit(fwfs, &dir2, NULL, 0);
            if (err) {
                goto cleanup;
            }

            while (true) {
                fwfs1_entry_t entry1;
                err = fwfs1_dir_next(fwfs, &dir1, &entry1);
                if (err && err != FWFS_ERR_NOENT) {
                    goto cleanup;
                }

                if (err == FWFS_ERR_NOENT) {
                    break;
                }

                // check that entry has not been moved
                if (entry1.d.type & 0x80) {
                    int moved = fwfs1_moved(fwfs, &entry1.d.u);
                    if (moved < 0) {
                        err = moved;
                        goto cleanup;
                    }

                    if (moved) {
                        continue;
                    }

                    entry1.d.type &= ~0x80;
                }

                // also fetch name
                char name[FWFS_NAME_MAX+1];
                memset(name, 0, sizeof(name));
                err = fwfs1_bd_read(fwfs, dir1.pair[0],
                        entry1.off + 4+entry1.d.elen+entry1.d.alen,
                        name, entry1.d.nlen);
                if (err) {
                    goto cleanup;
                }

                bool isdir = (entry1.d.type == FWFS1_TYPE_DIR);

                // create entry in new dir
                err = fwfs_dir_fetch(fwfs, &dir2, fwfs->root);
                if (err) {
                    goto cleanup;
                }

                uint16_t id;
                err = fwfs_dir_find(fwfs, &dir2, &(const char*){name}, &id);
                if (!(err == FWFS_ERR_NOENT && id != 0x3ff)) {
                    err = (err < 0) ? err : FWFS_ERR_EXIST;
                    goto cleanup;
                }

                fwfs1_entry_tole32(&entry1.d);
                err = fwfs_dir_commit(fwfs, &dir2, FWFS_MKATTRS(
                        {FWFS_MKTAG(FWFS_TYPE_CREATE, id, 0)},
                        {FWFS_MKTAG_IF_ELSE(isdir,
                            FWFS_TYPE_DIR, id, entry1.d.nlen,
                            FWFS_TYPE_REG, id, entry1.d.nlen),
                                name},
                        {FWFS_MKTAG_IF_ELSE(isdir,
                            FWFS_TYPE_DIRSTRUCT, id, sizeof(entry1.d.u),
                            FWFS_TYPE_CTZSTRUCT, id, sizeof(entry1.d.u)),
                                &entry1.d.u}));
                fwfs1_entry_fromle32(&entry1.d);
                if (err) {
                    goto cleanup;
                }
            }

            if (!fwfs_pair_isnull(fwfs, dir1.d.tail)) {
                // find last block and update tail to thread into fs
                err = fwfs_dir_fetch(fwfs, &dir2, fwfs->root);
                if (err) {
                    goto cleanup;
                }

                while (dir2.split) {
                    err = fwfs_dir_fetch(fwfs, &dir2, dir2.tail);
                    if (err) {
                        goto cleanup;
                    }
                }

                fwfs_pair_tole32(dir2.pair);
                err = fwfs_dir_commit(fwfs, &dir2, FWFS_MKATTRS(
                        {FWFS_MKTAG(FWFS_TYPE_SOFTTAIL, 0x3ff, 8), dir1.d.tail}));
                fwfs_pair_fromle32(dir2.pair);
                if (err) {
                    goto cleanup;
                }
            }

            // Copy over first block to thread into fs. Unfortunately
            // if this fails there is not much we can do.
            FWFS_DEBUG("Migrating {0x%"PRIx32", 0x%"PRIx32"} "
                        "-> {0x%"PRIx32", 0x%"PRIx32"}",
                    fwfs->root[0], fwfs->root[1], dir1.head[0], dir1.head[1]);

            err = fwfs_bd_erase(fwfs, dir1.head[1]);
            if (err) {
                goto cleanup;
            }

            err = fwfs_dir_fetch(fwfs, &dir2, fwfs->root);
            if (err) {
                goto cleanup;
            }

            for (fwfs_off_t i = 0; i < dir2.off; i++) {
                uint8_t dat;
                err = fwfs_bd_read(fwfs,
                        NULL, &fwfs->rcache_pool, dir2.off,
                        dir2.pair[0], i, &dat, 1);
                if (err) {
                    goto cleanup;
                }

                err = fwfs_bd_prog(fwfs,
                        &fwfs->pcache, &fwfs->rcache_pool, true,
                        dir1.head[1], i, &dat, 1);
                if (err) {
                    goto cleanup;
                }
            }

            err = fwfs_bd_flush(fwfs, &fwfs->pcache, &fwfs->rcache_pool, true);
            if (err) {
                goto cleanup;
            }
        }

        // Create new superblock. This marks a successful migration!
        err = fwfs1_dir_fetch(fwfs, &dir1,
                              (const fwfs_block_t[2]){fwfs->b0, fwfs->b1});
        if (err) {
            goto cleanup;
        }

        dir2.pair[0] = dir1.pair[0];
        dir2.pair[1] = dir1.pair[1];
        dir2.rev = dir1.d.rev;
        dir2.off = sizeof(dir2.rev);
        dir2.etag = 0xffffffff;
        dir2.count = 0;
        dir2.tail[0] = fwfs->fwfs1->root[0];
        dir2.tail[1] = fwfs->fwfs1->root[1];
        dir2.erased = false;
        dir2.split = true;

        fwfs_superblock_t superblock = {
            .version     = FWFS_DISK_VERSION,
            .block_size  = fwfs->cfg->block_size,
            .block_count = fwfs->cfg->block_count,
            .name_max    = fwfs->name_max,
            .file_max    = fwfs->file_max,
            .attr_max    = fwfs->attr_max,
        };

        fwfs_superblock_tole32(&superblock);
        err = fwfs_dir_commit(fwfs, &dir2, FWFS_MKATTRS(
                {FWFS_MKTAG(FWFS_TYPE_CREATE, 0, 0)},
                {FWFS_MKTAG(FWFS_TYPE_SUPERBLOCK, 0, 10), "firmwarefs"},
                {FWFS_MKTAG(FWFS_TYPE_INLINESTRUCT, 0, sizeof(superblock)),
                    &superblock}));
        if (err) {
            goto cleanup;
        }

        // sanity check that fetch works
        err = fwfs_dir_fetch(fwfs, &dir2, (const fwfs_block_t[2]){fwfs->b0, fwfs->b1});
        if (err) {
            goto cleanup;
        }

        // force compaction to prevent accidentally mounting v1
        dir2.erased = false;
        err = fwfs_dir_commit(fwfs, &dir2, NULL, 0);
        if (err) {
            goto cleanup;
        }
    }

cleanup:
    fwfs1_unmount(fwfs);
    FWFS_TRACE("fwfs_migrate -> %d", err);
    return err;
}

#endif
