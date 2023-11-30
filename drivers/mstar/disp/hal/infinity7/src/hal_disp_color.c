/*
* hal_disp_color.c- Sigmastar
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

#define _HAL_DISP_COLOR_C_

#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_color.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define ACE_ENABLE_PC_YUV_TO_RGB 0x01
#define ACE_ENABLE_PC_SRGB       0x02
#define ACE_ENABLE_HUE_256_STEP  0x04

#define ENABLE_CBCR 1
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

typedef struct HalDispColorConfig_s
{
    s16 *tSrgbMatrix;
    u8   bColorCorrectMatrixUpdate;

    u8                               u8ACEConfig;
    HalDispColorYuvToRgbMatrixType_e enMatrixType;

    u8 u8VideoRCon;
    u8 u8VideoGCon;
    u8 u8VideoBCon;
    u8 u8VideoContrast;
    u8 u8VideoSaturation;

    u8 u8BrightnessR;
    u8 u8BrightnessG;
    u8 u8BrightnessB;

#if (ENABLE_CBCR)
    u8 u8VideoCb;
    u8 u8VideoCr;
#endif

    u8 u8VideoHue;

    s16 sYuvToRGBMatrix[3][3];
    s16 sVideoSatHueMatrix[3][3];
    s16 sVideoContrastMatrix[3][3];
    s16 sColorCorrrectMatrix[3][3];

    // For PC
    u8  u8PCRCon;
    u8  u8PCGCon;
    u8  u8PCBCon;
    u8  u8PCContrast;
    s16 sPCConRGBMatrix[3][3];
} HalDispColorConfig_t;

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
HalDispColorConfig_t g_tColorCfg[HAL_DISP_DEVICE_MAX][E_DISP_COLOR_CSC_ID_NUM];

s16 g_stSDTVYuv2rgb[3][3] = {
    {0x0662, 0x04A8, 0x0000},   // 1.596,  1.164, 0
    {-0x0341, 0x04A8, -0x0190}, // -0.813, 1.164, -0.391
    {0x0000, 0x04A8, 0x0812}    // 0,      1.164, 2.018
};

s16 g_stHDTVYuv2rgb[3][3] = {
    {0x072C, 0x04A8, 0x0000},   // 1.793,  1.164, 0
    {-0x0223, 0x04A8, -0x00DA}, // -0.534, 1.164, -0.213
    {0x0000, 0x04A8, 0x0876}    // 0,      1.164, 2.115
};

s16 g_stByPassYuv2rgb[3][3] = {
    {0x0400, 0x0000, 0x0000}, // 1.793,  1.164, 0
    {0x0000, 0x0400, 0x0000}, // -0.534, 1.164, -0.213
    {0x0000, 0x0000, 0x0400}  // 0,      1.164, 2.115
};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static s16 _HalDispColorCalSine(u16 u16Hue, u8 b256Step)
{
    // sin((u8Hue * 3.1415926)/180)) * 1024

    s16 tHueToSine100[101] = {
        784,  // 0
        772,  // 1
        760,  // 2
        748,  // 3
        736,  // 4
        724,  // 5
        711,  // 6
        698,  // 7
        685,  // 8
        671,  // 9
        658,  // 10
        644,  // 11
        630,  // 12
        616,  // 13
        601,  // 14
        587,  // 15
        572,  // 16
        557,  // 17
        542,  // 18
        527,  // 19
        511,  // 20
        496,  // 21
        480,  // 22
        464,  // 23
        448,  // 24
        432,  // 25
        416,  // 26
        400,  // 27
        383,  // 28
        366,  // 29
        350,  // 30
        333,  // 31
        316,  // 32
        299,  // 33
        282,  // 34
        265,  // 35
        247,  // 36
        230,  // 37
        212,  // 38
        195,  // 39
        177,  // 40
        160,  // 41
        142,  // 42
        124,  // 43
        107,  // 44
        89,   // 45
        71,   // 46
        53,   // 47
        35,   // 48
        17,   // 49
        0,    // 50
        -17,  // 51
        -35,  // 52
        -53,  // 53
        -71,  // 54
        -89,  // 55
        -107, // 56
        -124, // 57
        -142, // 58
        -160, // 59
        -177, // 60
        -195, // 61
        -212, // 62
        -230, // 63
        -247, // 64
        -265, // 65
        -282, // 66
        -299, // 67
        -316, // 68
        -333, // 69
        -350, // 70
        -366, // 71
        -383, // 72
        -400, // 73
        -416, // 74
        -432, // 75
        -448, // 76
        -464, // 77
        -480, // 78
        -496, // 79
        -512, // 80
        -527, // 81
        -542, // 82
        -557, // 83
        -572, // 84
        -587, // 85
        -601, // 86
        -616, // 87
        -630, // 88
        -644, // 89
        -658, // 90
        -671, // 91
        -685, // 92
        -698, // 93
        -711, // 94
        -724, // 95
        -736, // 96
        -748, // 97
        -760, // 98
        -772, // 99
        -784, // 100
    };

    s16 tHueToSine256[257] = {
        806,   // 0
        817,   // 1
        828,   // 2
        838,   // 3
        848,   // 4
        858,   // 5
        868,   // 6
        877,   // 7
        886,   // 8
        895,   // 9
        904,   // 10
        912,   // 11
        920,   // 12
        928,   // 13
        935,   // 14
        942,   // 15
        949,   // 16
        955,   // 17
        962,   // 18
        968,   // 19
        973,   // 20
        979,   // 21
        984,   // 22
        989,   // 23
        993,   // 24
        997,   // 25
        1001,  // 26
        1005,  // 27
        1008,  // 28
        1011,  // 29
        1014,  // 30
        1016,  // 31
        1018,  // 32
        1020,  // 33
        1021,  // 34
        1022,  // 35
        1023,  // 36
        1023,  // 37
        1023,  // 38
        1023,  // 39
        1023,  // 40
        1022,  // 41
        1021,  // 42
        1020,  // 43
        1018,  // 44
        1016,  // 45
        1014,  // 46
        1011,  // 47
        1008,  // 48
        1005,  // 49
        1001,  // 50
        997,   // 51
        993,   // 52
        989,   // 53
        984,   // 54
        979,   // 55
        973,   // 56
        968,   // 57
        962,   // 58
        955,   // 59
        949,   // 60
        942,   // 61
        935,   // 62
        928,   // 63
        920,   // 64
        912,   // 65
        904,   // 66
        895,   // 67
        886,   // 68
        877,   // 69
        868,   // 70
        858,   // 71
        848,   // 72
        838,   // 73
        828,   // 74
        817,   // 75
        806,   // 76
        795,   // 77
        784,   // 78
        772,   // 79
        760,   // 80
        748,   // 81
        736,   // 82
        724,   // 83
        711,   // 84
        698,   // 85
        685,   // 86
        671,   // 87
        658,   // 88
        644,   // 89
        630,   // 90
        616,   // 91
        601,   // 92
        587,   // 93
        572,   // 94
        557,   // 95
        542,   // 96
        527,   // 97
        511,   // 98
        496,   // 99
        480,   // 100
        464,   // 101
        448,   // 102
        432,   // 103
        416,   // 104
        400,   // 105
        383,   // 106
        366,   // 107
        350,   // 108
        333,   // 109
        316,   // 110
        299,   // 111
        282,   // 112
        265,   // 113
        247,   // 114
        230,   // 115
        212,   // 116
        195,   // 117
        177,   // 118
        160,   // 119
        142,   // 120
        124,   // 121
        107,   // 122
        89,    // 123
        71,    // 124
        53,    // 125
        35,    // 126
        17,    // 127
        0,     // 128
        -17,   // 129
        -35,   // 130
        -53,   // 131
        -71,   // 132
        -89,   // 133
        -107,  // 134
        -124,  // 135
        -142,  // 136
        -160,  // 137
        -177,  // 138
        -195,  // 139
        -212,  // 140
        -230,  // 141
        -247,  // 142
        -265,  // 143
        -282,  // 144
        -299,  // 145
        -316,  // 146
        -333,  // 147
        -350,  // 148
        -366,  // 149
        -383,  // 150
        -400,  // 151
        -416,  // 152
        -432,  // 153
        -448,  // 154
        -464,  // 155
        -480,  // 156
        -496,  // 157
        -511,  // 158
        -527,  // 159
        -542,  // 160
        -557,  // 161
        -572,  // 162
        -587,  // 163
        -601,  // 164
        -616,  // 165
        -630,  // 166
        -644,  // 167
        -658,  // 168
        -671,  // 169
        -685,  // 170
        -698,  // 171
        -711,  // 172
        -724,  // 173
        -736,  // 174
        -748,  // 175
        -760,  // 176
        -772,  // 177
        -784,  // 178
        -795,  // 179
        -806,  // 180
        -817,  // 181
        -828,  // 182
        -838,  // 183
        -848,  // 184
        -858,  // 185
        -868,  // 186
        -877,  // 187
        -886,  // 188
        -895,  // 189
        -904,  // 190
        -912,  // 191
        -920,  // 192
        -928,  // 193
        -935,  // 194
        -942,  // 195
        -949,  // 196
        -955,  // 197
        -962,  // 198
        -968,  // 199
        -973,  // 200
        -979,  // 201
        -984,  // 202
        -989,  // 203
        -993,  // 204
        -997,  // 205
        -1001, // 206
        -1005, // 207
        -1008, // 208
        -1011, // 209
        -1014, // 210
        -1016, // 211
        -1018, // 212
        -1020, // 213
        -1021, // 214
        -1022, // 215
        -1023, // 216
        -1023, // 217
        -1023, // 218
        -1023, // 219
        -1023, // 220
        -1022, // 221
        -1021, // 222
        -1020, // 223
        -1018, // 224
        -1016, // 225
        -1014, // 226
        -1011, // 227
        -1008, // 228
        -1005, // 229
        -1001, // 230
        -997,  // 231
        -993,  // 232
        -989,  // 233
        -984,  // 234
        -979,  // 235
        -973,  // 236
        -968,  // 237
        -962,  // 238
        -955,  // 239
        -949,  // 240
        -942,  // 241
        -935,  // 242
        -928,  // 243
        -920,  // 244
        -912,  // 245
        -904,  // 246
        -895,  // 247
        -886,  // 248
        -877,  // 249
        -868,  // 250
        -858,  // 251
        -848,  // 252
        -838,  // 253
        -828,  // 254
        -817,  // 255
        -806,  // 256
    };

    if (b256Step)
    {
        if (u16Hue > 256)
        {
            u16Hue = 256;
        }
        return tHueToSine256[u16Hue];
    }
    if (!b256Step)
    {
        if (u16Hue > 100)
        {
            u16Hue = 100;
        }
        return tHueToSine100[u16Hue];
    }
    return 0;
}

static s16 _HalDispColorCalCosine(u16 u16Hue, u8 b256Step)
{
    // cos((u8Hue * 3.1415926)/180)) * 1024
    s16 tHueToCosine100[101] = {
        658,  // 0
        671,  // 1
        685,  // 2
        698,  // 3
        711,  // 4
        724,  // 5
        736,  // 6
        748,  // 7
        760,  // 8
        772,  // 9
        784,  // 10
        795,  // 11
        806,  // 12
        817,  // 13
        828,  // 14
        838,  // 15
        848,  // 16
        858,  // 17
        868,  // 18
        877,  // 19
        886,  // 20
        895,  // 21
        904,  // 22
        912,  // 23
        920,  // 24
        928,  // 25
        935,  // 26
        942,  // 27
        949,  // 28
        955,  // 29
        962,  // 30
        968,  // 31
        973,  // 32
        979,  // 33
        984,  // 34
        989,  // 35
        993,  // 36
        997,  // 37
        1001, // 38
        1005, // 39
        1008, // 40
        1011, // 41
        1014, // 42
        1016, // 43
        1018, // 44
        1020, // 45
        1021, // 46
        1022, // 47
        1023, // 48
        1023, // 49
        1024, // 50
        1023, // 51
        1023, // 52
        1022, // 53
        1021, // 54
        1020, // 55
        1018, // 56
        1016, // 57
        1014, // 58
        1011, // 59
        1008, // 60
        1005, // 61
        1001, // 62
        997,  // 63
        993,  // 64
        989,  // 65
        984,  // 66
        979,  // 67
        973,  // 68
        968,  // 69
        962,  // 70
        955,  // 71
        949,  // 72
        942,  // 73
        935,  // 74
        928,  // 75
        920,  // 76
        912,  // 77
        904,  // 78
        895,  // 79
        886,  // 80
        877,  // 81
        868,  // 82
        858,  // 83
        848,  // 84
        838,  // 85
        828,  // 86
        817,  // 87
        806,  // 88
        795,  // 89
        784,  // 90
        772,  // 91
        760,  // 92
        748,  // 93
        736,  // 94
        724,  // 95
        711,  // 96
        698,  // 97
        685,  // 98
        671,  // 99
        658,  // 100
    };

    s16 tHueToCosine256[257] = {
        -630, // 0
        -616, // 1
        -601, // 2
        -587, // 3
        -572, // 4
        -557, // 5
        -542, // 6
        -527, // 7
        -511, // 8
        -496, // 9
        -480, // 10
        -464, // 11
        -448, // 12
        -432, // 13
        -416, // 14
        -400, // 15
        -383, // 16
        -366, // 17
        -350, // 18
        -333, // 19
        -316, // 20
        -299, // 21
        -282, // 22
        -265, // 23
        -247, // 24
        -230, // 25
        -212, // 26
        -195, // 27
        -177, // 28
        -160, // 29
        -142, // 30
        -124, // 31
        -107, // 32
        -89,  // 33
        -71,  // 34
        -53,  // 35
        -35,  // 36
        -17,  // 37
        0,    // 38
        17,   // 39
        35,   // 40
        53,   // 41
        71,   // 42
        89,   // 43
        107,  // 44
        124,  // 45
        142,  // 46
        160,  // 47
        177,  // 48
        195,  // 49
        212,  // 50
        230,  // 51
        247,  // 52
        265,  // 53
        282,  // 54
        299,  // 55
        316,  // 56
        333,  // 57
        350,  // 58
        366,  // 59
        383,  // 60
        400,  // 61
        416,  // 62
        432,  // 63
        448,  // 64
        464,  // 65
        480,  // 66
        496,  // 67
        512,  // 68
        527,  // 69
        542,  // 70
        557,  // 71
        572,  // 72
        587,  // 73
        601,  // 74
        616,  // 75
        630,  // 76
        644,  // 77
        658,  // 78
        671,  // 79
        685,  // 80
        698,  // 81
        711,  // 82
        724,  // 83
        736,  // 84
        748,  // 85
        760,  // 86
        772,  // 87
        784,  // 88
        795,  // 89
        806,  // 90
        817,  // 91
        828,  // 92
        838,  // 93
        848,  // 94
        858,  // 95
        868,  // 96
        877,  // 97
        886,  // 98
        895,  // 99
        904,  // 100
        912,  // 101
        920,  // 102
        928,  // 103
        935,  // 104
        942,  // 105
        949,  // 106
        955,  // 107
        962,  // 108
        968,  // 109
        973,  // 110
        979,  // 111
        984,  // 112
        989,  // 113
        993,  // 114
        997,  // 115
        1001, // 116
        1005, // 117
        1008, // 118
        1011, // 119
        1014, // 120
        1016, // 121
        1018, // 122
        1020, // 123
        1021, // 124
        1022, // 125
        1023, // 126
        1023, // 127
        1024, // 128
        1023, // 129
        1023, // 130
        1022, // 131
        1021, // 132
        1020, // 133
        1018, // 134
        1016, // 135
        1014, // 136
        1011, // 137
        1008, // 138
        1005, // 139
        1001, // 140
        997,  // 141
        993,  // 142
        989,  // 143
        984,  // 144
        979,  // 145
        973,  // 146
        968,  // 147
        962,  // 148
        955,  // 149
        949,  // 150
        942,  // 151
        935,  // 152
        928,  // 153
        920,  // 154
        912,  // 155
        904,  // 156
        895,  // 157
        886,  // 158
        877,  // 159
        868,  // 160
        858,  // 161
        848,  // 162
        838,  // 163
        828,  // 164
        817,  // 165
        806,  // 166
        795,  // 167
        784,  // 168
        772,  // 169
        760,  // 170
        748,  // 171
        736,  // 172
        724,  // 173
        711,  // 174
        698,  // 175
        685,  // 176
        671,  // 177
        658,  // 178
        644,  // 179
        630,  // 180
        616,  // 181
        601,  // 182
        587,  // 183
        572,  // 184
        557,  // 185
        542,  // 186
        527,  // 187
        512,  // 188
        496,  // 189
        480,  // 190
        464,  // 191
        448,  // 192
        432,  // 193
        416,  // 194
        400,  // 195
        383,  // 196
        366,  // 197
        350,  // 198
        333,  // 199
        316,  // 200
        299,  // 201
        282,  // 202
        265,  // 203
        247,  // 204
        230,  // 205
        212,  // 206
        195,  // 207
        177,  // 208
        160,  // 209
        142,  // 210
        124,  // 211
        107,  // 212
        89,   // 213
        71,   // 214
        53,   // 215
        35,   // 216
        17,   // 217
        0,    // 218
        -17,  // 219
        -35,  // 220
        -53,  // 221
        -71,  // 222
        -89,  // 223
        -107, // 224
        -124, // 225
        -142, // 226
        -160, // 227
        -177, // 228
        -195, // 229
        -212, // 230
        -230, // 231
        -247, // 232
        -265, // 233
        -282, // 234
        -299, // 235
        -316, // 236
        -333, // 237
        -350, // 238
        -366, // 239
        -383, // 240
        -400, // 241
        -416, // 242
        -432, // 243
        -448, // 244
        -464, // 245
        -480, // 246
        -496, // 247
        -511, // 248
        -527, // 249
        -542, // 250
        -557, // 251
        -572, // 252
        -587, // 253
        -601, // 254
        -616, // 255
        -630, // 256
    };

    if (b256Step)
    {
        if (u16Hue > 256)
        {
            u16Hue = 256;
        }
        return tHueToCosine256[u16Hue];
    }
    if (!b256Step)
    {
        if (u16Hue > 100)
        {
            u16Hue = 100;
        }
        return tHueToCosine100[u16Hue];
    }
    return 0;
}
static void _HalDispColorArrayMultiply(s16 sFirst[3][3], s16 sSecond[3][3], s16 sResult[3][3])
{
    u8 u8Row, u8Col;

    // go line by line
    for (u8Row = 0; u8Row != 3; u8Row++)
    {
        // go column by column
        for (u8Col = 0; u8Col != 3; u8Col++)
        {
            sResult[u8Row][u8Col] =
                (((u32)sFirst[u8Row][0] * sSecond[0][u8Col]) + ((u32)sFirst[u8Row][1] * sSecond[1][u8Col])
                 + ((u32)sFirst[u8Row][2] * sSecond[2][u8Col]))
                >> 10;
        } // for
    }     // for
}

static HalDispColorConfig_t *_HalDispColorGetColorConfig(u8 u8Id, DrvDispCtxConfig_t *pstDispCtx)
{
    HalDispColorConfig_t *     pstColorCfg;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];
    if (pstDevContain->u32DevId < HAL_DISP_DEVICE_MAX && u8Id < E_DISP_COLOR_CSC_ID_NUM)
    {
        pstColorCfg = &g_tColorCfg[pstDevContain->u32DevId][u8Id];
    }
    else
    {
        pstColorCfg = NULL;
        DISP_ERR("%s %d, CtxType is not Device, path:%d\n", __FUNCTION__, __LINE__, (pstDevContain->u32DevId));
    }
    return pstColorCfg;
}

static void _HalDispColorSetVideoSatHueMatrix(u8 u8Id, DrvDispCtxConfig_t *pstDispCtx)
{
    u16                   u16Hue;
    HalDispColorConfig_t *pstColorCfg = _HalDispColorGetColorConfig(u8Id, pstDispCtx);
    void *                pCmdqCtx    = NULL;
#if (ENABLE_CBCR == 0)
    s16 s16Tmp;
#endif
    u8 b256Step = (pstColorCfg->u8ACEConfig & ACE_ENABLE_HUE_256_STEP) ? 1 : 0;

    pstColorCfg = _HalDispColorGetColorConfig(u8Id, pstDispCtx);
    HalDispGetCmdqByCtx((void *)pstDispCtx, &pCmdqCtx);
    /*
        if( pstColorCfg->u8ACEConfig & ACE_ENABLE_HUE_256_STEP )
        {
            u16Hue = ((pstColorCfg->u8VideoHue <= 0x80) ? (0x80 - pstColorCfg->u8VideoHue) :
       (360-(pstColorCfg->u8VideoHue-0x80)));
        }
        else
        {
            u16Hue = ((pstColorCfg->u8VideoHue <= 50) ? (50 - pstColorCfg->u8VideoHue) :
       (360-(pstColorCfg->u8VideoHue-50)));
        }
    */
    u16Hue = pstColorCfg->u8VideoHue;
