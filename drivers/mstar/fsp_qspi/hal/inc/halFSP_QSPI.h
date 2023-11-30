/*
* halFSP_QSPI.h- Sigmastar
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

#ifndef _HAL_FSP_QSPI_H
#define _HAL_FSP_QSPI_H
#include <common.h>

#define BIT(_bit_)                  (1 << (_bit_))
#define BITS(_bits_, _val_)         ((BIT(((1)?_bits_)+1)-BIT(((0)?_bits_))) & (_val_<<((0)?_bits_)))
#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))


#define FSP_CHK_NUM_WAITDONE                    10000
#define FSP_MAX_READ_BUF_CNT                    10

typedef enum _FSP_ERROR_NUM
{
    ERR_FSP_SUCCESS = 0x00,
    ERR_FSP_TIMEOUT = 0x01,
} FSP_ERRNO_e;

#define RIU_BASE_ADDR                           0x1F000000
#define FSP_BASE_ADDR                           (0x100D00 << 1)
#define QSPI_BASE_ADDR                          (0x100E00 << 1)
#define FSP_PMBASE_ADDR                         (0x001600 << 1)//PM
#define QSPI_PMBASE_ADDR                        (0x001700 << 1)//PM
#define REG_FSP_QUAD_MODE                       (0x75 << 1)
#define FSP_SINGLE_MODE                         0
#define FSP_QUAD_MODE                           1
#define FSP_DUAL_MODE                           2
#define FSP_CTRL2_MASK                          3
#define REG_FSP_WBF_SIZE_OUTSIDE                (0x78 << 1)
#define FSP_WBF_OUTSIZE_EN                      (1 << 12)
#define REG_FSP_WBF_OUTSIDE                     (0x79 << 1)
#define FSP_WBF_SIZE_OUTSIDE_MASK               0x0fff
#define REG_FSP_READ_BUFF                       (0x65 << 1)
#define REG_FSP_WRITE_SIZE                      (0x6A << 1)
#define REG_FSP_READ_SIZE                       (0x6B << 1)
#define REG_FSP_CTRL                            (0x6C << 1)
#define REG_FSP_TRIGGER                         (0x6D << 1)
#define TRIGGER_FSP                             1
#define REG_FSP_DONE                            (0x6E << 1)
#define FSP_DONE                                1
#define REG_FSP_AUTO_CHECK_ERROR                (0x6E << 1)
#define FSP_AUTOCHECK_ERROR                     2
#define REG_FSP_CLEAR_DONE                      (0x6F << 1)
#define FSP_CLEAR_DONE                          1
#define REG_FSP_CTRL2                           (0x75 << 1)
#define REG_FSP_CTRL3                           (0x75 << 1)
#define REG_FSP_CTRL4                           (0x76 << 1)
#define REG_FSP_WDATA                           0x00
// FSP Register
#define REG_FSP_WRITE_BUFF                      (0x60 << 1)
#define REG_FSP_WRITE_BUFF_ONE                  (0x61 << 1)
#define REG_FSP_WRITE_BUFF_TWO                  (0x62 << 1)
#define REG_FSP_WRITE_BUFF_THREE                (0x63 << 1)
#define REG_FSP_WRITE_BUFF2                     (0x70 << 1)
#define FSP_WRITE_BUF_JUMP_OFFSET               0x0A

#define REG_FSP_RDATA                           0x05
#define REG_FSP_WSIZE                           0x0A

#define FSP_SINGLE_WRITE_SIZE                   15
#define REG_FSP_RSIZE                           0x0B

#define FSP_ENABLE                              1
#define FSP_RESET                               2
#define FSP_INT                                 4
#define FSP_AUTOCHECK                           8
#define FSP_ENABLE_SECOND_CMD                   0x8000
#define FSP_ENABLE_THIRD_CMD                    0x4000

#define REG_FSP_OUTSIDE_WBF_SIZE                (0x78 << 1)
#define REG_FSP_OUTSIDE_WBF_CTRL                (0x79 << 1)
#define FSP_WBF_SIZE_OUTSIDE_SIZE               (1 << 11)
#define FSP_WBF_OUTSIDE_EN                      (1 << 12)
#define FSP_WBF_REPLACED(x)                     ((x) & BMASK(7:0))
#define FSP_WBF_MODE(x)                         (((x) << 8) & BMASK(11:8))
#define FSP_OUTSIDE_WBF_SIZE(x)                 ((x) & BMASK(11:0))
#define FSP_WBF_OUTSIDE_SRC(x)                  ((x) << 14)

#define REG_SPI_CHIP_SELECT                     (0x7A << 1)
#define REG_SPI_BURST_WRITE                     (0x0A << 1)
#define REG_SPI_TIMEOUT_VAL_L                   (0x66 << 1)
#define REG_SPI_TIMEOUT_VAL_H                   (0x67 << 1)
#define REG_SPI_TIMEOUT_CTRL                    (0x67 << 1)

#define REG_SPI_TIMEOUT_VAL_MASK                BMASK(7:0)
#define REG_SPI_TIMEOUT_EN_MASK                 BMASK(15:15)
#define REG_SPI_TIMEOUT_RST_MASK                BMASK(14:14)

#define REG_SPI_TIMEOUT_DS                      BITS(15:15,0)
#define REG_SPI_TIMEOUT_EN                      BITS(15:15,1)
#define REG_SPI_TIMEOUT_NRST                    BITS(14:14,0)
#define REG_SPI_TIMEOUT_RST                     BITS(14:14,1)

#define REG_SPI_SW_CS_EN                        0x01
#define REG_SPI_SW_CS_PULL_HIGH                 0x02

#define REG_SPI_MODE_SEL                        (0x72 << 1)
    #define REG_SPI_NORMAL_MODE                 0x00
    #define REG_SPI_FAST_READ                   0x01
    #define REG_SPI_CMD_3B                      0x02
    #define REG_SPI_CMD_BB                      0x03
    #define REG_SPI_CMD_6B                      0x0A
    #define REG_SPI_CMD_EB                      0x0B
    #define REG_SPI_CMD_0B                      0x0C
    #define REG_SPI_CMD_4EB                     0x0D
#define REG_SPI_FUNC_SET                        (0x7D << 1)
    #define REG_SPI_ADDR2_EN                    0x800
    #define REG_SPI_DUMMY_EN                    0x1000
    //only for two plane nand
    #define REG_SPI_WRAP_EN                     0x2000

#define REG_SPI_CKG_SPI                         (0x70 << 1)
    #define REG_SPI_USER_DUMMY_EN               0x10

#define REG_SPI_FSP_CZ_HIGH                     (0x6d << 1)

void HAL_PIU_NONPM_Write2Byte_Mask(u32 u32_address, u16 u16_val, u16 u16_mask);
void HAL_PM_SLEEP_Write2Byte(u32 u32_address, u16 u16_val);
void HAL_CHIP_Write2Byte(u32 u32_address, u16 u16_val);
void HAL_PM_SLEEP_Write2Byte(u32 u32_address, u16 u16_val);
void HAL_FSP_WriteByte(u32 u32_address, u8 u8_val);
void HAL_FSP_Write2Byte(u32 u32_address, u16 u16_val);
void HAL_QSPI_WriteByte(u32 u32_address, u16 u8_val);
void HAL_QSPI_Write2Byte(u32 u32_address, u16 u16_val);
u16 HAL_FSP_Read2Byte(u32 u32_address);
u8 HAL_FSP_ReadByte(u32 u32_address);
void HAL_QSPI_Write2Byte_Mask(u32 u32_address, u16 u16_val,u16 u16_mask);
void HAL_FSP_QSPI_BankSel(void);
u8 HAL_QSPI_ReadByte(u32 u32_address);


#define PM_SLEEP_BASE_ADDR   (0x000E00)
#define PIU_NONPM_BASE_ADDR  (0x100700)
#define CHIP_BASE_ADDR       (0x101E00)

#endif
