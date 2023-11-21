/*
* mhal_pnl.h- Sigmastar
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


/**
 * \defgroup HAL_PNL_group  HAL_PNL driver
 * @{
 */
#ifndef __MHAL_PNL_H__
#define __MHAL_PNL_H__

#include "mhal_pnl_datatype.h"
//=============================================================================
// API
//=============================================================================

#ifndef __MHAL_PNL_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif


INTERFACE bool MhalPnlCreateInstance(void **pCtx, MhalPnlLinkType_e enLinkType);
INTERFACE bool MhalPnlCreateInstanceEx(void **pCtx, MhalPnlLinkType_e enLinkType, u16 u16Id);

INTERFACE bool MhalPnlDestroyInstance(void *pCtx);

// Param Config
INTERFACE bool MhalPnlSetParamConfig(void *pCtx, MhalPnlParamConfig_t *pParamCfg);
INTERFACE bool MhalPnlGetParamConfig(void *pCtx, MhalPnlParamConfig_t *pParamCfg);

// Mipi Dis Param Config
INTERFACE bool MhalPnlSetMipiDsiConfig(void *pCtx, MhalPnlMipiDsiConfig_t *pMipiDsiCfg);
INTERFACE bool MhalPnlGetMipiDsiConfig(void *pCtx, MhalPnlMipiDsiConfig_t *pMipiDsiCfg);

// SSC
INTERFACE bool MhalPnlSetSscConfig(void *pCtx, MhalPnlSscConfig_t *pSscCfg);

// Display Config
INTERFACE bool MhalPnlSetTimingConfig(void *pCtx, MhalPnlTimingConfig_t *pTimingCfg);
INTERFACE bool MhalPnlGetTimingConfig(void *pCtx, MhalPnlTimingConfig_t *pTimingCfg);


// Power
INTERFACE bool MhalPnlSetPowerConfig(void *pCtx, MhalPnlPowerConfig_t *pPowerCfg);
INTERFACE bool MhalPnlGetPowerConfig(void *pCtx, MhalPnlPowerConfig_t *pPowerCfg);

// BackLight
INTERFACE bool MhalPnlSetBackLightOnOffConfig(void *pCtx, MhalPnlBackLightOnOffConfig_t *pBackLightCfg);
INTERFACE bool MhalPnlGetBackLightOnOffConfig(void *pCtx, MhalPnlBackLightOnOffConfig_t *pBackLightCfg);

// BackLight Level
INTERFACE bool MhalPnlSetBackLightLevelConfig(void *pCtx, MhalPnlBackLightLevelConfig_t *pBackLightCfg);
INTERFACE bool MhalPnlGetBackLightLevelConfig(void *pCtx, MhalPnlBackLightLevelConfig_t *pBackLightCfg);

// DrvCurrent
INTERFACE bool MhalPnlSetDrvCurrentConfig(void *pCtx, MhalPnlDrvCurrentConfig_t *pDrvCurrentCfg);

// Test Pattern
INTERFACE bool MhalPnlSetTestPatternConfig(void *pCtx, MhalPnlTestPatternConfig_t *pTestPatternCfg);

// Set Debug Level
INTERFACE bool MhalPnlSetDebugLevel(void *pDbgLevel);

//new unified config
INTERFACE bool MhalPnlSetUnifiedParamConfig(void *pCtx, MhalPnlUnifiedParamConfig_t *pUdParamCfg);
INTERFACE bool MhalPnlGetUnifiedParamConfig(void *pCtx, MhalPnlUnifiedParamConfig_t *pUdParamCfg);

#undef INTERFACE

#endif //
/** @} */ // end of HAL_PNL_group
