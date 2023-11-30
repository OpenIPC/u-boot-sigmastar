/*
* mhal_hdmitx.h- Sigmastar
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

#ifndef __MHAL_HDMITX_H__
#define __MHAL_HDMITX_H__

#include "mhal_hdmitx_datatype.h"
//=============================================================================
// API
//=============================================================================

#ifndef __MHAL_HDMITX_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

/// @brief Create instance of HDMITX
/// @param[out] pCtx: Context of created instance
/// @param[in]  u32Id: HDMITX ID
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxCreateInstance(void **pCtx, u32 u32Id);

/// @brief Get context of instance with specific ID
/// @param[out] pCtx: Context of created HDMITX instance with specific ID
/// @param[in]  u32Id: HDMITX ID
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxGetInstance(void **pCtx, u32 u32Id);

/// @brief Destroy instance of HDMITX
/// @param[in] pCtx: Context of created HDMITX instance
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxDestroyInstance(void *pCtx);

/// @brief Assign starting stage of output attribute of HDMITX
/// @param[in] pCtx: Context of created HDMITX instance
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxSetAttrBegin(void *pCtx);

/// @brief Assign otutput attribute of HDMITX
/// @param[in] pCtx: Context of created HDMITX instance
/// @param[in] pstAttrCfg: Attribute of HDMITX
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxSetAttr(void *pCtx, MhalHdmitxAttrConfig_t *pstAttrCfg);

/// @brief Assign ending stage of output attribute of HDMITX
/// @param[in] pCtx: Context of created HDMITX instance
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxSetAttrEnd(void *pCtx);

/// @brief Enalbe/disalbe mute setting of HDMITX
/// @param[in] pCtx: Context of created HDMITX instance
/// @param[in] pstMuteCfg: Configurationof of mute
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxSetMute(void *pCtx, MhalHdmitxMuteConfig_t *pstMuteCfg);

/// @brief Enalbe/disalbe  otuput signal of HDMITX
/// @param[in] pCtx: Context of created HDMITX instance
/// @param[in] pstSignalCfg: Configurationof of signal controller
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxSetSignal(void *pCtx, MhalHdmitxSignalConfig_t *pstSignalCfg);

/// @brief Set analog drving current of HDMITX
/// @param[in] pCtx: Context of created HDMITX instance
/// @param[in] pstDrvCurCfg: Configurationof of driving current
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxSetAnalogDrvCur(void *pCtx, MhalHdmitxAnaloDrvCurConfig_t *pstDrvCurCfg);

/// @brief Set Info Frame of HDMITX
/// @param[in] pCtx: Context of created HDMITX instance
/// @param[in] pstInfoFrameCfg: Configurationof of InfoFrame
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxSetInfoFrame(void *pCtx, MhalHdmitxInfoFrameConfig_t *pstInfoFrameCfg);

/// @brief Query sink information
/// @param[in] pCtx: Context of created HDMITX instance
/// @param[in] pstSinkInfoCfg: Configurationof of sink information
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxGetSinkInfo(void *pCtx, MhalHdmitxSinkInfoConfig_t *pstSinkInfoCfg);

/// @brief Set debug level of HDMITX module
/// @param[in] pCtx: Context of created HDMITX instance
/// @param[in] u32DbgLevel: debug level
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxSetDebugLevel(void *pCtx, u32 u32DbgLevel);

/// @brief Assign GPIO pin number of hot plug
/// @param[in] pCtx: Context of created hdmitx instance
/// @param[in] pstHpdCfg: Configurationof of GPIO pin number for hot plug
/// @return retval = E_MHAL_HDMITX_RET_SUCCESS is success,  Others is failure
INTERFACE MhalHdmitxRet_e MhalHdmitxSetHpdConfig(void *pCtx, MhalHdmitxHpdConfig_t *pstHpdCfg);

#endif