#if (ENABLE_CBCR)
    pstColorCfg->sVideoSatHueMatrix[2][2] =
        ((((u32)_HalDispColorCalCosine(u16Hue, b256Step) * pstColorCfg->u8VideoSaturation * pstColorCfg->u8VideoCb))
         >> 14);
    pstColorCfg->sVideoSatHueMatrix[0][0] =
        ((((u32)_HalDispColorCalCosine(u16Hue, b256Step) * pstColorCfg->u8VideoSaturation * pstColorCfg->u8VideoCr))
         >> 14);
    pstColorCfg->sVideoSatHueMatrix[2][0] =
        ((((u32)_HalDispColorCalSine(u16Hue, b256Step) * pstColorCfg->u8VideoSaturation * pstColorCfg->u8VideoCr))
         >> 14);
    pstColorCfg->sVideoSatHueMatrix[0][2] =
        (((-(u32)_HalDispColorCalSine(u16Hue, b256Step) * pstColorCfg->u8VideoSaturation * pstColorCfg->u8VideoCb))
         >> 14);

#else
    s16Tmp                                = ((s16)pstColorCfg->u8VideoSaturation * 8);
    pstColorCfg->sVideoSatHueMatrix[2][2] = ((((u32)_HalDispColorCalCosine(u16Hue, b256Step) * s16Tmp)) >> 10);
    pstColorCfg->sVideoSatHueMatrix[0][0] = ((((u32)_HalDispColorCalCosine(u16Hue, b256Step) * s16Tmp)) >> 10);
    pstColorCfg->sVideoSatHueMatrix[2][0] = ((((u32)_HalDispColorCalSine(u16Hue, b256Step) * s16Tmp)) >> 10);
    pstColorCfg->sVideoSatHueMatrix[0][2] = (((-(u32)_HalDispColorCalSine(u16Hue, b256Step) * s16Tmp)) >> 10);

