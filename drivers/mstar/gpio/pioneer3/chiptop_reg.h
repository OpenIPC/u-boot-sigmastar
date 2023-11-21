/*
* chiptop_reg.h- Sigmastar
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

#ifndef ___CHIPTOP_H
#define ___CHIPTOP_H


//#define REG_AUTO_GPIO_EN                        0x9
//    #define REG_AUTO_GPIO_EN_MASK                   BIT0|BIT1|BIT2
#define REG_EJ_MODE                             0x60
    #define REG_EJ_MODE_MASK                        BIT8|BIT9|BIT10
#define REG_ALL_PAD_IN                          0x28
    #define REG_ALL_PAD_IN_MASK                     BIT15
#define REG_TEST_IN_MODE                        0x12
    #define REG_TEST_IN_MODE_MASK                   BIT0|BIT1
#define REG_TEST_OUT_MODE                       0x12
    #define REG_TEST_OUT_MODE_MASK                  BIT4|BIT5
#define REG_I2C0_MODE                           0x6f
    #define REG_I2C0_MODE_MASK                      BIT0|BIT1|BIT2|BIT3
#define REG_I2C1_MODE                           0x6f
    #define REG_I2C1_MODE_MASK                      BIT4|BIT5|BIT6|BIT7
#define REG_SR0_MIPI_MODE                       0x74
    #define REG_SR0_MIPI_MODE_MASK                  BIT0
#define REG_SR1_MIPI_MODE                       0x74
    #define REG_SR1_MIPI_MODE_MASK                  BIT4|BIT5
#define REG_ISP_IR_MODE                         0x73
    #define REG_ISP_IR_MODE_MASK                    BIT0|BIT1
#define REG_SR0_CTRL_MODE                       0x74
    #define REG_SR0_CTRL_MODE_MASK                  BIT8|BIT9
#define REG_SR0_MCLK_MODE                       0x76
    #define REG_SR0_MCLK_MODE_MASK                  BIT0|BIT1
#define REG_SR0_PDN_MODE                        0x76
    #define REG_SR0_PDN_MODE_MASK                   BIT8
#define REG_SR0_RST_MODE                        0x76
    #define REG_SR0_RST_MODE_MASK                   BIT4|BIT5
#define REG_SR1_CTRL_MODE                       0x74
    #define REG_SR1_CTRL_MODE_MASK                  BIT12|BIT13
#define REG_SR1_MCLK_MODE                       0x77
    #define REG_SR1_MCLK_MODE_MASK                  BIT0|BIT1
#define REG_SR1_PDN_MODE                        0x77
    #define REG_SR1_PDN_MODE_MASK                   BIT8|BIT9
#define REG_SR1_RST_MODE                        0x77
    #define REG_SR1_RST_MODE_MASK                   BIT4|BIT5
#define REG_SR0_BT601_MODE                      0x75
    #define REG_SR0_BT601_MODE_MASK                 BIT0|BIT1
#define REG_SR0_BT656_MODE                      0x75
    #define REG_SR0_BT656_MODE_MASK                 BIT4|BIT5|BIT6
#define REG_ETH0_MODE                           0x6e
    #define REG_ETH0_MODE_MASK                      BIT0|BIT1|BIT2|BIT3
#define REG_TTL16_MODE                          0x6c
    #define REG_TTL16_MODE_MASK                     BIT0|BIT1|BIT2
#define REG_TTL18_MODE                          0x6c
    #define REG_TTL18_MODE_MASK                     BIT4|BIT5
#define REG_TTL24_MODE                          0x6c
    #define REG_TTL24_MODE_MASK                     BIT8|BIT9|BIT10
#define REG_RGB8_MODE                           0x6b
    #define REG_RGB8_MODE_MASK                      BIT0|BIT1
#define REG_BT656_OUT_MODE                      0x6c
    #define REG_BT656_OUT_MODE_MASK                 BIT12|BIT13
#define REG_PSPI0_SR_MODE                       0x69
    #define REG_PSPI0_SR_MODE_MASK                  BIT0|BIT1|BIT2
#define REG_PSPI0_G_MODE                        0x69
    #define REG_PSPI0_G_MODE_MASK                   BIT4|BIT5
#define REG_SPI0_MODE                           0x68
    #define REG_SPI0_MODE_MASK                      BIT0|BIT1|BIT2
#define REG_PSPI1_PL_MODE                       0x6a
    #define REG_PSPI1_PL_MODE_MASK                  BIT0|BIT1|BIT2|BIT3
#define REG_PSPI1_CS2_MODE                      0x6a
    #define REG_PSPI1_CS2_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_PSPI1_TE_MODE                       0x6a
    #define REG_PSPI1_TE_MODE_MASK                  BIT8|BIT9|BIT10|BIT11
#define REG_PSPI1_G_MODE                        0x6a
    #define REG_PSPI1_G_MODE_MASK                   BIT12|BIT13
#define REG_SPI1_MODE                           0x68
    #define REG_SPI1_MODE_MASK                      BIT8|BIT9|BIT10|BIT11
#define REG_SPI1_CS2_MODE                       0x68
    #define REG_SPI1_CS2_MODE_MASK                  BIT12|BIT13|BIT14
#define REG_SDIO_MODE                           0x67
    #define REG_SDIO_MODE_MASK                      BIT8|BIT9
#define REG_SD_CDZ_MODE                         0x67
    #define REG_SD_CDZ_MODE_MASK                    BIT0|BIT1
#define REG_KEY_READ0_MODE                      0x78
    #define REG_KEY_READ0_MODE_MASK                 BIT0|BIT1|BIT2|BIT3
#define REG_KEY_READ1_MODE                      0x78
    #define REG_KEY_READ1_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_KEY_READ2_MODE                      0x78
    #define REG_KEY_READ2_MODE_MASK                 BIT8|BIT9|BIT10|BIT11
#define REG_KEY_READ3_MODE                      0x78
    #define REG_KEY_READ3_MODE_MASK                 BIT12|BIT13|BIT14|BIT15
#define REG_KEY_READ4_MODE                      0x79
    #define REG_KEY_READ4_MODE_MASK                 BIT0|BIT1|BIT2|BIT3
#define REG_KEY_READ5_MODE                      0x79
    #define REG_KEY_READ5_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_KEY_READ6_MODE                      0x79
    #define REG_KEY_READ6_MODE_MASK                 BIT8|BIT9|BIT10|BIT11
#define REG_KEY_SCAN0_MODE                      0x7a
    #define REG_KEY_SCAN0_MODE_MASK                 BIT0|BIT1|BIT2|BIT3
#define REG_KEY_SCAN1_MODE                      0x7a
    #define REG_KEY_SCAN1_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_KEY_SCAN2_MODE                      0x7a
    #define REG_KEY_SCAN2_MODE_MASK                 BIT8|BIT9|BIT10|BIT11
#define REG_KEY_SCAN3_MODE                      0x7a
    #define REG_KEY_SCAN3_MODE_MASK                 BIT12|BIT13|BIT14|BIT15
#define REG_KEY_SCAN4_MODE                      0x7b
    #define REG_KEY_SCAN4_MODE_MASK                 BIT0|BIT1|BIT2|BIT3
#define REG_KEY_SCAN5_MODE                      0x7b
    #define REG_KEY_SCAN5_MODE_MASK                 BIT4|BIT5|BIT6|BIT7
#define REG_KEY_SCAN6_MODE                      0x7b
    #define REG_KEY_SCAN6_MODE_MASK                 BIT8|BIT9|BIT10|BIT11
#define REG_KEY_FIX_MODE                        0x79
    #define REG_KEY_FIX_MODE_MASK                   BIT12
#define REG_FUART_MODE                          0x6e
    #define REG_FUART_MODE_MASK                     BIT8|BIT9|BIT10|BIT11
#define REG_UART0_MODE                          0x6d
    #define REG_UART0_MODE_MASK                     BIT0|BIT1|BIT2
#define REG_UART1_MODE                          0x6d
    #define REG_UART1_MODE_MASK                     BIT4|BIT5|BIT6|BIT7
#define REG_UART2_MODE                          0x6d
    #define REG_UART2_MODE_MASK                     BIT8|BIT9|BIT10
#define REG_PWM0_MODE                           0x65
    #define REG_PWM0_MODE_MASK                      BIT0|BIT1|BIT2|BIT3
#define REG_PWM1_MODE                           0x65
    #define REG_PWM1_MODE_MASK                      BIT4|BIT5|BIT6|BIT7
#define REG_PWM2_MODE                           0x65
    #define REG_PWM2_MODE_MASK                      BIT8|BIT9|BIT10|BIT11
#define REG_PWM3_MODE                           0x65
    #define REG_PWM3_MODE_MASK                      BIT12|BIT13|BIT14|BIT15
#define REG_DMIC_MODE                           0x60
    #define REG_DMIC_MODE_MASK                      BIT0|BIT1|BIT2|BIT3
#define REG_I2S_MCK_MODE                        0x62
    #define REG_I2S_MCK_MODE_MASK                   BIT0|BIT1|BIT2
#define REG_I2S_RX_MODE                         0x62
    #define REG_I2S_RX_MODE_MASK                    BIT4|BIT5|BIT6
#define REG_I2S_TX_MODE                         0x62
    #define REG_I2S_TX_MODE_MASK                    BIT8|BIT9|BIT10
#define REG_I2S_RXTX_MODE                       0x62
    #define REG_I2S_RXTX_MODE_MASK                  BIT12|BIT13|BIT14
#define REG_BT1120_MODE                         0x72
    #define REG_BT1120_MODE_MASK                    BIT0|BIT1

#define REG_PM_SPI_GPIO                         0x35
    #define REG_PM_SPI_GPIO_MASK                    BIT0
#define REG_PM_SPIWPN_GPIO                      0x35
    #define REG_PM_SPIWPN_GPIO_MASK                 BIT4
#define REG_PM_SPIHOLDN_MODE                    0x35
    #define REG_PM_SPIHOLDN_MODE_MASK               BIT6|BIT7
#define REG_PM_SPICSZ1_GPIO                     0x35
    #define REG_PM_SPICSZ1_GPIO_MASK                BIT2
#define REG_PM_SPICSZ2_GPIO                     0x35
    #define REG_PM_SPICSZ2_GPIO_MASK                BIT3
#define REG_PM_PWM0_MODE                        0x28
    #define REG_PM_PWM0_MODE_MASK                   BIT0|BIT1
#define REG_PM_PWM1_MODE                        0x28
    #define REG_PM_PWM1_MODE_MASK                   BIT2|BIT3
#define REG_PM_PWM2_MODE                        0x28
    #define REG_PM_PWM2_MODE_MASK                   BIT6|BIT7
#define REG_PM_PWM3_MODE                        0x28
    #define REG_PM_PWM3_MODE_MASK                   BIT8|BIT9
#define REG_PM_UART1_MODE                       0x27
    #define REG_PM_UART1_MODE_MASK                  BIT8
#define REG_PM_VID_MODE                         0x28
    #define REG_PM_VID_MODE_MASK                    BIT12|BIT13
#define REG_PM_SD_CDZ_MODE                      0x28
    #define REG_PM_SD_CDZ_MODE_MASK                 BIT14
#define REG_PM_LED_MODE                         0x28
    #define REG_PM_LED_MODE_MASK                    BIT4|BIT5
#define REG_PM_PAD_EXT_MODE_0                   0x38
    #define REG_PM_PAD_EXT_MODE_0_MASK              BIT0
#define REG_PM_PAD_EXT_MODE_1                   0x38
    #define REG_PM_PAD_EXT_MODE_1_MASK              BIT1
#define REG_PM_PAD_EXT_MODE_2                   0x38
    #define REG_PM_PAD_EXT_MODE_2_MASK              BIT2
#define REG_PM_PAD_EXT_MODE_3                   0x38
    #define REG_PM_PAD_EXT_MODE_3_MASK              BIT3
#define REG_PM_PAD_EXT_MODE_4                   0x38
    #define REG_PM_PAD_EXT_MODE_4_MASK              BIT4
#define REG_PM_PAD_EXT_MODE_5                   0x38
    #define REG_PM_PAD_EXT_MODE_5_MASK              BIT5
#define REG_PM_PAD_EXT_MODE_6                   0x38
    #define REG_PM_PAD_EXT_MODE_6_MASK              BIT6
#define REG_PM_PAD_EXT_MODE_7                   0x38
    #define REG_PM_PAD_EXT_MODE_7_MASK              BIT7
#define REG_PM_PAD_EXT_MODE_8                   0x38
    #define REG_PM_PAD_EXT_MODE_8_MASK              BIT8
#define REG_PM_PAD_EXT_MODE_9                   0x38
    #define REG_PM_PAD_EXT_MODE_9_MASK              BIT9
#define REG_PM_PAD_EXT_MODE_10                  0x38
    #define REG_PM_PAD_EXT_MODE_10_MASK             BIT10
// PADMUX MASK MARCO END



#define PM_PADTOP_BANK        0x003F00
#define CHIPTOP_BANK          0x101E00
#define PADTOP_BANK           0x103C00
#define PADGPIO_BANK          0x103E00
#define PM_SAR_BANK           0x001400
#define PMSLEEP_BANK          0x000E00
#define PM_GPIO_BANK          0x000F00

#define REG_SR_IO00_GPIO_MODE                   0x0
    #define REG_SR_IO00_GPIO_MODE_MASK              BIT3
#define REG_SR_IO01_GPIO_MODE                   0x1
    #define REG_SR_IO01_GPIO_MODE_MASK              BIT3
#define REG_SR_IO02_GPIO_MODE                   0x2
    #define REG_SR_IO02_GPIO_MODE_MASK              BIT3
#define REG_SR_IO03_GPIO_MODE                   0x3
    #define REG_SR_IO03_GPIO_MODE_MASK              BIT3
#define REG_SR_IO04_GPIO_MODE                   0x4
    #define REG_SR_IO04_GPIO_MODE_MASK              BIT3
#define REG_SR_IO05_GPIO_MODE                   0x5
    #define REG_SR_IO05_GPIO_MODE_MASK              BIT3
#define REG_SR_IO06_GPIO_MODE                   0x6
    #define REG_SR_IO06_GPIO_MODE_MASK              BIT3
#define REG_SR_IO07_GPIO_MODE                   0x7
    #define REG_SR_IO07_GPIO_MODE_MASK              BIT3
#define REG_SR_IO08_GPIO_MODE                   0x8
    #define REG_SR_IO08_GPIO_MODE_MASK              BIT3
#define REG_SR_IO09_GPIO_MODE                   0x9
    #define REG_SR_IO09_GPIO_MODE_MASK              BIT3
#define REG_SR_IO10_GPIO_MODE                   0xA
    #define REG_SR_IO10_GPIO_MODE_MASK              BIT3
#define REG_SR_IO11_GPIO_MODE                   0xB
    #define REG_SR_IO11_GPIO_MODE_MASK              BIT3
#define REG_SR_IO12_GPIO_MODE                   0xC
    #define REG_SR_IO12_GPIO_MODE_MASK              BIT3
#define REG_SR_IO13_GPIO_MODE                   0xD
    #define REG_SR_IO13_GPIO_MODE_MASK              BIT3
#define REG_SR_IO14_GPIO_MODE                   0xE
    #define REG_SR_IO14_GPIO_MODE_MASK              BIT3
#define REG_SR_IO15_GPIO_MODE                   0xF
    #define REG_SR_IO15_GPIO_MODE_MASK              BIT3
#define REG_SR_IO16_GPIO_MODE                   0x10
    #define REG_SR_IO16_GPIO_MODE_MASK              BIT3
#define REG_TTL0_GPIO_MODE                      0x11
    #define REG_TTL0_GPIO_MODE_MASK                 BIT3
#define REG_TTL1_GPIO_MODE                      0x12
    #define REG_TTL1_GPIO_MODE_MASK                 BIT3
#define REG_TTL2_GPIO_MODE                      0x13
    #define REG_TTL2_GPIO_MODE_MASK                 BIT3
#define REG_TTL3_GPIO_MODE                      0x14
    #define REG_TTL3_GPIO_MODE_MASK                 BIT3
#define REG_TTL4_GPIO_MODE                      0x15
    #define REG_TTL4_GPIO_MODE_MASK                 BIT3
#define REG_TTL5_GPIO_MODE                      0x16
    #define REG_TTL5_GPIO_MODE_MASK                 BIT3
#define REG_TTL6_GPIO_MODE                      0x17
    #define REG_TTL6_GPIO_MODE_MASK                 BIT3
#define REG_TTL7_GPIO_MODE                      0x18
    #define REG_TTL7_GPIO_MODE_MASK                 BIT3
#define REG_TTL8_GPIO_MODE                      0x19
    #define REG_TTL8_GPIO_MODE_MASK                 BIT3
#define REG_TTL9_GPIO_MODE                      0x1A
    #define REG_TTL9_GPIO_MODE_MASK                 BIT3
#define REG_TTL10_GPIO_MODE                     0x1B
    #define REG_TTL10_GPIO_MODE_MASK                BIT3
#define REG_TTL11_GPIO_MODE                     0x1C
    #define REG_TTL11_GPIO_MODE_MASK                BIT3
#define REG_TTL12_GPIO_MODE                     0x1D
    #define REG_TTL12_GPIO_MODE_MASK                BIT3
#define REG_TTL13_GPIO_MODE                     0x1E
    #define REG_TTL13_GPIO_MODE_MASK                BIT3
#define REG_TTL14_GPIO_MODE                     0x1F
    #define REG_TTL14_GPIO_MODE_MASK                BIT3
#define REG_TTL15_GPIO_MODE                     0x20
    #define REG_TTL15_GPIO_MODE_MASK                BIT3
#define REG_TTL16_GPIO_MODE                     0x21
    #define REG_TTL16_GPIO_MODE_MASK                BIT3
#define REG_TTL17_GPIO_MODE                     0x22
    #define REG_TTL17_GPIO_MODE_MASK                BIT3
#define REG_TTL18_GPIO_MODE                     0x23
    #define REG_TTL18_GPIO_MODE_MASK                BIT3
#define REG_TTL19_GPIO_MODE                     0x24
    #define REG_TTL19_GPIO_MODE_MASK                BIT3
#define REG_TTL20_GPIO_MODE                     0x25
    #define REG_TTL20_GPIO_MODE_MASK                BIT3
#define REG_TTL21_GPIO_MODE                     0x26
    #define REG_TTL21_GPIO_MODE_MASK                BIT3
#define REG_KEY0_GPIO_MODE                      0x27
    #define REG_KEY0_GPIO_MODE_MASK                 BIT3
#define REG_KEY1_GPIO_MODE                      0x28
    #define REG_KEY1_GPIO_MODE_MASK                 BIT3
#define REG_KEY2_GPIO_MODE                      0x29
    #define REG_KEY2_GPIO_MODE_MASK                 BIT3
#define REG_KEY3_GPIO_MODE                      0x2A
    #define REG_KEY3_GPIO_MODE_MASK                 BIT3
#define REG_KEY4_GPIO_MODE                      0x2B
    #define REG_KEY4_GPIO_MODE_MASK                 BIT3
#define REG_KEY5_GPIO_MODE                      0x2C
    #define REG_KEY5_GPIO_MODE_MASK                 BIT3
#define REG_KEY6_GPIO_MODE                      0x2D
    #define REG_KEY6_GPIO_MODE_MASK                 BIT3
#define REG_KEY7_GPIO_MODE                      0x2E
    #define REG_KEY7_GPIO_MODE_MASK                 BIT3
#define REG_KEY8_GPIO_MODE                      0x2F
    #define REG_KEY8_GPIO_MODE_MASK                 BIT3
#define REG_KEY9_GPIO_MODE                      0x30
    #define REG_KEY9_GPIO_MODE_MASK                 BIT3
#define REG_KEY10_GPIO_MODE                      0x31
    #define REG_KEY10_GPIO_MODE_MASK                 BIT3
#define REG_KEY11_GPIO_MODE                      0x32
    #define REG_KEY11_GPIO_MODE_MASK                 BIT3
#define REG_KEY12_GPIO_MODE                      0x33
    #define REG_KEY12_GPIO_MODE_MASK                 BIT3
 #define REG_KEY13_GPIO_MODE                      0x34
    #define REG_KEY13_GPIO_MODE_MASK                 BIT3
#define REG_SD_D1_GPIO_MODE                     0x35
    #define REG_SD_D1_GPIO_MODE_MASK                BIT3
#define REG_SD_D0_GPIO_MODE                     0x36
    #define REG_SD_D0_GPIO_MODE_MASK                BIT3
#define REG_SD_CLK_GPIO_MODE                    0x37
    #define REG_SD_CLK_GPIO_MODE_MASK               BIT3
#define REG_SD_CMD_GPIO_MODE                    0x38
    #define REG_SD_CMD_GPIO_MODE_MASK               BIT3
#define REG_SD_D3_GPIO_MODE                     0x39
    #define REG_SD_D3_GPIO_MODE_MASK                BIT3
#define REG_SD_D2_GPIO_MODE                     0x3A
    #define REG_SD_D2_GPIO_MODE_MASK                BIT3
#define REG_SD_GPIO0_GPIO_MODE                  0x3B
    #define REG_SD_GPIO0_GPIO_MODE_MASK             BIT3
#define REG_SD_GPIO1_GPIO_MODE                  0x3C
    #define REG_SD_GPIO1_GPIO_MODE_MASK             BIT3
#define REG_GPIO0_GPIO_MODE                     0x3D
    #define REG_GPIO0_GPIO_MODE_MASK                BIT3
#define REG_GPIO1_GPIO_MODE                     0x3E
    #define REG_GPIO1_GPIO_MODE_MASK                BIT3
#define REG_GPIO2_GPIO_MODE                     0x3F
    #define REG_GPIO2_GPIO_MODE_MASK                BIT3
#define REG_GPIO3_GPIO_MODE                     0x40
    #define REG_GPIO3_GPIO_MODE_MASK                BIT3
#define REG_GPIO4_GPIO_MODE                     0x41
    #define REG_GPIO4_GPIO_MODE_MASK                BIT3
#define REG_GPIO5_GPIO_MODE                     0x42
    #define REG_GPIO5_GPIO_MODE_MASK                BIT3
#define REG_GPIO6_GPIO_MODE                     0x43
    #define REG_GPIO6_GPIO_MODE_MASK                BIT3
#define REG_GPIO7_GPIO_MODE                     0x44
    #define REG_GPIO7_GPIO_MODE_MASK                BIT3
#define REG_GPIO8_GPIO_MODE                     0x45
    #define REG_GPIO8_GPIO_MODE_MASK                BIT3
#define REG_GPIO9_GPIO_MODE                     0x46
    #define REG_GPIO9_GPIO_MODE_MASK                BIT3



#endif
