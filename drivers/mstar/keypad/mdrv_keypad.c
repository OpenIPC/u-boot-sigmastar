/*
* mdrv_keypad.c- Sigmastar
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

#include <linux/input.h>
#include "mdrv_keypad.h"
#include "mhal_keypad.h"
#include "drvGPIO.h"
#include "padmux.h"
#include "gpio.h"

#define  DEBUG_LOG   FALSE

#define MDRV_KEYPAD_DEBUG(format, args...)  do{ if(DEBUG_LOG) printf(format, ##args); }while(0)
#define MDRV_KEYPAD_ERROR(format, args...)  do{ printf(format, ##args); }while(0)

static U32 keypad_desc[KEYPAD_MAXROWNUM][KEYPAD_MAXCOLNUM] = {
    {KEY_ESC,KEY_8,        KEY_W,KEY_P,         KEY_F,          KEY_GRAVE,    KEY_N,         KEY_SPACE},
    {KEY_1,  KEY_9,        KEY_E,KEY_LEFTBRACE, KEY_G,          KEY_LEFTSHIFT,KEY_M,         KEY_CAPSLOCK},
    {KEY_2,  KEY_0,        KEY_R,KEY_RIGHTBRACE,KEY_H,          KEY_BACKSLASH,KEY_COMMA,     KEY_F1},
    {KEY_3,  KEY_MINUS,    KEY_T,KEY_ENTER,     KEY_J,          KEY_Z,        KEY_DOT,       KEY_F2},
    {KEY_4,  KEY_EQUAL,    KEY_Y,KEY_LEFTCTRL,  KEY_K,          KEY_X,        KEY_SLASH,     KEY_F3},
    {KEY_5,  KEY_BACKSPACE,KEY_U,KEY_A,         KEY_L,          KEY_C,        KEY_RIGHTSHIFT,KEY_F4},
    {KEY_6,  KEY_TAB,      KEY_I,KEY_S,         KEY_SEMICOLON,  KEY_V,        KEY_KPASTERISK,KEY_F5},
    {KEY_7,  KEY_Q,        KEY_O,KEY_D,         KEY_APOSTROPHE, KEY_B,        KEY_LEFTALT,   KEY_F6},
};


#define DEF_ROW  7
#define DEF_COL  7

#define DEFAULT_DEBOUNCE_TIME           5                 // ms unit
#define KEYPADINPUTCLK                  12000000          // 12M
#define KEYPADADJUSTCLK                 32000             // 32khz

/* ms unit. max.170(form designer)-min.42(form net) (0x1f0-0x7c) */
#define KEYPAD_ALIGNTIMELENGTH          100 //0x64

#define KEYSTASHMAXNUM 4

/**
 * mdrv_keypadcalc_debounce - returns debounce num access to clk
 * @clk: clk of keypad 
 *
 * debounce time = (setting number) * 4096 / (clk)
 * Return value of this function set to register glitch time num
 */
#define mdrv_keypadcalc_DEBOUNCE(clk)         (clk*DEFAULT_DEBOUNCE_TIME/4096/1000)

/**
 * mdrv_keypadcalc_scanrate - returns scanrate num access to scanclk
 * @scanclk: clk of keypad adjust CLK
 *
 * scan_rate time = (input clk rate) / (setting number +1).
 * Return value of this function set to register scan_rate time num.
 */
#define mdrv_keypadcalc_SCANRATE(scanclk)     (KEYPADINPUTCLK/scanclk-1)

/**
 * mdrv_keypadcalc_ALIGN - returns align num access to ALIGN_NUM
 * @ALIGN_NUM: time of align(ms unit).
 *
 * Align time length is (setting number) / (deglitch function clock);
 * When testing FPGA ,the align time length is (setting number) * 4096 / (FPGA clk);
 * Return value of this function set to register scan_rate time num.
 */
#define mdrv_keypadcalc_ALIGN(ALIGN_NUM)      (ALIGN_NUM*KEYPADINPUTCLK/4096/1000)

/**
 * mdrv_keypadMaskAllEN() - For disable mask key interrupt
 * @num: set to zero it's will simultaneously set all keymask.
 *       if need modify one key.set num
 * @Enbale: mask the key. if TURE No matter the key is pressed or not, irq will not arise.
 * Key num correspondence num please access to keypadnote
 * @Returns void
 */
