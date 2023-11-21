/*
* halHDMIUtilTx.c- Sigmastar
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

#define MHAL_HDMIUTILTX_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "mhal_common.h"
#include "drv_hdmitx_os.h"
#include "halHDMIUtilTx.h"
#include "regHDMITx.h"
#include "hdmitx_debug.h"
//#include "drvCPU.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

// static u32 g_pvHDMITxMapBase = 0U;
// static u32 g_pvPMMapBase = 0U;

void *g_pvHDMITxMapBase = (void *)0UL;
void *g_pvPMMapBase     = (void *)HDMITX_OS_MMIO_PM_BASE;
void *g_pvCoproBase     = (void *)0UL;

#define REGADDR(addr) (*((volatile u16 *)(g_pvPMMapBase + (addr << 1U))))
#define REGADDR8(addr) \
    (*((volatile u8 *)((g_pvPMMapBase + ((((u32)addr) & 0xFFFFFFFE) << 1U)) + (((u32)addr) & 0x00000001))))
#define REG(bank, addr)   (*((volatile u16 *)((g_pvPMMapBase + (bank << 1U)) + ((addr) << 2U))))
#define PMREG(bank, addr) (*((volatile u16 *)((g_pvPMMapBase + (bank << 1U)) + ((addr) << 2U))))
#define REG8(bank, addr)                                                                          \
    (*((volatile u8 *)((g_pvPMMapBase + (((u32)bank) << 1U)) + ((((u32)addr) & 0xFFFFFFFE) << 1U) \
                       + (((u32)addr) & 0x00000001))))

//++ start for IIC
#define PIN_HIGH 1U
#define PIN_LOW  0U
#define I2c_CHK  0

#define I2C_ACKNOWLEDGE     PIN_LOW
#define I2C_NON_ACKNOWLEDGE PIN_HIGH

#define HDMITX_I2C_SET_SCL(pin_state) (DrvHdmitxOsSetGpioValue(g_u8SwI2cSclPin, pin_state)) //
#define HDMITX_I2C_SET_SDA(pin_state) (DrvHdmitxOsSetGpioValue(g_u8SwI2cSdaPin, pin_state)) //
#define HDMITX_I2C_GET_SCL()          (DrvHdmitxOsGetGpioValue(g_u8SwI2cSclPin))            //
#define HDMITX_I2C_GET_SDA()          (DrvHdmitxOsGetGpioValue(g_u8SwI2cSdaPin))            //

#define I2C_CHECK_PIN_TIME      1000U // unit: 1 us
#define I2C_CHECK_PIN_CYCLE     8U    // cycle of check pin loopp
#define MCU_MICROSECOND_NOP_NUM 1U
#define I2C_CHECK_PIN_DUMMY     100U // 255 /*((I2C_CHECK_PIN_TIME / I2C_CHECK_PIN_CYCLE) * MCU_MICROSECOND_NOP_NUM)*/
#define I2C_ACCESS_DUMMY_TIME   3U

#define I2C_DEVICE_ADR_WRITE(slave_adr) (slave_adr & ~HDMITX_BIT0)
#define I2C_DEVICE_ADR_READ(slave_adr)  (slave_adr | HDMITX_BIT0)
//--- end of IIC

#define HDMITX_RX74_SLAVE_ADDR    0x74U
#define HDMITX_EDIDROM_SLAVE_ADDR 0xA0U

// wilson@kano -- for SCDC
#define HDMITX_SCDC_SLAVE_ADDR 0xA8U

#define HDMITX_SWI2C_PATCH_HALF_HIGH_VOLTAGE 1
//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

typedef enum
{
    E_I2C_TRANS_READ,
    E_I2C_TRANS_WRITE,
    E_I2C_TRANS_MAX,
} I2C_IO_TRANS_TYPE_E;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
u8 g_bDisableRegWrite = 0;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#ifdef CAMDRV_DEBUG
static u8 g_bDebugUtilFlag = TRUE;
#else
static u8 g_bDebugUtilFlag = FALSE;
#endif
static u32 g_u32DDCDelayCount = 1200U; //[K6] Modify i2c speed from 200 to 100kHz

u8 g_u8SwI2cSdaPin = 88;
u8 g_u8SwI2cSclPin = 87;
u8 g_bSwI2cEn      = 1;
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

