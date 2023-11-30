/*
* gop_sysfs.c- Sigmastar
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

#define _GOP_SYSFS_C_

#include <linux/device.h>
#include "cam_os_wrapper.h"
#include "gop_ut.h"
#include "cam_sysfs.h"

#ifdef GOP_OS_TYPE_LINUX_KERNEL_TEST

ssize_t check_goptest_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if(NULL!=buf)
    {
        GopUtStrConfig_t stStringCfg;

        GopUtParsingCommand((char *)buf, &stStringCfg);

        GopTestStore(&stStringCfg);
        return n;
    }
    return 0;
}

ssize_t check_goptest_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return GopTestShow(buf);
}
static DEVICE_ATTR(goptest,0644, check_goptest_show, check_goptest_store);
#endif

void DrvGopSysfsInit(struct device *device)
{
#ifdef GOP_OS_TYPE_LINUX_KERNEL_TEST
    CamDeviceCreateFile(device, &dev_attr_goptest);
#endif
}