#endif

    pstColorCfg->sVideoSatHueMatrix[1][1]     = 1024;
    pstColorCfg->sVideoSatHueMatrix[0][1]     = pstColorCfg->sVideoSatHueMatrix[1][0] =
        pstColorCfg->sVideoSatHueMatrix[1][2] = pstColorCfg->sVideoSatHueMatrix[2][1] = 0;

    // HalDispUtilityW2BYTEMSK(REG_DISP_MOPG_BK01_0F_L, ((u16)pstColorCfg->u8VideoHue) << 8, 0xFF00, pCmdqCtx);
    // HalDispUtilityW2BYTEMSK(REG_DISP_MOPG_BK01_1F_L, ((u16)pstColorCfg->u8VideoSaturation) << 8, 0xFF00, pCmdqCtx);
}

static void _HalDispColorSetVideoContrastMatrix(u8 u8Id, DrvDispCtxConfig_t *pstDispCtx)
{
    HalDispColorConfig_t *pstColorCfg = NULL;
    void *                pCmdqCtx    = NULL;

    pstColorCfg = _HalDispColorGetColorConfig(u8Id, pstDispCtx);
    HalDispGetCmdqByCtx((void *)pstDispCtx, &pCmdqCtx);

    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d: DevId:%d, Id:%d, Con=%d, RCon=%d, GCon=%d BCon=%d\n", __FUNCTION__, __LINE__,
             HalDispGetDeviceId(pstDispCtx, 0), u8Id, pstColorCfg->u8VideoContrast,
             pstColorCfg->u8VideoRCon, pstColorCfg->u8VideoGCon, pstColorCfg->u8VideoBCon);

    pstColorCfg->sVideoContrastMatrix[0][0] = ((u16)pstColorCfg->u8VideoRCon * (pstColorCfg->u8VideoContrast)) >> 4;
    pstColorCfg->sVideoContrastMatrix[1][1] = ((u16)pstColorCfg->u8VideoGCon * (pstColorCfg->u8VideoContrast)) >> 4;
    pstColorCfg->sVideoContrastMatrix[2][2] = ((u16)pstColorCfg->u8VideoBCon * (pstColorCfg->u8VideoContrast)) >> 4;

    // HalDispUtilityW2BYTEMSK(REG_DISP_MOPG_BK01_2F_L, ((u16)pstColorCfg->u8VideoRCon)<<8, 0xFF00, pCmdqCtx);
    // HalDispUtilityW2BYTEMSK(REG_DISP_MOPG_BK01_3F_L, ((u16)pstColorCfg->u8VideoGCon)<<8, 0xFF00, pCmdqCtx);
    // HalDispUtilityW2BYTEMSK(REG_DISP_MOPG_BK01_4F_L, ((u16)pstColorCfg->u8VideoBCon)<<8, 0xFF00, pCmdqCtx);
    // HalDispUtilityW2BYTEMSK(REG_DISP_MOPG_BK01_5F_L, ((u16)pstColorCfg->u8VideoContrast)<<8, 0xFF00, pCmdqCtx);
}