#define DBG_HDMIUTIL(_f)             \
    do                               \
    {                                \
        if (g_bDebugUtilFlag & TRUE) \
            (_f);                    \
    } while (0);

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//****************************************
// HalHdmitxUtilityI2cDelay()
//****************************************
void HalHdmitxUtilityI2cDelay(void)
{
    /*
     * set HDMITx I2C data rate to 50KHz
     */
    volatile u32 i = g_u32DDCDelayCount;
    while (i-- > 0)
    {
#ifdef __mips__
        __asm__ __volatile__("nop");
#endif

#ifdef __AEONR2__
        __asm__ __volatile__("l.nop");
#endif

#ifdef __arm__
        __asm__ __volatile__("mov r0, r0");
#endif
    }
}

/////////////////////////////////////////
// Set I2C SCL pin high/low.
//
// Arguments: bSet - high/low bit
/////////////////////////////////////////
void HalHdmitxUtilityI2cSetSclChk(u8 bSet)
{
#if I2c_CHK
    u8 ucDummy; // loop dummy
#endif

    HDMITX_I2C_SET_SCL(bSet); // set SCL pin
#if I2c_CHK
    if (bSet == PIN_HIGH) // if set pin high
    {
        ucDummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        while ((HDMITX_I2C_GET_SCL() == PIN_LOW) && (ucDummy--))
            ; // check SCL pull high
    }
    else
    {
        ucDummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        while ((HDMITX_I2C_GET_SCL() == PIN_HIGH) && (ucDummy--))
            ; // check SCL pull low
    }
#endif
}

/////////////////////////////////////////
// Set I2C SDA pin high/low
//
// Arguments: bSet - high/low bit
/////////////////////////////////////////
void HalHdmitxUtilityI2cSetSdaChk(u8 bSet)
{
#if I2c_CHK
    u8 ucDummy; // loop dummy
#endif

    HDMITX_I2C_SET_SDA(bSet); // set SDA pin

#if I2c_CHK
    if (bSet == PIN_HIGH) // if set pin high
    {
        ucDummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        while ((HDMITX_I2C_GET_SDA() == PIN_LOW) && (ucDummy--))
            ; // check SDA pull high
    }
#endif
}

/////////////////////////////////////////
// Set I2C SDA pin as input
//
// Arguments:
/////////////////////////////////////////
void HalHdmitxUtilityI2cSetSdaInput(void)
{
#if I2c_CHK
    volatile u8 ucDummy = 70; // loop dummy
    u8          bflag   = 0;
#endif

    HDMITX_I2C_GET_SDA(); // set SDA pin

#if I2c_CHK
    while (ucDummy-- > 0)
    {
        bflag = (HDMITX_I2C_GET_SDA() == PIN_HIGH) ? TRUE : FALSE;
    }
#endif
}

//////////////////////////////////////////////////////
// I2C start signal.
// <comment>
//  SCL ________
//              \_________
//  SDA _____
//           \____________
//
// Return value: None
//////////////////////////////////////////////////////
u8 HalHdmitxUtilityI2cStart(void)
{
    u8  bStatus = TRUE; // success status
    u32 u32OldIntr;

    // disable all interrupt
    u32OldIntr = DrvHdmitxOsDisableAllInterrupts();

    HalHdmitxUtilityI2cSetSdaChk(PIN_HIGH);
    HalHdmitxUtilityI2cDelay();
    HalHdmitxUtilityI2cSetSclChk(PIN_HIGH);
    HalHdmitxUtilityI2cDelay();

#if I2c_CHK
    // check pin error
    if ((HDMITX_I2C_GET_SCL() == PIN_LOW) || (HDMITX_I2C_GET_SDA() == PIN_LOW))
    {
        bStatus = FALSE;
        DBG_HDMIUTIL(printf("HalHdmitxUtilityI2cStart()::SCL or SDA could not pull low, SCL = %d, SDA= %d\n",
                            HDMITX_I2C_GET_SCL(), HDMITX_I2C_GET_SDA()));
    }
    else // success
#endif
    {
        HDMITX_I2C_SET_SDA(PIN_LOW);
        HalHdmitxUtilityI2cDelay();
        HDMITX_I2C_SET_SCL(PIN_LOW);
        HalHdmitxUtilityI2cDelay(); // AWU addded
    }

    // restore interrupt
    DrvHdmitxOsRestoreAllInterrupts(u32OldIntr);

    return bStatus;
}

