/*
* gop_ut.h- Sigmastar
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

#ifndef _GOP_UT_H_
#define _GOP_UT_H_

//-------------------------------------------------------------------------------------------------
//  SC Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    int argc;
    char *argv[100];
}GopUtStrConfig_t;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _GOP_UT_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void GopUtParsingCommand(char *str, GopUtStrConfig_t *pstStrCfg);
INTERFACE void GopTestStore(GopUtStrConfig_t *pstStringCfg);
INTERFACE int  GopTestShow(char *DstBuf);



#undef INTERFACE

#endif