static void _HalDispColorWriteColorMatrix(u8 u8Id, s16 *psMatrix, DrvDispCtxConfig_t *pstDispCtx)
{
    void *pCmdqCtx = NULL;
    u8    i, j;
    u32   u32Addr;
    s16   s16Tmp;
    u32   u32DevId = HalDispGetDeviceId(pstDispCtx, 0);

    if (HalDispGetCmdqByCtx((void *)pstDispCtx, &pCmdqCtx) == 0)
    {
        return;
    }

    if (u8Id == E_DISP_COLOR_CSC_ID_0)
    {
        u32Addr = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_CM3X3_COEF11);
    }
    else
    {
        u32Addr = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_CM3X3_2_COEF11);
    }

    if (pCmdqCtx && u32Addr)
    {
        for (i = 0; i != 3; i++)
        {
            for (j = 0; j != 3; j++)
            {
                s16Tmp = psMatrix[i * 3 + j];
                if (s16Tmp >= 0)
                {
                    if (s16Tmp > 0xfff)
                    {
                        s16Tmp = 0xfff;
                    }
                }
                else
                {
                    s16Tmp = s16Tmp * -1;
                    if (s16Tmp > 0xfff)
                    {
                        s16Tmp = 0xfff;
                    }
                    s16Tmp |= 0x1000;
                }
                HalDispUtilityW2BYTEMSK(u32Addr, s16Tmp, mask_of_disp_top_op20_reg_cm3x3_coef11, pCmdqCtx);
                u32Addr += 2;
            } // for
        }     // for
    }
}
static void _HalDispColorSetVideoColorMatrix(u8 u8Id, DrvDispCtxConfig_t *pstDispCtx)
{
    HalDispColorConfig_t *pstColorCfg = NULL;
    s16                   sResultTmp1[3][3];
    s16                   sResultTmp2[3][3];
    s16 *                 psOutTab = NULL;
    u32                   u32DevId = 0;

    if (pstDispCtx && pstDispCtx->pstCtxContain)
    {
        u32DevId = HalDispGetDeviceId(pstDispCtx, 0);
    }
    pstColorCfg = _HalDispColorGetColorConfig(u8Id, pstDispCtx);

    // Adjust hue&saturation, and then YUV to RGB
    _HalDispColorArrayMultiply(pstColorCfg->sYuvToRGBMatrix, pstColorCfg->sVideoSatHueMatrix, sResultTmp1);

    // Adjust contrast-RGB
    _HalDispColorArrayMultiply(pstColorCfg->sVideoContrastMatrix, sResultTmp1, sResultTmp2);

    // Do color correction
    if (pstColorCfg->bColorCorrectMatrixUpdate)
    {
        _HalDispColorArrayMultiply(pstColorCfg->sColorCorrrectMatrix, sResultTmp2, sResultTmp1);
        psOutTab = (s16 *)sResultTmp1;
    }

    if (psOutTab)
    {
        _HalDispColorWriteColorMatrix(u8Id, psOutTab, pstDispCtx);
    }
    HalDispColorSetColorMatrixMd(u8Id, pstColorCfg->enMatrixType, u32DevId, pstDispCtx);
    HalDispColorSetColorMatrixEn(u8Id, 1, u32DevId, pstDispCtx);
}