/////////////////////////////////////////
// I2C stop signal.
// <comment>
//              ____________
//  SCL _______/
//                 _________
//  SDA __________/
/////////////////////////////////////////
void HalHdmitxUtilityI2cStop(void)
{
    u32 u32OldIntr;

    // disable all interrupt
    u32OldIntr = DrvHdmitxOsDisableAllInterrupts();

    HDMITX_I2C_SET_SCL(PIN_LOW);
    HalHdmitxUtilityI2cDelay();
    HDMITX_I2C_SET_SDA(PIN_LOW);
    HalHdmitxUtilityI2cDelay();
    HalHdmitxUtilityI2cSetSclChk(PIN_HIGH);
    HalHdmitxUtilityI2cDelay();
    HalHdmitxUtilityI2cSetSdaChk(PIN_HIGH);
    HalHdmitxUtilityI2cDelay();

    // restore interrupt
    DrvHdmitxOsRestoreAllInterrupts(u32OldIntr);
}

//////////////////////////////////////////////////////////////////////////
// I2C receive byte from device.
//
// Return value: receive byte
//////////////////////////////////////////////////////////////////////////
u8 HalHdmitxUtilityI2cReceiveByte(u16 bAck)
{
    u8 ucReceive = 0;
    u8 ucMask    = 0x80;

    u32 u32OldIntr;

    // disable all interrupt
    u32OldIntr = DrvHdmitxOsDisableAllInterrupts();

    HalHdmitxUtilityI2cSetSdaInput();
    HalHdmitxUtilityI2cDelay(); // AWU added

    while (ucMask)
    {
        HalHdmitxUtilityI2cSetSclChk(PIN_HIGH);
        HalHdmitxUtilityI2cDelay();

        if (HDMITX_I2C_GET_SDA() == PIN_HIGH)
        {
            ucReceive |= ucMask;
        }
        HalHdmitxUtilityI2cSetSclChk(PIN_LOW);
        HalHdmitxUtilityI2cDelay();
        ucMask >>= 1; // next
    }                 // while

    if (bAck) // acknowledge
    {
        HalHdmitxUtilityI2cSetSdaChk(I2C_ACKNOWLEDGE);
    }
    else // non-acknowledge
    {
        HalHdmitxUtilityI2cSetSdaChk(I2C_NON_ACKNOWLEDGE);
    }

    HalHdmitxUtilityI2cDelay();
    HalHdmitxUtilityI2cSetSclChk(PIN_HIGH);
    HalHdmitxUtilityI2cDelay();
    HDMITX_I2C_SET_SCL(PIN_LOW);
    HalHdmitxUtilityI2cDelay();

    // restore interrupt
    DrvHdmitxOsRestoreAllInterrupts(u32OldIntr);

    return ucReceive;
}

//////////////////////////////////////////////////////////////////////////
// I2C send byte to device.
//
// Arguments: uc_val - send byte
// Return value: I2C acknowledge bit
//               I2C_ACKNOWLEDGE/I2C_NON_ACKNOWLEDGE
//////////////////////////////////////////////////////////////////////////
u8 HalHdmitxUtilityI2cSendByte(u8 uc_val)
{
    u8 ucMask = 0x80;
    u8 bAck; // acknowledge bit

    u32 u32OldIntr;

    // disable all interrupt
    u32OldIntr = DrvHdmitxOsDisableAllInterrupts();

    while (ucMask)
    {
        if (uc_val & ucMask)
        {
#if HDMITX_SWI2C_PATCH_HALF_HIGH_VOLTAGE
            if (ucMask == 0x01)
            {
                HalHdmitxUtilityI2cSetSdaInput();
            }
            else
#endif
            {
                HalHdmitxUtilityI2cSetSdaChk(PIN_HIGH);
            }
        }
        else
        {
            HalHdmitxUtilityI2cSetSdaChk(PIN_LOW);
        }
        HalHdmitxUtilityI2cDelay();
        HalHdmitxUtilityI2cSetSclChk(PIN_HIGH); // clock
        HalHdmitxUtilityI2cDelay();
        HalHdmitxUtilityI2cSetSclChk(PIN_LOW);
        HalHdmitxUtilityI2cDelay();

        ucMask >>= 1; // next
    }                 // while

    // recieve acknowledge
    HalHdmitxUtilityI2cSetSdaInput();
    HalHdmitxUtilityI2cDelay();
    HalHdmitxUtilityI2cSetSclChk(PIN_HIGH);
    HalHdmitxUtilityI2cDelay();
    bAck = HDMITX_I2C_GET_SDA(); // recieve acknowlege
    HDMITX_I2C_SET_SCL(PIN_LOW);
    HalHdmitxUtilityI2cDelay();

    // restore interrupt
    DrvHdmitxOsRestoreAllInterrupts(u32OldIntr);

    return (bAck);
}

