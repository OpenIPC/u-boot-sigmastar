/*
* PnlTbl.c- Sigmastar
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
#define __PNL_TBL_C__

#include "drv_pnl_os.h"
#include "mhal_pnl.h"
#include "mhal_pnl_datatype.h"
#include "PnlTblDef.h"
#include "PnlTbl.h"

//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------

#define FLAG_DELAY            0xFE
#define FLAG_END_OF_TABLE     0xFF   // END OF REGISTERS MARKER
//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------

#if (RM68172_SUPPORTED || T30P133_480X854_SUPPORTED)

#include <common.h>
#include "../drivers/mstar/gpio/drvGPIO.h"

#define REGFLAG_DELAY                                         0xFFFE
#define REGFLAG_END_OF_TABLE                                  0xFFFF   // END OF REGISTERS MARKER

#define Delay_us(n) udelay(n);
#define Delay_ms(n) udelay(1000*n)


#define BOARD_CONFIG_EVB_I5      0
#define BOARD_CONFIG_EVB_I6E      1
#define BOARD_CONFIG_CUST001      2     //i6e BGA2 512MB+512MB

#define BOARD_CONFIG_NAME       BOARD_CONFIG_CUST001

#define GPIO_HIGH       (1)
#define GPIO_LOW        (0)

unsigned int u32GPIOLCE;
unsigned int u32GPIOCLK;
unsigned int u32GPIOSDI;
unsigned int u32GPIORST;

#define SPI_CMD_DBG(dbglv, _fmt, _args...)    \
        do                                          \
        if(dbglv)              \
        {                                           \
                printf(_fmt, ## _args);             \
        }while(0)

void _toggle_SPI_RST(unsigned long index)
{
    if(u32GPIORST != 0xffff){
        if(index)
            mdrv_gpio_set_high(u32GPIORST);
        else
            mdrv_gpio_set_low(u32GPIORST);
    }
}

void _toggle_SPI_CS(unsigned long index)
{
    if(index) 	mdrv_gpio_set_high(u32GPIOLCE);
    else       	mdrv_gpio_set_low(u32GPIOLCE);
}

void _toggle_SPI_SCL(unsigned long index)
{
    if(index) 	mdrv_gpio_set_high(u32GPIOCLK);
    else       	mdrv_gpio_set_low(u32GPIOCLK);
}

void _toggle_SPI_SDI(unsigned long index)
{
    if(index) mdrv_gpio_set_high(u32GPIOSDI);
    else      mdrv_gpio_set_low(u32GPIOSDI);
}


void _SPI_Init(u16 u16LSCE_GPIO_NUM, u16 u16LSCK_GPIO_NUM, u16 u16LSDA_GPIO_NUM, u16 u16LRST_GPIO_NUM)
{
    MDrv_GPIO_Pad_Set(u16LSCE_GPIO_NUM);
    MDrv_GPIO_Pad_Set(u16LSCK_GPIO_NUM);
    MDrv_GPIO_Pad_Set(u16LSDA_GPIO_NUM);
    MDrv_GPIO_Pad_Set(u16LRST_GPIO_NUM);

    u32GPIOLCE = u16LSCE_GPIO_NUM;
    u32GPIOCLK = u16LSCK_GPIO_NUM;
    u32GPIOSDI = u16LSDA_GPIO_NUM;
    u32GPIORST = u16LRST_GPIO_NUM;

    _toggle_SPI_RST(0);
    Delay_ms(100);
    _toggle_SPI_RST(1);
    _toggle_SPI_CS(1);
    _toggle_SPI_SCL(1);
    _toggle_SPI_SDI(1);

}

void _SPI_writebits(u32 value,int bits)
{
    unsigned int i;
    unsigned int MSB_First = 1<<(bits-1);
    _toggle_SPI_CS(1);
    //Delay_us(1);
    _toggle_SPI_SDI(1);
    _toggle_SPI_SCL(0);
    Delay_us(1);
    _toggle_SPI_CS(0);
    for(i=0;i< bits ;i++)
    {
        Delay_us(1);
        _toggle_SPI_SCL(0);
        if(value & MSB_First)
            _toggle_SPI_SDI(1);
        else
            _toggle_SPI_SDI(0);
        Delay_us(1);
        _toggle_SPI_SCL(1);
        value <<= 1;
    }
    Delay_us(1);
    _toggle_SPI_CS(1);
    Delay_us(2);
}

#if 0
void _SPI_write2byte(u32 value)
{
    unsigned int i;
    unsigned int MSB_First = 0x8000;
    _toggle_SPI_CS(1);
    Delay_us(1);
    _toggle_SPI_CS(0);
    _toggle_SPI_SDI(1);
    _toggle_SPI_SCL(0);
    Delay_us(1);
    for(i=0;i< 16 ;i++)
    {
        Delay_us(10);
        _toggle_SPI_SCL(0);
        if(value & MSB_First)
            _toggle_SPI_SDI(1);
        else
            _toggle_SPI_SDI(0);
        Delay_us(3);
        _toggle_SPI_SCL(1);
        value <<= 1;
    }
    _toggle_SPI_CS(1);
    Delay_us(5);
}
#endif

#if 0
static void push_table(unsigned short *table, u32 count,int bits)
{
    u32 i;

    for (i = 0; i < count; i++) {
        unsigned short cmd;
        cmd = *table;
        table += 1;

        // if (i < 10)
        //     printf("cmd:0x%04x\n",cmd);

        switch (cmd) {
        case REGFLAG_DELAY :
            Delay_ms(*(table + 1));
            break;
        case REGFLAG_END_OF_TABLE :
            break;
        default:
            //_SPI_write2byte(cmd);
            _SPI_writebits(cmd, bits);
        }
        table += 2;
    }
}

void Lcm_init(unsigned char *stLCMTable, u32 u32CmdCount)
{
    u8 tableElementByte= *stLCMTable;
    u16 *u16TablePtr;
    u16  u8SpiSlaveSize;
    u16  u8SkipNonCmdCnt=0;
    struct spi_attr stSpiCfg={0};

    if(tableElementByte == 2)
    {
        u16TablePtr = (u16 *)stLCMTable;
        u16TablePtr++;
        u8SkipNonCmdCnt++;
        u8SpiSlaveSize = *u16TablePtr++;
        u8SkipNonCmdCnt++;

        if(u8SpiSlaveSize >0)
        {
            stSpiCfg.edge_trigger_mode = *u16TablePtr++;
            stSpiCfg.clock          =  *u16TablePtr++;
            stSpiCfg.gpio_mode_OnOff    =  *u16TablePtr++;
            stSpiCfg.mosi_gpio_num  =  *u16TablePtr++;
            stSpiCfg.miso_gpio_num  =  *u16TablePtr++;
            stSpiCfg.cs_gpio_num    =  *u16TablePtr++;
            stSpiCfg.clk_gpio_num   =  *u16TablePtr++;
            stSpiCfg.rst_gpio_num   =  *u16TablePtr++;
            stSpiCfg.bl_gpio_num    =  *u16TablePtr++;
            stSpiCfg.cmd_mode       =  *u16TablePtr++;
            stSpiCfg.cmd_num        =  *u16TablePtr++;

            _SPI_Init(stSpiCfg.cs_gpio_num, stSpiCfg.clk_gpio_num, stSpiCfg.mosi_gpio_num, stSpiCfg.rst_gpio_num);
            u8SkipNonCmdCnt+=u8SpiSlaveSize;
            SPI_CMD_DBG(1, "%s %d:: Edge trigger mode = %d, clock= %d, gpio_mode=%d, cs_gpio_num= %d, clk_gpio_num=%d,mosi_gpio_num= %d,miso_gpio_num=%d, rst_gpio_num=%d, bl_gpio_num=%d, cmd_num=%d, \n",
                            __FUNCTION__, __LINE__,
                            stSpiCfg.edge_trigger_mode,
                            stSpiCfg.clock,
                            stSpiCfg.gpio_mode_OnOff,
                            stSpiCfg.cs_gpio_num,
                            stSpiCfg.clk_gpio_num,
                            stSpiCfg.mosi_gpio_num,
                            stSpiCfg.miso_gpio_num,
                            stSpiCfg.rst_gpio_num,
                            stSpiCfg.bl_gpio_num,
                            stSpiCfg.cmd_num
                        );

        }

        SPI_CMD_DBG(0, "%s %d:: first cmd  %x  cmd_count= %d    \n",
                            __FUNCTION__, __LINE__,
                            *u16TablePtr,u32CmdCount
                            );

        u32CmdCount = u32CmdCount- u8SkipNonCmdCnt*tableElementByte;

         SPI_CMD_DBG(0, "%s %d:: first cmd  %x  cmd_count= %d    \n",
                            __FUNCTION__, __LINE__,
                            *u16TablePtr,u32CmdCount
                            );

        push_table(u16TablePtr,u32CmdCount/(stSpiCfg.cmd_num*tableElementByte),stSpiCfg.cmd_mode);

        //Turn on backlight
        if (stSpiCfg.bl_gpio_num != 0xffff){
            MDrv_GPIO_Pad_Set(stSpiCfg.bl_gpio_num);
            mdrv_gpio_set_high(stSpiCfg.bl_gpio_num);
        }

        //parser header
    }
}
#endif

void Lcm_init_Ini(unsigned short *stLCMTable, u32 u32CmdCount,struct spi_attr stSpiCfg)
{
    u16 u16PanelCmdCnt = 0;
    u16 n = 0;
    LCM_setting_table   mPnlInitCmdTable[400] = {0};  // arry size increased from "300" to "400" for some panel
                                                      // (e.g., TTL_SPI panel LX50FWB4001V3) w/ more initial commands

    _SPI_Init(stSpiCfg.cs_gpio_num, stSpiCfg.clk_gpio_num, stSpiCfg.mosi_gpio_num, stSpiCfg.rst_gpio_num);
    SPI_CMD_DBG(1, "%s %d:: Edge trigger mode = %d, clock= %d, gpio_mode=%d, cs_gpio_num= %d, clk_gpio_num=%d,mosi_gpio_num= %d,miso_gpio_num=%d, rst_gpio_num=%d, bl_gpio_num=%d, cmd_num=%d, \n",
                    __FUNCTION__, __LINE__,
                    stSpiCfg.edge_trigger_mode,
                    stSpiCfg.clock,
                    stSpiCfg.gpio_mode_OnOff,
                    stSpiCfg.cs_gpio_num,
                    stSpiCfg.clk_gpio_num,
                    stSpiCfg.mosi_gpio_num,
                    stSpiCfg.miso_gpio_num,
                    stSpiCfg.rst_gpio_num,
                    stSpiCfg.bl_gpio_num,
                    stSpiCfg.cmd_num
                );

    // covert CMD table from Array to struct LCM_setting_table
    if(stLCMTable)
    {
        int i = 0;
        int j = 0;
        int k = 0;
        memset(mPnlInitCmdTable,0x00,sizeof(mPnlInitCmdTable));
        while(i < u32CmdCount)
        {
            if (REGFLAG_END_OF_TABLE == stLCMTable[i])
            {
                mPnlInitCmdTable[j].cmd = REGFLAG_END_OF_TABLE;
                j++;
                break;
            }
            else if (REGFLAG_DELAY == stLCMTable[i])
            {
                mPnlInitCmdTable[j].cmd = REGFLAG_DELAY;
                mPnlInitCmdTable[j].count = stLCMTable[i+1];
                j++;
                i += 3;
            }
            else
            {
                mPnlInitCmdTable[j].cmd = stLCMTable[i];
                mPnlInitCmdTable[j].count = stLCMTable[i+1];
                if (mPnlInitCmdTable[j].count > 0)
                {
                    for(k = 0;k < mPnlInitCmdTable[j].count; k++)
                    {
                        mPnlInitCmdTable[j].para_list[k] = stLCMTable[i+2+k];
                    }
                    i += k;
                }
                i += 3;
                j++;
            }
        }
        u16PanelCmdCnt = j;  // panel SPI cmd count
    }

    //Send Init para
    for (n = 0; n < u16PanelCmdCnt; n++)
    {
        if (REGFLAG_DELAY == mPnlInitCmdTable[n].cmd)
        {
            Delay_us(mPnlInitCmdTable[n].count);
            //printf("spi delay:0x%04x \n",mPnlInitCmdTable[n].count);
        }
        else if (REGFLAG_END_OF_TABLE == mPnlInitCmdTable[n].cmd)
            break;
        else
        {
            //printf("spi CMD:0x%04x \n",mPnlInitCmdTable[n].cmd);
            _SPI_writebits((u16)mPnlInitCmdTable[n].cmd, stSpiCfg.cmd_mode);
        }
    }

    //Turn on backlight
    if (stSpiCfg.bl_gpio_num != 0xffff){
        MDrv_GPIO_Pad_Set(stSpiCfg.bl_gpio_num);
        mdrv_gpio_set_high(stSpiCfg.bl_gpio_num);
    }
    //parser header
}

#endif


#if (INNOVLUX_1280_800_SUPPORTED)
MhalPnlParamConfig_t  tPanel_InnovLux1280x800[] =
{
    {
        "InnoLux1280x800",    //const char *pPanelName; ///<  PanelName

#if !defined (__aarch64__)
        0, //u16 u32AlignmentDummy0;
#endif

        0,                    //u8             u8Dither;         ///<  Diether On?off
        E_MHAL_PNL_LINK_LVDS, //MhalPnlLinkType_e eLinkType;     ///<  Panel LinkType

        0,        //u8 u8DualPort      :1;          ///<  DualPort on/off
        0,        //u8 u8SwapPort      :1;          ///<  Swap Port on/off
        0,        //u8 u8SwapOdd_ML    :1;          ///<  Swap Odd ML
        0,        //u8 u8SwapEven_ML   :1;          ///<  Swap Even ML
        0,        //u8 u8SwapOdd_RB    :1;          ///<  Swap Odd RB
        0,        //u8 u8SwapEven_RB   :1;          ///<  Swap Even RB

        1,        //u8 u8SwapLVDS_POL  :1;          ///<  Swap LVDS Channel Polairyt
        1,        //u8 u8SwapLVDS_CH   :1;          ///<  Swap LVDS channel
        0,        //u8 u8PDP10BIT      :1;          ///<  PDP 10bits on/off
        1,        //u8 u8LVDS_TI_MODE  :1;          ///<  Ti Mode On/Off


        0,        //u8 u8DCLKDelay;                 ///<  DCLK Delay
        0,        //u8 u8InvDCLK   :1;              ///<  CLK Invert
        0,        //u8 u8InvDE     :1;              ///<  DE Invert
        0,        //u8 u8InvHSync  :1;              ///<  HSync Invert
        0,        //u8 u8InvVSync  :1;              ///<  VSync Invert

        0x01,     //u8 u8DCKLCurrent;              ///< PANEL_DCLK_CURRENT
        0x01,     //u8 u8DECurrent;                ///< PANEL_DE_CURRENT
        0x01,     //u8 u8ODDDataCurrent;           ///< PANEL_ODD_DATA_CURRENT
        0x01,     //u8 u8EvenDataCurrent;          ///< PANEL_EVEN_DATA_CURRENT

        30,       //u16 u16OnTiming1;                ///<  time between panel & data while turn on power
        400,      //u16 u16OnTiming2;                ///<  time between data & back light while turn on power
        80,       //u16 u16OffTiming1;               ///<  time between back light & data while turn off power
        30,       //u16 u16OffTiming2;               ///<  time between data & panel while turn off power

        20,       //u16 u16HSyncWidth;               ///<  Hsync Width
        70,       //u16 u16HSyncBackPorch;           ///<  Hsync back porch

        3,        //u16 u16VSyncWidth;               ///<  Vsync width
        10,       //u16 u16VSyncBackPorch;           ///<  Vsync back porch

        90,       //u16 u16HStart;                   ///<  HDe start
        0,        //u16 u16VStart;                   ///<  VDe start
        1280,     //u16 u16Width;                    ///<  Panel Width
        800,      //u16 u16Height;                   ///<  Panel Height

        1470,     //u16 u16MaxHTotal;                ///<  Max H Total
        1440,     //u16 u16HTotal;                   ///<  H Total
        1410,     //u16 u16MinHTotal;                ///<  Min H Total

        833,      //u16 u16MaxVTotal;                ///<  Max V Total
        824,      //u16 u16VTotal;                   ///<  V Total
        815,      //u16 u16MinVTotal;                ///<  Min V Total

        74,       //u16 u16MaxDCLK;                  ///<  Max DCLK
        72,       //u16 u16DCLK;                     ///<  DCLK ( Htt * Vtt * Fps)
        69,       //u16 u16MinDCLK;                  ///<  Min DCLK

        0x0019,    //u16 u16SpreadSpectrumStep;       ///<  Step of SSC
        0x00C0,    //u16 u16SpreadSpectrumSpan;       ///<  Span of SSC

        0xA0,      //u8 u8DimmingCtl;                 ///<  Dimming Value
        0xFF,      //u8 u8MaxPWMVal;                  ///<  Max Dimming Value
        0x50,      //u8 u8MinPWMVal;                  ///<  Min Dimming Value

        0,                            //u8 u8DeinterMode   :1;                  ///<  DeInter Mode
        E_MHAL_PNL_ASPECT_RATIO_WIDE, //MhalPnlAspectRatio_e ePanelAspectRatio; ///<  Aspec Ratio

        0,                            //u16 u16LVDSTxSwapValue;         // LVDS Swap Value
        E_MHAL_PNL_TI_8BIT_MODE,      //MhalPnlTiBitMode_e eTiBitMode;  // Ti Bit Mode
        E_MHAL_PNL_OUTPUT_10BIT_MODE, //MhalPnlOutputFormatBitMode_e eOutputFormatBitMode;

        0,        //u8 u8SwapOdd_RG    :1;          ///<  Swap Odd RG
        0,        //u8 u8SwapEven_RG   :1;          ///<  Swap Even RG
        0,        //u8 u8SwapOdd_GB    :1;          ///<  Swap Odd GB
        0,        //u8 u8SwapEven_GB   :1;          ///<  Swap Even GB

        0,        //u8 u8DoubleClk     :1;                      ///<  Double CLK On/off
        0x1C848E, //u32 u32MaxSET;                              ///<  Max Lpll Set
        0x18EB59, //u32 u32MinSET;                              ///<  Min Lpll Set
        E_MHAL_PNL_CHG_HTOTAL, //MhalPnlOutputTimingMode_e eOutTimingMode;   ///<  Define which panel output timing change mode is used to change VFreq for same panel
        0,                     //u8 u8NoiseDith     :1;                      ///<  Noise Dither On/Off
        E_MHAL_PNL_CH_SWAP_4,  // MhalPnlChannelSwapType_e enCh0             ///<  Channel swap for CH0
        E_MHAL_PNL_CH_SWAP_3,  // MhalPnlChannelSwapType_e enCh1             ///<  Channel swap for CH1
        E_MHAL_PNL_CH_SWAP_2,  // MhalPnlChannelSwapType_e enCh2             ///<  Channel swap for CH2
        E_MHAL_PNL_CH_SWAP_1,  // MhalPnlChannelSwapType_e enCh3             ///<  Channel swap for CH3
        E_MHAL_PNL_CH_SWAP_0,  // MhalPnlChannelSwapType_e enCh4             ///<  Channel swap for CH4
    }
};
#endif

#if (RM68200_SUPPORTED || HX8304_SUPPORTED)
MhalPnlParamConfig_t  tPanel_720x1280_60[] =
{
    {
        "InnoLux1280x800",    //const char *pPanelName; ///<  PanelName

#if !defined (__aarch64__)
        0, //u16 u32AlignmentDummy0;
#endif

        0,                    //u8             u8Dither;         ///<  Diether On?off
        E_MHAL_PNL_LINK_MIPI_DSI, //MhalPnlLinkType_e eLinkType;     ///<  Panel LinkType

        0,        //u8 u8DualPort      :1;          ///<  DualPort on/off
        0,        //u8 u8SwapPort      :1;          ///<  Swap Port on/off
        0,        //u8 u8SwapOdd_ML    :1;          ///<  Swap Odd ML
        0,        //u8 u8SwapEven_ML   :1;          ///<  Swap Even ML
        0,        //u8 u8SwapOdd_RB    :1;          ///<  Swap Odd RB
        0,        //u8 u8SwapEven_RB   :1;          ///<  Swap Even RB

        1,        //u8 u8SwapLVDS_POL  :1;          ///<  Swap LVDS Channel Polairyt
        1,        //u8 u8SwapLVDS_CH   :1;          ///<  Swap LVDS channel
        0,        //u8 u8PDP10BIT      :1;          ///<  PDP 10bits on/off
        1,        //u8 u8LVDS_TI_MODE  :1;          ///<  Ti Mode On/Off


        0,        //u8 u8DCLKDelay;                 ///<  DCLK Delay
        0,        //u8 u8InvDCLK   :1;              ///<  CLK Invert
        0,        //u8 u8InvDE     :1;              ///<  DE Invert
        0,        //u8 u8InvHSync  :1;              ///<  HSync Invert
        0,        //u8 u8InvVSync  :1;              ///<  VSync Invert

        0x01,     //u8 u8DCKLCurrent;              ///< PANEL_DCLK_CURRENT
        0x01,     //u8 u8DECurrent;                ///< PANEL_DE_CURRENT
        0x01,     //u8 u8ODDDataCurrent;           ///< PANEL_ODD_DATA_CURRENT
        0x01,     //u8 u8EvenDataCurrent;          ///< PANEL_EVEN_DATA_CURRENT

        30,       //u16 u16OnTiming1;                ///<  time between panel & data while turn on power
        400,      //u16 u16OnTiming2;                ///<  time between data & back light while turn on power
        80,       //u16 u16OffTiming1;               ///<  time between back light & data while turn off power
        30,       //u16 u16OffTiming2;               ///<  time between data & panel while turn off power

        6,        //u16 u16HSyncWidth;               ///<  Hsync Width
        60,       //u16 u16HSyncBackPorch;           ///<  Hsync back porch

        40,       //u16 u16VSyncWidth;               ///<  Vsync width
        220,      //u16 u16VSyncBackPorch;           ///<  Vsync back porch

        66,       //u16 u16HStart;                   ///<  HDe start
        0,        //u16 u16VStart;                   ///<  VDe start
        720,      //u16 u16Width;                    ///<  Panel Width
        1280,     //u16 u16Height;                   ///<  Panel Height

        850,      //u16 u16MaxHTotal;                ///<  Max H Total
        800,      //u16 u16HTotal;                   ///<  H Total
        700,      //u16 u16MinHTotal;                ///<  Min H Total

        1750,     //u16 u16MaxVTotal;                ///<  Max V Total
        1650,     //u16 u16VTotal;                   ///<  V Total
        1550,     //u16 u16MinVTotal;                ///<  Min V Total

        84,       //u16 u16MaxDCLK;                  ///<  Max DCLK
        75,       //u16 u16DCLK;                     ///<  DCLK ( Htt * Vtt * Fps)
        65,       //u16 u16MinDCLK;                  ///<  Min DCLK

        0x0019,    //u16 u16SpreadSpectrumStep;       ///<  Step of SSC
        0x00C0,    //u16 u16SpreadSpectrumSpan;       ///<  Span of SSC

        0xA0,      //u8 u8DimmingCtl;                 ///<  Dimming Value
        0xFF,      //u8 u8MaxPWMVal;                  ///<  Max Dimming Value
        0x50,      //u8 u8MinPWMVal;                  ///<  Min Dimming Value

        0,                            //u8 u8DeinterMode   :1;                  ///<  DeInter Mode
        E_MHAL_PNL_ASPECT_RATIO_WIDE, //MhalPnlAspectRatio_e ePanelAspectRatio; ///<  Aspec Ratio

        0,                            //u16 u16LVDSTxSwapValue;         // LVDS Swap Value
        E_MHAL_PNL_TI_8BIT_MODE,      //MhalPnlTiBitMode_e eTiBitMode;  // Ti Bit Mode
        E_MHAL_PNL_OUTPUT_10BIT_MODE, //MhalPnlOutputFormatBitMode_e eOutputFormatBitMode;

        0,        //u8 u8SwapOdd_RG    :1;          ///<  Swap Odd RG
        0,        //u8 u8SwapEven_RG   :1;          ///<  Swap Even RG
        0,        //u8 u8SwapOdd_GB    :1;          ///<  Swap Odd GB
        0,        //u8 u8SwapEven_GB   :1;          ///<  Swap Even GB

        0,        //u8 u8DoubleClk     :1;                      ///<  Double CLK On/off
        0x1C848E, //u32 u32MaxSET;                              ///<  Max Lpll Set
        0x18EB59, //u32 u32MinSET;                              ///<  Min Lpll Set
        E_MHAL_PNL_CHG_HTOTAL, //MhalPnlOutputTimingMode_e eOutTimingMode;   ///<  Define which panel output timing change mode is used to change VFreq for same panel
        0,                     //u8 u8NoiseDith     :1;                      ///<  Noise Dither On/Off
        E_MHAL_PNL_CH_SWAP_2,  // MhalPnlChannelSwapType_e enCh0             ///<  Channel swap for CH0
        E_MHAL_PNL_CH_SWAP_0,  // MhalPnlChannelSwapType_e enCh1             ///<  Channel swap for CH1
        E_MHAL_PNL_CH_SWAP_1,  // MhalPnlChannelSwapType_e enCh2             ///<  Channel swap for CH2
        E_MHAL_PNL_CH_SWAP_3,  // MhalPnlChannelSwapType_e enCh3             ///<  Channel swap for CH3
        E_MHAL_PNL_CH_SWAP_4,  // MhalPnlChannelSwapType_e enCh4             ///<  Channel swap for CH4
    }
};
#endif


#if (RM68200_SUPPORTED)
u8 FITI_OTA7001A_CMD[] =
{
    0xFE, 1, 0x01,
    0x27, 1, 0x0A,
    0x29, 1, 0x0A,
    0x2B, 1, 0xE5,
    0x24, 1, 0xC0,
    0x25, 1, 0x53,
    0x26, 1, 0x00,
    0x16, 1, 0x52, //wrong
    0x2F, 1, 0x54,
    0x34, 1, 0x57,
    0x1B, 1, 0x00,
    0x12, 1, 0x0A,
    0x1A, 1, 0x06,
    0x46, 1, 0x4D,
    0x52, 1, 0x90,
    0x53, 1, 0x00,
    0x54, 1, 0x90,
    0x55, 1, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x03,
    0x00, 1, 0x05,
    0x01, 1, 0x16,
    0x02, 1, 0x09,
    0x03, 1, 0x0D,
    0x04, 1, 0x00,
    0x05, 1, 0x00,
    0x06, 1, 0x50,
    0x07, 1, 0x05,
    0x08, 1, 0x16,
    0x09, 1, 0x0B,
    0x0A, 1, 0x0F,
    0x0B, 1, 0x00,
    0x0C, 1, 0x00,
    0x0D, 1, 0x50,
    0x0E, 1, 0x03,
    0x0F, 1, 0x04,
    0x10, 1, 0x05,
    0x11, 1, 0x06,
    0x12, 1, 0x00,
    0x13, 1, 0x54,
    0x14, 1, 0x00,
    0x15, 1, 0xC5,
    0x16, 1, 0x08,
    0x17, 1, 0x07,
    0x18, 1, 0x08,
    0x19, 1, 0x09,
    0x1A, 1, 0x0A,
    0x1B, 1, 0x00,
    0x1C, 1, 0x54,
    0x1D, 1, 0x00,
    0x1E, 1, 0x85,
    0x1F, 1, 0x08,
    0x20, 1, 0x00,
    0x21, 1, 0x00,
    0x22, 1, 0x03,
    0x23, 1, 0x1F,
    0x24, 1, 0x00,
    0x25, 1, 0x28,
    0x26, 1, 0x00,
    0x27, 1, 0x1F,
    0x28, 1, 0x00,
    0x29, 1, 0x28,
    0x2A, 1, 0x00,
    0x2B, 1, 0x00,
    0x2D, 1, 0x00,
    0x2F, 1, 0x00,
    0x30, 1, 0x00,
    0x31, 1, 0x00,
    0x32, 1, 0x00,
    0x33, 1, 0x00,
    0x34, 1, 0x00,
    0x35, 1, 0x00,
    0x36, 1, 0x00,
    0x37, 1, 0x00,
    0x38, 1, 0x00,
    0x39, 1, 0x00,
    0x3A, 1, 0x00,
    0x3B, 1, 0x00,
    0x3D, 1, 0x00,
    0x3F, 1, 0x00,
    0x40, 1, 0x00,
    0x3F, 1, 0x00,
    0x41, 1, 0x00,
    0x42, 1, 0x00,
    0x43, 1, 0x00,
    0x44, 1, 0x00,
    0x45, 1, 0x00,
    0x46, 1, 0x00,
    0x47, 1, 0x00,
    0x48, 1, 0x00,
    0x49, 1, 0x00,
    0x4A, 1, 0x00,
    0x4B, 1, 0x00,
    0x4C, 1, 0x00,
    0x4D, 1, 0x00,
    0x4E, 1, 0x00,
    0x4F, 1, 0x00,
    0x50, 1, 0x00,
    0x51, 1, 0x00,
    0x52, 1, 0x00,
    0x53, 1, 0x00,
    0x54, 1, 0x00,
    0x55, 1, 0x00,
    0x56, 1, 0x00,
    0x58, 1, 0x00,
    0x59, 1, 0x00,
    0x5A, 1, 0x00,
    0x5B, 1, 0x00,
    0x5C, 1, 0x00,
    0x5D, 1, 0x00,
    0x5E, 1, 0x00,
    0x5F, 1, 0x00,
    0x60, 1, 0x00,
    0x61, 1, 0x00,
    0x62, 1, 0x00,
    0x63, 1, 0x00,
    0x64, 1, 0x00,
    0x65, 1, 0x00,
    0x66, 1, 0x00,
    0x67, 1, 0x00,
    0x68, 1, 0x00,
    0x69, 1, 0x00,
    0x6A, 1, 0x00,
    0x6B, 1, 0x00,
    0x6C, 1, 0x00,
    0x6D, 1, 0x00,
    0x6E, 1, 0x00,
    0x6F, 1, 0x00,
    0x70, 1, 0x00,
    0x71, 1, 0x00,
    0x72, 1, 0x00,
    0x73, 1, 0x00,
    0x74, 1, 0x04,
    0x75, 1, 0x04,
    0x76, 1, 0x04,
    0x77, 1, 0x04,
    0x78, 1, 0x00,
    0x79, 1, 0x00,
    0x7A, 1, 0x00,
    0x7B, 1, 0x00,
    0x7C, 1, 0x00,
    0x7D, 1, 0x00,
    0x7E, 1, 0x86,
    0x7F, 1, 0x02,
    0x80, 1, 0x0E,
    0x81, 1, 0x0C,
    0x82, 1, 0x0A,
    0x83, 1, 0x08,
    0x84, 1, 0x3F,
    0x85, 1, 0x3F,
    0x86, 1, 0x3F,
    0x87, 1, 0x3F,
    0x88, 1, 0x3F,
    0x89, 1, 0x3F,
    0x8A, 1, 0x3F,
    0x8B, 1, 0x3F,
    0x8C, 1, 0x3F,
    0x8D, 1, 0x3F,
    0x8E, 1, 0x3F,
    0x8F, 1, 0x3F,
    0x90, 1, 0x00,
    0x91, 1, 0x04,
    0x92, 1, 0x3F,
    0x93, 1, 0x3F,
    0x94, 1, 0x3F,
    0x95, 1, 0x3F,
    0x96, 1, 0x05,
    0x97, 1, 0x01,
    0x98, 1, 0x3F,
    0x99, 1, 0x3F,
    0x9A, 1, 0x3F,
    0x9B, 1, 0x3F,
    0x9C, 1, 0x3F,
    0x9D, 1, 0x3F,
    0x9E, 1, 0x3F,
    0x9F, 1, 0x3F,
    0xA0, 1, 0x3F,
    0xA2, 1, 0x3F,
    0xA3, 1, 0x3F,
    0xA4, 1, 0x3F,
    0xA5, 1, 0x09,
    0xA6, 1, 0x0B,
    0xA7, 1, 0x0D,
    0xA9, 1, 0x0F,
    FLAG_DELAY, FLAG_DELAY, 10,
    0xAA, 1, 0x03, // wrong
    FLAG_DELAY, FLAG_DELAY, 10,
    0xAB, 1, 0x07, //wrong
    FLAG_DELAY, FLAG_DELAY, 10,
    0xAC, 1, 0x01,
    0xAD, 1, 0x05,
    0xAE, 1, 0x0D,
    0xAF, 1, 0x0F,
    0xB0, 1, 0x09,
    0xB1, 1, 0x0B,
    0xB2, 1, 0x3F,
    0xB3, 1, 0x3F,
    0xB4, 1, 0x3F,
    0xB5, 1, 0x3F,
    0xB6, 1, 0x3F,
    0xB7, 1, 0x3F,
    0xB8, 1, 0x3F,
    0xB9, 1, 0x3F,
    0xBA, 1, 0x3F,
    0xBB, 1, 0x3F,
    0xBC, 1, 0x3F,
    0xBD, 1, 0x3F,
    0xBE, 1, 0x07,
    0xBF, 1, 0x03,
    0xC0, 1, 0x3F,
    0xC1, 1, 0x3F,
    0xC2, 1, 0x3F,
    0xC3, 1, 0x3F,
    0xC4, 1, 0x02,
    0xC5, 1, 0x06,
    0xC6, 1, 0x3F,
    0xC7, 1, 0x3F,
    0xC8, 1, 0x3F,
    0xC9, 1, 0x3F,
    0xCA, 1, 0x3F,
    0xCB, 1, 0x3F,
    0xCC, 1, 0x3F,
    0xCD, 1, 0x3F,
    0xCE, 1, 0x3F,
    0xCF, 1, 0x3F,
    0xD0, 1, 0x3F,
    0xD1, 1, 0x3F,
    0xD2, 1, 0x0A,
    0xD3, 1, 0x08,
    0xD4, 1, 0x0E,
    0xD5, 1, 0x0C,
    0xD6, 1, 0x04,
    0xD7, 1, 0x00,
    0xDC, 1, 0x02,
    0xDE, 1, 0x10,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x04,
    0x60, 1, 0x00,
    0x61, 1, 0x0C,
    0x62, 1, 0x14,
    0x63, 1, 0x0F,
    0x64, 1, 0x08,
    0x65, 1, 0x15,
    0x66, 1, 0x0F,
    0x67, 1, 0x0B,
    0x68, 1, 0x17,
    0x69, 1, 0x0D,
    0x6A, 1, 0x10,
    0x6B, 1, 0x09,
    0x6C, 1, 0x0F,
    0x6D, 1, 0x11,
    0x6E, 1, 0x0B,
    0x6F, 1, 0x00,
    0x70, 1, 0x00,
    0x71, 1, 0x0C,
    0x72, 1, 0x14,
    0x73, 1, 0x0F,
    0x74, 1, 0x08,
    0x75, 1, 0x15,
    0x76, 1, 0x0F,
    0x77, 1, 0x0B,
    0x78, 1, 0x17,
    0x79, 1, 0x0D,
    0x7A, 1, 0x10,
    0x7B, 1, 0x09,
    0x7C, 1, 0x0F,
    0x7D, 1, 0x11,
    0x7E, 1, 0x0B,
    0x7F, 1, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x0E,
    0x01, 1, 0x75,
    0x49, 1, 0x56,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x00,
    0x58, 1, 0xA9,
    0x11, 0, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0x29, 0, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    FLAG_END_OF_TABLE, FLAG_END_OF_TABLE,
};

//==============================================================================
MhalPnlMipiDsiConfig_t tPanel_RM68200_720x1280_4Lane_Sync_Pulse_RGB888 =
{
    //HsTrail HsPrpr HsZero ClkHsPrpr ClkHsExit ClkTrail ClkZero ClkHsPost DaHsExit ContDet
      0x05,   0x03,  0x05,  0x0A,     0x0E,     0x03,    0x0B,   0x0A,     0x05,    0x00,
    //Lpx   TaGet  TaSure  TaGo
      0x10, 0x1a,  0x16,   0x32,

    //Hac,  Hpw,  Hbp,  Hfp,  Vac,  Vpw, Vbp, Vfp,  Bllp, Fps
      720,  5,    25,   5,    1280,  40, 220, 110,  0,    60,

    E_MHAL_PNL_MIPI_DSI_LANE_4,      // MhalPnlMipiDsiLaneMode_e enLaneNum;
    E_MHAL_PNL_MIPI_DSI_RGB888,      // MhalPnlMipiDsiFormat_e enFormat;
    E_MHAL_PNL_MIPI_DSI_SYNC_PULSE,  // MhalPnlMipiDsiCtrlMode_e enCtrl;

    FITI_OTA7001A_CMD,
    sizeof(FITI_OTA7001A_CMD),
    1, 0x01AF, 0x01B9, 0x80D2,
};
#endif

//==============================================================================


#if (HX8304_SUPPORTED)
u8 Hx8304_CMD[] =
{
    0xb9,   3,     0xff,   0x83,   0x94,  //ExtCmd
    0xb1,   10,      0x48,   0x11,   0x71,   0x9,   0x32,   0x54,   0x71,   0x71,   0x4e,   0x43,
    0xba,   6,        0x61,   0x3,   0x68,   0x6b,   0xb2,   0xc0,
    0xd4,   1,     0x2,
    0xb2,   11,       0x65,   0x80,   0x64,   0x05,   0x07,     0x2f,   0x00,   0x00,   0x00,   0x00,   0xc8,
    0xb4,   21,       0x26,   0x76,   0x26,   0x76,   0x26,     0x26,   0x05,   0x10,   0x86,   0x75,
                              0x00,   0x3f,   0x26,   0x76,   0x26 ,    0x76,   0x26,   0x26,   0x05,   0x10,
                              0x86,
    0xd3,   33,       0x0,   0x0,   0x4,   0x4,   0x1,   0x1,   0x10,   0x0,   0x32,   0x10,   0x0,   0x0,   0x0,   0x32,   0x15,   0x4,
                          0x5,   0x4,   0x32,   0x15,   0x14,   0x5,   0x14,   0x37,   0x33,   0x4,   0x4,   0x37,   0x0,   0x0,   0x47,   0x5,   0x40,
    0xd5,   44,      0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x25,
                            0x24,   0x27,   0x26,   0x18,   0x18,   0x11,   0x10,   0x15,   0x14,   0x13,   0x12,   0x17,   0x16,   0x1,   0x0,   0x5,
                            0x4,   0x3,   0x2,   0x7,   0x6,   0x21,   0x20,   0x23,   0x22,   0x18,   0x18,   0x18,   0x18,
    0xd6,   44,    0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x18,   0x22,
                            0x23,   0x20,   0x21,   0x18,   0x18,   0x12,   0x13,   0x16,   0x17,   0x10,   0x11,   0x14,   0x15,   0x6,   0x7,   0x2,   0x3,
                            0x4,   0x5,   0x0,   0x1,   0x26,   0x27,   0x24,   0x25,   0x18,   0x18,   0x18,   0x18,
    0xe0,   58,    0x0,   0x4,   0x11,   0x1b,   0x1e,   0x22,   0x27,   0x26,   0x51,   0x60,   0x70,   0x6f,   0x7a,   0x8c,   0x94,   0x9a,
                            0xa9,   0xac,   0xa9,   0xb9,   0xc8,   0x64,   0x62,   0x67,   0x69,   0x6a,   0x7f,   0x7f,   0x7f,   0x0,   0x4,   0x11,
                            0x1b,   0x1e,   0x22,   0x27,   0x26,   0x51,   0x60,   0x70,   0x6f,   0x7a,   0x8c,   0x94,   0x9a,   0xa9,   0xac,   0xa9,
                            0xb9,   0xc8,   0x64,   0x62,   0x67,   0x69,   0x6a,   0x7f,   0x7f,   0x7f,
    0xcc,   1,     0x3,
    0xc0,   2,     0x1f,   0x31,
    0xb6,   2,     0x43,   0x43,
    0xbd,   1,     0x1,
    0xb1,   1,     0x0,
    0xbd,   1,     0x0,
    0xbf,   7,     0x40,   0x81,   0x50,   0x0,   0x1a,   0xfc,   0x1,

    0xb2,   12,     0x65,   0x80,   0x64,   0x5,   0x7,   0x2f,   0x0,   0x0,   0x0,   0x0,   0xc0,   0x18,
    0x11,   0,  0,
    FLAG_DELAY, FLAG_DELAY, 200,
    0x29,   0,  0,    //DisplayOn
    FLAG_DELAY, FLAG_DELAY, 200,
    FLAG_END_OF_TABLE, FLAG_END_OF_TABLE,
};

MhalPnlMipiDsiConfig_t tPanel_HX8394_720x1280_2Lane_Sync_Pulse_RGB888 =
{
    //HsTrail HsPrpr HsZero ClkHsPrpr ClkHsExit ClkTrail ClkZero ClkHsPost DaHsExit ContDet
      0x05,   0x03,  0x05,  0x0A,     0x0E,     0x03,    0x0B,   0x0A,     0x05,    0x00,
    //Lpx   TaGet  TaSure  TaGo
      0x03, 0x1a,  0x16,   0x32,

    //Hac,  Hpw,  Hbp,  Hfp,  Vac,  Vpw, Vbp, Vfp,  Bllp, Fps
      720,  6,    670,  60,   1280, 6,   11,  9,    0,    60,

    E_MHAL_PNL_MIPI_DSI_LANE_2,      // MhalPnlMipiDsiLaneMode_e enLaneNum;
    E_MHAL_PNL_MIPI_DSI_RGB888,      // MhalPnlMipiDsiFormat_e enFormat;
    E_MHAL_PNL_MIPI_DSI_SYNC_PULSE,  // MhalPnlMipiDsiCtrlMode_e enCtrl;

    //Hx8304_CMD_Test_Pattern,
    //sizeof(Hx8304_CMD_Test_Pattern),
    Hx8304_CMD,
    sizeof(Hx8304_CMD),
    0, 0, 0, 0,
};


#endif
//==============================================================================



//-------------------------------------------------------------------------------------------------
// Pirvate Function
//-------------------------------------------------------------------------------------------------