static void mdrv_keypadMaskAllEN(U32 num,bool Enbale)
{
    U32 i;
    if(num == 0)
    {
        for(i=1;i<=KEPYAD_MAXKEYNUM_TWOSTATUS; i++)
        {
            mhal_KeyPad_SetMask(i-1,Enbale);
        }
    }
    else
        mhal_KeyPad_SetMask(num,Enbale);
    return;
}

/**
 * mdrv_keypadSetForce() -For Force key interrupt
 * @num: set to zero it's will simultaneously set all keyForce.
 *       if need modify one key.set num
 * @Enbale: Force the key. if TURE No matter the key is pressed or not, irq will arise
 * Key num correspondence num please access to keypadnote
 * @Returns void
 */
static void mdrv_keypadSetForce(U32 num,bool Enbale)
{
    S32 i;
    if(num == 0)
    {
        for(i=1;i<=KEPYAD_MAXKEYNUM_TWOSTATUS; i++)
        {
            mhal_KeyPad_SetForce(i-1,Enbale);
        }
    }
    else
        mhal_KeyPad_SetForce(num,Enbale);
    return;
}

/**
 * mdrv_keypadGlhrALLEN() -For enbale keypad glictch time form HW.
 * @Enbale: glictch the key.for disable glictch remove if needed
 * @Returns void
 */
static void mdrv_keypadGlhrALLEN(bool Enbale)
{
    S32 i;
    for(i=0;i<KEPYAD_MAXKEYNUM; i++)
    {
        mhal_KeyPad_SetGlhrEN(i,Enbale);
    }
    return;
}

/**
 * mdrv_keypadSetColRowNum() -set keypad Col&column amount
 * @row: for keypad row amount.
 * @col: for keypad column amount.
 * @Returns void
 */
static void mdrv_keypadSetColRowNum( U32 row,U32 col )
{
    mhal_KeyPad_SetColNum(col);
    mhal_KeyPad_SetRowNum(row);
    return;
}

/**
 * mdrv_keypadReset() -Reset keypad if need. 
 * it will clear irq and already pressed key register value
 * @Returns void
 */
static void mdrv_keypadReset(void)
{
    mhal_KeyPad_RST(TRUE);
    mhal_KeyPad_RST(FALSE);
    return;
}

/**
 * mdrv_keypad_init() -init keypad if change mode or glictch time etc.
 * @ddata: pointer to drvdata
 * alignnum only set in mode1 or mode2.in other mode should be reset.
 * @Returns KEYPAD_SUCCESS
 */
void mdrv_keypad_init(KEYPAD_STANDARD_E standard)
{
    S32 scanrate=0;
    U32 align,debounce;
    U32 padmux,gpio;
    U8  u8standard,u8cow,u8col;
    U8  u8_cnt;
    
    mhal_KeyPad_CLK(TRUE);

    mdrv_keypadReset();

    u8standard = (U8)standard;
    

    if(u8standard == E_KP_STANDARD_MODE1)      {u8cow=7;u8col=7;padmux = PINMUX_FOR_BT1120_MODE_1;gpio=PAD_KEY0;}
    else if(u8standard == E_KP_STANDARD_MODE2) {u8cow=4;u8col=4;padmux = PINMUX_FOR_BT1120_MODE_2;gpio=PAD_KEY0;}
    else if(u8standard == E_KP_STANDARD_MODE3) {u8cow=3;u8col=3;padmux = PINMUX_FOR_BT1120_MODE_3;gpio=PAD_KEY8;}
    else{u8cow=7;u8col=7;u8standard=1;padmux = PINMUX_FOR_BT1120_MODE_1;gpio=PAD_KEY0;} //default use 7*7


    for(u8_cnt = 0; u8_cnt < (u8cow*2); u8_cnt++)
        MDrv_GPIO_PadVal_Set(gpio+u8_cnt,padmux);
    
    //mhal_keypad_pinmux(u8standard); // 1 -> 7*7    2 -> 4*4   3 -> 3.3 

    mdrv_keypadSetColRowNum(u8cow,u8col);
    mdrv_keypadMaskAllEN(0,TRUE); // use polling mode.mask irq

    scanrate = mdrv_keypadcalc_SCANRATE(KEYPADADJUSTCLK);
    mhal_KeyPad_SetScanDfs(scanrate);
    mhal_KeyPad_SetGlhrmDfs(scanrate);
    mhal_KeyPad_ModeSel(E_KP_STATUS_MODE_0);


    align = mdrv_keypadcalc_ALIGN(KEYPAD_ALIGNTIMELENGTH);
    mhal_KeyPad_KEYPAD_ALIGN_NUM(align);


    mdrv_keypadGlhrALLEN(TRUE);
    mdrv_keypadSetForce(0,FALSE);
    debounce = mdrv_keypadcalc_DEBOUNCE(KEYPADINPUTCLK);
    mhal_KeyPad_SetGlhrNum(debounce);
    mhal_KeyPad_EnableScan(TRUE);

    return;
}



