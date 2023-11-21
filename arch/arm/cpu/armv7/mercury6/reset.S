/*
* reset.S- Sigmastar
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
/*------------------------------------------------------------------------------
    Function Code
-------------------------------------------------------------------------------*/
.extern Chip_Set_RebootType
	.align	5
.globl reset_cpu
reset_cpu:
    ldr   r0, =0x02
    bl    Chip_Set_RebootType
    ldr   r1, =0x1F001CB8
    mov   r3, #0x00FF
    str   r3, [r1]
    mov   r3, #0x0079
	str   r3, [r1]
	nop
	nop
	nop
	nop

_loop_forever:
	b	_loop_forever

    .arch_extension sec
    .global smcfn
    .type smcfn, %function
smcfn:
    smc #0
    bx lr
    .arch_extension nosec

