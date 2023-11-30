/*
* fwfs_tftl.c- Sigmastar
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
#include <linux/string.h>
#include "fwfs_util.h"
#include "fwfs_tftl.h"

#define TFTL_MAGIC_METADATA     (0xa)
#define TFTL_MAGIC_CTZ          (0xb)
#define TFTL_MAGIC_NULL         (0xc)
#define TFTL_MAGIC_INLINE       (0xd)

#define TFTL_STATE_BITS_NUM     (0x4)
#define TFTL_STATE_BITS_MASK    (0xf)

#define TFTL_STATE_FREE         (0x0)
#define TFTL_STATE_CTZ          (0x1)
#define TFTL_STATE_METADATA     (0x2)
#define TFTL_STATE_ABANDON      (0x3)
#define TFTL_STATE_UNKNOWN      (0x4)
#define TFTL_STATE_INVALID      (0xf)

#define TFTL_MKLEB(MAGIC, SUBBLOCK_ID, BLOCK_ID)                \
    (((MAGIC & 0xf) << 28) | ((SUBBLOCK_ID & 0x3f) << 22) |     \
     (BLOCK_ID & 0x3fffff))

static bool fwfs_tftl_header_check(fwfs_t *fwfs, fwfs_block_t block,
                                   fwfs_off_t index, uint32_t fid);
static void fwfs_tftl_visitor_prepare(fwfs_t *fwfs);
static void fwfs_tftl_visitor_commit(fwfs_t *fwfs);
static int fwfs_tftl_visitor(void *p, fwfs_block_t block,
                             fwfs_off_t index, uint32_t fid);

static inline fwfs_block_t fwfs_tftl_subblock_id(fwfs_block_t block) {
    return (block & 0x0fc00000) >> 22;
}

static inline uint8_t fwfs_tftl_magic(fwfs_block_t block) {
    return (block & 0xf0000000) >> 28;
}

static inline bool fwfs_tftl_leb_check(fwfs_block_t block, bool ext) {
    uint8_t magic = fwfs_tftl_magic(block);

    switch (magic) {
    case TFTL_MAGIC_METADATA:
    case TFTL_MAGIC_CTZ:
        return true;
    case TFTL_MAGIC_NULL:
    case TFTL_MAGIC_INLINE:
        return ext;
    default:
        return false;
    }
}

static uint8_t fwfs_tftl_get_state(fwfs_t *fwfs, uint8_t *states,
                                  fwfs_block_t block_id,
                                  fwfs_block_t subblock_id)
{
    FWFS_TRACE("fwfs_tftl_get_state(%p, %p, 0x%"PRIx32", 0x%"PRIx32")",
               (void*)fwfs, (void*)states, block_id, subblock_id);
    FWFS_ASSERT(fwfs->cfg->subblock_size != 0);
    uint32_t i, j;
    uint32_t mask;

    // subblocks per block
    i = fwfs->cfg->block_size / fwfs->cfg->subblock_size;
    // subblocks number need to skip
    i = i * block_id;
    // bits need to skip
    i = i * TFTL_STATE_BITS_NUM;
    // extra bits implied by subblock_id need to skip
    i += subblock_id * TFTL_STATE_BITS_NUM;

    j = i % 8;
    i = i / 8;
    mask = TFTL_STATE_BITS_MASK << j;

    FWFS_TRACE("fwfs_tftl_get_state %"PRIu32" %"PRIu32" 0x%"PRIx32" -> "
               "0x%"PRIx32, i, j, mask, (states[i] & mask) >> j);
    return (states[i] & mask) >> j;

}

static void fwfs_tftl_set_state(fwfs_t *fwfs, uint8_t *states,
                               fwfs_block_t block_id,
                               fwfs_block_t subblock_id,
                               uint8_t state)
{
    FWFS_TRACE("fwfs_tftl_set_state(%p, %p, 0x%"PRIx32", 0x%"PRIx32", "
               "0x%"PRIx32")",
               (void*)fwfs, (void*)states, block_id, subblock_id,
               (uint32_t)state);
    FWFS_ASSERT(fwfs->cfg->subblock_size != 0);
    uint32_t i, j;
    uint32_t mask, umask;
    uint8_t nstate;

    // subblocks per block
    i = fwfs->cfg->block_size / fwfs->cfg->subblock_size;
    // subblocks number need to skip
    i = i * block_id;
    // bits need to skip
    i = i * TFTL_STATE_BITS_NUM;
    // extra bits implied by subblock_id need to skip
    i += subblock_id * TFTL_STATE_BITS_NUM;

    j = i % 8;
    i = i / 8;

    mask = TFTL_STATE_BITS_MASK << j;
    umask = ~mask;

    nstate = (states[i] & umask) | ((state & TFTL_STATE_BITS_MASK) << j);
    FWFS_TRACE("fwfs_tftl_set_state %"PRIu32" %"PRIu32" 0x%"PRIx32" "
               "0x%"PRIx32" -> 0x%"PRIx32,
               i, j, mask,
              (uint32_t)((states[i] & mask) >> j),
              (uint32_t)((nstate & mask) >> j));
    states[i] = nstate;
}

static inline void fwfs_tftl_setmetadata(fwfs_t *fwfs, uint8_t *states,
                                        fwfs_block_t block_id)
{
    uint32_t i;

    for (i = 0; i < (fwfs->cfg->block_size / fwfs->cfg->subblock_size); i++)
        fwfs_tftl_set_state(fwfs, states, block_id, i, TFTL_STATE_METADATA);
}

static inline void fwfs_tftl_setctz(fwfs_t *fwfs, uint8_t *states,
                                   fwfs_block_t block_id,
                                   fwfs_block_t subblock_id)
{
    fwfs_tftl_set_state(fwfs, states, block_id, subblock_id, TFTL_STATE_CTZ);
}

static inline void fwfs_tftl_setfree(fwfs_t *fwfs, uint8_t *states,
                                    fwfs_block_t block_id)
{
    uint32_t i;

    for (i = 0; i < (fwfs->cfg->block_size / fwfs->cfg->subblock_size); i++)
        fwfs_tftl_set_state(fwfs, states, block_id, i, TFTL_STATE_FREE);
}

static inline void fwfs_tftl_free_to_unknown(fwfs_t *fwfs, uint8_t *states,
                                             fwfs_block_t block_id)
{
    uint32_t i;
    uint8_t state;

    for (i = 0; i < (fwfs->cfg->block_size / fwfs->cfg->subblock_size); i++) {
        state = fwfs_tftl_get_state(fwfs, fwfs->tftl.states, block_id, i);
        if (state == TFTL_STATE_FREE)
            fwfs_tftl_set_state(fwfs, states, block_id, i, TFTL_STATE_UNKNOWN);
    }
}

static inline bool fwfs_tftl_hasctz(fwfs_t *fwfs, fwfs_block_t block_id)
{
    uint32_t i;

    for (i = 0; i < (fwfs->cfg->block_size / fwfs->cfg->subblock_size); i++) {
        if (fwfs_tftl_get_state(fwfs, fwfs->tftl.states, block_id, i) ==
            TFTL_STATE_CTZ)
            return true;
    }

    return false;
}

static bool fwfs_tftl_isfree(fwfs_t *fwfs, fwfs_block_t block_id)
{
    uint32_t i;
    uint8_t state;

    for (i = 0; i < (fwfs->cfg->block_size / fwfs->cfg->subblock_size); i++) {
        state = fwfs_tftl_get_state(fwfs, fwfs->tftl.states, block_id, i);
        if (state != TFTL_STATE_FREE && state != TFTL_STATE_ABANDON &&
            state != TFTL_STATE_UNKNOWN)
            return false;
    }

    return true;
}

fwfs_block_t fwfs_tftl_block_id(fwfs_block_t block) {
    return block & 0x003fffff;
}

fwfs_size_t fwfs_tftl_states_size(fwfs_t *fwfs) {
    uint32_t i;

    // Subblocks per block
    i = fwfs->cfg->block_size / fwfs->cfg->subblock_size;

    // Total subblocks
    i = i * fwfs->cfg->block_count;

    // Total bits
    i = i * TFTL_STATE_BITS_NUM;

    // Align to byte boundary
    if ((i % 8) != 0)
        i += 8 - (i % 8);

    return i = i / 8;
}

int fwfs_tftl_init(fwfs_t *fwfs) {
    fwfs->b0 = TFTL_MKLEB(TFTL_MAGIC_METADATA, 0, 0);
    fwfs->b1 = TFTL_MKLEB(TFTL_MAGIC_METADATA, 0, 1);
    fwfs->bn = TFTL_MKLEB(TFTL_MAGIC_NULL, 0, 0);
    fwfs->bi = TFTL_MKLEB(TFTL_MAGIC_INLINE, 0, 0);

    fwfs->tftl.size = fwfs_tftl_states_size(fwfs);

    fwfs->tftl.states = (uint8_t  *)fwfs_malloc(fwfs->tftl.size);
    if (!fwfs->tftl.states)
        return FWFS_ERR_NOMEM;
    memset(fwfs->tftl.states, 0, fwfs->tftl.size);


    fwfs->tftl.snapshot = (uint8_t  *)fwfs_malloc(fwfs->tftl.size);
    if (!fwfs->tftl.snapshot)
        return FWFS_ERR_NOMEM;
    memset(fwfs->tftl.snapshot, 0, fwfs->tftl.size);

    fwfs->tftl.checked = false;
    fwfs->tftl.header_size = FWFS_TFTL_HEADER_SIZE;
    return 0;
}

int fwfs_tftl_deinit(fwfs_t *fwfs) {
    if (!fwfs->tftl.states) {
        fwfs_free(fwfs->tftl.states);
    }

    if (!fwfs->tftl.snapshot) {
        fwfs_free(fwfs->tftl.snapshot);
    }

    return 0;
}

void fwfs_tftl_visitor_prepare(fwfs_t *fwfs) {
    memset(fwfs->tftl.snapshot, 0, fwfs->tftl.size);
}

int fwfs_tftl_visitor(void *p, fwfs_block_t block, fwfs_off_t index,
                      uint32_t fid) {
    FWFS_TRACE("fwfs_tftl_visitor(%p, 0x%"PRIx32")", (void*)p, block);
    fwfs_t *fwfs = (fwfs_t*)p;
    (void)index;

    if (fwfs_tftl_magic(block) == TFTL_MAGIC_METADATA) {
        fwfs_block_t block_id;

        block_id = fwfs_tftl_block_id(block);
        fwfs_tftl_setmetadata(fwfs, fwfs->tftl.snapshot, block_id);
    } else if (fwfs_tftl_magic(block) == TFTL_MAGIC_CTZ) {
        fwfs_block_t block_id;
        fwfs_block_t subblock_id;

        if (fwfs->cfg->debug) {
            fwfs_tftl_header_check(fwfs, block, index, fid);
        }

        block_id = fwfs_tftl_block_id(block);
        subblock_id = fwfs_tftl_subblock_id(block);
        fwfs_tftl_setctz(fwfs, fwfs->tftl.snapshot, block_id, subblock_id);
    } else {
        FWFS_ERROR("Invalid LEB 0x%"PRIx32" (0x%x)", block, fwfs_tftl_magic(block));
        FWFS_ASSERT(false);
    }

    return 0;
}

static inline uint8_t fwfs_tftl_state_transfer(uint8_t state, uint8_t snap) {
    switch (state) {
    case TFTL_STATE_FREE:
        switch (snap) {
        case TFTL_STATE_FREE:
            return TFTL_STATE_FREE;
        case TFTL_STATE_CTZ:
            return TFTL_STATE_CTZ;
        case TFTL_STATE_METADATA:
            return TFTL_STATE_METADATA;
        case TFTL_STATE_UNKNOWN:
        default:
            return TFTL_STATE_INVALID;
        }
        break;
    case TFTL_STATE_CTZ:
        switch (snap) {
        case TFTL_STATE_FREE:
            return TFTL_STATE_ABANDON;
        case TFTL_STATE_CTZ:
            return TFTL_STATE_CTZ;
        case TFTL_STATE_METADATA:
        case TFTL_STATE_UNKNOWN:
        default:
            return TFTL_STATE_INVALID;
        }
        break;
    case TFTL_STATE_METADATA:
        switch (snap) {
        case TFTL_STATE_FREE:
            return TFTL_STATE_ABANDON;
        case TFTL_STATE_METADATA:
            return TFTL_STATE_METADATA;
        case TFTL_STATE_CTZ:
        case TFTL_STATE_UNKNOWN:
        default:
            return TFTL_STATE_INVALID;
        }
        break;
    case TFTL_STATE_ABANDON:
        switch (snap) {
        case TFTL_STATE_FREE:
            return TFTL_STATE_ABANDON;
        case TFTL_STATE_CTZ:
        case TFTL_STATE_METADATA:
        case TFTL_STATE_UNKNOWN:
        default:
            return TFTL_STATE_INVALID;
        }
        break;
    case TFTL_STATE_UNKNOWN:
        switch (snap) {
        case TFTL_STATE_FREE:
            return TFTL_STATE_UNKNOWN;
        case TFTL_STATE_CTZ:
        case TFTL_STATE_METADATA:
        case TFTL_STATE_UNKNOWN:
        default:
            return TFTL_STATE_INVALID;
        }
        break;
    default:
        break;
    }
    return TFTL_STATE_INVALID;
}

static bool fwfs_tftl_partial_write_check(fwfs_t *fwfs, fwfs_block_t block) {
    FWFS_TRACE("fwfs_tftl_partial_write_check(%p, 0x%"PRIx32")", (void*)fwfs, block);
    fwfs_tftl_header_t header;
    int err;

    err = fwfs_bd_read_ext(fwfs, sizeof(header), block, 0, &header,
                          sizeof(header));
    if (err) {
        FWFS_TRACE("fwfs_tftl_partial_write_check err -> %d", err);
        return false;
    }

    header.tag = fwfs_frombe32(header.tag);

    FWFS_TRACE("fwfs_tftl_partial_write_check -> 0x%"PRIx32"", header.tag);
    if (header.tag & 0x80000000)
        return true;
    return false;
}

static bool fwfs_tftl_header_check(fwfs_t *fwfs, fwfs_block_t block,
                                   fwfs_off_t index, uint32_t fid) {
    FWFS_TRACE("fwfs_tftl_index_check(%p, 0x%"PRIx32", %"PRIu32")", (void*)fwfs,
               block, index);
    fwfs_tftl_header_t header;
    int err;
    fwfs_off_t tmp;

    err = fwfs_bd_read_ext(fwfs, sizeof(header), block, 0, &header,
                          sizeof(header));
    if (err) {
        FWFS_TRACE("fwfs_tftl_index_check err -> %d", err);
        return false;
    }
    header.tag = fwfs_frombe32(header.tag);

    if (header.tag & 0x80000000) {
        FWFS_ERROR("Invalid header 0x%"PRIx32" in block 0x%"PRIx32
                   "(index:0x%"PRIx32",fid:0x%"PRIx32")",
                   header.tag, block, index, fid);
        FWFS_ASSERT(false);
        return false;
    }

    tmp = header.tag & 0x0000ffff;
    FWFS_TRACE("fwfs_tftl_index_check 0x%"PRIx32" -> %"PRIu32, header.tag, tmp);

    if ((header.tag & 0x7fff0000) != (fid & 0x7fff0000)) {
        FWFS_ERROR("Recorded fid %"PRIu32" doesn't match real "
                   "fid %"PRIu32" for block 0x%"PRIx32"(0x%"PRIx32")",
                   header.tag & 0x7fff0000,
                   fid & 0x7fff0000,
                   block, index);
        FWFS_ASSERT(false);
        return false;
    } else {
        FWFS_TRACE("Recorded fid 0x%"PRIx32" matches real "
                   "fid 0x%"PRIx32"(0x%"PRIx32" for block 0x%"PRIx32,
                   header.tag & 0x7fff0000,
                   fid, (fid & 0x7fff0000), block);
    }

    if (tmp != index) {
        FWFS_ERROR("Recorded index %"PRIu32" doesn't match real "
                   "index %"PRIu32" for block 0x%"PRIx32"(0x%"PRIx32")",
                   tmp, index, block, fid);
        FWFS_ASSERT(false);
        return false;
    } else {
        FWFS_TRACE("Recorded index %"PRIu32" matches real "
                   "index %"PRIu32" for block 0x%"PRIx32,
                   tmp, index, block);
    }
    return true;
}

static bool fwfs_tftl_erased_check(fwfs_t *fwfs, fwfs_block_t block) {
    FWFS_TRACE("fwfs_tftl_erased_check(%p, %"PRIu32")", (void*)fwfs, block);
    int err;
    uint32_t data;
    fwfs_size_t block_size;
    fwfs_off_t off;
    int len;

    if (fwfs_tftl_magic(block) == TFTL_MAGIC_METADATA) {
        block_size = fwfs->cfg->block_size;
    } else if (fwfs_tftl_magic(block) == TFTL_MAGIC_CTZ) {
        block_size = fwfs->cfg->subblock_size;
    } else {
        FWFS_ERROR("Invalid LEB 0x%x (0x%x)", block, fwfs_tftl_magic(block));
        FWFS_ASSERT(false);
        return false;
    }

    len = block_size / sizeof(data);
    off = 0;
    for (int i = 0; i < len; i++) {
        err = fwfs_bd_read_ext(fwfs, block_size, block, off, &data,
                               sizeof(data));
        if (err) {
            FWFS_TRACE("fwfs_tftl_erased_check err -> %d", err);
            return false;
        }

        if (data != 0xffffffff) {
            FWFS_ERROR("Bad data 0x%"PRIx32" at 0x%"PRIx32" in erased block "
                       "0x%"PRIx32, data, off, block);
            FWFS_ASSERT(false);
            return false;
        }
        off += sizeof(data);
    }
    return true;
}

void fwfs_tftl_visitor_commit(fwfs_t *fwfs) {
    uint32_t i, j;
    uint8_t snap;
    uint8_t state;
    uint8_t nstate;

    for (i = 0; i < fwfs->cfg->block_count; i++) {
        for (j = 0; j < (fwfs->cfg->block_size/fwfs->cfg->subblock_size);
             j++) {
            snap = fwfs_tftl_get_state(fwfs, fwfs->tftl.snapshot, i, j);
            state = fwfs_tftl_get_state(fwfs, fwfs->tftl.states, i, j);

            nstate = fwfs_tftl_state_transfer(state, snap);
            if (nstate == TFTL_STATE_INVALID) {
                FWFS_ERROR("Invalid states (0x%x, 0x%x) for %"PRIu32"-%"PRIu32,
                          state, snap, i, j);
                continue;
            }

            if (nstate != state)
                fwfs_tftl_set_state(fwfs, fwfs->tftl.states, i, j, nstate);
        }

        if (!fwfs->tftl.checked) {
            if (fwfs_tftl_hasctz(fwfs, i)) {
                fwfs_tftl_free_to_unknown(fwfs, fwfs->tftl.states, i);
            }
        }
    }

    if (!fwfs->tftl.checked)
        fwfs->tftl.checked = true;
}

static int fwfs_tftl_dump_states(fwfs_t *fwfs) {
    uint32_t i, j, index = 0;
    uint8_t state;
    char *buffer;
    uint32_t buflen = 512;

    buffer = (char *)fwfs_malloc(buflen);
    if (!buffer) {
        FWFS_ERROR("Out of memory!");
        return FWFS_ERR_NOMEM;
    }
    memset(buffer, 0, buflen);

    FWFS_INFO("Firmwarefs states dump:");
    for (i = 0; i < fwfs->cfg->block_count; i++) {
        for (j = 0; j < (fwfs->cfg->block_size/fwfs->cfg->subblock_size);
             j++) {
            state = fwfs_tftl_get_state(fwfs, fwfs->tftl.states, i, j);

            index = strlen(buffer);
            if (j == 0) {
                snprintf(buffer + index, buflen - index, "%04d:", (int)i);
                index += 5;
            }
            snprintf(buffer + index, buflen - index, "%d(0x%x) ", (int)j, state);
        }
        FWFS_INFO("%s", buffer);
        memset(buffer, 0, buflen);
    }
    fwfs_free(buffer);
    return FWFS_ERR_OK;
}

int fwfs_tftl_alloc_block(fwfs_t *fwfs, fwfs_block_t *block) {
    FWFS_TRACE("fwfs_tftl_alloc_block(%p, %p(0x%"PRIx32"))",
               (void*)fwfs, (void*)block, block ? *block : 0x0);
    while (true) {
        while (fwfs->free.i != fwfs->free.size) {
            fwfs_block_t off = fwfs->free.i;
            fwfs->free.i += 1;
            fwfs->free.ack -= 1;

            if (!(fwfs->free.buffer[off / 32] & (1U << (off % 32)))) {
                // found a free block
                *block = fwfs->free.off + off;

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

        if (fwfs->free.ack == 0) {
            FWFS_ERROR("No more free space %"PRIu32,
                       fwfs->free.i + fwfs->free.off);
            if (fwfs->cfg->dump_on_full || fwfs->cfg->debug)
                fwfs_tftl_dump_states(fwfs);
            return FWFS_ERR_NOSPC;
        }

        fwfs->free.off = (fwfs->free.off + fwfs->free.size)
            % fwfs->cfg->block_count;
        fwfs->free.size = fwfs_min(fwfs_min(8*fwfs->cfg->lookahead_size,
                                            fwfs->free.ack),
                                   fwfs->cfg->block_count - fwfs->free.off);
        fwfs->free.i = 0;

        fwfs_tftl_visitor_prepare(fwfs);
        int err = fwfs_fs_traverse(fwfs, fwfs_tftl_visitor, fwfs);
        if (err) {
            fwfs_alloc_reset(fwfs);
            return err;
        }
        fwfs_tftl_visitor_commit(fwfs);

        // find mask of free blocks from tree
        memset(fwfs->free.buffer, 0, fwfs->cfg->lookahead_size);
        for (uint32_t i = 0; i < fwfs->free.size; i++) {
            fwfs_block_t off = i + fwfs->free.off;
            if (!fwfs_tftl_isfree(fwfs, (fwfs_block_t)off)) {
                fwfs->free.buffer[i / 32] |= 1U << (i % 32);
            }
        }
    }
}

int fwfs_tftl_alloc(fwfs_t *fwfs, fwfs_block_t *block, bool ctz) {
    FWFS_TRACE("fwfs_tftl_alloc(%p, %p, %d)", (void*)fwfs, (void*)block, ctz);
    uint8_t state, j, count;
    uint32_t i;
    fwfs_block_t nblock;
    int err;

    if (!ctz) {
        err = fwfs_tftl_alloc_block(fwfs, &nblock);
        if (err)
            return err;

        fwfs_tftl_setmetadata(fwfs, fwfs->tftl.states, nblock);
        *block = TFTL_MKLEB(TFTL_MAGIC_METADATA, 0x0, nblock);
        FWFS_TRACE("fwfs_tftl_alloc 0x%"PRIx32" -> 0x%"PRIx32"", nblock, *block);
        return 0;
    }

    if (fwfs->tftl.checked) {
        fwfs_tftl_visitor_prepare(fwfs);
        err = fwfs_fs_traverse(fwfs, fwfs_tftl_visitor, fwfs);
        if (err) {
            fwfs_alloc_reset(fwfs);
            return err;
        }
        fwfs_tftl_visitor_commit(fwfs);
    }

    count = fwfs->cfg->block_size/fwfs->cfg->subblock_size;
    for (i = 0; i < fwfs->cfg->block_count; i++) {
        if (!fwfs_tftl_hasctz(fwfs, i))
            continue;

        for (j = 0; j < count; j++) {
            state = fwfs_tftl_get_state(fwfs, fwfs->tftl.states, i, j);

            // Do partial write check for unknown state subblock.
            if (state == TFTL_STATE_UNKNOWN) {
                fwfs_block_t leb = TFTL_MKLEB(TFTL_MAGIC_CTZ, j, i);

                if (!fwfs_tftl_partial_write_check(fwfs, leb)) {
                    state = TFTL_STATE_ABANDON;
                } else {
                    state = TFTL_STATE_FREE;
                }
                fwfs_tftl_set_state(fwfs, fwfs->tftl.states, i, j, state);
            }

            // Check whether all following subblocks are in free state or
            // unknown state.
            if (state == TFTL_STATE_FREE) {
                if (j != (count - 1)) {
                    uint8_t tmp;
                    uint8_t m = j + 1;

                    for (; m < count; m++) {
                        tmp = fwfs_tftl_get_state(fwfs, fwfs->tftl.states,
                                                  i, j);
                        if (tmp != TFTL_STATE_FREE &&
                            tmp != TFTL_STATE_UNKNOWN) {
                            FWFS_ERROR("Found in use or abandoned subblock "
                                       "%d after free subblock %d in %"PRIu32,
                                       m, j, i);
                            FWFS_ASSERT(false);
                        }
                    }
                }

                if (fwfs->cfg->debug >= 2) {
                    fwfs_tftl_erased_check(fwfs,
                                           TFTL_MKLEB(TFTL_MAGIC_CTZ, j, i));
                }

                fwfs_tftl_setctz(fwfs, fwfs->tftl.states, i, j);
                *block = TFTL_MKLEB(TFTL_MAGIC_CTZ, j, i);
                FWFS_TRACE("fwfs_tftl_alloc 0x%"PRIx32" 0x%d -> 0x%"PRIx32,
                           i, j, *block);
                return 0;
            }
        }
    }

    err = fwfs_tftl_alloc_block(fwfs, &nblock);
    if (err)
        return err;

    fwfs->cfg->erase(fwfs->cfg, nblock);
    fwfs_tftl_setfree(fwfs, fwfs->tftl.states, nblock);

    fwfs_tftl_setctz(fwfs, fwfs->tftl.states, nblock, 0);
    *block = TFTL_MKLEB(TFTL_MAGIC_CTZ, 0, nblock);
    FWFS_TRACE("fwfs_tftl_alloc 0x%"PRIx32" 0x%"PRIx32" -> 0x%"PRIx32,
               nblock, (fwfs_block_t)0, *block);
    return 0;
}

fwfs_size_t fwfs_tftl_header_size(fwfs_t *fwfs, fwfs_block_t block) {
    (void)fwfs;
    if (fwfs_tftl_magic(block) == TFTL_MAGIC_CTZ) {
        return FWFS_TFTL_HEADER_SIZE;
    } else if (fwfs_tftl_magic(block) == TFTL_MAGIC_METADATA) {
        return 0;
    } else {
        FWFS_ERROR("Invalid LEB 0x%"PRIx32" (0x%x)", block, fwfs_tftl_magic(block));
        FWFS_ASSERT(false);
        return 0;
    }
}

fwfs_size_t fwfs_tftl_block_size(fwfs_t *fwfs, fwfs_block_t block) {
    if (fwfs_tftl_magic(block) == TFTL_MAGIC_CTZ) {
        return fwfs->cfg->subblock_size;
    } else if (fwfs_tftl_magic(block) == TFTL_MAGIC_METADATA) {
        return fwfs->cfg->block_size;
    } else if (fwfs_tftl_magic(block) == TFTL_MAGIC_INLINE) {
        return fwfs->cfg->block_size;
    } else {
        FWFS_ERROR("Invalid LEB 0x%"PRIx32" (0x%x)", block, fwfs_tftl_magic(block));
        FWFS_ASSERT(false);
        return 0;
    }
}

int fwfs_tftl_fs_size(fwfs_t *fwfs, fwfs_size_t *size) {
    uint32_t i, j;
    uint8_t state;
    int err;

    fwfs_tftl_visitor_prepare(fwfs);
    err = fwfs_fs_traverseraw(fwfs, fwfs_tftl_visitor, fwfs, false);
    if (err)
        return err;
    fwfs_tftl_visitor_commit(fwfs);

    *size = 0;
    for (i = 0; i < fwfs->cfg->block_count; i++) {
        state = TFTL_STATE_INVALID;
        for (j = 0; j < (fwfs->cfg->block_size/fwfs->cfg->subblock_size);
             j++) {
            state = fwfs_tftl_get_state(fwfs, fwfs->tftl.states, i, j);
            if (state != TFTL_STATE_FREE)
                break;
        }

        if (state != TFTL_STATE_FREE)
            *size += 1;
    }

    return err;
}

int fwfs_tftl_header_fill(fwfs_t *fwfs, fwfs_tftl_header_t *header,
                          fwfs_block_t block, fwfs_off_t index,
                          uint32_t fid)
{
    (void)fwfs;
    if (!fwfs_tftl_leb_check(block, false)) {
        FWFS_ERROR("Invalid LEB 0x%"PRIx32" (0x%x)", block,
                   fwfs_tftl_magic(block));
        FWFS_ASSERT(false);
        return 0;
    }

    if (fwfs_tftl_magic(block) != TFTL_MAGIC_CTZ) {
        return 0;
    }

    header->tag = index & 0x0000ffff;
    header->tag = header->tag | (fid & 0x7fff0000);
    //header->tag = 0x7f7f7f7f;
    header->tag = header->tag & 0x7fffffff;
    header->tag = fwfs_tobe32(header->tag);

    return 0;
}

int fwfs_tftl_read (const struct fwfs_config *c, fwfs_block_t block,
                   fwfs_off_t off, void *buffer, fwfs_size_t size) {
    FWFS_TRACE("fwfs_tftl_read(%p, 0x%"PRIx32", 0x%"PRIx32", %p, "
               "%"PRIu32")", (void*)c,
               block, off, (void*)buffer, size);
    fwfs_block_t block_id = fwfs_tftl_block_id(block);

    if (fwfs_tftl_magic(block) == TFTL_MAGIC_CTZ) {
        fwfs_block_t subblock_id = fwfs_tftl_subblock_id(block);
        fwfs_off_t offset = subblock_id * c->subblock_size;

        FWFS_TRACE("fwfs_tftl_read(%p, 0x%"PRIx32", 0x%"PRIx32", 0x%"PRIx32")", (void*)c,
                  block_id, subblock_id, offset);
        return c->read(c, block_id, off + offset, buffer, size);
    } else if (fwfs_tftl_magic(block) == TFTL_MAGIC_METADATA) {
        FWFS_TRACE("fwfs_tftl_read(%p, 0x%"PRIx32", 0x%"PRIx32")", (void*)c,
                  block_id, off);
        return c->read(c, block_id, off, buffer, size);
    } else {
        FWFS_ERROR("Invalid LEB 0x%"PRIx32" (0x%x)", block, fwfs_tftl_magic(block));
        FWFS_ASSERT(false);
        return FWFS_ERR_INVAL;
    }
}

int fwfs_tftl_prog(const struct fwfs_config *c, fwfs_block_t block,
                  fwfs_off_t off, const void *buffer, fwfs_size_t size) {
    FWFS_TRACE("fwfs_tftl_prog(%p, 0x%"PRIx32", 0x%"PRIx32", %p, %"PRIu32")",
               (void*)c, block, off, (void*)buffer, size);
    fwfs_block_t block_id = fwfs_tftl_block_id(block);

    if (fwfs_tftl_magic(block) == TFTL_MAGIC_CTZ) {
        fwfs_block_t subblock_id = fwfs_tftl_subblock_id(block);
        fwfs_off_t offset = subblock_id * c->subblock_size;

        FWFS_TRACE("fwfs_tftl_prog(%p, 0x%"PRIx32", 0x%"PRIx32", 0x%"PRIx32")", (void*)c,
                  block_id, subblock_id, offset);
        return c->prog(c, block_id, off + offset, buffer, size);
    } else if (fwfs_tftl_magic(block) == TFTL_MAGIC_METADATA) {
        FWFS_TRACE("fwfs_tftl_prog(%p, 0x%"PRIx32", 0x%"PRIx32")", (void*)c,
                  block_id, off);
        return c->prog(c, block_id, off, buffer, size);
    } else {
        FWFS_ERROR("Invalid LEB 0x%"PRIx32" (0x%x)", block, fwfs_tftl_magic(block));
        FWFS_ASSERT(false);
        return FWFS_ERR_INVAL;
    }
}

int fwfs_tftl_erase(const struct fwfs_config *c, fwfs_block_t block) {
    FWFS_TRACE("fwfs_tftl_erase(%p, 0x%"PRIx32")", (void*)c, block);

    if (fwfs_tftl_magic(block) == TFTL_MAGIC_CTZ) {
        return 0;
    } else if (fwfs_tftl_magic(block) == TFTL_MAGIC_METADATA) {
        return c->erase(c, fwfs_tftl_block_id(block));
    } else {
        FWFS_ERROR("Invalid LEB 0x%"PRIx32" (0x%x)", block, fwfs_tftl_magic(block));
        FWFS_ASSERT(false);
        return FWFS_ERR_INVAL;
    }
}

int fwfs_tftl_sync(const struct fwfs_config *c) {
    FWFS_TRACE("fwfs_tftl_sync(%p)", (void*)c);
    return c->sync(c);
}
