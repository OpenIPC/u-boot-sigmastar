/*
* hal_pnl_chip.h- Sigmastar
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

#ifndef _HAL_PNL_CHIP_H_
#define _HAL_PNL_CHIP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define CHIP_REVISION_U01   1
#define CHIP_REVISION_U02   2

#define HAL_PNL_CTX_INST_MAX        2


#define HAL_PNL_SIGNAL_CTRL_CH_MAX  5


#define HAL_PNL_MOD_CH_MAX   5
#define HLA_PNL_CH_SWAP_MAX  5

#define HAL_PNL_MIPI_DSI_FLAG_DELAY                         0xFE
#define HAL_PNL_MIPI_DSI_FLAG_END_OF_TABLE                  0xFF   // END OF REGISTERS MARKER


#define HAL_PNL_MCU_FLAG_DELAY                              0xFFFFFE
#define HAL_PNL_MCU_FLAG_END_OF_TABLE                       0xFFFFFF   // END OF REGISTERS MARKER

#define HAL_PNL_MIPI_DSI_DCS_MAXIMUM_RETURN_PACKET_SIZE	    0x37
#define HAL_PNL_MIPI_DSI_DCS_SHORT_PACKET_ID_0              0x05
#define HAL_PNL_MIPI_DSI_DCS_SHORT_PACKET_ID_1              0x15
#define HAL_PNL_MIPI_DSI_DCS_LONG_WRITE_PACKET_ID	        0x39
#define HAL_PNL_MIPI_DSI_DCS_READ_PACKET_ID                 0x06

#define HAL_PNL_MIPI_DSI_GERNERIC_SHORT_PACKET_ID_0         0x03
#define HAL_PNL_MIPI_DSI_GERNERIC_SHORT_PACKET_ID_1         0x13
#define HAL_PNL_MIPI_DSI_GERNERIC_SHORT_PACKET_ID_2         0x23
#define HAL_PNL_MIPI_DSI_GERNERIC_LONG_WRITE_PACKET_ID	    0x29
#define HAL_PNL_MIPI_DSI_GERNERIC_READ_0_PARAM_PACKET_ID    0x04
#define HAL_PNL_MIPI_DSI_GERNERIC_READ_1_PARAM_PACKET_ID    0x14
#define HAL_PNL_MIPI_DSI_GERNERIC_READ_2_PARAM_PACKET_ID    0x24

#define AS_UINT32(x)    (*(volatile u32 *)((void*)x))
#define ALIGN_TO(x, n)  (((x) + ((n) - 1)) & ~((n) - 1))

#define HAL_PNL_CLK_NUM             1

#define CLK_MHZ(x)                  (x*1000000)

#define HAL_PNL_CLK_MIPI_TX_DSI_RATE        CLK_MHZ(0)


#define HAL_PNL_CLK_ON_SETTING \
{ \
    1, \
}

#define HAL_PNL_CLK_OFF_SETTING \
{ \
    0, \
}


#define HAL_PNL_CLK_RATE_SETTING \
{ \
    HAL_PNL_CLK_MIPI_TX_DSI_RATE, \
}

#define HAL_PNL_CLK_OFF_RATE_SETTING \
{ \
    HAL_PNL_CLK_MIPI_TX_DSI_RATE, \
}

// Mux_ATTR = 1 : clk idx
// Mux_ATTR = 0 : clk rate
#define HAL_PNL_CLK_MUX_ATTR \
{ \
    1, \
}

#define HAL_PNL_CLK_NAME \
{   \
    "mipitxdsi",          \
}

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


#endif

