/*
* drv_audio_io.h- Sigmastar
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

#ifndef DRV_AUDIO_IO_H_
#define DRV_AUDIO_IO_H_

#define TRACE_UT_LEVEL_TAG  0
#define ERROR_UT_LEVEL_TAG  1
#define LOG_UT_LEVEL_TAG    1

#define TRACE_UT_LEVEL  "[AUD UT TRACE] "
#define ERROR_UT_LEVEL  "[AUD UT ERROR] "
#define LOG_UT_LEVEL    "[AUD UT LOG] "

#define LOG_MSG (1)
#if LOG_MSG
#define AUD_UT_PRINTF(level ,fmt, arg...)   if (level##_TAG) CamOsPrintf(level fmt, ##arg);
#else
#define AUD_UT_PRINTF(level ,fmt, arg...)
#endif

#endif /* DRV_AUDIO_IO_H_ */