//////////////////////////////////////////////////////////////////////////
// I2C access start.
//
// Arguments: ucSlaveAdr - slave address
//            trans_t - E_I2C_TRANS_WRITE/E_I2C_TRANS_READ
//////////////////////////////////////////////////////////////////////////
u8 HalHdmitxUtilityI2cAccessStart(u8 ucSlaveAdr, I2C_IO_TRANS_TYPE_E trans_t)
{
    u8 ucDummy; // loop dummy

    if (trans_t == E_I2C_TRANS_READ) // check i2c read or write
    {
        ucSlaveAdr = I2C_DEVICE_ADR_READ(ucSlaveAdr); // read
    }
    else
    {
        ucSlaveAdr = I2C_DEVICE_ADR_WRITE(ucSlaveAdr); // write
    }

    ucDummy = I2C_ACCESS_DUMMY_TIME;
    while (ucDummy--)
    {
        if (HalHdmitxUtilityI2cStart() == FALSE)
        {
            continue;
        }

        if (HalHdmitxUtilityI2cSendByte(ucSlaveAdr) == I2C_ACKNOWLEDGE) // check acknowledge
        {
            return TRUE;
        }

        HalHdmitxUtilityI2cStop();

        // DrvHdmitxOsMsDelay(1);//delay 1ms
        DrvHdmitxOsUsDelay(100); // delay 100us
    }                            // while

    return FALSE;
}

