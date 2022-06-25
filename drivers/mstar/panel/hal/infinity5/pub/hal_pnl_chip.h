/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifndef _HAL_PNL_CHIP_H_
#define _HAL_PNL_CHIP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define     HAL_PNL_CTX_INST_MAX        2


#define     HAL_PNL_SIGNAL_CTRL_CH_MAX  5


#define     HAL_PNL_MOD_CH_MAX   5
#define     HLA_PNL_CH_SWAP_MAX  5

#define     HAL_PNL_MIPI_DSI_FLAG_DELAY                         0xFE
#define     HAL_PNL_MIPI_DSI_FLAG_END_OF_TABLE                  0xFF   // END OF REGISTERS MARKER


#define     HAL_PNL_MIPI_DSI_DCS_MAXIMUM_RETURN_PACKET_SIZE	    0x37
#define     HAL_PNL_MIPI_DSI_DCS_SHORT_PACKET_ID_0              0x05
#define     HAL_PNL_MIPI_DSI_DCS_SHORT_PACKET_ID_1              0x15
#define     HAL_PNL_MIPI_DSI_DCS_LONG_WRITE_PACKET_ID	        0x39
#define     HAL_PNL_MIPI_DSI_DCS_READ_PACKET_ID                 0x06

#define     HAL_PNL_MIPI_DSI_GERNERIC_SHORT_PACKET_ID_0         0x03
#define     HAL_PNL_MIPI_DSI_GERNERIC_SHORT_PACKET_ID_1         0x13
#define     HAL_PNL_MIPI_DSI_GERNERIC_SHORT_PACKET_ID_2         0x23
#define     HAL_PNL_MIPI_DSI_GERNERIC_LONG_WRITE_PACKET_ID	    0x29
#define     HAL_PNL_MIPI_DSI_GERNERIC_READ_0_PARAM_PACKET_ID    0x04
#define     HAL_PNL_MIPI_DSI_GERNERIC_READ_1_PARAM_PACKET_ID    0x14
#define     HAL_PNL_MIPI_DSI_GERNERIC_READ_2_PARAM_PACKET_ID    0x24

#define     AS_UINT32(x)    (*(volatile u32 *)((void*)x))
#define     ALIGN_TO(x, n)  (((x) + ((n) - 1)) & ~((n) - 1))

#define HAL_PNL_CLK_ODCLK    3  //clk_odclk=3 (lpll_clk)

#define HAL_PNL_CLK_ON_SETTING \
{ \
    1,\
}

#define HAL_PNL_CLK_OFF_SETTING \
{ \
    0,\
}


#define HAL_PNL_CLK_RATE_SETTING \
{ \
    HAL_PNL_CLK_ODCLK, \
}

#define 	HAL_PNL_CLK_NUM										1
#define 	HAL_PNL_CLK_MUX_ATTR \
{ \
    0, \
}


#define HAL_PNL_CLK_NAME \
{   \
    "CLK_odclk",         \
}

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


#endif

