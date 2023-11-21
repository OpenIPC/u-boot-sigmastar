/*
* drv_pnl_os_header.h- Sigmastar
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

#ifndef _DRV_PNL_OS_HEADER_H_
#define _DRV_PNL_OS_HEADER_H_

#include <common.h>
#include <command.h>
#include <config.h>
#include <malloc.h>
#include <stdlib.h>

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define CamOsPrintf  printf


#define WRITE_SCL_REG(addr, type, data)  ((*(volatile type *)(addr)) = (data))
#define READ_SCL_REG(addr, type)         ((*(volatile type *)(addr)))


#define READ_BYTE(x)         READ_SCL_REG(x, u8)
#define READ_WORD(x)         READ_SCL_REG(x, u16)
#define READ_LONG(x)         READ_SCL_REG(x, u32)
#define WRITE_BYTE(x, y)     WRITE_SCL_REG(x, u8, y)
#define WRITE_WORD(x, y)     WRITE_SCL_REG(x, u16, y)
#define WRITE_LONG(x, y)     WRITE_SCL_REG(x, u32, y)


#endif