static void _HalDispColorSetBrightnessEn(u8 u8Id, u8 bEn, DrvDispCtxConfig_t *pstDispCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg;
    u32   u32DevId = HalDispGetDeviceId(pstDispCtx, 0);

    if (HalDispGetCmdqByCtx((void *)pstDispCtx, &pCmdqCtx) == 0)
    {
        return;
    }
    if (u8Id == E_DISP_COLOR_CSC_ID_0)
    {
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_BRI_EN);
        if (pCmdqCtx && u32Reg)
        {
            HalDispUtilityW2BYTEMSK(u32Reg, bEn ? mask_of_disp_top_op20_reg_bri_en : 0x0000,
                                    mask_of_disp_top_op20_reg_bri_en, pCmdqCtx);
        }
    }
    else
    {
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_BRI_2_EN);
        if (pCmdqCtx && u32Reg)
        {
            HalDispUtilityW2BYTEMSK(u32Reg, bEn ? mask_of_disp_top_op20_reg_bri_2_en : 0x0000,
                                    mask_of_disp_top_op20_reg_bri_2_en, pCmdqCtx);
        }
    }
}

static void _HalDispColorSetBrightness(u8 u8Id, u8 u8BrightnessR, u8 u8BrightnessG, u8 u8BrightnessB,
                                       DrvDispCtxConfig_t *pstDispCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pstDispCtx, 0);

    if (HalDispGetCmdqByCtx((void *)pstDispCtx, &pCmdqCtx) == 0)
    {
        return;
    }
    if (u8Id == E_DISP_COLOR_CSC_ID_0)
    {
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_BRI_R);
        if (pCmdqCtx && u32Reg)
        {
            HalDispUtilityW2BYTEMSK(u32Reg, ((u16)u8BrightnessR) << shift_of_disp_top_op20_reg_bri_r,
                                    mask_of_disp_top_op20_reg_bri_r, pCmdqCtx);
        }
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_BRI_G);
        if (pCmdqCtx && u32Reg)
        {
            HalDispUtilityW2BYTEMSK(u32Reg, ((u16)u8BrightnessG) << shift_of_disp_top_op20_reg_bri_g,
                                    mask_of_disp_top_op20_reg_bri_g, pCmdqCtx);
        }
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_BRI_B);
        if (pCmdqCtx && u32Reg)
        {
            HalDispUtilityW2BYTEMSK(u32Reg, ((u16)u8BrightnessB) << shift_of_disp_top_op20_reg_bri_b,
                                    mask_of_disp_top_op20_reg_bri_b, pCmdqCtx);
        }
    }
    else
    {
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_BRI_2_R);
        if (pCmdqCtx && u32Reg)
        {
            HalDispUtilityW2BYTEMSK(u32Reg, ((u16)u8BrightnessR) << shift_of_disp_top_op20_reg_bri_2_r,
                                    mask_of_disp_top_op20_reg_bri_2_r, pCmdqCtx);
        }
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_BRI_2_G);
        if (pCmdqCtx && u32Reg)
        {
            HalDispUtilityW2BYTEMSK(u32Reg, ((u16)u8BrightnessG) << shift_of_disp_top_op20_reg_bri_2_g,
                                    mask_of_disp_top_op20_reg_bri_2_g, pCmdqCtx);
        }
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_BRI_2_B);
        if (pCmdqCtx && u32Reg)
        {
            HalDispUtilityW2BYTEMSK(u32Reg, ((u16)u8BrightnessB) << shift_of_disp_top_op20_reg_bri_2_b,
                                    mask_of_disp_top_op20_reg_bri_2_b, pCmdqCtx);
        }
    }
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

