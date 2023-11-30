/*
* mstar_i2c_reg_infinity6e.h- Sigmastar
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


#ifndef __HAL_BUS_I2C_REG_I6E_H__
#define __HAL_BUS_I2C_REG_I6E_H__

/*=============================================================*/
// Include files
/*=============================================================*/
#include <asm/types.h>

/*=============================================================*/
// Extern definition
/*=============================================================*/

#define BUS_I2C_ERROR_LEVEL				(_DEV)
#define BUS_I2C_TRACE_LEVEL				(BUS_I2C_ERROR_LEVEL | LEVEL_6)
#define IIC0_BASE_REG 		(0x1F223000)
#define IIC1_BASE_REG 		(0x1F223200)
#define CHIPTOP_BASE_REG 	(0x1F207800)
#define CLKGEN_BASE_REG 	(0x1F207000)
volatile u16 * const g_ptI2C0      = (u16*) (IIC0_BASE_REG);
volatile u16 * const g_ptI2C1      = (u16*) (IIC1_BASE_REG);
volatile u16 * const g_ptCLKGEN    = (u16*) (CLKGEN_BASE_REG);
volatile u16 * const g_ptCHIPTOP   = (u16*) (CHIPTOP_BASE_REG);
#define Chip_Phys_to_MIU(x) (x - 0x20000000)

//############################
//
//IP bank address : for pad mux in chiptop
//
//############################
#define CHIP_REG_BASE                  0 //(0x101E00)
#define CHIP_GPIO1_REG_BASE            0//(0x101A00)

//for port 0
#define CHIP_REG_HWI2C_MIIC0            (CHIP_REG_BASE+ (0x6f*2))
    #define CHIP_MIIC0_PAD_0            (0)
    #define CHIP_MIIC0_PAD_1            (__BIT0)
    #define CHIP_MIIC0_PAD_2            (__BIT1)
    #define CHIP_MIIC0_PAD_3            (__BIT0|__BIT1)
    #define CHIP_MIIC0_PAD_4            (__BIT2)
	#define CHIP_MIIC0_PAD_5            (__BIT0|__BIT2)
    #define CHIP_MIIC0_PAD_MSK          (__BIT0|__BIT1|__BIT2)

//for port 1
#define CHIP_REG_HWI2C_MIIC1            (CHIP_REG_BASE+ (0x6f*2))
    #define CHIP_MIIC1_PAD_0            (0)
    #define CHIP_MIIC1_PAD_1            (__BIT4)
    #define CHIP_MIIC1_PAD_2            (__BIT5)
    #define CHIP_MIIC1_PAD_3            (__BIT4|__BIT5)
    #define CHIP_MIIC1_PAD_MSK          (__BIT4|__BIT5)

//for port 2
#define CHIP_REG_HWI2C_MIIC2            (CHIP_REG_BASE+ (0x6f*2+1))
    #define CHIP_MIIC2_PAD_0            (0)
    #define CHIP_MIIC2_PAD_1            (__BIT0)
    #define CHIP_MIIC2_PAD_2            (__BIT1)
    #define CHIP_MIIC2_PAD_3            (__BIT0|__BIT1)
	#define CHIP_MIIC2_PAD_4            (__BIT2)
	#define CHIP_MIIC2_PAD_5            (__BIT0|__BIT2)
    #define CHIP_MIIC2_PAD_MSK          (__BIT0|__BIT1|__BIT2)


//for port 3
#define CHIP_REG_HWI2C_DDCR             (CHIP_REG_BASE+ (0x57*2)) //0x1EAE
    #define CHIP_DDCR_PAD_0             (0)
    #define CHIP_DDCR_PAD_1             (__BIT1)
    #define CHIP_DDCR_PAD_MSK           (__BIT1|__BIT0)
#endif //__HAL_BUS_I2C_REG_I6E_H__

