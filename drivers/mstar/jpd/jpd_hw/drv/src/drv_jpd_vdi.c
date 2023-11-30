/*
 * drv_jpd_vdi.c- Sigmastar
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

#if !defined(__I_SW__)
//#include <linux/dma-mapping.h>
#endif

#include "drv_jpd_vdi.h"
#include "hal_jpd_def.h"

//-------------------------------------------------------------------------------------------------
//  Local Compiler Options
//-------------------------------------------------------------------------------------------------
#define AVAIL (__mem_avail__[0])

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define HLEN      (sizeof(MEMT))
#define MIN_BLOCK (HLEN * 4)

//#define AVAIL   (__mem_avail__[0])

#define MIN_POOL_SIZE (HLEN * 10)

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief \b Struct \b Name: __memt__
/// @brief \b Struct \b Description: mpool structure for jpd
//-----------------------------------------------------------------------------
typedef struct Mem_s
{
    struct Mem_s *next; ///< single-linked list
    MS_U32        len;  ///< length of following block
} MEMT, *MEMP;

MEMT __mem_avail__[2] = {
    {NULL, 0}, /* HEAD for the available block list */
    {NULL, 0}, /* UNUSED but necessary so free doesn't join HEAD or ROVER with the pool */
};
static JPD_MMA_info_t g_sJpdmmaInfo[MAX_JPD_DEVICE_NUM];

void DrvJpdVdiMemFree(void *memp)
{
    MEMP q;  /* ptr to free block */
    MEMP p;  /* q->next */
    MEMP p0; /* block to free */

    if ((memp == NULL) || (AVAIL.len == 0))
    {
        return;
    }

    p0 = (MEMP)memp;
    p0 = &p0[-1]; /* get address of header */

    q = &AVAIL;

    while (1)
    {
        p = q->next;

        if ((p == NULL) || (p > (MEMP)memp))
        {
            break;
        }

        q = p;
    }

    if ((p != NULL) && ((((MS_U8 *)memp) + p0->len) == (MS_U8 *)p))
    {
        p0->len += p->len + HLEN;
        p0->next = p->next;
    }
    else
    {
        p0->next = p;
    }

    if ((((MS_U8 *)q) + q->len + HLEN) == (MS_U8 *)p0)
    {
        q->len += p0->len + HLEN;
        q->next = p0->next;
    }
    else
    {
        q->next = p0;
    }
}

MS_BOOL DrvJpdVdiMemInitPool(void *pool, MS_U32 size)
{
    if (size < MIN_POOL_SIZE)
    {
        return FALSE; /* FAILURE */
    }

    __mem_avail__[0].next = NULL;
    __mem_avail__[0].len  = 0;
    __mem_avail__[1].next = NULL;
    __mem_avail__[1].len  = 0;

    if (pool == NULL)
    {
        pool = (void *)1;
        size--;
    }

    /* Don't memset JPEG memory pool, it is cobuffer with display buffer. */
    //    JPEG_memset(pool, 0, size);

    AVAIL.next = (struct Mem_s *)pool;
    AVAIL.len  = size;

    (AVAIL.next)->next = NULL;
    (AVAIL.next)->len  = size - HLEN;

    return TRUE; /* SUCCESS */
}
void *DrvJpdVdiMemAlloc(MS_U32 size)
{
    MEMP   q; /* ptr to free block */
    MEMP   p; /* q->next */
    MS_U32 k; /* space remaining in the allocated block */
    q = &AVAIL;

    while (1)
    {
        p = q->next;
        if (p == NULL)
        {
            return (NULL); /* FAILURE */
        }

        if (p->len >= size)
        {
            break;
        }

        q = p;
    }

    k = p->len - size; /* calc. remaining bytes in block */

    if (k < MIN_BLOCK) /* rem. bytes too small for new block */
    {
        q->next = p->next;
        return (&p[1]); /* SUCCESS */
    }

    k -= HLEN;
    p->len = k;

    q       = (MEMP)((unsigned long)((MS_U8 *)(&p[1])) + k);
    q->len  = size;
    q->next = NULL;

    return (&q[1]); /* SUCCESS */
}