void HalDispColorSetColorMatrixEn(u8 u8Id, u8 bEn, u32 u32DevId, void *pstDispCtx)
{
    u32                 u32CtrlReg;
    DrvDispCtxConfig_t *pCtx     = pstDispCtx;
    void *              pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pstDispCtx, &pCmdqCtx);
    if (pCtx && pCtx->pstCtxContain)
    {
        u32DevId = HalDispGetDeviceId(pCtx, 0);
    }
    if (u8Id == E_DISP_COLOR_CSC_ID_0)
    {
        u32CtrlReg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_CM3X3_EN);
    }
    else
    {
        u32CtrlReg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_CM3X3_2_EN);
    }
    if (u32CtrlReg == 0)
    {
        return;
    }
    HalDispUtilityW2BYTEMSK(u32CtrlReg, bEn ? mask_of_disp_top_op20_reg_cm3x3_en : 0,
                            mask_of_disp_top_op20_reg_cm3x3_en, pCmdqCtx);
}
void HalDispColorSetColorMatrixMd(u8 u8Id, HalDispColorYuvToRgbMatrixType_e enMatrixType, u32 u32DevId,
                                  void *pstDispCtx)
{
    u32                 u32CtrlReg;
    DrvDispCtxConfig_t *pCtx   = pstDispCtx;
    u16                 u16Msk = mask_of_disp_top_op20_reg_cm3x3_in_range_1 | mask_of_disp_top_op20_reg_cm3x3_in_range_2
                 | mask_of_disp_top_op20_reg_cm3x3_in_range_3;
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pstDispCtx, &pCmdqCtx);
    if (pCtx && pCtx->pstCtxContain)
    {
        u32DevId = HalDispGetDeviceId(pCtx, 0);
    }
    if (u8Id == E_DISP_COLOR_CSC_ID_0)
    {
        u32CtrlReg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_CM3X3_EN);
    }
    else
    {
        u32CtrlReg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_CM3X3_2_EN);
    }
    if (u32CtrlReg == 0)
    {
        return;
    }
    if (enMatrixType == E_DISP_COLOR_YUV_2_RGB_MATRIX_BYPASS)
    {
        HalDispUtilityW2BYTEMSK(u32CtrlReg, 0, u16Msk, pCmdqCtx);
    }
    else
    {
        HalDispUtilityW2BYTEMSK(u32CtrlReg,
                                mask_of_disp_top_op20_reg_cm3x3_in_range_1 | mask_of_disp_top_op20_reg_cm3x3_in_range_3,
                                u16Msk, pCmdqCtx);
    }
}

