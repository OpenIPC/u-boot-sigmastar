/*
* audio_test_ioctl.h- Sigmastar
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

#ifndef _AUDIO_TEST_IOCTL_H_
#define _AUDIO_TEST_IOCTL_H_

#define AUDIO_IOCTL_MAGIC   'S'

#define AUDRV_PCM_TEST          _IOW(AUDIO_IOCTL_MAGIC, 0x0, struct AUD_PcmCfg_s)
#define AUDRV_SET_DPGA_GAIN     _IOW(AUDIO_IOCTL_MAGIC, 0x1, struct AUD_GainCfg_s)
#define AUDRV_SET_ADC_GAIN      _IOW(AUDIO_IOCTL_MAGIC, 0x2, struct AUD_GainCfg_s)
#define AUDRV_SET_DMIC_GAIN     _IOW(AUDIO_IOCTL_MAGIC, 0x3, struct AUD_GainCfg_s)
#define AUDRV_BENCH_AIO_START   _IOW(AUDIO_IOCTL_MAGIC, 0x4, struct AUD_PcmCfg_s)
#define AUDRV_BENCH_AIO_STOP    _IO(AUDIO_IOCTL_MAGIC, 0x5)
#define AUDRV_BENCH_READ        _IOW(AUDIO_IOCTL_MAGIC, 0x6, struct AUD_BenchRead_s)
#define AUDRV_DMA_SINEGEN       _IOW(AUDIO_IOCTL_MAGIC, 0x7, struct AUD_SineGenCfg_s)

#define IOCTL_AUDIO_MAXNR 0x7

#endif /* _AUDIO_TEST_IOCTL_H_ */
