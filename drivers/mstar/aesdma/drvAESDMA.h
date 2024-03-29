/*
* drvAESDMA.h- Sigmastar
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


#ifndef _DRV_AESDMA_H_
#define _DRV_AESDMA_H_

#include "halAESDMA.h"

typedef enum
{
    E_AESDMA_KEY_CIPHER,
    E_AESDMA_KEY_OTP_EFUSE_KEY1,
    E_AESDMA_KEY_OTP_EFUSE_KEY2,
    E_AESDMA_KEY_OTP_EFUSE_KEY3,
    E_AESDMA_KEY_OTP_EFUSE_KEY4,
    E_AESDMA_KEY_OTP_EFUSE_KEY5,
    E_AESDMA_KEY_OTP_EFUSE_KEY6,
    E_AESDMA_KEY_OTP_EFUSE_KEY7,
    E_AESDMA_KEY_OTP_EFUSE_KEY8,
    E_AESDMA_KEY_NUM
} enumAESDMA_KeyType;

typedef enum
{
    E_AESDMA_CHAINMODE_ECB,
    E_AESDMA_CHAINMODE_CTR,
    E_AESDMA_CHAINMODE_CBC,
    E_AESDMA_CHAINMODE_NUM
} enumAESDMA_ChainMode;

typedef struct
{
    U32 u32SrcAddr;
    U32 u32Size;
    U32 u32DstAddr;
    enumAESDMA_KeyType eKeyType;
    U16 *pu16Key;
    BOOL bSetIV;
    BOOL bDecrypt;
    U16 *pu16IV;
    enumAESDMA_ChainMode eChainMode;
} __attribute__((aligned(16))) aesdmaConfig;

typedef enum
{
    E_SHA_MODE_1,
    E_SHA_MODE_256,
    E_SHA_MODE_NUM
} enumShaMode;

typedef struct
{
    U32 *pu32Sig;
    U32 *pu32KeyN;
    U32 *pu32KeyE;
    U32 *pu32Output;
    BOOL bHwKey;
    BOOL bPublicKey;
    U32 u32KeyLen;
    U32 u32SigLen;
    U32 u32OutputLen;
} __attribute__((aligned(16))) rsaConfig;

void MDrv_AESDMA_Run(aesdmaConfig *pConfig);
void MDrv_SHA_Run(U32 u32SrcAddr, U32 u32Size, enumShaMode eMode, U16 *pu16Output);
void MDrv_RSA_Run(rsaConfig *pConfig);
void runDecrypt(U32 u32ImageAddr, U32 u32ImageSize, U32 u32KeySel, U16* pu16Key);
BOOL runAuthenticate(U32 u32ImageAddr, U32 u32ImageSize, U32* pu32Key);
BOOL runAuthenticate2(U32 u32ImageAddr, U32 u32ImageSize, U32 u32Key, U32 u32KeySize, U32 u32Sig, U32 u32SigSize);

#endif

