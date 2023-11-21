/*
 * mdrvParts.h- Sigmastar
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
#ifndef _MDRVPART_H_
#define _MDRVPART_H_

#define MAX_PARTS_TBL_SIZE       0x800

typedef struct parts_info
{
    u8 au8_partName[16];   //The partition's name
    u8 u8_Trunk;           //A distinguishing number for partitions with the same name,e.g. IPL0,IPL1,IPL2
    u8 u8_BackupTrunk;     //The backup_partition's index
    u8 u8_Active;          //Mark the partition is active or not
    u8 u8_Group;           //Reserved function, e.g. put IPL0,IPL_CUST1,UBOOT0 into the same group,then there status are all active
    u32 u32_Offset;        //The partition's start address(byte)
    u32 u32_Size;          //The partition's size
    u32 u32_Reserved;
}PARTS_INFO_t;

typedef struct parts_tbl
{
    u8 au8_magic[16];        //"SSTARSEMICIS0001"
    u32 u32_Checksum;
    u32 u32_Size;
    //PARTS_INFO_t *pst_partsInfo;
}PARTS_TBL_t;

void MDRV_PARTS_hardware_init_tlb(u8 *buf);
void MDRV_PARTS_init_tlb(u8 *buf);
u8   MDRV_PARTS_is_tbl_header(u8 *pni_buf);
u8   MDrv_PARTS_is_tbl_valid(u8 *pni_buf);
u8   MDRV_PARTS_mark_active(u8 *pu8_PartName, u8 u8_Trunk);
u8   MDRV_PARTS_get_part(u8 *pu8_PartName, u8 u8_Trunk, PARTS_INFO_t *pst_PartInfo);
u8   MDRV_PARTS_get_active_part(u8 *pu8_PartName, PARTS_INFO_t *pst_PartInfo);
u8   MDRV_PARTS_get_part_nm(u8 u8_partId, PARTS_INFO_t *pst_partInfo);
u32  MDRV_PARTS_load_part(PARTS_INFO_t *pst_PartInfo, u32 u32_offset, u8 *pu8_data, u32 u32_size);
u32  MDRV_PARTS_block_isbad(PARTS_INFO_t *pst_partInfo, u32 u32_offset);
u32  MDRV_PARTS_read(PARTS_INFO_t *pst_PartInfo, u32 u32_offset, u8 *pu8_data, u32 u32_size);
u32  MDRV_PARTS_write(PARTS_INFO_t *pst_PartInfo, u32 u32_offset, u8 *pu8_data, u32 u32_size);
u32  MDRV_PARTS_erase(PARTS_INFO_t *pst_PartInfo, u32 u32_offset, u32 u32_size);

#endif /* _MDRVPART_H_ */