u8 HalDispColorInitVar(void *pCtx)
{
    // For video
    u8                    i;
    static u8             bInit[HAL_DISP_DEVICE_MAX] = {FALSE, FALSE, FALSE};
    DrvDispCtxConfig_t *  pstDispCtx                 = (DrvDispCtxConfig_t *)pCtx;
    HalDispColorConfig_t *pstColorCfg                = NULL;
    u8                    bRet                       = 1;

    if (pstDispCtx && pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        if (bInit[pstDispCtx->u32ContainIdx] == FALSE)
        {
            for (i = 0; i < E_DISP_COLOR_CSC_ID_NUM; i++)
            {
                pstColorCfg = _HalDispColorGetColorConfig(i, pstDispCtx);

                pstColorCfg->u8VideoRCon       = 0x80;
                pstColorCfg->u8VideoGCon       = 0x80;
                pstColorCfg->u8VideoBCon       = 0x80;
                pstColorCfg->u8VideoContrast   = 0x80;
                pstColorCfg->u8VideoSaturation = 0x80;
#if (ENABLE_CBCR)
                pstColorCfg->u8VideoCb = 0x80;
                pstColorCfg->u8VideoCr = 0x80;
#endif
                pstColorCfg->u8VideoHue = 50;

                pstColorCfg->u8BrightnessR = 0x80;
                pstColorCfg->u8BrightnessG = 0x80;
                pstColorCfg->u8BrightnessB = 0x80;

                pstColorCfg->sVideoSatHueMatrix[0][0]         = pstColorCfg->sVideoSatHueMatrix[1][1] =
                    pstColorCfg->sVideoSatHueMatrix[2][2]     = 1024;
                pstColorCfg->sVideoSatHueMatrix[0][1]         = pstColorCfg->sVideoSatHueMatrix[1][0] =
                    pstColorCfg->sVideoSatHueMatrix[2][0]     = pstColorCfg->sVideoSatHueMatrix[0][2] =
                        pstColorCfg->sVideoSatHueMatrix[1][2] = pstColorCfg->sVideoSatHueMatrix[2][1] = 0;

                pstColorCfg->sVideoContrastMatrix[0][0]         = pstColorCfg->sVideoContrastMatrix[1][1] =
                    pstColorCfg->sVideoContrastMatrix[2][2]     = 1024;
                pstColorCfg->sVideoContrastMatrix[0][1]         = pstColorCfg->sVideoContrastMatrix[1][0] =
                    pstColorCfg->sVideoContrastMatrix[2][0]     = pstColorCfg->sVideoContrastMatrix[0][2] =
                        pstColorCfg->sVideoContrastMatrix[1][2] = pstColorCfg->sVideoContrastMatrix[2][1] = 0;

                pstColorCfg->enMatrixType                  = E_DISP_COLOR_YUV_2_RGB_MATRIX_BYPASS;
                pstColorCfg->sYuvToRGBMatrix[0][0]         = pstColorCfg->sYuvToRGBMatrix[1][1] =
                    pstColorCfg->sYuvToRGBMatrix[2][2]     = 1024;
                pstColorCfg->sYuvToRGBMatrix[0][1]         = pstColorCfg->sYuvToRGBMatrix[1][0] =
                    pstColorCfg->sYuvToRGBMatrix[2][0]     = pstColorCfg->sYuvToRGBMatrix[0][2] =
                        pstColorCfg->sYuvToRGBMatrix[1][2] = pstColorCfg->sYuvToRGBMatrix[2][1] = 0;

                pstColorCfg->sColorCorrrectMatrix[0][0]         = pstColorCfg->sColorCorrrectMatrix[1][1] =
                    pstColorCfg->sColorCorrrectMatrix[2][2]     = 1024;
                pstColorCfg->sColorCorrrectMatrix[0][1]         = pstColorCfg->sColorCorrrectMatrix[1][0] =
                    pstColorCfg->sColorCorrrectMatrix[2][0]     = pstColorCfg->sColorCorrrectMatrix[0][2] =
                        pstColorCfg->sColorCorrrectMatrix[1][2] = pstColorCfg->sColorCorrrectMatrix[2][1] = 0;

                // For PC
                pstColorCfg->u8PCRCon     = 0x80;
                pstColorCfg->u8PCGCon     = 0x80;
                pstColorCfg->u8PCBCon     = 0x80;
                pstColorCfg->u8PCContrast = 0x80;

                pstColorCfg->sPCConRGBMatrix[0][0]         = pstColorCfg->sPCConRGBMatrix[1][1] =
                    pstColorCfg->sPCConRGBMatrix[2][2]     = 1024;
                pstColorCfg->sPCConRGBMatrix[0][1]         = pstColorCfg->sPCConRGBMatrix[1][0] =
                    pstColorCfg->sPCConRGBMatrix[2][0]     = pstColorCfg->sPCConRGBMatrix[0][2] =
                        pstColorCfg->sPCConRGBMatrix[1][2] = pstColorCfg->sPCConRGBMatrix[2][1] = 0;

                // PC sRGB matrix
                pstColorCfg->tSrgbMatrix = NULL;

                // Color correction matrix for Video
                pstColorCfg->bColorCorrectMatrixUpdate = 0;

                pstColorCfg->u8ACEConfig = 0;
            }
            bInit[pstDispCtx->u32ContainIdx] = TRUE;
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType is not Device, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}

u8 HalDispColorSetColorCorrectMatrix(u8 u8Id, s16 *psColorCorrectMatrix, void *pCtx)
{
    u8                    i, j;
    DrvDispCtxConfig_t *  pstDispCtx  = (DrvDispCtxConfig_t *)pCtx;
    HalDispColorConfig_t *pstColorCfg = NULL;
    u8                    bRet        = 1;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        pstColorCfg = _HalDispColorGetColorConfig(u8Id, pstDispCtx);

        if (psColorCorrectMatrix)
        {
            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                {
                    pstColorCfg->sColorCorrrectMatrix[i][j] = *(psColorCorrectMatrix + (i * 3) + j);
                }
            }

            pstColorCfg->bColorCorrectMatrixUpdate = 1;
        }

        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d: DevId:%d Id:%d, \n %d %d %d \n %d %d %d \n %d %d %d \n", __FUNCTION__,
                 __LINE__, HalDispGetDeviceId(pstDispCtx, 0), u8Id,
                 pstColorCfg->sColorCorrrectMatrix[0][0], pstColorCfg->sColorCorrrectMatrix[0][1],
                 pstColorCfg->sColorCorrrectMatrix[0][2], pstColorCfg->sColorCorrrectMatrix[1][0],
                 pstColorCfg->sColorCorrrectMatrix[1][1], pstColorCfg->sColorCorrrectMatrix[1][2],
                 pstColorCfg->sColorCorrrectMatrix[2][0], pstColorCfg->sColorCorrrectMatrix[2][1],
                 pstColorCfg->sColorCorrrectMatrix[2][2]);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType is not Device, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}

