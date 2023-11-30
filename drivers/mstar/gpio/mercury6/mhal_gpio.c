/*
* mhal_gpio.c- Sigmastar
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

#ifdef UBOOT
#include <linux/kernel.h>
#include <linux/irqdomain.h>
#endif
#include "mhal_gpio.h"
#include "gpio.h"
#include "padmux.h"
#include "mhal_pinmux.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define _CONCAT( a, b )     a##b
#define CONCAT( a, b )      _CONCAT( a, b )

// PADTOP
#define GPIO0_PAD        PAD_PM_UART_RX1
#define GPIO0_OEN        0x3F32, BIT2
#define GPIO0_OUT        0x3F32, BIT1
#define GPIO0_IN         0x3F32, BIT0

#define GPIO1_PAD        PAD_PM_UART_TX1
#define GPIO1_OEN        0x3F40, BIT2
#define GPIO1_OUT        0x3F40, BIT1
#define GPIO1_IN         0x3F40, BIT0

#define GPIO2_PAD        PAD_PM_UART_RX
#define GPIO2_OEN        0x3F42, BIT2
#define GPIO2_OUT        0x3F42, BIT1
#define GPIO2_IN         0x3F42, BIT0

#define GPIO3_PAD        PAD_PM_UART_TX
#define GPIO3_OEN        0x3F44, BIT2
#define GPIO3_OUT        0x3F44, BIT1
#define GPIO3_IN         0x3F44, BIT0

#define GPIO4_PAD        PAD_PM_I2CM_SCL
#define GPIO4_OEN        0x3F22, BIT2
#define GPIO4_OUT        0x3F22, BIT1
#define GPIO4_IN         0x3F22, BIT0

#define GPIO5_PAD        PAD_PM_I2CM_SDA
#define GPIO5_OEN        0x3F24, BIT2
#define GPIO5_OUT        0x3F24, BIT1
#define GPIO5_IN         0x3F24, BIT0

#define GPIO6_PAD        PAD_PM_GPIO0
#define GPIO6_OEN        0x3F00, BIT2
#define GPIO6_OUT        0x3F00, BIT1
#define GPIO6_IN         0x3F00, BIT0

#define GPIO7_PAD        PAD_PM_GPIO1
#define GPIO7_OEN        0x3F02, BIT2
#define GPIO7_OUT        0x3F02, BIT1
#define GPIO7_IN         0x3F02, BIT0

#define GPIO8_PAD        PAD_PM_GPIO2
#define GPIO8_OEN        0x3F04, BIT2
#define GPIO8_OUT        0x3F04, BIT1
#define GPIO8_IN         0x3F04, BIT0

#define GPIO9_PAD        PAD_PM_GPIO3
#define GPIO9_OEN        0x3F06, BIT2
#define GPIO9_OUT        0x3F06, BIT1
#define GPIO9_IN         0x3F06, BIT0

#define GPIO10_PAD       PAD_PM_GPIO4
#define GPIO10_OEN       0x3F08, BIT2
#define GPIO10_OUT       0x3F08, BIT1
#define GPIO10_IN        0x3F08, BIT0

#define GPIO11_PAD       PAD_PM_GPIO5
#define GPIO11_OEN       0x3F0A, BIT2
#define GPIO11_OUT       0x3F0A, BIT1
#define GPIO11_IN        0x3F0A, BIT0

#define GPIO12_PAD       PAD_PM_GPIO6
#define GPIO12_OEN       0x3F0C, BIT2
#define GPIO12_OUT       0x3F0C, BIT1
#define GPIO12_IN        0x3F0C, BIT0

#define GPIO13_PAD       PAD_PM_GPIO7
#define GPIO13_OEN       0x3F0E, BIT2
#define GPIO13_OUT       0x3F0E, BIT1
#define GPIO13_IN        0x3F0E, BIT0

#define GPIO14_PAD       PAD_PM_SPI_CZ
#define GPIO14_OEN       0x3F26, BIT2
#define GPIO14_OUT       0x3F26, BIT1
#define GPIO14_IN        0x3F26, BIT0

#define GPIO15_PAD       PAD_PM_SPI_CK
#define GPIO15_OEN       0x3F28, BIT2
#define GPIO15_OUT       0x3F28, BIT1
#define GPIO15_IN        0x3F28, BIT0

#define GPIO16_PAD       PAD_PM_SPI_DI
#define GPIO16_OEN       0x3F2A, BIT2
#define GPIO16_OUT       0x3F2A, BIT1
#define GPIO16_IN        0x3F2A, BIT0

#define GPIO17_PAD       PAD_PM_SPI_DO
#define GPIO17_OEN       0x3F2C, BIT2
#define GPIO17_OUT       0x3F2C, BIT1
#define GPIO17_IN        0x3F2C, BIT0

#define GPIO18_PAD       PAD_PM_SPI_WPZ
#define GPIO18_OEN       0x3F2E, BIT2
#define GPIO18_OUT       0x3F2E, BIT1
#define GPIO18_IN        0x3F2E, BIT0

#define GPIO19_PAD       PAD_PM_SPI_HLD
#define GPIO19_OEN       0x3F30, BIT2
#define GPIO19_OUT       0x3F30, BIT1
#define GPIO19_IN        0x3F30, BIT0

#define GPIO20_PAD       PAD_PM_IRIN
#define GPIO20_OEN       0x3F46, BIT2
#define GPIO20_OUT       0x3F46, BIT1
#define GPIO20_IN        0x3F46, BIT0

#define GPIO21_PAD       PAD_SAR_GPIO0
#define GPIO21_OEN       0x1423, BIT0
#define GPIO21_OUT       0x1424, BIT0
#define GPIO21_IN        0x1425, BIT0

#define GPIO22_PAD       PAD_SAR_GPIO1
#define GPIO22_OEN       0x1423, BIT1
#define GPIO22_OUT       0x1424, BIT1
#define GPIO22_IN        0x1425, BIT1

#define GPIO23_PAD       PAD_SAR_GPIO2
#define GPIO23_OEN       0x1423, BIT2
#define GPIO23_OUT       0x1424, BIT2
#define GPIO23_IN        0x1425, BIT2

#define GPIO24_PAD       PAD_SAR_GPIO3
#define GPIO24_OEN       0x1423, BIT3
#define GPIO24_OUT       0x1424, BIT3
#define GPIO24_IN        0x1425, BIT3

#define GPIO25_PAD       PAD_SD0_CDZ
#define GPIO25_OEN       0x103E00, BIT2
#define GPIO25_OUT       0x103E00, BIT1
#define GPIO25_IN        0x103E00, BIT0

#define GPIO26_PAD       PAD_SD0_D1
#define GPIO26_OEN       0x103E02, BIT2
#define GPIO26_OUT       0x103E02, BIT1
#define GPIO26_IN        0x103E02, BIT0

#define GPIO27_PAD       PAD_SD0_D0
#define GPIO27_OEN       0x103E04, BIT2
#define GPIO27_OUT       0x103E04, BIT1
#define GPIO27_IN        0x103E04, BIT0

#define GPIO28_PAD       PAD_SD0_CLK
#define GPIO28_OEN       0x103E06, BIT2
#define GPIO28_OUT       0x103E06, BIT1
#define GPIO28_IN        0x103E06, BIT0

#define GPIO29_PAD       PAD_SD0_CMD
#define GPIO29_OEN       0x103E08, BIT2
#define GPIO29_OUT       0x103E08, BIT1
#define GPIO29_IN        0x103E08, BIT0

#define GPIO30_PAD       PAD_SD0_D3
#define GPIO30_OEN       0x103E0A, BIT2
#define GPIO30_OUT       0x103E0A, BIT1
#define GPIO30_IN        0x103E0A, BIT0

#define GPIO31_PAD       PAD_SD0_D2
#define GPIO31_OEN       0x103E0C, BIT2
#define GPIO31_OUT       0x103E0C, BIT1
#define GPIO31_IN        0x103E0C, BIT0

#define GPIO32_PAD       PAD_FUART_RX
#define GPIO32_OEN       0x103E0E, BIT2
#define GPIO32_OUT       0x103E0E, BIT1
#define GPIO32_IN        0x103E0E, BIT0

#define GPIO33_PAD       PAD_FUART_TX
#define GPIO33_OEN       0x103E10, BIT2
#define GPIO33_OUT       0x103E10, BIT1
#define GPIO33_IN        0x103E10, BIT0

#define GPIO34_PAD       PAD_FUART_CTS
#define GPIO34_OEN       0x103E12, BIT2
#define GPIO34_OUT       0x103E12, BIT1
#define GPIO34_IN        0x103E12, BIT0

#define GPIO35_PAD       PAD_FUART_RTS
#define GPIO35_OEN       0x103E14, BIT2
#define GPIO35_OUT       0x103E14, BIT1
#define GPIO35_IN        0x103E14, BIT0

#define GPIO36_PAD       PAD_I2C0_SCL
#define GPIO36_OEN       0x103E16, BIT2
#define GPIO36_OUT       0x103E16, BIT1
#define GPIO36_IN        0x103E16, BIT0

#define GPIO37_PAD       PAD_I2C0_SDA
#define GPIO37_OEN       0x103E18, BIT2
#define GPIO37_OUT       0x103E18, BIT1
#define GPIO37_IN        0x103E18, BIT0

#define GPIO38_PAD       PAD_ETH_LED0
#define GPIO38_OEN       0x103E1A, BIT2
#define GPIO38_OUT       0x103E1A, BIT1
#define GPIO38_IN        0x103E1A, BIT0

#define GPIO39_PAD       PAD_ETH_LED1
#define GPIO39_OEN       0x103E1C, BIT2
#define GPIO39_OUT       0x103E1C, BIT1
#define GPIO39_IN        0x103E1C, BIT0

#define GPIO40_PAD       PAD_SPI0_DO
#define GPIO40_OEN       0x103E1E, BIT2
#define GPIO40_OUT       0x103E1E, BIT1
#define GPIO40_IN        0x103E1E, BIT0

#define GPIO41_PAD       PAD_SPI0_DI
#define GPIO41_OEN       0x103E20, BIT2
#define GPIO41_OUT       0x103E20, BIT1
#define GPIO41_IN        0x103E20, BIT0

#define GPIO42_PAD       PAD_SPI0_CK
#define GPIO42_OEN       0x103E22, BIT2
#define GPIO42_OUT       0x103E22, BIT1
#define GPIO42_IN        0x103E22, BIT0

#define GPIO43_PAD       PAD_SPI0_CZ
#define GPIO43_OEN       0x103E24, BIT2
#define GPIO43_OUT       0x103E24, BIT1
#define GPIO43_IN        0x103E24, BIT0

#define GPIO44_PAD       PAD_GPIO0
#define GPIO44_OEN       0x103E26, BIT2
#define GPIO44_OUT       0x103E26, BIT1
#define GPIO44_IN        0x103E26, BIT0

#define GPIO45_PAD       PAD_GPIO1
#define GPIO45_OEN       0x103E28, BIT2
#define GPIO45_OUT       0x103E28, BIT1
#define GPIO45_IN        0x103E28, BIT0

#define GPIO46_PAD       PAD_GPIO2
#define GPIO46_OEN       0x103E2A, BIT2
#define GPIO46_OUT       0x103E2A, BIT1
#define GPIO46_IN        0x103E2A, BIT0

#define GPIO47_PAD       PAD_GPIO3
#define GPIO47_OEN       0x103E2C, BIT2
#define GPIO47_OUT       0x103E2C, BIT1
#define GPIO47_IN        0x103E2C, BIT0

#define GPIO48_PAD       PAD_GPIO4
#define GPIO48_OEN       0x103E2E, BIT2
#define GPIO48_OUT       0x103E2E, BIT1
#define GPIO48_IN        0x103E2E, BIT0

#define GPIO49_PAD       PAD_GPIO5
#define GPIO49_OEN       0x103E30, BIT2
#define GPIO49_OUT       0x103E30, BIT1
#define GPIO49_IN        0x103E30, BIT0

#define GPIO50_PAD       PAD_GPIO6
#define GPIO50_OEN       0x103E32, BIT2
#define GPIO50_OUT       0x103E32, BIT1
#define GPIO50_IN        0x103E32, BIT0

#define GPIO51_PAD       PAD_GPIO7
#define GPIO51_OEN       0x103E34, BIT2
#define GPIO51_OUT       0x103E34, BIT1
#define GPIO51_IN        0x103E34, BIT0

#define GPIO52_PAD       PAD_SR0_IO00
#define GPIO52_OEN       0x103E36, BIT2
#define GPIO52_OUT       0x103E36, BIT1
#define GPIO52_IN        0x103E36, BIT0

#define GPIO53_PAD       PAD_SR0_IO01
#define GPIO53_OEN       0x103E38, BIT2
#define GPIO53_OUT       0x103E38, BIT1
#define GPIO53_IN        0x103E38, BIT0

#define GPIO54_PAD       PAD_SR0_IO02
#define GPIO54_OEN       0x103E3A, BIT2
#define GPIO54_OUT       0x103E3A, BIT1
#define GPIO54_IN        0x103E3A, BIT0

#define GPIO55_PAD       PAD_SR0_IO03
#define GPIO55_OEN       0x103E3C, BIT2
#define GPIO55_OUT       0x103E3C, BIT1
#define GPIO55_IN        0x103E3C, BIT0

#define GPIO56_PAD       PAD_SR0_IO04
#define GPIO56_OEN       0x103E3E, BIT2
#define GPIO56_OUT       0x103E3E, BIT1
#define GPIO56_IN        0x103E3E, BIT0

#define GPIO57_PAD       PAD_SR0_IO05
#define GPIO57_OEN       0x103E40, BIT2
#define GPIO57_OUT       0x103E40, BIT1
#define GPIO57_IN        0x103E40, BIT0

#define GPIO58_PAD       PAD_SR0_IO06
#define GPIO58_OEN       0x103E42, BIT2
#define GPIO58_OUT       0x103E42, BIT1
#define GPIO58_IN        0x103E42, BIT0

#define GPIO59_PAD       PAD_SR0_IO07
#define GPIO59_OEN       0x103E44, BIT2
#define GPIO59_OUT       0x103E44, BIT1
#define GPIO59_IN        0x103E44, BIT0

#define GPIO60_PAD       PAD_SR0_IO08
#define GPIO60_OEN       0x103E46, BIT2
#define GPIO60_OUT       0x103E46, BIT1
#define GPIO60_IN        0x103E46, BIT0

#define GPIO61_PAD       PAD_SR0_IO09
#define GPIO61_OEN       0x103E48, BIT2
#define GPIO61_OUT       0x103E48, BIT1
#define GPIO61_IN        0x103E48, BIT0

#define GPIO62_PAD       PAD_SR0_IO10
#define GPIO62_OEN       0x103E4A, BIT2
#define GPIO62_OUT       0x103E4A, BIT1
#define GPIO62_IN        0x103E4A, BIT0

#define GPIO63_PAD       PAD_SR0_IO11
#define GPIO63_OEN       0x103E4C, BIT2
#define GPIO63_OUT       0x103E4C, BIT1
#define GPIO63_IN        0x103E4C, BIT0

#define GPIO64_PAD       PAD_SR0_IO12
#define GPIO64_OEN       0x103E4E, BIT2
#define GPIO64_OUT       0x103E4E, BIT1
#define GPIO64_IN        0x103E4E, BIT0

#define GPIO65_PAD       PAD_SR0_IO13
#define GPIO65_OEN       0x103E50, BIT2
#define GPIO65_OUT       0x103E50, BIT1
#define GPIO65_IN        0x103E50, BIT0

#define GPIO66_PAD       PAD_SR0_IO14
#define GPIO66_OEN       0x103E52, BIT2
#define GPIO66_OUT       0x103E52, BIT1
#define GPIO66_IN        0x103E52, BIT0

#define GPIO67_PAD       PAD_SR0_IO15
#define GPIO67_OEN       0x103E54, BIT2
#define GPIO67_OUT       0x103E54, BIT1
#define GPIO67_IN        0x103E54, BIT0

#define GPIO68_PAD       PAD_SR0_IO16
#define GPIO68_OEN       0x103E56, BIT2
#define GPIO68_OUT       0x103E56, BIT1
#define GPIO68_IN        0x103E56, BIT0

#define GPIO69_PAD       PAD_SR0_IO17
#define GPIO69_OEN       0x103E58, BIT2
#define GPIO69_OUT       0x103E58, BIT1
#define GPIO69_IN        0x103E58, BIT0

#define GPIO70_PAD       PAD_SR0_IO18
#define GPIO70_OEN       0x103E5A, BIT2
#define GPIO70_OUT       0x103E5A, BIT1
#define GPIO70_IN        0x103E5A, BIT0

#define GPIO71_PAD       PAD_SR0_IO19
#define GPIO71_OEN       0x103E5C, BIT2
#define GPIO71_OUT       0x103E5C, BIT1
#define GPIO71_IN        0x103E5C, BIT0

#define GPIO72_PAD       PAD_SR1_IO00
#define GPIO72_OEN       0x103E5E, BIT2
#define GPIO72_OUT       0x103E5E, BIT1
#define GPIO72_IN        0x103E5E, BIT0

#define GPIO73_PAD       PAD_SR1_IO01
#define GPIO73_OEN       0x103E60, BIT2
#define GPIO73_OUT       0x103E60, BIT1
#define GPIO73_IN        0x103E60, BIT0

#define GPIO74_PAD       PAD_SR1_IO02
#define GPIO74_OEN       0x103E62, BIT2
#define GPIO74_OUT       0x103E62, BIT1
#define GPIO74_IN        0x103E62, BIT0

#define GPIO75_PAD       PAD_SR1_IO03
#define GPIO75_OEN       0x103E64, BIT2
#define GPIO75_OUT       0x103E64, BIT1
#define GPIO75_IN        0x103E64, BIT0

#define GPIO76_PAD       PAD_SR1_IO04
#define GPIO76_OEN       0x103E66, BIT2
#define GPIO76_OUT       0x103E66, BIT1
#define GPIO76_IN        0x103E66, BIT0

#define GPIO77_PAD       PAD_SR1_IO05
#define GPIO77_OEN       0x103E68, BIT2
#define GPIO77_OUT       0x103E68, BIT1
#define GPIO77_IN        0x103E68, BIT0

#define GPIO78_PAD       PAD_SR1_IO06
#define GPIO78_OEN       0x103E6A, BIT2
#define GPIO78_OUT       0x103E6A, BIT1
#define GPIO78_IN        0x103E6A, BIT0

#define GPIO79_PAD       PAD_SR1_IO07
#define GPIO79_OEN       0x103E6C, BIT2
#define GPIO79_OUT       0x103E6C, BIT1
#define GPIO79_IN        0x103E6C, BIT0

#define GPIO80_PAD       PAD_SR1_IO08
#define GPIO80_OEN       0x103E6E, BIT2
#define GPIO80_OUT       0x103E6E, BIT1
#define GPIO80_IN        0x103E6E, BIT0

#define GPIO81_PAD       PAD_SR1_IO09
#define GPIO81_OEN       0x103E70, BIT2
#define GPIO81_OUT       0x103E70, BIT1
#define GPIO81_IN        0x103E70, BIT0

#define GPIO82_PAD       PAD_SR1_IO10
#define GPIO82_OEN       0x103E72, BIT2
#define GPIO82_OUT       0x103E72, BIT1
#define GPIO82_IN        0x103E72, BIT0

#define GPIO83_PAD       PAD_SR1_IO11
#define GPIO83_OEN       0x103E74, BIT2
#define GPIO83_OUT       0x103E74, BIT1
#define GPIO83_IN        0x103E74, BIT0

#define GPIO84_PAD       PAD_SR1_IO12
#define GPIO84_OEN       0x103E76, BIT2
#define GPIO84_OUT       0x103E76, BIT1
#define GPIO84_IN        0x103E76, BIT0

#define GPIO85_PAD       PAD_SR1_IO13
#define GPIO85_OEN       0x103E78, BIT2
#define GPIO85_OUT       0x103E78, BIT1
#define GPIO85_IN        0x103E78, BIT0

#define GPIO86_PAD       PAD_SR1_IO14
#define GPIO86_OEN       0x103E7A, BIT2
#define GPIO86_OUT       0x103E7A, BIT1
#define GPIO86_IN        0x103E7A, BIT0

#define GPIO87_PAD       PAD_SR1_IO15
#define GPIO87_OEN       0x103E7C, BIT2
#define GPIO87_OUT       0x103E7C, BIT1
#define GPIO87_IN        0x103E7C, BIT0

#define GPIO88_PAD       PAD_SR1_IO16
#define GPIO88_OEN       0x103E7E, BIT2
#define GPIO88_OUT       0x103E7E, BIT1
#define GPIO88_IN        0x103E7E, BIT0

#define GPIO89_PAD       PAD_SR1_IO17
#define GPIO89_OEN       0x103E80, BIT2
#define GPIO89_OUT       0x103E80, BIT1
#define GPIO89_IN        0x103E80, BIT0

#define GPIO90_PAD       PAD_SR1_IO18
#define GPIO90_OEN       0x103E82, BIT2
#define GPIO90_OUT       0x103E82, BIT1
#define GPIO90_IN        0x103E82, BIT0

#define GPIO91_PAD       PAD_SR1_IO19
#define GPIO91_OEN       0x103E84, BIT2
#define GPIO91_OUT       0x103E84, BIT1
#define GPIO91_IN        0x103E84, BIT0

#define GPIO92_PAD       PAD_TX0_IO00
#define GPIO92_OEN       0x103E86, BIT2
#define GPIO92_OUT       0x103E86, BIT1
#define GPIO92_IN        0x103E86, BIT0

#define GPIO93_PAD       PAD_TX0_IO01
#define GPIO93_OEN       0x103E88, BIT2
#define GPIO93_OUT       0x103E88, BIT1
#define GPIO93_IN        0x103E88, BIT0

#define GPIO94_PAD       PAD_TX0_IO02
#define GPIO94_OEN       0x103E8A, BIT2
#define GPIO94_OUT       0x103E8A, BIT1
#define GPIO94_IN        0x103E8A, BIT0

#define GPIO95_PAD       PAD_TX0_IO03
#define GPIO95_OEN       0x103E8C, BIT2
#define GPIO95_OUT       0x103E8C, BIT1
#define GPIO95_IN        0x103E8C, BIT0

#define GPIO96_PAD       PAD_TX0_IO04
#define GPIO96_OEN       0x103E8E, BIT2
#define GPIO96_OUT       0x103E8E, BIT1
#define GPIO96_IN        0x103E8E, BIT0

#define GPIO97_PAD       PAD_TX0_IO05
#define GPIO97_OEN       0x103E90, BIT2
#define GPIO97_OUT       0x103E90, BIT1
#define GPIO97_IN        0x103E90, BIT0

#define GPIO98_PAD       PAD_TX0_IO06
#define GPIO98_OEN       0x103E92, BIT2
#define GPIO98_OUT       0x103E92, BIT1
#define GPIO98_IN        0x103E92, BIT0

#define GPIO99_PAD       PAD_TX0_IO07
#define GPIO99_OEN       0x103E94, BIT2
#define GPIO99_OUT       0x103E94, BIT1
#define GPIO99_IN        0x103E94, BIT0

#define GPIO100_PAD      PAD_TX0_IO08
#define GPIO100_OEN      0x103E96, BIT2
#define GPIO100_OUT      0x103E96, BIT1
#define GPIO100_IN       0x103E96, BIT0

#define GPIO101_PAD      PAD_TX0_IO09
#define GPIO101_OEN      0x103E98, BIT2
#define GPIO101_OUT      0x103E98, BIT1
#define GPIO101_IN       0x103E98, BIT0

#define GPIO102_PAD      PAD_TTL0
#define GPIO102_OEN      0x103E9A, BIT2
#define GPIO102_OUT      0x103E9A, BIT1
#define GPIO102_IN       0x103E9A, BIT0

#define GPIO103_PAD      PAD_TTL1
#define GPIO103_OEN      0x103E9C, BIT2
#define GPIO103_OUT      0x103E9C, BIT1
#define GPIO103_IN       0x103E9C, BIT0

#define GPIO104_PAD      PAD_TTL2
#define GPIO104_OEN      0x103E9E, BIT2
#define GPIO104_OUT      0x103E9E, BIT1
#define GPIO104_IN       0x103E9E, BIT0

#define GPIO105_PAD      PAD_TTL3
#define GPIO105_OEN      0x103EA0, BIT2
#define GPIO105_OUT      0x103EA0, BIT1
#define GPIO105_IN       0x103EA0, BIT0

#define GPIO106_PAD      PAD_TTL4
#define GPIO106_OEN      0x103EA2, BIT2
#define GPIO106_OUT      0x103EA2, BIT1
#define GPIO106_IN       0x103EA2, BIT0

#define GPIO107_PAD      PAD_TTL5
#define GPIO107_OEN      0x103EA4, BIT2
#define GPIO107_OUT      0x103EA4, BIT1
#define GPIO107_IN       0x103EA4, BIT0

#define GPIO108_PAD      PAD_TTL6
#define GPIO108_OEN      0x103EA6, BIT2
#define GPIO108_OUT      0x103EA6, BIT1
#define GPIO108_IN       0x103EA6, BIT0

#define GPIO109_PAD      PAD_TTL7
#define GPIO109_OEN      0x103EA8, BIT2
#define GPIO109_OUT      0x103EA8, BIT1
#define GPIO109_IN       0x103EA8, BIT0

#define GPIO110_PAD      PAD_TTL8
#define GPIO110_OEN      0x103EAA, BIT2
#define GPIO110_OUT      0x103EAA, BIT1
#define GPIO110_IN       0x103EAA, BIT0

#define GPIO111_PAD      PAD_TTL9
#define GPIO111_OEN      0x103EAC, BIT2
#define GPIO111_OUT      0x103EAC, BIT1
#define GPIO111_IN       0x103EAC, BIT0

#define GPIO112_PAD      PAD_TTL10
#define GPIO112_OEN      0x103EAE, BIT2
#define GPIO112_OUT      0x103EAE, BIT1
#define GPIO112_IN       0x103EAE, BIT0

#define GPIO113_PAD      PAD_TTL11
#define GPIO113_OEN      0x103EB0, BIT2
#define GPIO113_OUT      0x103EB0, BIT1
#define GPIO113_IN       0x103EB0, BIT0

#define GPIO114_PAD      PAD_TTL12
#define GPIO114_OEN      0x103EB2, BIT2
#define GPIO114_OUT      0x103EB2, BIT1
#define GPIO114_IN       0x103EB2, BIT0

#define GPIO115_PAD      PAD_TTL13
#define GPIO115_OEN      0x103EB4, BIT2
#define GPIO115_OUT      0x103EB4, BIT1
#define GPIO115_IN       0x103EB4, BIT0

#define GPIO116_PAD      PAD_TTL14
#define GPIO116_OEN      0x103EB6, BIT2
#define GPIO116_OUT      0x103EB6, BIT1
#define GPIO116_IN       0x103EB6, BIT0

#define GPIO117_PAD      PAD_TTL15
#define GPIO117_OEN      0x103EB8, BIT2
#define GPIO117_OUT      0x103EB8, BIT1
#define GPIO117_IN       0x103EB8, BIT0

#define GPIO118_PAD      PAD_TTL16
#define GPIO118_OEN      0x103EBA, BIT2
#define GPIO118_OUT      0x103EBA, BIT1
#define GPIO118_IN       0x103EBA, BIT0

#define GPIO119_PAD      PAD_TTL17
#define GPIO119_OEN      0x103EBC, BIT2
#define GPIO119_OUT      0x103EBC, BIT1
#define GPIO119_IN       0x103EBC, BIT0

#define GPIO120_PAD      PAD_TTL18
#define GPIO120_OEN      0x103EBE, BIT2
#define GPIO120_OUT      0x103EBE, BIT1
#define GPIO120_IN       0x103EBE, BIT0

#define GPIO121_PAD      PAD_TTL19
#define GPIO121_OEN      0x103EC0, BIT2
#define GPIO121_OUT      0x103EC0, BIT1
#define GPIO121_IN       0x103EC0, BIT0

#define GPIO122_PAD      PAD_TTL20
#define GPIO122_OEN      0x103EC2, BIT2
#define GPIO122_OUT      0x103EC2, BIT1
#define GPIO122_IN       0x103EC2, BIT0

#define GPIO123_PAD      PAD_TTL21
#define GPIO123_OEN      0x103EC4, BIT2
#define GPIO123_OUT      0x103EC4, BIT1
#define GPIO123_IN       0x103EC4, BIT0

#define GPIO124_PAD      PAD_TTL22
#define GPIO124_OEN      0x103EC6, BIT2
#define GPIO124_OUT      0x103EC6, BIT1
#define GPIO124_IN       0x103EC6, BIT0

#define GPIO125_PAD      PAD_TTL23
#define GPIO125_OEN      0x103EC8, BIT2
#define GPIO125_OUT      0x103EC8, BIT1
#define GPIO125_IN       0x103EC8, BIT0

#define GPIO126_PAD      PAD_TTL24
#define GPIO126_OEN      0x103ECA, BIT2
#define GPIO126_OUT      0x103ECA, BIT1
#define GPIO126_IN       0x103ECA, BIT0

#define GPIO127_PAD      PAD_TTL25
#define GPIO127_OEN      0x103ECC, BIT2
#define GPIO127_OUT      0x103ECC, BIT1
#define GPIO127_IN       0x103ECC, BIT0

#define GPIO128_PAD      PAD_TTL26
#define GPIO128_OEN      0x103ECE, BIT2
#define GPIO128_OUT      0x103ECE, BIT1
#define GPIO128_IN       0x103ECE, BIT0

#define GPIO129_PAD      PAD_TTL27
#define GPIO129_OEN      0x103ED0, BIT2
#define GPIO129_OUT      0x103ED0, BIT1
#define GPIO129_IN       0x103ED0, BIT0

#define GPIO130_PAD      PAD_SATA_GPIO
#define GPIO130_OEN      0x103ED2, BIT2
#define GPIO130_OUT      0x103ED2, BIT1
#define GPIO130_IN       0x103ED2, BIT0

#define GPIO131_PAD      PAD_HDMITX_HPD
#define GPIO131_OEN      0x103ED4, BIT2
#define GPIO131_OUT      0x103ED4, BIT1
#define GPIO131_IN       0x103ED4, BIT0

#define GPIO132_PAD      PAD_HDMITX_SDA
#define GPIO132_OEN      0x103ED6, BIT2
#define GPIO132_OUT      0x103ED6, BIT1
#define GPIO132_IN       0x103ED6, BIT0

#define GPIO133_PAD      PAD_HDMITX_SCL
#define GPIO133_OEN      0x103ED8, BIT2
#define GPIO133_OUT      0x103ED8, BIT1
#define GPIO133_IN       0x103ED8, BIT0

#define GPIO134_PAD      PAD_VSYNC_OUT
#define GPIO134_OEN      0x103EDA, BIT2
#define GPIO134_OUT      0x103EDA, BIT1
#define GPIO134_IN       0x103EDA, BIT0

#define GPIO135_PAD      PAD_HSYNC_OUT
#define GPIO135_OEN      0x103EDC, BIT2
#define GPIO135_OUT      0x103EDC, BIT1
#define GPIO135_IN       0x103EDC, BIT0

#define GPIO136_PAD      PAD_GPIO8
#define GPIO136_OEN      0x103EDE, BIT2
#define GPIO136_OUT      0x103EDE, BIT1
#define GPIO136_IN       0x103EDE, BIT0

#define GPIO137_PAD      PAD_GPIO9
#define GPIO137_OEN      0x103EE0, BIT2
#define GPIO137_OUT      0x103EE0, BIT1
#define GPIO137_IN       0x103EE0, BIT0

#define GPIO138_PAD      PAD_GPIO10
#define GPIO138_OEN      0x103EE2, BIT2
#define GPIO138_OUT      0x103EE2, BIT1
#define GPIO138_IN       0x103EE2, BIT0

#define GPIO139_PAD      PAD_GPIO11
#define GPIO139_OEN      0x103EE4, BIT2
#define GPIO139_OUT      0x103EE4, BIT1
#define GPIO139_IN       0x103EE4, BIT0

#define GPIO140_PAD      PAD_GPIO12
#define GPIO140_OEN      0x103EE6, BIT2
#define GPIO140_OUT      0x103EE6, BIT1
#define GPIO140_IN       0x103EE6, BIT0

#define GPIO141_PAD      PAD_GPIO13
#define GPIO141_OEN      0x103EE8, BIT2
#define GPIO141_OUT      0x103EE8, BIT1
#define GPIO141_IN       0x103EE8, BIT0

#define GPIO142_PAD      PAD_GPIO14
#define GPIO142_OEN      0x103EEA, BIT2
#define GPIO142_OUT      0x103EEA, BIT1
#define GPIO142_IN       0x103EEA, BIT0

#define GPIO143_PAD      PAD_GPIO15
#define GPIO143_OEN      0x103EEC, BIT2
#define GPIO143_OUT      0x103EEC, BIT1
#define GPIO143_IN       0x103EEC, BIT0

#define GPIO144_PAD      PAD_GPIO16
#define GPIO144_OEN      0x103EEE, BIT2
#define GPIO144_OUT      0x103EEE, BIT1
#define GPIO144_IN       0x103EEE, BIT0

#define GPIO145_PAD      PAD_SD_GPIO0
#define GPIO145_OEN      0x103EF0, BIT2
#define GPIO145_OUT      0x103EF0, BIT1
#define GPIO145_IN       0x103EF0, BIT0

#define GPIO146_PAD      PAD_SD1_GPIO0
#define GPIO146_OEN      0x103EF2, BIT2
#define GPIO146_OUT      0x103EF2, BIT1
#define GPIO146_IN       0x103EF2, BIT0

#define GPIO147_PAD      PAD_SD1_GPIO1
#define GPIO147_OEN      0x103EF4, BIT2
#define GPIO147_OUT      0x103EF4, BIT1
#define GPIO147_IN       0x103EF4, BIT0

#define GPIO148_PAD      PAD_SD1_CDZ
#define GPIO148_OEN      0x103EF6, BIT2
#define GPIO148_OUT      0x103EF6, BIT1
#define GPIO148_IN       0x103EF6, BIT0

#define GPIO149_PAD      PAD_SD1_D1
#define GPIO149_OEN      0x103EF8, BIT2
#define GPIO149_OUT      0x103EF8, BIT1
#define GPIO149_IN       0x103EF8, BIT0

#define GPIO150_PAD      PAD_SD1_D0
#define GPIO150_OEN      0x103EFA, BIT2
#define GPIO150_OUT      0x103EFA, BIT1
#define GPIO150_IN       0x103EFA, BIT0

#define GPIO151_PAD      PAD_SD1_CLK
#define GPIO151_OEN      0x103EFC, BIT2
#define GPIO151_OUT      0x103EFC, BIT1
#define GPIO151_IN       0x103EFC, BIT0

#define GPIO152_PAD      PAD_SD1_CMD
#define GPIO152_OEN      0x103EFE, BIT2
#define GPIO152_OUT      0x103EFE, BIT1
#define GPIO152_IN       0x103EFE, BIT0

#define GPIO153_PAD      PAD_SD1_D3
#define GPIO153_OEN      0x103C00, BIT2
#define GPIO153_OUT      0x103C00, BIT1
#define GPIO153_IN       0x103C00, BIT0

#define GPIO154_PAD      PAD_SD1_D2
#define GPIO154_OEN      0x103C02, BIT2
#define GPIO154_OUT      0x103C02, BIT1
#define GPIO154_IN       0x103C02, BIT0

#define GPIO155_PAD      PAD_ETH_RN
#define GPIO155_OEN      0x1516E2, BIT4
#define GPIO155_OUT      0x1516E4, BIT0
#define GPIO155_IN       0x1516E4, BIT4

#define GPIO156_PAD      PAD_ETH_RP
#define GPIO156_OEN      0x1516E2, BIT5
#define GPIO156_OUT      0x1516E4, BIT1
#define GPIO156_IN       0x1516E4, BIT5

#define GPIO157_PAD      PAD_ETH_TN
#define GPIO157_OEN      0x1516E2, BIT6
#define GPIO157_OUT      0x1516E4, BIT2
#define GPIO157_IN       0x1516E4, BIT6

#define GPIO158_PAD      PAD_ETH_TP
#define GPIO158_OEN      0x1516E2, BIT7
#define GPIO158_OUT      0x1516E4, BIT3
#define GPIO158_IN       0x1516E4, BIT7

#define GPIO159_PAD      PAD_DM_P0
#define GPIO159_OEN      0x14210A, BIT4
#define GPIO159_OUT      0x14210A, BIT2
#define GPIO159_IN       0x142131, BIT5

#define GPIO160_PAD      PAD_DP_P0
#define GPIO160_OEN      0x14210A, BIT5
#define GPIO160_OUT      0x14210A, BIT3
#define GPIO160_IN       0x142131, BIT4

#define GPIO161_PAD      PAD_DM_P1
#define GPIO161_OEN      0x14290A, BIT4
#define GPIO161_OUT      0x14290A, BIT2
#define GPIO161_IN       0x142931, BIT5

#define GPIO162_PAD      PAD_DP_P1
#define GPIO162_OEN      0x14290A, BIT5
#define GPIO162_OUT      0x14290A, BIT3
#define GPIO162_IN       0x142931, BIT4

U32 gChipBaseAddr    = 0x1F203C00;
U32 gPmSleepBaseAddr = 0x1F001C00;
U32 gSarBaseAddr     = 0x1F002800;
U32 gRIUBaseAddr     = 0x1F000000;

#define MHal_CHIPTOP_REG(addr)  (*(volatile U8*)(gChipBaseAddr + (((addr) & ~1)<<1) + (addr & 1)))
#define MHal_PM_SLEEP_REG(addr) (*(volatile U8*)(gPmSleepBaseAddr + (((addr) & ~1)<<1) + (addr & 1)))
#define MHal_SAR_GPIO_REG(addr) (*(volatile U8*)(gSarBaseAddr + (((addr) & ~1)<<1) + (addr & 1)))
#define MHal_RIU_REG(addr)      (*(volatile U8*)(gRIUBaseAddr + (((addr) & ~1)<<1) + (addr & 1)))

#define REG_ALL_PAD_IN     0x24

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#ifdef UBOOT

static int _pmsleep_to_irq_table[] = {
        INT_PMSLEEP_UART_RX1,
        INT_PMSLEEP_UART_TX1,
        INT_PMSLEEP_UART_RX,
        INT_PMSLEEP_UART_TX,
        INT_PMSLEEP_I2CM_SCL,
        INT_PMSLEEP_I2CM_SDA,
        INT_PMSLEEP_GPIO0,
        INT_PMSLEEP_GPIO1,
        INT_PMSLEEP_GPIO2,
        INT_PMSLEEP_GPIO3,
        INT_PMSLEEP_GPIO4,
        INT_PMSLEEP_GPIO5,
        INT_PMSLEEP_GPIO6,
        INT_PMSLEEP_GPIO7,
        INT_PMSLEEP_SPI_CZ,
        INT_PMSLEEP_SPI_CK,
        INT_PMSLEEP_SPI_DI,
        INT_PMSLEEP_SPI_DO,
        INT_PMSLEEP_SPI_WPZ,
        INT_PMSLEEP_SPI_HLD,
        INT_PMSLEEP_INVALID,
        INT_PMSLEEP_SAR_GPIO0,
        INT_PMSLEEP_SAR_GPIO1,
        INT_PMSLEEP_SAR_GPIO2,
        INT_PMSLEEP_SAR_GPIO3,
};

static int _gpi_to_irq_table[] = {

        INT_GPI_FIQ_PAD_SD0_CDZ,
        INT_GPI_FIQ_PAD_SD0_D1,
        INT_GPI_FIQ_PAD_SD0_D0,
        INT_GPI_FIQ_PAD_SD0_CLK,
        INT_GPI_FIQ_PAD_SD0_CMD,
        INT_GPI_FIQ_PAD_SD0_D3,
        INT_GPI_FIQ_PAD_SD0_D2,
        INT_GPI_FIQ_PAD_FUART_RX,
        INT_GPI_FIQ_PAD_FUART_TX,
        INT_GPI_FIQ_PAD_FUART_CTS,
        INT_GPI_FIQ_PAD_FUART_RTS,
        INT_GPI_FIQ_PAD_I2C0_SCL,
        INT_GPI_FIQ_PAD_I2C0_SDA,
        INT_GPI_FIQ_PAD_ETH_LED0,
        INT_GPI_FIQ_PAD_ETH_LED1,
        INT_GPI_FIQ_PAD_SPI0_DO,
        INT_GPI_FIQ_PAD_SPI0_DI,
        INT_GPI_FIQ_PAD_SPI0_CK,
        INT_GPI_FIQ_PAD_SPI0_CZ,
        INT_GPI_FIQ_PAD_GPIO0,
        INT_GPI_FIQ_PAD_GPIO1,
        INT_GPI_FIQ_PAD_GPIO2,
        INT_GPI_FIQ_PAD_GPIO3,
        INT_GPI_FIQ_PAD_GPIO4,
        INT_GPI_FIQ_PAD_GPIO5,
        INT_GPI_FIQ_PAD_GPIO6,
        INT_GPI_FIQ_PAD_GPIO7,
        INT_GPI_FIQ_PAD_SR0_IO00,
        INT_GPI_FIQ_PAD_SR0_IO01,
        INT_GPI_FIQ_PAD_SR0_IO02,
        INT_GPI_FIQ_PAD_SR0_IO03,
        INT_GPI_FIQ_PAD_SR0_IO04,
        INT_GPI_FIQ_PAD_SR0_IO05,
        INT_GPI_FIQ_PAD_SR0_IO06,
        INT_GPI_FIQ_PAD_SR0_IO07,
        INT_GPI_FIQ_PAD_SR0_IO08,
        INT_GPI_FIQ_PAD_SR0_IO09,
        INT_GPI_FIQ_PAD_SR0_IO10,
        INT_GPI_FIQ_PAD_SR0_IO11,
        INT_GPI_FIQ_PAD_SR0_IO12,
        INT_GPI_FIQ_PAD_SR0_IO13,
        INT_GPI_FIQ_PAD_SR0_IO14,
        INT_GPI_FIQ_PAD_SR0_IO15,
        INT_GPI_FIQ_PAD_SR0_IO16,
        INT_GPI_FIQ_PAD_SR0_IO17,
        INT_GPI_FIQ_PAD_SR0_IO18,
        INT_GPI_FIQ_PAD_SR0_IO19,
        INT_GPI_FIQ_PAD_SR1_IO00,
        INT_GPI_FIQ_PAD_SR1_IO01,
        INT_GPI_FIQ_PAD_SR1_IO02,
        INT_GPI_FIQ_PAD_SR1_IO03,
        INT_GPI_FIQ_PAD_SR1_IO04,
        INT_GPI_FIQ_PAD_SR1_IO05,
        INT_GPI_FIQ_PAD_SR1_IO06,
        INT_GPI_FIQ_PAD_SR1_IO07,
        INT_GPI_FIQ_PAD_SR1_IO08,
        INT_GPI_FIQ_PAD_SR1_IO09,
        INT_GPI_FIQ_PAD_SR1_IO10,
        INT_GPI_FIQ_PAD_SR1_IO11,
        INT_GPI_FIQ_PAD_SR1_IO12,
        INT_GPI_FIQ_PAD_SR1_IO13,
        INT_GPI_FIQ_PAD_SR1_IO14,
        INT_GPI_FIQ_PAD_SR1_IO15,
        INT_GPI_FIQ_PAD_SR1_IO16,
        INT_GPI_FIQ_PAD_SR1_IO17,
        INT_GPI_FIQ_PAD_SR1_IO18,
        INT_GPI_FIQ_PAD_SR1_IO19,
        INT_GPI_FIQ_PAD_TX0_IO00,
        INT_GPI_FIQ_PAD_TX0_IO01,
        INT_GPI_FIQ_PAD_TX0_IO02,
        INT_GPI_FIQ_PAD_TX0_IO03,
        INT_GPI_FIQ_PAD_TX0_IO04,
        INT_GPI_FIQ_PAD_TX0_IO05,
        INT_GPI_FIQ_PAD_TX0_IO06,
        INT_GPI_FIQ_PAD_TX0_IO07,
        INT_GPI_FIQ_PAD_TX0_IO08,
        INT_GPI_FIQ_PAD_TX0_IO09,
        INT_GPI_FIQ_PAD_TTL0,
        INT_GPI_FIQ_PAD_TTL1,
        INT_GPI_FIQ_PAD_TTL2,
        INT_GPI_FIQ_PAD_TTL3,
        INT_GPI_FIQ_PAD_TTL4,
        INT_GPI_FIQ_PAD_TTL5,
        INT_GPI_FIQ_PAD_TTL6,
        INT_GPI_FIQ_PAD_TTL7,
        INT_GPI_FIQ_PAD_TTL8,
        INT_GPI_FIQ_PAD_TTL9,
        INT_GPI_FIQ_PAD_TTL10,
        INT_GPI_FIQ_PAD_TTL11,
        INT_GPI_FIQ_PAD_TTL12,
        INT_GPI_FIQ_PAD_TTL13,
        INT_GPI_FIQ_PAD_TTL14,
        INT_GPI_FIQ_PAD_TTL15,
        INT_GPI_FIQ_PAD_TTL16,
        INT_GPI_FIQ_PAD_TTL17,
        INT_GPI_FIQ_PAD_TTL18,
        INT_GPI_FIQ_PAD_TTL19,
        INT_GPI_FIQ_PAD_TTL20,
        INT_GPI_FIQ_PAD_TTL21,
        INT_GPI_FIQ_PAD_TTL22,
        INT_GPI_FIQ_PAD_TTL23,
        INT_GPI_FIQ_PAD_TTL24,
        INT_GPI_FIQ_PAD_TTL25,
        INT_GPI_FIQ_PAD_TTL26,
        INT_GPI_FIQ_PAD_TTL27,
        INT_GPI_FIQ_PAD_SATA_GPIO,
        INT_GPI_FIQ_PAD_HDMITX_HPD,
        INT_GPI_FIQ_PAD_HDMITX_SDA,
        INT_GPI_FIQ_PAD_HDMITX_SCL,
        INT_GPI_FIQ_PAD_VSYNC_OUT,
        INT_GPI_FIQ_PAD_HSYNC_OUT,
        INT_GPI_FIQ_PAD_GPIO8,
        INT_GPI_FIQ_PAD_GPIO9,
        INT_GPI_FIQ_PAD_GPIO10,
        INT_GPI_FIQ_PAD_GPIO11,
        INT_GPI_FIQ_PAD_GPIO12,
        INT_GPI_FIQ_PAD_GPIO13,
        INT_GPI_FIQ_PAD_GPIO14,
        INT_GPI_FIQ_PAD_GPIO15,
        INT_GPI_FIQ_PAD_GPIO16,
        INT_GPI_FIQ_PAD_SD_GPIO0,
        INT_GPI_FIQ_PAD_SD1_GPIO0,
        INT_GPI_FIQ_PAD_SD1_GPIO1,
        INT_GPI_FIQ_PAD_SD1_CDZ,
        INT_GPI_FIQ_PAD_SD1_D1,
        INT_GPI_FIQ_PAD_SD1_D0,
        INT_GPI_FIQ_PAD_SD1_CLK,
        INT_GPI_FIQ_PAD_SD1_CMD,
        INT_GPI_FIQ_PAD_SD1_D3,
        INT_GPI_FIQ_PAD_SD1_D2,

};
#endif

static const struct gpio_setting
{
    U32 r_oen;
    U8  m_oen;
    U32 r_out;
    U8  m_out;
    U32 r_in;
    U8  m_in;
} gpio_table[] =
{

#define __GPIO__(_x_)   { CONCAT(CONCAT(GPIO, _x_), _OEN),   \
                          CONCAT(CONCAT(GPIO, _x_), _OUT),   \
                          CONCAT(CONCAT(GPIO, _x_), _IN) }
#define __GPIO(_x_)     __GPIO__(_x_)

//
// !! WARNING !! DO NOT MODIFIY !!!!
//
// These defines order must match following
// 1. the PAD name in GPIO excel
// 2. the perl script to generate the package header file
//
    __GPIO(0),   __GPIO(1),   __GPIO(2),   __GPIO(3),   __GPIO(4),   __GPIO(5),   __GPIO(6),   __GPIO(7),
    __GPIO(8),   __GPIO(9),   __GPIO(10),  __GPIO(11),  __GPIO(12),  __GPIO(13),  __GPIO(14),  __GPIO(15),
    __GPIO(16),  __GPIO(17),  __GPIO(18),  __GPIO(19),  __GPIO(20),  __GPIO(21),  __GPIO(22),  __GPIO(23),
    __GPIO(24),  __GPIO(25),  __GPIO(26),  __GPIO(27),  __GPIO(28),  __GPIO(29),  __GPIO(30),  __GPIO(31),
    __GPIO(32),  __GPIO(33),  __GPIO(34),  __GPIO(35),  __GPIO(36),  __GPIO(37),  __GPIO(38),  __GPIO(39),
    __GPIO(40),  __GPIO(41),  __GPIO(42),  __GPIO(43),  __GPIO(44),  __GPIO(45),  __GPIO(46),  __GPIO(47),
    __GPIO(48),  __GPIO(49),  __GPIO(50),  __GPIO(51),  __GPIO(52),  __GPIO(53),  __GPIO(54),  __GPIO(55),
    __GPIO(56),  __GPIO(57),  __GPIO(58),  __GPIO(59),  __GPIO(60),  __GPIO(61),  __GPIO(62),  __GPIO(63),
    __GPIO(64),  __GPIO(65),  __GPIO(66),  __GPIO(67),  __GPIO(68),  __GPIO(69),  __GPIO(70),  __GPIO(71),
    __GPIO(72),  __GPIO(73),  __GPIO(74),  __GPIO(75),  __GPIO(76),  __GPIO(77),  __GPIO(78),  __GPIO(79),
    __GPIO(80),  __GPIO(81),  __GPIO(82),  __GPIO(83),  __GPIO(84),  __GPIO(85),  __GPIO(86),  __GPIO(87),
    __GPIO(88),  __GPIO(89),  __GPIO(90),  __GPIO(91),  __GPIO(92),  __GPIO(93),  __GPIO(94),  __GPIO(95),
    __GPIO(96),  __GPIO(97),  __GPIO(98),  __GPIO(99),  __GPIO(100), __GPIO(101), __GPIO(102), __GPIO(103),
    __GPIO(104), __GPIO(105), __GPIO(106), __GPIO(107), __GPIO(108), __GPIO(109), __GPIO(110), __GPIO(111),
    __GPIO(112), __GPIO(113), __GPIO(114), __GPIO(115), __GPIO(116), __GPIO(117), __GPIO(118), __GPIO(119),
    __GPIO(120), __GPIO(121), __GPIO(122), __GPIO(123), __GPIO(124), __GPIO(125), __GPIO(126), __GPIO(127),
    __GPIO(128), __GPIO(129), __GPIO(130), __GPIO(131), __GPIO(132), __GPIO(133), __GPIO(134), __GPIO(135),
    __GPIO(136), __GPIO(137), __GPIO(138), __GPIO(139), __GPIO(140), __GPIO(141), __GPIO(142), __GPIO(143),
    __GPIO(144), __GPIO(145), __GPIO(146), __GPIO(147), __GPIO(148), __GPIO(149), __GPIO(150), __GPIO(151),
    __GPIO(152), __GPIO(153), __GPIO(154), __GPIO(155), __GPIO(156), __GPIO(157), __GPIO(158), __GPIO(159),
    __GPIO(160), __GPIO(161), __GPIO(162),
};

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

void MHal_GPIO_Init(void)
{
    MHal_CHIPTOP_REG(REG_ALL_PAD_IN) &= ~(BIT0|BIT1);
}

void MHal_GPIO_Pad_Set(MS_GPIO_NUM u8IndexGPIO)
{
    HalPadSetVal((U32 )u8IndexGPIO, (U32 )PINMUX_FOR_GPIO_MODE);
}

int MHal_GPIO_PadGroupMode_Set(U32 u32PadMode)
{
    (void)u32PadMode;
    return -1; // not support
}

int MHal_GPIO_PadVal_Set(MS_GPIO_NUM u8IndexGPIO, U32 u32PadMode)
{
    (void)u8IndexGPIO;
    (void)u32PadMode;
    return -1; // not support
}

void MHal_GPIO_Pad_Oen(MS_GPIO_NUM u8IndexGPIO)
{
    MHal_RIU_REG(gpio_table[u8IndexGPIO].r_oen) &= (~gpio_table[u8IndexGPIO].m_oen);
}

void MHal_GPIO_Pad_Odn(MS_GPIO_NUM u8IndexGPIO)
{
    MHal_RIU_REG(gpio_table[u8IndexGPIO].r_oen) |= gpio_table[u8IndexGPIO].m_oen;
}

int MHal_GPIO_Pad_Level(MS_GPIO_NUM u8IndexGPIO)
{
    return ((MHal_RIU_REG(gpio_table[u8IndexGPIO].r_in)&gpio_table[u8IndexGPIO].m_in)? 1 : 0);
}

U8 MHal_GPIO_Pad_InOut(MS_GPIO_NUM u8IndexGPIO)
{
    return ((MHal_RIU_REG(gpio_table[u8IndexGPIO].r_oen)&gpio_table[u8IndexGPIO].m_oen)? 1 : 0);
}

void MHal_GPIO_Pull_High(MS_GPIO_NUM u8IndexGPIO)
{
    MHal_RIU_REG(gpio_table[u8IndexGPIO].r_out) |= gpio_table[u8IndexGPIO].m_out;
}

void MHal_GPIO_Pull_Low(MS_GPIO_NUM u8IndexGPIO)
{
    MHal_RIU_REG(gpio_table[u8IndexGPIO].r_out) &= (~gpio_table[u8IndexGPIO].m_out);
}

void MHal_GPIO_Set_High(MS_GPIO_NUM u8IndexGPIO)
{
    MHal_RIU_REG(gpio_table[u8IndexGPIO].r_oen) &= (~gpio_table[u8IndexGPIO].m_oen);
    MHal_RIU_REG(gpio_table[u8IndexGPIO].r_out) |= gpio_table[u8IndexGPIO].m_out;
}

void MHal_GPIO_Set_Low(MS_GPIO_NUM u8IndexGPIO)
{
    MHal_RIU_REG(gpio_table[u8IndexGPIO].r_oen) &= (~gpio_table[u8IndexGPIO].m_oen);
    MHal_RIU_REG(gpio_table[u8IndexGPIO].r_out) &= (~gpio_table[u8IndexGPIO].m_out);
}

void MHal_Enable_GPIO_INT(MS_GPIO_NUM u8IndexGPIO)
{
    // TBD
}


void MHal_GPIO_Set_Input(MS_GPIO_NUM u32IndexGPIO)
{
    MHal_RIU_REG(gpio_table[u32IndexGPIO].r_oen) |= (gpio_table[u32IndexGPIO].m_oen);
}

void MHal_GPIO_Set_Output(MS_GPIO_NUM u32IndexGPIO)
{

    MHal_RIU_REG(gpio_table[u32IndexGPIO].r_oen) &= (gpio_table[u32IndexGPIO].m_oen);
}

int MHal_GPIO_Get_InOut(MS_GPIO_NUM u32IndexGPIO)
{

    return ((MHal_RIU_REG(gpio_table[u32IndexGPIO].r_oen)&gpio_table[u32IndexGPIO].m_oen)? 1 : 0);

}

#ifdef UBOOT

static int PMSLEEP_GPIO_To_Irq(U8 u8IndexGPIO)
{
    if ((u8IndexGPIO < PAD_PM_UART_RX1) || (u8IndexGPIO > PAD_SAR_GPIO3))
    {
        return -1;
    }
    else
    {
        if(_pmsleep_to_irq_table[u8IndexGPIO - PAD_PM_UART_RX1] != INT_PMSLEEP_INVALID)
            return _pmsleep_to_irq_table[u8IndexGPIO - PAD_PM_UART_RX1];
        else
            return -1;
    }
}

static int GPI_GPIO_To_Irq(U8 u8IndexGPIO)
{
    if ((u8IndexGPIO < PAD_SD0_CDZ) || (u8IndexGPIO > PAD_SD1_D2))
        return -1;
    else
        return _gpi_to_irq_table[u8IndexGPIO- PAD_SD0_CDZ];
}

//MHal_GPIO_To_Irq return any virq
int MHal_GPIO_To_Irq(U8 u8IndexGPIO)
{
    struct device_node *intr_node;
    struct irq_domain *intr_domain;
    struct irq_fwspec fwspec;
    int hwirq, virq = -1;

    if ((hwirq = PMSLEEP_GPIO_To_Irq(u8IndexGPIO)) >= 0)
    {
        //get virtual irq number for request_irq
        intr_node = of_find_compatible_node(NULL, NULL, "sstar,pm-intc");
        intr_domain = irq_find_host(intr_node);
        if(!intr_domain)
            return -ENXIO;

        fwspec.param_count = 1;
        fwspec.param[0] = hwirq;
        fwspec.fwnode = of_node_to_fwnode(intr_node);
        virq = irq_create_fwspec_mapping(&fwspec);
    }
    else if ((hwirq = GPI_GPIO_To_Irq(u8IndexGPIO)) >= 0)
    {
        //get virtual irq number for request_irq
        intr_node = of_find_compatible_node(NULL, NULL, "sstar,gpi-intc");
        intr_domain = irq_find_host(intr_node);
        if(!intr_domain)
            return -ENXIO;

        fwspec.param_count = 1;
        fwspec.param[0] = hwirq;
        fwspec.fwnode = of_node_to_fwnode(intr_node);
        virq = irq_create_fwspec_mapping(&fwspec);
    }

    return virq;
}
#endif
void MHal_GPIO_Set_POLARITY(MS_GPIO_NUM u8IndexGPIO, U8 reverse)
{
    // TBD
}