/////////////////////////////////////////////////////////////////
// I2C read bytes from device.
//
// Arguments: ucSlaveAdr - slave address
//            ucSubAdr - sub address
//            pBuf - pointer of buffer
//            ucBufLen - length of buffer
/////////////////////////////////////////////////////////////////
u8 HalHdmitxUtilityI2cBurstReadBytes(u8 ucSlaveAdr, u8 ucSubAdr, u8 *pBuf, u16 ucBufLen)
{
    u8 ucDummy; // loop dummy
    u8 result = FALSE;

    if (g_bSwI2cEn)
    {
        ucDummy = I2C_ACCESS_DUMMY_TIME;
        while (ucDummy--)
        {
            if (HalHdmitxUtilityI2cAccessStart(ucSlaveAdr, E_I2C_TRANS_WRITE) == FALSE)
            {
                continue;
            }
            if (HalHdmitxUtilityI2cSendByte(ucSubAdr) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
            {
                DBG_HDMIUTIL(printf("HalHdmitxUtilityI2cBurstReadBytes()::No ACK\n"));
                continue;
            }

            // HalHdmitxUtilityI2cStop();//AWU added

            if (HalHdmitxUtilityI2cAccessStart(ucSlaveAdr, E_I2C_TRANS_READ) == FALSE)
            {
                continue;
            }
            while (ucBufLen--) // loop to burst read
            {
                *pBuf = HalHdmitxUtilityI2cReceiveByte(ucBufLen); // receive byte

                // if(*pBuf) DBG_HDMITX(printf("HalHdmitxUtilityI2cBurstReadBytes()::BINGO\n"));

                pBuf++; // next byte pointer
            }           // while
            result = TRUE;
            break;
        } // while

        HalHdmitxUtilityI2cStop();
    }
    else
    {
        return DrvHdmitxOsSetI2cReadBytes(ucSlaveAdr, ucSubAdr, pBuf, ucBufLen);
    }
    return result;
}

/////////////////////////////////////////////////////////////////
// I2C write bytes to device.
//
// Arguments: ucSlaveAdr - slave address
//            ucSubAdr - sub address
//            pBuf - pointer of buffer
//            ucBufLen - length of buffer
/////////////////////////////////////////////////////////////////
u8 HalHdmitxUtilityI2cBurstWriteBytes(u8 ucSlaveAdr, u8 ucSubAdr, u8 *pBuf, u16 ucBufLen)
{
    u8 ucDummy; // loop dummy
    u8 result = FALSE;

    if (g_bSwI2cEn)
    {
        ucDummy = I2C_ACCESS_DUMMY_TIME;
        while (ucDummy--)
        {
            if (HalHdmitxUtilityI2cAccessStart(ucSlaveAdr, E_I2C_TRANS_WRITE) == FALSE)
            {
                continue;
            }
            if (HalHdmitxUtilityI2cSendByte(ucSubAdr) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
            {
                DBG_HDMIUTIL(printf("HalHdmitxUtilityI2cBurstReadBytes()::No ACK\n"));
                continue;
            }

            while (ucBufLen--) // loop of writting data
            {
                result = HalHdmitxUtilityI2cSendByte(*pBuf); // send byte, fix coverity impact.

                pBuf++; // next byte pointer
            }           // while
            result = TRUE;
            break;
        } // while

        HalHdmitxUtilityI2cStop();
    }
    else
    {
        return DrvHdmitxOsSetI2cWriteBytes(ucSlaveAdr, ucSubAdr, pBuf, ucBufLen, 1);
    }
    return result;
}

/////////////////////////////////////////////////////////////////
// I2C read a byte from device.
//
// Arguments: ucSlaveAdr - slave address
//            ucSubAdr - sub address
//            pBuf - return buffer point
// Return value: read byte
/////////////////////////////////////////////////////////////////
u8 HalHdmitxUtilityI2cReadByte(u8 ucSlaveAdr, u8 ucSubAdr, u8 *pBuf)
{
    return (HalHdmitxUtilityI2cBurstReadBytes(ucSlaveAdr, ucSubAdr, pBuf, 1));
}

/////////////////////////////////////////////////////////////////
// I2C write a byte from device.
//
// Arguments: ucSlaveAdr - slave address
//            ucSubAdr - sub address
//            uc_val - write byte
/////////////////////////////////////////////////////////////////
u8 HalHdmitxUtilityI2cWriteByte(u8 ucSlaveAdr, u8 ucSubAdr, u8 uc_val)
{
    return (HalHdmitxUtilityI2cBurstWriteBytes(ucSlaveAdr, ucSubAdr, &uc_val, 1));
}

//------------------------------------------------------------------------------
/// @brief Set HDMITx register base address
/// @param[in] u32Base
/// @return None
//------------------------------------------------------------------------------
void HalHdmitxUtilitySetIOMapBase(void *paBase, void *paPMBase, void *paCoproBase)
{
    g_pvHDMITxMapBase = paBase;
    g_pvPMMapBase     = paPMBase;
    g_pvCoproBase     = paCoproBase;
}

// This routine read edid
u8 HalHdmitxUtilityEdidReadBytes(u8 addr, u8 len, u8 *buf)
{
    return (HalHdmitxUtilityI2cBurstReadBytes(HDMITX_EDIDROM_SLAVE_ADDR, addr, buf, len));
}

//      TxEdidSetSegment
u8 HalHdmitxUtilityEdidSetSegment(u8 value)
{
    u8 bRet = TRUE;

    if (g_bSwI2cEn)
    {
        if (HalHdmitxUtilityI2cAccessStart(0x60, E_I2C_TRANS_WRITE) == FALSE)
        {
            bRet = FALSE;
            printf("%s %d, Start Fail\n", __FUNCTION__, __LINE__);
        }
        if (HalHdmitxUtilityI2cSendByte(value) == I2C_NON_ACKNOWLEDGE) // send byte
        {
            printf("%s %d, SendByte Fail\n", __FUNCTION__, __LINE__);
            bRet = FALSE;
        }
    }
    else
    {
        if (DrvHdmitxOsSetI2cWriteBytes(0x60, value, NULL, 0, 0) == FALSE)
        {
            printf("%s %d, EdidSetSegment Fail\n", __FUNCTION__, __LINE__);
            bRet = FALSE;
        }
    }

    return bRet;
}

//  This routine read the block in EDID
u8 HalHdmitxUtilityEdidReadBlock(u8 num, u8 *buf)
{
#if 1
    u8 u8StartAddr;

    u8StartAddr = (num & 0x01) ? 0x80 : 0x00;

    if (num > 1)
    {
        if (HalHdmitxUtilityEdidSetSegment(num / 2) == FALSE)
        {
            return FALSE;
        }
    }
    if (HalHdmitxUtilityEdidReadBytes(u8StartAddr, 128, buf) == FALSE)
    {
        return FALSE;
    }
    return TRUE;
#else
    u8 i;

    for (i = 0; i < 10; i++)
    {
        HalHdmitxUtilityI2cSetSdaChk(PIN_HIGH);
        HalHdmitxUtilityI2cSetSclChk(PIN_HIGH);
        HalHdmitxUtilityI2cDelay();
        HalHdmitxUtilityI2cSetSdaChk(PIN_LOW);
        HalHdmitxUtilityI2cSetSclChk(PIN_LOW);
        HalHdmitxUtilityI2cDelay();
    }
    HalHdmitxUtilityI2cSetSdaChk(PIN_HIGH);
    HalHdmitxUtilityI2cSetSclChk(PIN_HIGH);
    return TRUE;
#endif
}

//------------------------------------------------------------------------------
/// @brief This routine reads HDMI Register
/// @param[in] bank register bank
/// @param[in] address register address
/// @return register value
//------------------------------------------------------------------------------
u16 HalHdmitxUtilityR2BYTE(u32 addr)
{
    return (REGADDR(addr));
}

u8 HalHdmitxUtilityRBYTE(u32 addr)
{
    return (REGADDR8(addr));
}

//------------------------------------------------------------------------------
/// @brief This routine writes HDMI Register with mask
/// @param[in] bank register bank
/// @param[in] address register address
/// @param[in] reg_mask mask value
/// @param[in] reg_data register data
/// @return None
//------------------------------------------------------------------------------
void HalHdmitxUtilityW2BYTEMSK(u32 addr, u16 reg_data, u16 reg_mask)
{
    u16 u16Regvalue;

    if (g_bDisableRegWrite == TRUE)
    {
        return;
    }
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL, "[%s]addr:%x data:%hx mask:%hx\n", __FUNCTION__, addr, reg_data, reg_mask);
    u16Regvalue   = (REGADDR(addr) & (~reg_mask)) | (reg_data & reg_mask);
    REGADDR(addr) = u16Regvalue;
}

void HalHdmitxUtilityWBYTEMSK(u32 addr, u8 reg_data, u8 reg_mask)
{
    u8 u8Regvalue;

    if (g_bDisableRegWrite == TRUE)
    {
        return;
    }
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL, "[%s]addr:%x data:%hx mask:%hx\n", __FUNCTION__, addr, reg_data, reg_mask);
    u8Regvalue     = (REGADDR8(addr) & (~reg_mask)) | (reg_data & reg_mask);
    REGADDR8(addr) = u8Regvalue;
}
void HalHdmitxUtilityUtilDebugEnable(u8 benable)
{
    g_bDebugUtilFlag = benable;
}

