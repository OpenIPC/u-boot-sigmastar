/*
* usb_power.c- Sigmastar
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

#include "gpio.h"
#include "mhal_gpio.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"

void USB_Power_On(void)
{
    if (INREG16(GET_REG_ADDR(REG_ADDR_BASE_MAILBOX, 0x10)) & BIT14)
    {
        /* QFN128 */
        MHal_GPIO_Pad_Set(PAD_PM_GPIO2);
        MHal_GPIO_Set_High(PAD_PM_GPIO2);
    } else { /* QFN88 */
        MHal_GPIO_Pad_Set(PAD_SAR_GPIO2);
        MHal_GPIO_Set_High(PAD_SAR_GPIO2);
    }

    return;
}
//--------------------