void MhalJpdVdiResetMmaInfo(unsigned int u32DevId)
{
    memset(&g_sJpdmmaInfo[u32DevId], 0, sizeof(g_sJpdmmaInfo[u32DevId]));
}

JPD_MMA_info_t *MhalJpdVdiGetMmaInfo(unsigned int u32DevId)
{
    if (u32DevId >= MAX_JPD_DEVICE_NUM)
    {
        return NULL;
    }

    return &g_sJpdmmaInfo[u32DevId];
}

// NOTE: if pMmaHeapName == NULL,will allocate from MMA heap; else if pMmaHeapName == "mma_heap_codec" will allocate
// from codec heap.
int MhalJpdVdiMmaAlloc(unsigned int u32DevId, Jpd_AddrBuffer_t *pDevBuf, MS_U8 *pMmaHeapName, char *name,
                       MS_U8 u8NeedVmap)
{
    MS_PHY          phyAddr;
    MS_U32          ret           = -1;
    JPD_MMA_info_t *pJpdMmaDevice = NULL;
    pJpdMmaDevice                 = MhalJpdVdiGetMmaInfo(u32DevId);

    if (name)
    {
        ret = pJpdMmaDevice->JpdMMACB.AllocMma(NULL, pMmaHeapName, (MS_U8 *)name, pDevBuf->size, &phyAddr);
    }
    else
    {
        ret = pJpdMmaDevice->JpdMMACB.AllocMma(NULL, pMmaHeapName, (MS_U8 *)"jpd_dma", pDevBuf->size, &phyAddr);
    }

    if (0 != ret)
    {
        pDevBuf->phys_addr = 0;
        JPD_LOGE("alloc cma fail %s %d Name:%s size:0x%x\n", __FUNCTION__, __LINE__, (name == NULL) ? "jpd_dma" : name,
                 pDevBuf->size);
        return -1;
    }

    pDevBuf->phys_addr = JPD_MIU_ADDR((unsigned long long)phyAddr); // set at the beginning of base address
    if (u8NeedVmap)
    {
        pDevBuf->virt_addr = pJpdMmaDevice->JpdMMACB.Map(phyAddr, pDevBuf->size, false);
        if (!pDevBuf->virt_addr)
        {
            pJpdMmaDevice->JpdMMACB.FreeMma(NULL, phyAddr);
            pDevBuf->phys_addr = 0;
            JPD_LOGE("cma map fail %s %d Name:%s size:0x%x\n", __FUNCTION__, __LINE__,
                     (name == NULL) ? "jpd_dma" : name, pDevBuf->size);
            return -1;
        }
        memset(pDevBuf->virt_addr, 0x00, pDevBuf->size);
        pJpdMmaDevice->JpdMMACB.Flush(pDevBuf->virt_addr, pDevBuf->size);
    }
    else
    {
        pDevBuf->virt_addr = 0;
    }
    // pDevBuf->base = map_phys_addr_to_cpu_base(pDevBuf->phys_addr);

    return 0;
}

void MhalJpdVdiMmaFree(unsigned int u32DevId, Jpd_AddrBuffer_t *pDevBuf)
{
    JPD_MMA_info_t *pJpdMmaDevice = NULL;
    pJpdMmaDevice                 = MhalJpdVdiGetMmaInfo(u32DevId);

    if (pDevBuf->virt_addr)
    {
        pJpdMmaDevice->JpdMMACB.Unmap(pDevBuf->virt_addr);
        pDevBuf->virt_addr = 0;
    }

    if (pDevBuf->phys_addr == 0)
    {
        JPD_LOGE("MhalJpdvdiMMAFree pDevBuf->phys_addr invalid\n");
    }
    pJpdMmaDevice->JpdMMACB.FreeMma(NULL, (MS_U64)JPD_PHY_ADDR(pDevBuf->phys_addr));
    pDevBuf->phys_addr = 0;
}
