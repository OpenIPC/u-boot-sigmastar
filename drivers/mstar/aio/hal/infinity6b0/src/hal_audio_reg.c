/*
* hal_audio_reg.c- Sigmastar
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
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

//=============================================================================
// Include files
//=============================================================================
#include "hal_audio_common.h"
#include "hal_audio_sys.h"
#include "hal_audio_reg.h"


//=============================================================================
// Variable definition
//=============================================================================
static U32 _gnBaseRegAddr = BACH_RIU_BASE_ADDR;

void HalBachSysSetBankBaseAddr(U32 nAddr)
{
    _gnBaseRegAddr = nAddr;
}

void HalBachWriteRegByte(U32 nAddr, U8 regMsk, U8 nValue)
{
    U8 nConfigValue;
    nConfigValue = READ_BYTE(_gnBaseRegAddr + ((nAddr) << 1) - ((nAddr) & 1));
    nConfigValue &= ~regMsk;
    nConfigValue |= (nValue & regMsk);
    WRITE_BYTE(_gnBaseRegAddr + ((nAddr) << 1) - ((nAddr) & 1), nConfigValue);
}

void HalBachWriteReg2Byte(U32 nAddr, U16 regMsk, U16 nValue)
{
    U16 nConfigValue;
    nConfigValue = READ_WORD(_gnBaseRegAddr + ((nAddr) << 1));
    nConfigValue &= ~regMsk;
    nConfigValue |= (nValue & regMsk);
    WRITE_WORD(_gnBaseRegAddr + ((nAddr) << 1), nConfigValue);
}

void HalBachWriteReg(BachRegBank_e nBank, U8 nAddr, U16 regMsk, U16 nValue)
{
    U16 nConfigValue;

    switch(nBank)
    {
        case BACH_REG_BANK1:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_1 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_1 + (nAddr)) << 1), nConfigValue);
            break;
        case BACH_REG_BANK2:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_2 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_2 + (nAddr)) << 1), nConfigValue);
            break;
        case BACH_REG_BANK3:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_3 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_3 + (nAddr)) << 1), nConfigValue);
            break;
        case BACH_REG_BANK4:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_4 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_4 + (nAddr)) << 1), nConfigValue);
            break;
        default:
            break;
    }
}

U16 HalBachReadReg2Byte(U32 nAddr)
{
    return READ_WORD(_gnBaseRegAddr + ((nAddr) << 1));
}

U8 HalBachReadRegByte(U32 nAddr)
{
    return READ_BYTE(_gnBaseRegAddr + ((nAddr) << 1) - ((nAddr) & 1));
}

U16 HalBachReadReg(BachRegBank_e nBank, U8 nAddr)
{
    switch(nBank)
    {
        case BACH_REG_BANK1:
            return READ_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_1 + (nAddr)) << 1));
        case BACH_REG_BANK2:
            return READ_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_2 + (nAddr)) << 1));
        case BACH_REG_BANK3:
            return READ_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_3 + (nAddr)) << 1));
        case BACH_REG_BANK4:
            return READ_WORD(_gnBaseRegAddr + ((BACH_REG_BANK_4 + (nAddr)) << 1));
        default:
            return 0;
    }
}
