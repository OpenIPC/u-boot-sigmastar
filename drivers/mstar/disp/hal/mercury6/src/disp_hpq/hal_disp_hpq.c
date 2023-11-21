/*
* hal_disp_hpq.c- Sigmastar
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

#define _HAL_DISP_HPQ_C_

#include "drv_disp_os.h"

#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"

#include "hal_disp_st.h"
#include "hal_disp_hpq.h"

//hpq
#include "drv_disp_hpq_os.h"
#include "hal_scl_pq_color_reg.h"
#include "drv_scl_pq_define.h"
#include "drv_scl_pq_declare.h"
#include "drv_scl_pq.h"
#include "drv_scl_pq_data_types.h"
#include "drv_disp_pq_bin.h"

#include "hal_disp_hpq_main.c"
#include "hal_disp_hpq_if.c"

#include "drv_disp_hpq_bin.c"



//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

// ----------------------------------
void HalDispHpqLoadBin(void *pCtx, void *pCfg)
{
    HalDispPqConfig_t *pPqCfg = (HalDispPqConfig_t *)pCfg;
   	MS_PQ_Init_Info stPqInfo;

    memset(&stPqInfo, 0, sizeof(stPqInfo));

	stPqInfo.u8PQTextBinCnt                 =   0;
	stPqInfo.u8PQBinCnt 	                =   1;
	stPqInfo.stPQBinInfo[0].PQ_Bin_BufSize  =	pPqCfg->u32DataSize;
	stPqInfo.stPQBinInfo[0].pPQBin_AddrVirt =   (void *)pPqCfg->pData;
	stPqInfo.stPQBinInfo[0].PQBin_PhyAddr   =   (u32)pPqCfg->pData;
    stPqInfo.stPQBinInfo[0].u8PQID          =   0;
    
	MDrv_PQ_Init(&stPqInfo, pCtx);
    MDrv_PQ_LoadSettings(0);
}

void HalDispHpqLoadData(u16 u16Id, u8 *pu8Data, u32 u32DataSize)
{
    MDrv_PQ_LoadTableData(0, 0, u16Id, pu8Data, u32DataSize);
}