u32 HalHdmitxUtilityGetDDCDelayCount(void)
{
    return g_u32DDCDelayCount;
}

void HalHdmitxUtilitySetDDCDelayCount(u32 u32Delay)
{
    g_u32DDCDelayCount = u32Delay;
}

u8 HalHdmitxUtilityAdjustDDCFreq(u32 u32Speed_K)
{
#define DELAY_CNT(SpeedKHz)                                                                       \
    ((u32FactorDelay / (SpeedKHz)) - ((u32Parameter1 + u32AdjParam) - ((SpeedKHz) / u32AdjParam)) \
     + ((1 << ((u32Parameter2 - SpeedKHz) / 40))))

    u32 u32FactorDelay  = 50400UL;
    u32 u32FactorAdjust = 11040UL;
    u32 u32ParamBase1   = 130UL;
    u32 u32Parameter1   = 130UL;
    u32 u32Parameter2   = 440UL;
    u32 u32AdjParam     = 0;
    u32 u32CpuSpeedMHz  = 0;

    // u32CpuSpeedMHz = ((u32) ((volatile u16*)(g_pvCoproBase))[0x1EE2]);
    // u32CpuSpeedMHz = u32CpuSpeedMHz*12*4)/1000;

    u32CpuSpeedMHz  = DrvHdmitxOsGetCpuSpeedMhz();
    u32FactorDelay  = u32CpuSpeedMHz * 100;
    u32FactorAdjust = (u32CpuSpeedMHz >= 312) ? 10000UL : 13000UL;

    if (u32CpuSpeedMHz > 0)
    {
        u32AdjParam = u32FactorAdjust / u32CpuSpeedMHz;
        DBG_HDMIUTIL(printf("[%s][%d] u32AdjParam = 0x%X, u32CpuSpeedMHz = 0x%X \n", __FUNCTION__, __LINE__,
                            u32AdjParam, u32CpuSpeedMHz));
    }
    else
    {
        DBG_HDMIUTIL(printf("%s, Error parameter u32CpuSpeedMHz = 0x%X", __FUNCTION__, u32CpuSpeedMHz));
        return FALSE;
    }

    if (u32AdjParam == 0)
    {
        g_u32DDCDelayCount = 250;
        return FALSE;
    }

    u32Parameter2 = 440UL;
    //(2) assign base for parameter

    if (u32CpuSpeedMHz >= 1000)
    {
        u32ParamBase1 = 150UL;
    }
    else if (u32CpuSpeedMHz >= 900)
    {
        u32ParamBase1 = 140UL;
    }
    else if (u32CpuSpeedMHz >= 780)
    {
        u32ParamBase1 = 135UL;
    }
    else if (u32CpuSpeedMHz >= 720)
    {
        u32ParamBase1 = 130UL;
    }
    else if (u32CpuSpeedMHz >= 650)
    {
        u32ParamBase1 = 125UL;
    }
    else if (u32CpuSpeedMHz >= 600)
    {
        u32ParamBase1 = 110UL;
    }
    else if (u32CpuSpeedMHz >= 560)
    {
        u32ParamBase1 = 100UL;
    }
    else if (u32CpuSpeedMHz >= 530)
    {
        u32ParamBase1 = 95UL;
    }
    else if (u32CpuSpeedMHz >= 500)
    {
        u32ParamBase1 = 90UL;
    }
    else if (u32CpuSpeedMHz >= 480)
    {
        u32ParamBase1 = 85UL;
    }
    else if (u32CpuSpeedMHz >= 430)
    {
        u32ParamBase1 = 80UL;
    }
    else if (u32CpuSpeedMHz >= 400)
    {
        u32ParamBase1 = 75UL;
    }
    else if (u32CpuSpeedMHz >= 384)
    {
        u32ParamBase1 = 70UL;
    }
    else if (u32CpuSpeedMHz >= 360)
    {
        u32ParamBase1 = 65UL;
    }
    else if (u32CpuSpeedMHz >= 336)
    {
        u32ParamBase1 = 60UL;
    }
    else if (u32CpuSpeedMHz >= 312)
    {
        u32ParamBase1 = 40UL;
    }
    else if (u32CpuSpeedMHz >= 240)
    {
        u32ParamBase1 = 10UL;
    }
    else if (u32CpuSpeedMHz >= 216)
    {
        u32ParamBase1 = 0UL;
    }
    else
    {
        u32ParamBase1 = 0UL;
    }
    //(3) compute parameter 1 by base
    if (u32Speed_K >= 100)
    {
        u32Parameter1 = u32ParamBase1 + 250; // 100K level
    }
    else if (u32Speed_K >= 75)
    {
        u32Parameter1 = u32ParamBase1 + 340; // 75K level
    }
    else if (u32Speed_K >= 50)
    {
        u32Parameter1 = u32ParamBase1 + 560; // 50K level
    }
    else
    {
        u32Parameter1 = u32ParamBase1 + 860; // 30K level
    }

    // g_u32DDCDelayCount = 250;
    DBG_HDMIUTIL(
        printf("[%s][%d] u32Speed_K = 0x%X , u32AdjParam = 0x%X \n", __FUNCTION__, __LINE__, u32Speed_K, u32AdjParam));

    //(4) compute delay counts
    if ((u32Speed_K > 0) && (u32AdjParam > 0))
    {
        g_u32DDCDelayCount = DELAY_CNT(u32Speed_K);
        DBG_HDMIUTIL(printf("[%s][%d] g_u32DDCDelayCount = 0x%X \n", __FUNCTION__, __LINE__, g_u32DDCDelayCount));
    }
    else
    {
        DBG_HDMIUTIL(printf("[%s][%d] Error parameter u32Speed_K = 0x%X , u32AdjParam = 0x%X", __FUNCTION__, __LINE__,
                            u32Speed_K, u32AdjParam));
        return FALSE;
    }
    return TRUE;
}

u8 HalHdmitxUtilityGetSwI2cEn(void)
{
    return g_bSwI2cEn;
}

u8 HalHdmitxUtilityGetSwI2cSdaPin(void)
{
    return g_u8SwI2cSdaPin;
}

u8 HalHdmitxUtilityGetSwI2cSclPin(void)
{
    return g_u8SwI2cSclPin;
}