u8 HalDispColorSeletYuvToRgbMatrix(u8 u8Id, HalDispColorYuvToRgbMatrixType_e enType, s16 *psYuv2RgbMatrix, void *pCtx)
{
    s16 *                 psMatrix = NULL;
    u8                    i, j;
    DrvDispCtxConfig_t *  pstDispCtx  = (DrvDispCtxConfig_t *)pCtx;
    HalDispColorConfig_t *pstColorCfg = NULL;
    u8                    bRet        = 1;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        pstColorCfg = _HalDispColorGetColorConfig(u8Id, pstDispCtx);

        if (enType == E_DISP_COLOR_YUV_2_RGB_MATRIX_USER)
        {
            psMatrix = psYuv2RgbMatrix;
        }
        else if (enType == E_DISP_COLOR_YUV_2_RGB_MATRIX_SDTV)
        {
            psMatrix = &g_stSDTVYuv2rgb[0][0];
        }
        else if (enType == E_DISP_COLOR_YUV_2_RGB_MATRIX_BYPASS)
        {
            psMatrix = &g_stByPassYuv2rgb[0][0];
        }
        else if (enType == E_DISP_COLOR_YUV_2_RGB_MATRIX_HDTV)
        {
            psMatrix = &g_stHDTVYuv2rgb[0][0];
        }
        else
        {
            DISP_ERR("%s %d, MatrixType(%d) is not correct\n", __FUNCTION__, __LINE__, enType);
            bRet = 0;
        }

        if (psMatrix)
        {
            pstColorCfg->enMatrixType = enType;

            for (i = 0; i < 3; i++)
            {
                for (j = 0; j < 3; j++)
                {
                    pstColorCfg->sYuvToRGBMatrix[i][j] = *(psMatrix + (i * 3) + j);
                }
            }
        }

        DISP_DBG(
            DISP_DBG_LEVEL_COLOR, "%s %d: DevId:%d Id:%d, \n %04x %04x %04x \n %04x %04x %04x \n %04x %04x %04x \n",
            __FUNCTION__, __LINE__, HalDispGetDeviceId(pstDispCtx, 0), u8Id,
            pstColorCfg->sYuvToRGBMatrix[0][0], pstColorCfg->sYuvToRGBMatrix[0][1], pstColorCfg->sYuvToRGBMatrix[0][2],
            pstColorCfg->sYuvToRGBMatrix[1][0], pstColorCfg->sYuvToRGBMatrix[1][1], pstColorCfg->sYuvToRGBMatrix[1][2],
            pstColorCfg->sYuvToRGBMatrix[2][0], pstColorCfg->sYuvToRGBMatrix[2][1], pstColorCfg->sYuvToRGBMatrix[2][2]);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType is not Device, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}

u8 HalDispColorAdjustBrightness(u8 u8Id, u8 u8BrightnessR, u8 u8BrightnessG, u8 u8BrightnessB, void *pCtx)
{
    DrvDispCtxConfig_t *  pstDispCtx  = (DrvDispCtxConfig_t *)pCtx;
    HalDispColorConfig_t *pstColorCfg = NULL;
    u8                    bRet        = 1;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        pstColorCfg = _HalDispColorGetColorConfig(u8Id, pstDispCtx);

        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d: DevId:%d, Id:%d, Brightness=%d %d %d\n", __FUNCTION__, __LINE__,
                 HalDispGetDeviceId(pstDispCtx, 0), u8Id, u8BrightnessR,
                 u8BrightnessG, u8BrightnessB);

        pstColorCfg->u8BrightnessR = u8BrightnessR;
        pstColorCfg->u8BrightnessG = u8BrightnessG;
        pstColorCfg->u8BrightnessB = u8BrightnessB;

        _HalDispColorSetBrightnessEn(u8Id, 1, pstDispCtx);
        _HalDispColorSetBrightness(u8Id, u8BrightnessR, u8BrightnessG, u8BrightnessB, pstDispCtx);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType is not Device, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}

u8 HalDispColorAdjustHCS(u8 u8Id, u8 u8Hue, u8 u8Saturation, u8 u8Contrast, void *pCtx)
{
    DrvDispCtxConfig_t *  pstDispCtx  = (DrvDispCtxConfig_t *)pCtx;
    HalDispColorConfig_t *pstColorCfg = NULL;
    u8                    bRet        = 1;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d: DevId:%d, Id:%d, Hue=%d, Sat=%d, Con=%d\n", __FUNCTION__, __LINE__,
                 HalDispGetDeviceId(pstDispCtx, 0), u8Id, u8Hue, u8Saturation,
                 u8Contrast);

        pstColorCfg                    = _HalDispColorGetColorConfig(u8Id, pstDispCtx);
        pstColorCfg->u8VideoSaturation = u8Saturation;
        pstColorCfg->u8VideoContrast   = u8Contrast;
        pstColorCfg->u8VideoHue        = u8Hue;
        _HalDispColorSetVideoSatHueMatrix(u8Id, pstDispCtx);
        _HalDispColorSetVideoContrastMatrix(u8Id, pstDispCtx);
        _HalDispColorSetVideoColorMatrix(u8Id, pstDispCtx);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType is not Device, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}

u8 HalDispColorAdjustVideoRGB(u8 u8Id, u8 u8RCon, u8 u8GCon, u8 u8BCon, void *pCtx)
{
    DrvDispCtxConfig_t *  pstDispCtx  = (DrvDispCtxConfig_t *)pCtx;
    HalDispColorConfig_t *pstColorCfg = NULL;
    u8                    bRet        = 1;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d: DevId:%d Id:%d, R=%d G=%d B=%d\n", __FUNCTION__, __LINE__,
                 HalDispGetDeviceId(pstDispCtx, 0), u8Id, u8RCon, u8GCon, u8BCon);

        pstColorCfg              = _HalDispColorGetColorConfig(u8Id, pstDispCtx);
        pstColorCfg->u8VideoRCon = u8RCon;
        pstColorCfg->u8VideoGCon = u8GCon;
        pstColorCfg->u8VideoBCon = u8BCon;
        _HalDispColorSetVideoContrastMatrix(u8Id, pstDispCtx);
        _HalDispColorSetVideoColorMatrix(u8Id, pstDispCtx);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType is not Device, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}
