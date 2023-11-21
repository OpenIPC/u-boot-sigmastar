/*
 * hal_jpd_reg.h- Sigmastar
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

#ifndef __MHAL_JPD_REG_H__
#define __MHAL_JPD_REG_H__

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define BASE_REG_RIU_PA 0x1F000000
// 100AH	1	0	0	0	0	0	0	0	0	clkgen2	0	clkgen2
//#define NJPD_CLKGEN2_BASE                0x00A00
#define NJPD_CLKGEN2_BASE (0x103800 << 1)

//#define NJPD1_REG_BASE                   0x23200
//#define NJPD1_REG_BASE                   0x162200
//#define NJPD2_REG_BASE                   0x23300

#define JPDFLAG (0x100400 << 1)

#define BK_NJPD1_SETLFAG (JPDFLAG + NJPD_OFFSET(0x00))

// NJPD1 register=======================================================================

// Global Setting
#define BK_NJPD1_GLOBAL_SETTING00 (NJPD_OFFSET(0x00))
#define BK_NJPD1_GLOBAL_SETTING01 (NJPD_OFFSET(0x01))
#define BK_NJPD1_GLOBAL_SETTING02 (NJPD_OFFSET(0x02))

// Pitch Width
#define BK_NJPD1_PITCH_WIDTH (NJPD_OFFSET(0x03))

// Restart Interval
#define BK_NJPD1_RESTART_INTERVAL (NJPD_OFFSET(0x05))

// Image Size
#define BK_NJPD1_IMG_HSIZE (NJPD_OFFSET(0x06))
#define BK_NJPD1_IMG_VSIZE (NJPD_OFFSET(0x07))

// Write-one-clear
#define BK_NJPD1_WRITE_ONE_CLEAR (NJPD_OFFSET(0x08))

// Region of Interest
#define BK_NJPD1_ROI_H_START (NJPD_OFFSET(0x09))
#define BK_NJPD1_ROI_V_START (NJPD_OFFSET(0x0a))
#define BK_NJPD1_ROI_H_SIZE  (NJPD_OFFSET(0x0b))
#define BK_NJPD1_ROI_V_SIZE  (NJPD_OFFSET(0x0c))

// Gated-Clock Control
#define BK_NJPD1_GATED_CLOCK_CTRL (NJPD_OFFSET(0x0d))

// Miu Interface
#define BK_NJPD1_MIU_READ_STATUS               (NJPD_OFFSET(0x0e))
#define BK_NJPD1_MIU_IBUFFER_CNT               (NJPD_OFFSET(0x0f))
#define BK_NJPD1_MIU_READ_START_ADDR_L         (NJPD_OFFSET(0x10))
#define BK_NJPD1_MIU_READ_START_ADDR_H         (NJPD_OFFSET(0x11))
#define BK_NJPD1_MIU_READ_BUFFER0_START_ADDR_L (NJPD_OFFSET(0x12))
#define BK_NJPD1_MIU_READ_BUFFER0_START_ADDR_H (NJPD_OFFSET(0x13))
#define BK_NJPD1_MIU_READ_BUFFER0_END_ADDR_L   (NJPD_OFFSET(0x14))
#define BK_NJPD1_MIU_READ_BUFFER0_END_ADDR_H   (NJPD_OFFSET(0x15))
#define BK_NJPD1_MIU_READ_BUFFER1_START_ADDR_L (NJPD_OFFSET(0x16))
#define BK_NJPD1_MIU_READ_BUFFER1_START_ADDR_H (NJPD_OFFSET(0x17))
#define BK_NJPD1_MIU_READ_BUFFER1_END_ADDR_L   (NJPD_OFFSET(0x18))
#define BK_NJPD1_MIU_READ_BUFFER1_END_ADDR_H   (NJPD_OFFSET(0x19))
#define BK_NJPD1_MIU_WRITE_START_ADDR_L        (NJPD_OFFSET(0x1a))
#define BK_NJPD1_MIU_WRITE_START_ADDR_H        (NJPD_OFFSET(0x1b))
#define BK_NJPD1_MIU_WRITE_POINTER_ADDR_L      (NJPD_OFFSET(0x1c))
#define BK_NJPD1_MIU_WRITE_POINTER_ADDR_H      (NJPD_OFFSET(0x1d))
#define BK_NJPD1_MIU_READ_POINTER_ADDR_L       (NJPD_OFFSET(0x1e))
#define BK_NJPD1_MIU_READ_POINTER_ADDR_H       (NJPD_OFFSET(0x1f))
#define BK_NJPD1_MIU_HTABLE_START_ADDR_L       (NJPD_OFFSET(0x20))
#define BK_NJPD1_MIU_HTABLE_START_ADDR_H       (NJPD_OFFSET(0x21))
#define BK_NJPD1_MIU_GTABLE_START_ADDR_L       (NJPD_OFFSET(0x22))
#define BK_NJPD1_MIU_GTABLE_START_ADDR_H       (NJPD_OFFSET(0x23))
#define BK_NJPD1_MIU_QTABLE_START_ADDR_L       (NJPD_OFFSET(0x24))
#define BK_NJPD1_MIU_QTABLE_START_ADDR_H       (NJPD_OFFSET(0x25))
#define BK_NJPD1_MIU_HTABLE_SIZE               (NJPD_OFFSET(0x26))
#define BK_NJPD1_SET_CHROMA_VALUE              (NJPD_OFFSET(0x27))
#define BK_NJPD1_IBUF_READ_LENGTH              (NJPD_OFFSET(0x28))

// Interrupt
#define BK_NJPD1_IRQ_CLEAR   (NJPD_OFFSET(0x29))
#define BK_NJPD1_IRQ_FORCE   (NJPD_OFFSET(0x2a))
#define BK_NJPD1_IRQ_MASK    (NJPD_OFFSET(0x2b))
#define BK_NJPD1_IRQ_FINAL_S (NJPD_OFFSET(0x2c))
#define BK_NJPD1_IRQ_RAW_S   (NJPD_OFFSET(0x2d))

// Sram Gated-Clock Control
#define BK_NJPD1_SRAM_GATED_CLK_CTRL_00 (NJPD_OFFSET(0x2e))
#define BK_NJPD1_SRAM_GATED_CLK_CTRL_01 (NJPD_OFFSET(0x2f))

// Debug
#define BK_NJPD1_ROW_IDEX            (NJPD_OFFSET(0x30))
#define BK_NJPD1_COLUMN_IDEX         (NJPD_OFFSET(0x31))
#define BK_NJPD1_DEBUG_BUS_SELECT    (NJPD_OFFSET(0x32))
#define BK_NJPD1_DEBUG_BUS_H         (NJPD_OFFSET(0x33))
#define BK_NJPD1_DEBUG_BUS_L         (NJPD_OFFSET(0x34))
#define BK_NJPD1_IBUF_BYTE_COUNT_L   (NJPD_OFFSET(0x35))
#define BK_NJPD1_IBUF_BYTE_COUNT_H   (NJPD_OFFSET(0x36))
#define BK_NJPD1_VLD_BYTE_COUNT_L    (NJPD_OFFSET(0x37))
#define BK_NJPD1_VLD_BYTE_COUNT_H    (NJPD_OFFSET(0x38))
#define BK_NJPD1_VLD_DECODING_DATA_L (NJPD_OFFSET(0x39))
#define BK_NJPD1_VLD_DECODING_DATA_H (NJPD_OFFSET(0x3a))
#define BK_NJPD1_DEBUG_TRIG_CYCLE    (NJPD_OFFSET(0x3b))
#define BK_NJPD1_DEBUG_TRIG_MBX      (NJPD_OFFSET(0x3c))
#define BK_NJPD1_DEBUG_TRIG_MBY      (NJPD_OFFSET(0x3d))
#define BK_NJPD1_DEBUG_TRIGGER       (NJPD_OFFSET(0x3e))

// BIST
#define BK_NJPD1_BIST_FAIL (NJPD_OFFSET(0x3f))

// TBC RIU Interface
#define BK_NJPD1_TBC         (NJPD_OFFSET(0x40))
#define BK_NJPD1_TBC_WDATA0  (NJPD_OFFSET(0x41))
#define BK_NJPD1_TBC_WDATA1  (NJPD_OFFSET(0x42))
#define BK_NJPD1_TBC_RDATA_L (NJPD_OFFSET(0x43))
#define BK_NJPD1_TBC_RDATA_H (NJPD_OFFSET(0x44))

// Max Huffman Table Address
#define BK_NJPD1_Y_MAX_HUFFTABLE_ADDRESS  (NJPD_OFFSET(0x45))
#define BK_NJPD1_CB_MAX_HUFFTABLE_ADDRESS (NJPD_OFFSET(0x46))
#define BK_NJPD1_CR_MAX_HUFFTABLE_ADDRESS (NJPD_OFFSET(0x47))

// Spare
#define BK_NJPD1_SPARE00 (NJPD_OFFSET(0x48))
#define BK_NJPD1_SPARE01 (NJPD_OFFSET(0x49))
#define BK_NJPD1_SPARE02 (NJPD_OFFSET(0x4a))
#define BK_NJPD1_SPARE03 (NJPD_OFFSET(0x4b))
#define BK_NJPD1_SPARE04 (NJPD_OFFSET(0x4c))
#define BK_NJPD1_SPARE05 (NJPD_OFFSET(0x4d))
#define BK_NJPD1_SPARE06 (NJPD_OFFSET(0x4e))
#define BK_NJPD1_SPARE07 (NJPD_OFFSET(0x4f))

#define BK_NJPD1_SPARE07 (NJPD_OFFSET(0x4f))

#define BK_NJPD1_MARB_SETTING_00 (NJPD_OFFSET(0x50))
#define BK_NJPD1_MARB_SETTING_01 (NJPD_OFFSET(0x51))
#define BK_NJPD1_MARB_SETTING_02 (NJPD_OFFSET(0x52))
#define BK_NJPD1_MARB_SETTING_03 (NJPD_OFFSET(0x53))
#define BK_NJPD1_MARB_SETTING_04 (NJPD_OFFSET(0x54))
#define BK_NJPD1_MARB_SETTING_05 (NJPD_OFFSET(0x55))
#define BK_NJPD1_MARB_SETTING_06 (NJPD_OFFSET(0x56))
#define BK_NJPD1_MARB_SETTING_07 (NJPD_OFFSET(0x57))

#define BK_NJPD1_MARB_UBOUND_0_L (NJPD_OFFSET(0x58))
#define BK_NJPD1_MARB_UBOUND_0_H (NJPD_OFFSET(0x59))
#define BK_NJPD1_MARB_LBOUND_0_L (NJPD_OFFSET(0x5a))
#define BK_NJPD1_MARB_LBOUND_0_H (NJPD_OFFSET(0x5b))

#define BK_NJPD1_CRC_MODE (NJPD_OFFSET(0x6d))

#define BK_NJPD1_READBUFADDR_OFFSET_64B  (NJPD_OFFSET(0x69))
#define BK_NJPD1_WRITEBUFADDR_OFFSET_64B (NJPD_OFFSET(0x6a))
#define BK_NJPD1_TABLE_ADDR_OFFSET_64B   (NJPD_OFFSET(0x6F))

#define BK_NJPD1_MARB_CRC_RESULT_0    (NJPD_OFFSET(0x70))
#define BK_NJPD1_MARB_CRC_RESULT_1    (NJPD_OFFSET(0x71))
#define BK_NJPD1_MARB_CRC_RESULT_2    (NJPD_OFFSET(0x72))
#define BK_NJPD1_MARB_CRC_RESULT_3    (NJPD_OFFSET(0x73))
#define BK_NJPD1_MARB_RESET           (NJPD_OFFSET(0x74))
#define BK_NJPD1_HANDSHAKE_CNT        (NJPD_OFFSET(0x74))
#define BK_NJPD1_SW_MB_ROW_CNT        (NJPD_OFFSET(0x75))
#define BK_NJPD1_HANDSHAKE            (NJPD_OFFSET(0x75))
#define BK_NJPD1_TOP_MARB_PORT_ENABLE (NJPD_OFFSET(0x76))

#define BK_NJPD1_GENERAL(x) (NJPD_OFFSET(x))

#define BK_MIU0_GENERAL(x) (NJPD_MIU0_BASE + NJPD_OFFSET(x))
//#define BK_MIU1_GENERAL(x)          (NJPD_MIU1_BASE+NJPD_OFFSET(x))

// CLKGEN2
// For k6 we use bank 0x0A,offset 0x11 bit(0:3) to set reg_ckg_njpd
//#define NJPD_CLOCK                                  (NJPD_CLKGEN2_BASE+NJPD_OFFSET(0x11))     // k6
#define NJPD_CLOCK (NJPD_CLKGEN2_BASE + NJPD_OFFSET(0x4a)) // M6

// CLK_NJPD clock setting
//[0]: disable clock
//[1]: invert clock
//[3:2]: Select clock source
//      00: 288 MHz
//      01: 320 MHz
//      10: 192 MHz
//      11: 144 MHz

// MIU
#define NJPD_MIU0_BASE (0x101200 << 1)
//#define NJPD_MIU1_BASE                  (0x100600<<1)

// k6: h0043	h0043	15	0	reg_rq2_mask
#define NJPD_MIU0_RQ2_MASK (NJPD_MIU0_BASE + NJPD_OFFSET(0x43))
//#define NJPD_MIU1_RQ2_MASK              (NJPD_MIU1_BASE+NJPD_OFFSET(0x43))
#define NJPD0_MIU0_CLIENT_NJPD NJPD_BIT(7) // bit7 of the first byte
#define NJPD0_MIU1_CLIENT_NJPD NJPD_BIT(7) // bit7 of the first byte
#define NJPD0_MIU0_MIU_SEL2    (NJPD_MIU0_BASE + NJPD_OFFSET(0x7a))
#endif //__MHAL_JPD_REG_H__