/**
 * mdrv_keypad_hex2bit() - Returns bits of buttons Numbering
 * @val: data of register 
 *
 * @Returns Number of register has been pressed bit num
 */
static U32 mdrv_keypad_hex2bit(U32 val)
{
    U32 i;
    for(i=0;i<=3;i++)
    {
        if( (val>>i)&BIT0 )
            return i;
    }
    return 0;
}

/**
 * mdrv_keypad_GetRulenum() - Returns Number of buttons Numbering
 * @value: Button numbers of keypadnote
 * @rulenum: Store the col&row nums for get access report value
 *
 * max supprose four key pressed simultaneously. rulenum only alloc four space
 * @Returns Number of success
 */
static U32 mdrv_keypad_GetRulenum(U32 value,struct ms_keypad_rulenum *rulenum)
{
    int keyvalue=value;
    if(keyvalue <= 0x80)
        rulenum->keyValue = keyvalue;
    else
        rulenum->keyValue = (keyvalue-0x80);

    if(keyvalue >= KEPYAD_MAXKEYNUM )
        keyvalue -= KEPYAD_MAXKEYNUM;
    rulenum->col = keyvalue/KEYPAD_MAXCOLNUM;
    rulenum->row = keyvalue%KEYPAD_MAXROWNUM;

    return TRUE;
}

/**
 * mdrv_keypad_GetPressedkey() - Returns Number of buttons pressed simultaneously
 * @data: pointer to ms_keypad_date
 * @stagecode Button release or press
 * max supprose four key pressed simultaneously
 * @Returns Number of buttons pressed simultaneously
 */
static U32 mdrv_keypad_GetPressedkey(struct ms_keypad_date *data,U32 *stagecode)
{
    U32 tmp=0;
    U32 i=0,j=0;
    U32 code=0; /*num of Weight coefficient */
    U32 value0=0;//,value1=0,value2=0,value3=0;

    for(i=0; i<=3; i++)
    {
        /* this 4 mean of 16bit have 4*4 bit*/
        for(j=0;j<4;j++)
        {
            if(data->s_value[i] == 0)
                continue;
            tmp = ((data->s_value[i])>>(4*j)) & 0xf; /* judgment 4bit of register value */
            if(tmp == 0)
                continue;

            MDRV_KEYPAD_DEBUG("find value %x\n",tmp);
            code = *stagecode = i;
            switch(tmp)
            {
                case 0x1:
                case 0x2:
                case 0x4:
                case 0x8:
                     {
                        value0 = code*16+mdrv_keypad_hex2bit(tmp)+(j*4);
                        if(!mdrv_keypad_GetRulenum(value0,&data->rulenum[0]))
                            return 0;
                     }
                default:return 0;
            }
        }
    }
    return 0;
}

/**
 * mdrv_keypad_Scan() -Scan keypad by once,if have value,return keyvalve,else return 0.
 * @Param void
 * please add 
 * @Returns KEYPAD value, return zero is no key pressed
 */
U32 mdrv_keypad_Scan(void)
{
    U32 stage,icnt=0;
    U32 value=0,code;
    struct ms_keypad_date st_keypad;
    int row,col;
    memset(&st_keypad, 0, sizeof(struct ms_keypad_date));
    memset(&st_keypad.s_value, 0, (sizeof(U32)*8));
    memset(&st_keypad.rulenum[0], 0, (sizeof(struct ms_keypad_rulenum)));

    for(stage=0; stage<=3;stage++)
    {
        mhal_KeyPad_GetRaw_Stage(stage,&value);
        if(value == 0)
            icnt++;
        st_keypad.s_value[stage] = value;
    }
    if(icnt == 4)
    {
        return 0;
    }
    mdrv_keypad_GetPressedkey(&st_keypad,&code);
    MDRV_KEYPAD_DEBUG("<%d %d>\n",st_keypad.rulenum[0].row,st_keypad.rulenum[0].col);

    row = st_keypad.rulenum[0].row;
    col = st_keypad.rulenum[0].col;

    return keypad_desc[row][col];
}


