/*
* drv_audio_dbg.h- Sigmastar
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

#ifndef __DRV_AUDIO_DEBUG_H__
#define __DRV_AUDIO_DEBUG_H__
//------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------

#if 0 // All Enable
#define TRACE_LEVEL_TAG     1
#define ERROR_LEVEL_TAG     1
#define DEBUG_LEVEL_TAG     1
#define IRQ_LEVEL_TAG       1
#define MHAL_LEVEL_TAG      1
#define LC_LEVEL_TAG        1 // Level Count
#else
#define TRACE_LEVEL_TAG     0
#define ERROR_LEVEL_TAG     1
#define DEBUG_LEVEL_TAG     0
#define IRQ_LEVEL_TAG       0
#define MHAL_LEVEL_TAG      0
#define LC_LEVEL_TAG        0 // Level Count
#endif

#define TRACE_LEVEL         "[AUDIO TRACE]"
#define ERROR_LEVEL         "[AUDIO ERROR]"
#define DEBUG_LEVEL         "[AUDIO DEBUG]"
#define IRQ_LEVEL           "[AUDIO IRQ]"
#define MHAL_LEVEL          "[AUDIO MHAL]"
#define LC_LEVEL            "[AUDIO LC]"

#define LOG_MSG 					1
#if LOG_MSG
#define AUD_PRINTF(level ,fmt, arg...)		if (level##_TAG) CamOsPrintf(level fmt, ##arg);
#else
#define AUD_PRINTF(level ,fmt, arg...)
#endif

#endif  /* __DRV_AUDIO_DEBUG_H__ */
