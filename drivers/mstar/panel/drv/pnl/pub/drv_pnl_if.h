/*
* drv_pnl_if.h- Sigmastar
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

#ifndef _DRV_PNL_IF_H_
#define _DRV_PNL_IF_H_


#ifdef _DRV_PNL_IF_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif


INTERFACE bool DrvPnlIfCreateInstance(void **pCtx, MhalPnlLinkType_e enLinkType, u16 u16Id);
INTERFACE bool DrvPnlIfDestroyInstance(void *pCtx);
INTERFACE bool DrvPnlIfGetInstance(void **pCtx, MhalPnlLinkType_e enLinkType, u16 u16Id);
INTERFACE bool DrvPnlIfSetParamConfig(void *pCtx, MhalPnlParamConfig_t *pstParamCfg);
INTERFACE bool DrvPnlIfGetParamConfig(void *pCtx, MhalPnlParamConfig_t *pstParamCfg);
INTERFACE bool DrvPnlIfSetMipiDsiConfig(void *pCtx, MhalPnlMipiDsiConfig_t *pstMipiDsiCfg);
INTERFACE bool DrvPnlIfGetMipiDsiConfig(void *pCtx, MhalPnlMipiDsiConfig_t *pstMipiDsiCfg);
INTERFACE bool DrvPnlIfSetSscConfig(void *pCtx, MhalPnlSscConfig_t *pstSscCfg);
INTERFACE bool DrvPnlIfSetTimingConfig(void *pCtx, MhalPnlTimingConfig_t *pstTimingCfg);
INTERFACE bool DrvPnlIfGetTimingConfig(void *pCtx, MhalPnlTimingConfig_t *pstTimingCfg);
INTERFACE bool DrvPnlIfSetPowerConfig(void *pCtx, MhalPnlPowerConfig_t *pstPowerCfg);
INTERFACE bool DrvPnlIfGetPowerConfig(void *pCtx, MhalPnlPowerConfig_t *pstPowerCfg);
INTERFACE bool DrvPnlIfSetBackLightOnOffConfig(void *pCtx, MhalPnlBackLightOnOffConfig_t *pstBackLightOnOffCfg);
INTERFACE bool DrvPnlIfGetBackLightOnOffConfig(void *pCtx, MhalPnlBackLightOnOffConfig_t *pstBackLightOnOffCfg);
INTERFACE bool DrvPnlIfSetBackLightLevelConfig(void *pCtx, MhalPnlBackLightLevelConfig_t *pstBackLightLevelCfg);
INTERFACE bool DrvPnlIfGetBackLightLevelConfig(void *pCtx, MhalPnlBackLightLevelConfig_t *pstBackLightLevelCfg);
INTERFACE bool DrvPnlIfSetDrvCurrentConfig(void *pCtx, MhalPnlDrvCurrentConfig_t *pstCurrentCfg);
INTERFACE bool DrvPnlIfSetTestPatternConfig(void *pCtx, MhalPnlTestPatternConfig_t *pstTestPatternCfg);
INTERFACE bool DrvPnlIfSetDbgLevel(void *pDbgLevel);
INTERFACE bool DrvPnlIfSetClkConfig(void *pCtx, bool *pbEn, u32 *pu32ClkRate, u32 u32ClkNum);
INTERFACE bool DrvPnlIfGetClkConfig(void *pCtx, bool *pbEn, u32 *pu32ClkRate, u32 u32ClkNum);
//new
INTERFACE bool DrvPnlIfSetUnifiedParamConfig(void *pCtx, MhalPnlUnifiedParamConfig_t *pUdParamCfg);
INTERFACE bool DrvPnlIfGetUnifiedParamConfig(void *pCtx, MhalPnlUnifiedParamConfig_t *pUdParamCfg);

#undef INTERFACE
#endif
