/*
 * mdrvPni.h- Sigmastar
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
#ifndef _MDRVPNI_H_
#define _MDRVPNI_H_

u8 MDRV_PNI_init_tlb(u8 *pni_buf);
u8 MDRV_PNI_write_back(u8 *pni_buf);

#endif /* _MDRVPNI_H_ */
