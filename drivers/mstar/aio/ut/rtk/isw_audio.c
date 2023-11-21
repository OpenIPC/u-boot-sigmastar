/*
* isw_audio.c- Sigmastar
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

// Include files
/*=============================================================*/
#include <string.h>
#include "sys_sys_isw_cli.h"
#include "hal_audio_common.h"
#include "mhal_audio.h"
#include "sys_MsWrapper_cus_os_int_ctrl.h"
#include "cam_os_wrapper.h"



/*=============================================================*/
// Macro definition
/*=============================================================*/
#define DMA_BUF_SIZE (1024 * 48 * 2 * 2)
#define DUMP_BUF_SIZE (DMA_BUF_SIZE * 6)

//
#define TRACE_UT_LEVEL_TAG  1
#define ERROR_UT_LEVEL_TAG  1

#define TRACE_UT_LEVEL  "[AUD UT TRACE]"
#define ERROR_UT_LEVEL  "[AUD UT ERROR]"

#define LOG_MSG (1)
#if LOG_MSG
#define AUD_UT_PRINTF(level ,fmt, arg...)   if (level##_TAG) CamOsPrintf(level fmt, ##arg);
#else
#define AUD_UT_PRINTF(level ,fmt, arg...)
#endif



#define MIU_OFFSET 0x00000000 // 0x20000000

typedef u32 dma_addr_t;

struct dma_buffer
{
    unsigned char *area;	/* virtual pointer */
    dma_addr_t addr;	/* physical address */
    size_t bytes;		/* buffer size in bytes */
};

struct dma_buffer buf;
struct dma_buffer buf_AO;

void *pVirtPtr = NULL;
void *pMiuAddr = NULL;
void *pPhysAddr = NULL;

static u8 use_thread = 1;

typedef struct ao_info
{
    int     dev;
    int     total_size;
    u8      *pDataArray;
    int     nArrSize;
    int     nStartThres;

} _AO_Info_t;

typedef struct ai_info
{
    int     dev;
    int     total_size;
    int     channelSize;
    int     channels;
    int     tmpBufSize;
    u8      *tmpBuf;
    int     interleaved;
    int     nSpecialDumpChannels;

} _AI_Info_t;


typedef struct AUD_I2sCfg_s
{
    int nTdmMode;
    int nMsMode;
    int nBitWidth;
    int nFmt;
    int u16Channels;
    int e4WireMode;
    int eMck;
}_AUD_I2sCfg_s;

typedef struct AUD_PcmCfg_s
{
    int nRate;
    int nBitWidth;
    int n16Channels;
    int nInterleaved;
    int n32PeriodSize; //bytes
    int n32StartThres;
    int nTimeMs;
    int nI2sConfig;
    _AUD_I2sCfg_s sI2s;
    int nSpecialDumpChannels;
}_AUDPcmCfg_t;



/*=============================================================*/
// Local Variable definition && Local function prototype
/*=============================================================*/
CliParseToken_t _g_atAudDrvMenuTbl[];
static char _szAudMenuHelpTxt[] = "Audio I-SW test Menu";
static char _szAudMenuUsageTxt[] = "";

//static u32 _gnWriteTotalSize = 0;
//static u32 _gnReadTotalSize = 0;
//static Ms_Flag_t _tDmaFlag;

extern int atoi( const char * /* int_str */ );

/*=============================================================*/
// Global Variable definition
/*=============================================================*/
// 1st Sub-Menu to export
CliParseToken_t g_atAudMenuTbl[] =
{
    {{NULL, NULL}, "audio_test",      _szAudMenuHelpTxt,      _szAudMenuUsageTxt,     NULL,       _g_atAudDrvMenuTbl},
    PARSE_TOKEN_DELIMITER
};

//__align(64) static u8 _gMcuDmaBuf1[DMA_BUF_SIZE];
//__align(64) static u8 _gMcuDmaBuf2[DMA_BUF_SIZE];
//__align(64) static u8 _gMcuDmaBuf3[DMA_BUF_SIZE];
//__align(64) static u8 _gMcuDmaBuf4[DMA_BUF_SIZE*20];

extern u8 tone_48k[192];
extern u8 tone_48k_2[192];
extern u8 tone_44_1k[1764];
extern u8 tone_32k[128];
extern u8 tone_24k[96];
extern u8 tone_22_05k[1764];
extern u8 tone_16k[64];
extern u8 tone_12k[48];
extern u8 tone_11_025k[1764];
extern u8 tone_8k[32];

static u8 tone_48k_mono[] =
{
#if 1 // 1K sine , 48K sample rate
    0x00, 0x00, 0x5F, 0x08, 0x9B, 0x10, 0x8D, 0x18,
    0x13, 0x20, 0x0D, 0x27, 0x5C, 0x2D, 0xE5, 0x32,
    0x8F, 0x37, 0x44, 0x3B, 0xF7, 0x3D, 0x9A, 0x3F,
    0x27, 0x40, 0x9A, 0x3F, 0xF6, 0x3D, 0x45, 0x3B,
    0x8F, 0x37, 0xE5, 0x32, 0x5C, 0x2D, 0x0E, 0x27,
    0x13, 0x20, 0x8C, 0x18, 0x9A, 0x10, 0x60, 0x08,
    0x00, 0x00, 0xA1, 0xF7, 0x66, 0xEF, 0x73, 0xE7,
    0xED, 0xDF, 0xF3, 0xD8, 0xA3, 0xD2, 0x1B, 0xCD,
    0x71, 0xC8, 0xBC, 0xC4, 0x09, 0xC2, 0x66, 0xC0,
    0xD9, 0xBF, 0x66, 0xC0, 0x09, 0xC2, 0xBB, 0xC4,
    0x72, 0xC8, 0x1B, 0xCD, 0xA4, 0xD2, 0xF2, 0xD8,
    0xEC, 0xDF, 0x73, 0xE7, 0x66, 0xEF, 0xA1, 0xF7,

    0x00, 0x00, 0x5F, 0x08, 0x9B, 0x10, 0x8D, 0x18,
    0x13, 0x20, 0x0D, 0x27, 0x5C, 0x2D, 0xE5, 0x32,
    0x8F, 0x37, 0x44, 0x3B, 0xF7, 0x3D, 0x9A, 0x3F,
    0x27, 0x40, 0x9A, 0x3F, 0xF6, 0x3D, 0x45, 0x3B,
    0x8F, 0x37, 0xE5, 0x32, 0x5C, 0x2D, 0x0E, 0x27,
    0x13, 0x20, 0x8C, 0x18, 0x9A, 0x10, 0x60, 0x08,
    0x00, 0x00, 0xA1, 0xF7, 0x66, 0xEF, 0x73, 0xE7,
    0xED, 0xDF, 0xF3, 0xD8, 0xA3, 0xD2, 0x1B, 0xCD,
    0x71, 0xC8, 0xBC, 0xC4, 0x09, 0xC2, 0x66, 0xC0,
    0xD9, 0xBF, 0x66, 0xC0, 0x09, 0xC2, 0xBB, 0xC4,
    0x72, 0xC8, 0x1B, 0xCD, 0xA4, 0xD2, 0xF2, 0xD8,
    0xEC, 0xDF, 0x73, 0xE7, 0x66, 0xEF, 0xA1, 0xF7,

    0x00, 0x00, 0x5F, 0x08, 0x9B, 0x10, 0x8D, 0x18,
    0x13, 0x20, 0x0D, 0x27, 0x5C, 0x2D, 0xE5, 0x32,
    0x8F, 0x37, 0x44, 0x3B, 0xF7, 0x3D, 0x9A, 0x3F,
    0x27, 0x40, 0x9A, 0x3F, 0xF6, 0x3D, 0x45, 0x3B,
    0x8F, 0x37, 0xE5, 0x32, 0x5C, 0x2D, 0x0E, 0x27,
    0x13, 0x20, 0x8C, 0x18, 0x9A, 0x10, 0x60, 0x08,
    0x00, 0x00, 0xA1, 0xF7, 0x66, 0xEF, 0x73, 0xE7,
    0xED, 0xDF, 0xF3, 0xD8, 0xA3, 0xD2, 0x1B, 0xCD,
    0x71, 0xC8, 0xBC, 0xC4, 0x09, 0xC2, 0x66, 0xC0,
    0xD9, 0xBF, 0x66, 0xC0, 0x09, 0xC2, 0xBB, 0xC4,
    0x72, 0xC8, 0x1B, 0xCD, 0xA4, 0xD2, 0xF2, 0xD8,
    0xEC, 0xDF, 0x73, 0xE7, 0x66, 0xEF, 0xA1, 0xF7,

    0x00, 0x00, 0x5F, 0x08, 0x9B, 0x10, 0x8D, 0x18,
    0x13, 0x20, 0x0D, 0x27, 0x5C, 0x2D, 0xE5, 0x32,
    0x8F, 0x37, 0x44, 0x3B, 0xF7, 0x3D, 0x9A, 0x3F,
    0x27, 0x40, 0x9A, 0x3F, 0xF6, 0x3D, 0x45, 0x3B,
    0x8F, 0x37, 0xE5, 0x32, 0x5C, 0x2D, 0x0E, 0x27,
    0x13, 0x20, 0x8C, 0x18, 0x9A, 0x10, 0x60, 0x08,
    0x00, 0x00, 0xA1, 0xF7, 0x66, 0xEF, 0x73, 0xE7,
    0xED, 0xDF, 0xF3, 0xD8, 0xA3, 0xD2, 0x1B, 0xCD,
    0x71, 0xC8, 0xBC, 0xC4, 0x09, 0xC2, 0x66, 0xC0,
    0xD9, 0xBF, 0x66, 0xC0, 0x09, 0xC2, 0xBB, 0xC4,
    0x72, 0xC8, 0x1B, 0xCD, 0xA4, 0xD2, 0xF2, 0xD8,
    0xEC, 0xDF, 0x73, 0xE7, 0x66, 0xEF, 0xA1, 0xF7,

    0x00, 0x00, 0x5F, 0x08, 0x9B, 0x10, 0x8D, 0x18,
    0x13, 0x20, 0x0D, 0x27, 0x5C, 0x2D, 0xE5, 0x32,
    0x8F, 0x37, 0x44, 0x3B, 0xF7, 0x3D, 0x9A, 0x3F,
    0x27, 0x40, 0x9A, 0x3F, 0xF6, 0x3D, 0x45, 0x3B,
    0x8F, 0x37, 0xE5, 0x32, 0x5C, 0x2D, 0x0E, 0x27,
    0x13, 0x20, 0x8C, 0x18, 0x9A, 0x10, 0x60, 0x08,
    0x00, 0x00, 0xA1, 0xF7, 0x66, 0xEF, 0x73, 0xE7,
    0xED, 0xDF, 0xF3, 0xD8, 0xA3, 0xD2, 0x1B, 0xCD,
    0x71, 0xC8, 0xBC, 0xC4, 0x09, 0xC2, 0x66, 0xC0,
    0xD9, 0xBF, 0x66, 0xC0, 0x09, 0xC2, 0xBB, 0xC4,
    0x72, 0xC8, 0x1B, 0xCD, 0xA4, 0xD2, 0xF2, 0xD8,
    0xEC, 0xDF, 0x73, 0xE7, 0x66, 0xEF, 0xA1, 0xF7,

    0x00, 0x00, 0x5F, 0x08, 0x9B, 0x10, 0x8D, 0x18,
    0x13, 0x20, 0x0D, 0x27, 0x5C, 0x2D, 0xE5, 0x32,
    0x8F, 0x37, 0x44, 0x3B, 0xF7, 0x3D, 0x9A, 0x3F,
    0x27, 0x40, 0x9A, 0x3F, 0xF6, 0x3D, 0x45, 0x3B,
    0x8F, 0x37, 0xE5, 0x32, 0x5C, 0x2D, 0x0E, 0x27,
    0x13, 0x20, 0x8C, 0x18, 0x9A, 0x10, 0x60, 0x08,
    0x00, 0x00, 0xA1, 0xF7, 0x66, 0xEF, 0x73, 0xE7,
    0xED, 0xDF, 0xF3, 0xD8, 0xA3, 0xD2, 0x1B, 0xCD,
    0x71, 0xC8, 0xBC, 0xC4, 0x09, 0xC2, 0x66, 0xC0,

#if TEST_AO_SPECIAL_SINE
    //
#else
    0xD9, 0xBF, 0x66, 0xC0, 0x09, 0xC2, 0xBB, 0xC4,
    0x72, 0xC8, 0x1B, 0xCD, 0xA4, 0xD2, 0xF2, 0xD8,
    0xEC, 0xDF, 0x73, 0xE7, 0x66, 0xEF, 0xA1, 0xF7,
#endif

#else
    0x00, 0x00, 0xf9, 0x18, 0xfb, 0x30, 0x1c, 0x47,
    0x82, 0x5a, 0x6d, 0x6a, 0x41, 0x76, 0x8a, 0x7d,
    0xff, 0x7f, 0x8b, 0x7d, 0x42, 0x76, 0x6f, 0x6a,
    0x84, 0x5a, 0x1f, 0x47, 0xfe, 0x30, 0xfc, 0x18,
    0x03, 0x00, 0x0a, 0xe7, 0x07, 0xcf, 0xe6, 0xb8,
    0x80, 0xa5, 0x95, 0x95, 0xc0, 0x89, 0x76, 0x82,
    0x00, 0x80, 0x75, 0x82, 0xbc, 0x89, 0x90, 0x95,
    0x7a, 0xa5, 0xde, 0xb8, 0xff, 0xce, 0x02, 0xe7,
    0x00, 0x00, 0xf9, 0x18, 0xfb, 0x30, 0x1c, 0x47,
    0x82, 0x5a, 0x6d, 0x6a, 0x41, 0x76, 0x8a, 0x7d,
    0xff, 0x7f, 0x8b, 0x7d, 0x42, 0x76, 0x6f, 0x6a,
    0x84, 0x5a, 0x1f, 0x47, 0xfe, 0x30, 0xfc, 0x18,
    0x03, 0x00, 0x0a, 0xe7, 0x07, 0xcf, 0xe6, 0xb8,
    0x80, 0xa5, 0x95, 0x95, 0xc0, 0x89, 0x76, 0x82,
    0x00, 0x80, 0x75, 0x82, 0xbc, 0x89, 0x90, 0x95,
    0x7a, 0xa5, 0xde, 0xb8, 0xff, 0xce, 0x02, 0xe7,
    0x00, 0x00, 0xf9, 0x18, 0xfb, 0x30, 0x1c, 0x47,
    0x82, 0x5a, 0x6d, 0x6a, 0x41, 0x76, 0x8a, 0x7d,
    0xff, 0x7f, 0x8b, 0x7d, 0x42, 0x76, 0x6f, 0x6a,
    0x84, 0x5a, 0x1f, 0x47, 0xfe, 0x30, 0xfc, 0x18,
    0x03, 0x00, 0x0a, 0xe7, 0x07, 0xcf, 0xe6, 0xb8,
    0x80, 0xa5, 0x95, 0x95, 0xc0, 0x89, 0x76, 0x82,
    0x00, 0x80, 0x75, 0x82, 0xbc, 0x89, 0x90, 0x95,
    0x7a, 0xa5, 0xde, 0xb8, 0xff, 0xce, 0x02, 0xe7,
    0x00, 0x00, 0xf9, 0x18, 0xfb, 0x30, 0x1c, 0x47,
    0x82, 0x5a, 0x6d, 0x6a, 0x41, 0x76, 0x8a, 0x7d,
    0xff, 0x7f, 0x8b, 0x7d, 0x42, 0x76, 0x6f, 0x6a,
    0x84, 0x5a, 0x1f, 0x47, 0xfe, 0x30, 0xfc, 0x18,
    0x03, 0x00, 0x0a, 0xe7, 0x07, 0xcf, 0xe6, 0xb8,
    0x80, 0xa5, 0x95, 0x95, 0xc0, 0x89, 0x76, 0x82,
    0x00, 0x80, 0x75, 0x82, 0xbc, 0x89, 0x90, 0x95,
    0x7a, 0xa5, 0xde, 0xb8, 0xff, 0xce, 0x02, 0xe7
#endif
};

/*=============================================================*/
// Local function prototype
/*=============================================================*/
//static void _hAudioTestEnableDmaIrq(void);
//static void _hAudioTestDmaIsr(void);
//static void _hAudioTestDmaIsrProcData(void);
//static void _hAudioTestDmaIsrFlag(void);

static int _hDma1ReaderTest(void);
static int _hDma1WriterTest(void);
//static void _hDma1ReaderIsrTest(void);
//static void _hDma1WriterIsrTest(void);

//static void _hDma1ReaderIsrThreadTest(void);
//static void _CamOsThreadTestDma1Reader(void);
//static void _hDma1WriterIsrThreadTest(void);
//static void _CamOsThreadTestDma1Writer(void);

//static void _hDma1ReaderWriterIsrThreadTest(void);

/*=============================================================*/
// Local Variable definition && Local function prototype
/*=============================================================*/

#if 0
static int _hInitTest(CLI_t *pCli, char *p);
static char _szInitTestHelpTxt[] = "Init test";
static char _szInitTestUsageTxt[] =
    "Usage: InitTest\n";
#endif

static int _hDmaTest(CLI_t *pCli, char *p);
static char _szDmaTestHelpTxt[] = "DMA test";
static char _szDmaTestUsageTxt[] =
    "Usage: DmaTest DmaId\n"
    "   DmaId - Dma1Reader, Dma1Writer, Dma1RW\n";
#if 0
static int _hDmaIsrTest(CLI_t *pCli, char *p);
static char _szDmaIsrTestHelpTxt[] = "DMA ISR test";
static char _szDmaIsrTestUsageTxt[] =
    "Usage: DmaIsrTest DmaId\n"
    "   DmaId - Dma1Reader, Dma1Writer, Dma1ReaderTrd, Dma1WriterTrd, Dma1RdWrIsrTrd\n";

static int _hSrcTest(CLI_t *pCli, char *p);
static char _szSrcTestHelpTxt[] = "SRC test";
static char _szSrcTestUsageTxt[] =
    "Usage: SrcTest SrcNum SampleRate\n"
    "   SrcNum - 1, 2\n"
    "   SampleRate - 8, 11_025, 12, 16, 22_05, 24, 32, 44_1, 48\n";

static int _hDpgaTest(CLI_t *pCli, char *p);
static char _szDpgaTestHelpTxt[] = "DPGA test";
static char _szDpgaTestUsageTxt[] =
    "Usage: DpgaTest DpgaId Gain\n"
    "   DpgaId - MMC1, DEC1, ADC1\n"
    "   Gain - -64 ~ 12 dB\n";


static int _hMicGainTest(CLI_t *pCli, char *p);
static char _szMicGainTestHelpTxt[] = "Mic Gain test";
static char _szMicGainTestUsageTxt[] =
    "Usage: MicGainTest MicGainId  GainLevel\n"
    "   MicGainId    - PreGain, AdcGain\n"
    "   PreGainLevel - 0~3 [13, 23, 30, 36 dB]\n"
    "   AdcGainLevel - 0~7 [-6~15 dB, 3dB/Step]\n";

static int _hI2sTxTest(CLI_t *pCli, char *p);
static char _szI2sTxTestHelpTxt[] = "I2sTx test";
static char _szI2sTxTestUsageTxt[] =
    "Usage: I2sTxTest MuxId SampleRate \n"
    "   MuxId      - Dma1Reader, Dma1Writer, SRC \n"
    "   SampleRate - 8000, 16000, 32000, 48000 \n";
#endif

// Internal 2nd Sub-Menu
CliParseToken_t _g_atAudDrvMenuTbl[] =
{
//    {{NULL, NULL}, "InitTest",            _szInitTestHelpTxt,             _szInitTestUsageTxt,            _hInitTest,             NULL},
    {{NULL, NULL}, "DmaTest",             _szDmaTestHelpTxt,              _szDmaTestUsageTxt,             _hDmaTest,              NULL},
//    {{NULL, NULL}, "DmaIsrTest",          _szDmaIsrTestHelpTxt,           _szDmaIsrTestUsageTxt,          _hDmaIsrTest,           NULL},
//    {{NULL, NULL}, "SrcTest",             _szSrcTestHelpTxt,              _szSrcTestUsageTxt,             _hSrcTest,              NULL},
//    {{NULL, NULL}, "DpgaTest",            _szDpgaTestHelpTxt,             _szDpgaTestUsageTxt,            _hDpgaTest,             NULL},
//    {{NULL, NULL}, "MicGainTest",         _szMicGainTestHelpTxt,          _szMicGainTestUsageTxt,         _hMicGainTest,          NULL},
//    {{NULL, NULL}, "I2sTxTest",           _szI2sTxTestHelpTxt,            _szI2sTxTestUsageTxt,           _hI2sTxTest,            NULL},

    PARSE_TOKEN_DELIMITER
};

// Internal 3rd Sub-Menu


/*=============================================================*/
// Local Variable definition
/*=============================================================*/
#if 0
static int _hInitTest(CLI_t *pCli, char *p)
{
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "System init\n");
    DrvAudioSysInit();

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Init test... Done!\n");

    return eCLI_PARSE_OK;
}
#endif

/* DMA Test */
static int _hDmaTest(CLI_t *pCli, char *p)
{
#if 1
    char *pDmaId;
    static int nFirst = 1;
    CamOsThread TaskDma1RdHandle;
    CamOsThread TaskDma1WrHandle;

    if (CliTokenCount(pCli) == 1)
    {
        pDmaId = CliTokenPop(pCli);
        if (!pDmaId)
            goto HELP_EXIT;
    }
    else
        goto HELP_EXIT;

    if ( nFirst != 0 )
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "MHAL_AUDIO_Init !!!\n");

        MHAL_AUDIO_Init(NULL);
        nFirst = 0;
    }

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "_hDmaTest !!!\n");

    if(!strcmp(pDmaId, "Dma1Reader")) // play sine table for a while
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "_hDma1ReaderTest \n");
        _hDma1ReaderTest();
    }
    else if(!strcmp(pDmaId, "Dma1Writer")) // start recording to pMcuNonCacheDmaBuf1, then copy to _gMcuDmaBuf4
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "_hDma1WriterTest \n");
        _hDma1WriterTest();
    }
    else if(!strcmp(pDmaId, "Dma1RW")) // start recording to pMcuNonCacheDmaBuf1, then copy to _gMcuDmaBuf4
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Dma1RW \n");
        CamOsThreadCreate(&TaskDma1RdHandle, NULL, (void *)_hDma1ReaderTest, NULL);
        CamOsThreadCreate(&TaskDma1WrHandle, NULL, (void *)_hDma1WriterTest, NULL);
        CamOsThreadJoin(TaskDma1RdHandle);
        CamOsThreadJoin(TaskDma1WrHandle);
    }
    else
    {
        goto HELP_EXIT;
    }

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "DMA test... Done!\n");
    return eCLI_PARSE_OK;

HELP_EXIT:
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Invalid parameter\n");
    return eCLI_PARSE_ERROR;
#else
    return eCLI_PARSE_OK;
#endif
}

#if 0
/* DMA ISR Test: using DMA ISR to write or read data */
static int _hDmaIsrTest(CLI_t *pCli, char *p)
{
    char *pDmaId;

    if (CliTokenCount(pCli) == 1)
    {
        pDmaId = CliTokenPop(pCli);
        if (!pDmaId)
            goto HELP_EXIT;
    }
    else
        goto HELP_EXIT;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "System init\n");
    DrvAudioSysInit();

    if(!strcmp(pDmaId, "Dma1Reader")) // play sine table for a while
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Open DAC \n");
        BachAtopPath_e ePath = BACH_ATOP_LINEOUT;
        DrvAudioOpenAtop(ePath);
        _hDma1ReaderIsrTest();
    }
    else if(!strcmp(pDmaId, "Dma1Writer")) // start recording to pMcuNonCacheDmaBuf1, then copy to _gMcuDmaBuf4
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Open Mic \n");
        BachAtopPath_e ePath = BACH_ATOP_MIC;
        DrvAudioOpenAtop(ePath);
        _hDma1WriterIsrTest();
    }
    else if(!strcmp(pDmaId, "Dma1ReaderTrd")) //test using thread to write data
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Open DAC \n");
        BachAtopPath_e ePath = BACH_ATOP_LINEOUT;
        DrvAudioOpenAtop(ePath);
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Test Dma1Reader using thread to write data \n");
        _hDma1ReaderIsrThreadTest();
    }
    else if(!strcmp(pDmaId, "Dma1WriterTrd"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Open Mic \n");
        BachAtopPath_e ePath = BACH_ATOP_MIC;
        DrvAudioOpenAtop(ePath);
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Test Dma1Writer using thread to read data \n");
        _hDma1WriterIsrThreadTest();
    }
    else if(!strcmp(pDmaId, "Dma1RdWrIsrTrd")) //test Dma Reader/Writer work together
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Open Mic \n");
        BachAtopPath_e ePath1 = BACH_ATOP_MIC;
        DrvAudioOpenAtop(ePath1);

        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Open DAC \n");
        BachAtopPath_e ePath2 = BACH_ATOP_LINEOUT;
        DrvAudioOpenAtop(ePath2);

        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Test Dma1 Reader/Writer using thread together \n");
        _hDma1ReaderWriterIsrThreadTest();
    }
    else
    {
        goto HELP_EXIT;
    }

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "DMA ISR test... Done!\n");
    return eCLI_PARSE_OK;

HELP_EXIT:
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Invalid parameter\n");
    return eCLI_PARSE_ERROR;
}
#endif

/**
* @brief
* Test DPGAs: MMC1, DEC1, ADC1
* @param[in]
*
* @retval
*
*/
#if 0
static int _hDpgaTest(CLI_t *pCli, char *p)
{
    char *pDpgaId, *pGain;
    s8 nGain;
    BachPath_e ePath;

    if (CliTokenCount(pCli) == 2)
    {
        pDpgaId = CliTokenPop(pCli);
        if (!pDpgaId)
            goto HELP_EXIT;
        pGain = CliTokenPop(pCli);
        if (!pGain)
            goto HELP_EXIT;
    }
    else
        goto HELP_EXIT;

    nGain = atoi(pGain);

    if(!strcmp(pDpgaId, "MMC1"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "MMC1 - %ddB\n", nGain);

        ePath = BACH_PATH_PLAYBACK;
        DrvAudioSetPathGain(ePath, nGain);
    }
    else if(!strcmp(pDpgaId, "DEC1"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "DEC1 - %ddB\n", nGain);
        HalAudioDpgaSetGain(BACH_DPGA_DEC1, nGain);

    }
    else if(!strcmp(pDpgaId, "ADC1"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "ADC1 - %ddB\n", nGain);

        ePath = BACH_PATH_CAPTURE;
        DrvAudioSetPathGain(ePath, nGain);
    }


    AUD_UT_PRINTF(TRACE_UT_LEVEL, "DPGA test... Done!\n");
    return eCLI_PARSE_OK;

HELP_EXIT:
    AUD_UT_PRINTF(TRACE_UT_LEVEL, _szDpgaTestUsageTxt);
    return eCLI_PARSE_ERROR;
}
#endif

/**
* @brief
* Test SRC1 in 9 typical rate.
* Play predefine sine table in different sampling rate with DMA1 reader.
* Test CODEC1 in 4 typical rate: Record Mic data to DMA1 writer
* @param[in]
*
* @retval
*
*/
#if 0
static int _hSrcTest(CLI_t *pCli, char *p)
{
    char *pSrcId, *pSampleRate;
    u32   nToneSize = 0, nOverrunTh, nUnderrunTh, nDataSize, nActualSize, nTimes;
    u8   *pTone = NULL;
    bool  bOverrun, bUnderrun;
    BachDmaChannel_e eDmaChannel;

    if (CliTokenCount(pCli) == 2)
    {
        pSrcId = CliTokenPop(pCli);
        if (!pSrcId)
            goto HELP_EXIT;
        pSampleRate = CliTokenPop(pCli);
        if (!pSampleRate)
            goto HELP_EXIT;
    }
    else
        goto HELP_EXIT;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "System init\n");
    DrvAudioSysInit();

    if(!strcmp(pSrcId, "SRC1"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "SRC1\n");
        eDmaChannel = BACH_DMA_READER1;

        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Open DAC \n");
        BachAtopPath_e ePath = BACH_ATOP_LINEOUT;
        DrvAudioOpenAtop(ePath);

        if(!strcmp(pSampleRate, "8"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "8k\n");
            //HalAudioDmaPlaySetSamplingRate(8000);
            //HalAudioSetStereoRate(SRC_8KHZ);
            DrvAudioDmaSetSamplingRate(eDmaChannel, 8000);
            pTone = tone_8k;
            nToneSize = sizeof(tone_8k);
        }
        else if(!strcmp(pSampleRate, "11_025"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "11.025k\n");
            //HalAudioDmaPlaySetSamplingRate(11025);
            //HalAudioSetStereoRate(SRC_11_025KHZ);
            DrvAudioDmaSetSamplingRate(eDmaChannel, 11025);
            pTone = tone_11_025k;
            nToneSize = sizeof(tone_11_025k);
        }
        else if(!strcmp(pSampleRate, "12"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "12k\n");
            //HalAudioDmaPlaySetSamplingRate(12000);
            //HalAudioSetStereoRate(SRC_12KHZ);
            DrvAudioDmaSetSamplingRate(eDmaChannel, 12000);
            pTone = tone_12k;
            nToneSize = sizeof(tone_12k);
        }
        else if(!strcmp(pSampleRate, "16"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "16k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 16000);
            //HalAudioDmaPlaySetSamplingRate(16000);
            //HalAudioSetStereoRate(SRC_16KHZ);
            pTone = tone_16k;
            nToneSize = sizeof(tone_16k);
        }
        else if(!strcmp(pSampleRate, "22_05"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "22.05k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 22050);
            //HalAudioDmaPlaySetSamplingRate(22050);
            //HalAudioSetStereoRate(SRC_22_05KHZ);
            pTone = tone_22_05k;
            nToneSize = sizeof(tone_22_05k);
        }
        else if(!strcmp(pSampleRate, "24"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "24k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 24000);
            //HalAudioDmaPlaySetSamplingRate(24000);
            //HalAudioSetStereoRate(SRC_24KHZ);
            pTone = tone_24k;
            nToneSize = sizeof(tone_24k);
        }
        else if(!strcmp(pSampleRate, "32"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "32k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 32000);
            //HalAudioDmaPlaySetSamplingRate(32000);
            //HalAudioSetStereoRate(SRC_32KHZ);
            pTone = tone_32k;
            nToneSize = sizeof(tone_32k);
        }
        else if(!strcmp(pSampleRate, "44_1"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "44.1k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 44100);
            //HalAudioDmaPlaySetSamplingRate(44100);
            //HalAudioSetStereoRate(SRC_44_1KHZ);
            pTone = tone_44_1k;
            nToneSize = sizeof(tone_44_1k);
        }
        else if(!strcmp(pSampleRate, "48"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "48k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 48000);
            //HalAudioDmaPlaySetSamplingRate(48000);
            //HalAudioSetStereoRate(SRC_48KHZ);
            pTone = tone_48k;
            nToneSize = sizeof(tone_48k);
        }
        else
            goto HELP_EXIT;

        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Drv audio dma reset\n");
        DrvAudioDmaReset();

        u8* pMcuNonCacheDmaBuf1 = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);
        if(pMcuNonCacheDmaBuf1 == NULL)
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
            goto HELP_EXIT;
        }

        memset(pMcuNonCacheDmaBuf1, 0 ,DMA_BUF_SIZE);
        DrvAudioDmaSetBufAddr(eDmaChannel, pMcuNonCacheDmaBuf1, DMA_BUF_SIZE);

        nOverrunTh = DMA_BUF_SIZE / 4 * 3;
        nUnderrunTh = DMA_BUF_SIZE / 4;
        DrvAudioDmaSetThreshold(eDmaChannel, nOverrunTh, nUnderrunTh);

        DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
        while(bUnderrun)
        {
            DrvAudioDmaPlayWriteData(eDmaChannel, pTone, nToneSize, &nActualSize);
            DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
        }

        //_hAudioTestEnableDmaIrq();
        DrvAudioRegDmaIrqCB(_hAudioTestDmaIsr);
        DrvAudioEnableDmaIrq();

        /* init and start */
        DrvAudioDmaStart(eDmaChannel);

        // DrvAudioDmaStart already Enable DMA interrupt
        //DrvAudioDmaCtrlInt(eDmaChannel, TRUE, FALSE, TRUE);

        nTimes = 0;
        while(1)
        {
            DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
            while(bOverrun)
            {
                SYS_UDELAY(100);
                DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
            }

            DrvAudioDmaPlayWriteData(eDmaChannel, pTone, nToneSize, &nActualSize);

            nTimes++;
            if(nTimes > 30000)
            {
                //while(!HalAudioDmaIsEmpty(eDmaChannel) || !HalAudioDmaIsLocalEmpty(eDmaChannel));
                while(!HalAudioDmaIsLocalEmpty(eDmaChannel));
                AUD_UT_PRINTF(TRACE_UT_LEVEL, "empty cnt %d\n",HalAudioDmaGetLevelCnt(eDmaChannel));
                DrvAudioDmaStop(eDmaChannel);
                break;
            }
        }
        // free non-cache memory
        MsReleaseMemory(pMcuNonCacheDmaBuf1);

    }
    else if(!strcmp(pSrcId, "CODE1"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "CODEC1\n");

        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Open Mic \n");
        BachAtopPath_e ePath = BACH_ATOP_MIC;
        DrvAudioOpenAtop(ePath);

        eDmaChannel = BACH_DMA_WRITER1;

        if(!strcmp(pSampleRate, "8"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "8k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 8000);
        }
        else if(!strcmp(pSampleRate, "16"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "16k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 16000);
        }
        else if(!strcmp(pSampleRate, "32"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "32k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 32000);
        }
        else if(!strcmp(pSampleRate, "48"))
        {
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "48k\n");
            DrvAudioDmaSetSamplingRate(eDmaChannel, 48000);
        }
        else
            goto HELP_EXIT;

        _hDma1WriterTest();
    }
    else
    {
        goto HELP_EXIT;
    }


    AUD_UT_PRINTF(TRACE_UT_LEVEL, "SRC test... Done!\n");
    return eCLI_PARSE_OK;

HELP_EXIT:
    AUD_UT_PRINTF(TRACE_UT_LEVEL, _szSrcTestUsageTxt);
    return eCLI_PARSE_ERROR;
}
#endif

/**
* @brief
* Test Mic Gain: PreGain, AdcGain
* PreGainLevel - 0~3 [13, 23, 30, 36 dB]"
* AdcGainLevel - 0~7 [-6~15 dB, 3dB/Step]";
* @param[in]
*
* @retval
*
*/
#if 0
static int _hMicGainTest(CLI_t *pCli, char *p)
{
    char *pMicGainId, *pGainLevel;
    u16 nLevel;

    if (CliTokenCount(pCli) == 2)
    {
        pMicGainId = CliTokenPop(pCli);
        if (!pMicGainId)
            goto HELP_EXIT;
        pGainLevel = CliTokenPop(pCli);
        if (!pGainLevel)
            goto HELP_EXIT;
    }
    else
        goto HELP_EXIT;

    nLevel = atoi(pGainLevel);

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "System init\n");
    DrvAudioSysInit();

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Open Mic \n");
    BachAtopPath_e ePath = BACH_ATOP_MIC;
    DrvAudioOpenAtop(ePath);

    if(!strcmp(pMicGainId, "PreGain"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "PreGain - Level:%d\n", nLevel);

        DrvAudioAtopMicPreGain(nLevel);
    }
    else if(!strcmp(pMicGainId, "AdcGain"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "AdcGain - Level:%d\n", nLevel);
        DrvAudioAtopAdcGain(nLevel, ePath);
    }
    else
        goto HELP_EXIT;

    _hDma1WriterTest();

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "MicGain test... Done!\n");
    return eCLI_PARSE_OK;

HELP_EXIT:
    AUD_UT_PRINTF(TRACE_UT_LEVEL, _szMicGainTestUsageTxt);
    return eCLI_PARSE_ERROR;
}
#endif

/**
* @brief
* Test I2sTx: Dma1Reader, Dma1Writer, SRC
* @param[in]
*
* @retval
*
*/
#if 0
static int _hI2sTxTest(CLI_t *pCli, char *p)
{
    char *pI2sTxPathId, *pSampleRate;
    u32 nSampleRate;
    BachI2sTxMux_e eMux;

    if (CliTokenCount(pCli) == 2)
    {
        pI2sTxPathId = CliTokenPop(pCli);
        if (!pI2sTxPathId)
            goto HELP_EXIT;

        pSampleRate = CliTokenPop(pCli);
        if (!pSampleRate)
            goto HELP_EXIT;
    }
    else
        goto HELP_EXIT;

    nSampleRate = atoi(pSampleRate);


    if(!strcmp(pI2sTxPathId, "Dma1Reader"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "I2sTx mux is %s \n", pI2sTxPathId);
        eMux = BACH_I2STX_DMARD1;
        DrvAudioI2sTxCtrl(eMux, nSampleRate);
    }
    else if(!strcmp(pI2sTxPathId, "Dma1Writer"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "I2sTx path is %s \n", pI2sTxPathId);
        eMux = BACH_I2STX_DMAWR1;
        DrvAudioI2sTxCtrl(eMux, nSampleRate);

    }
    else if(!strcmp(pI2sTxPathId, "SRC"))
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "I2sTx path is %s \n", pI2sTxPathId);
        eMux = BACH_I2STX_SRC;
        DrvAudioI2sTxCtrl(eMux, nSampleRate);
    }


    AUD_UT_PRINTF(TRACE_UT_LEVEL, "I2sTx test... Done!\n");
    return eCLI_PARSE_OK;

HELP_EXIT:
    AUD_UT_PRINTF(TRACE_UT_LEVEL, _szI2sTxTestUsageTxt);
    return eCLI_PARSE_ERROR;
}
#endif

/**
* @brief
*
* @param[in]
*
* @retval
*
*/
/*
static void _hAudioTestEnableDmaIrq(void)
{
    MsIntInitParam_u uInitParam = {{0}};

    uInitParam.intc.eMap         = INTC_MAP_IRQ;
    uInitParam.intc.ePriority    = INTC_PRIORITY_7;
    uInitParam.intc.pfnIsr       = _hAudioTestDmaIsr;

    MsInitInterrupt(&uInitParam, MS_INT_NUM_IRQ_BACH);

    MsUnmaskInterrupt(MS_INT_NUM_IRQ_BACH);
}
*/

/**
* @brief
*
* @param[in]
*
* @retval
*
*/
#if 0
static void _hAudioTestDmaIsr(void)
{
    u16 nEvent;

    DrvAudioDmaEventHandler(&nEvent);

    if(nEvent & HAL_AUDIO_DMA1_WR_FULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_WR_FULL!\n");
    }

    if(nEvent & HAL_AUDIO_DMA1_WR_OVERRUN)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_WR_OVERRUN!\n");
        DrvAudioDmaCtrlInt(BACH_DMA_WRITER1, FALSE, FALSE, FALSE);
    }

    if(nEvent & HAL_AUDIO_DMA1_WR_UNDERRUN)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_WR_UNDERRUN!\n");
    }

    if(nEvent & HAL_AUDIO_DMA1_RD_EMPTY)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_RD_EMPTY!\n");
    }

    if(nEvent & HAL_AUDIO_DMA1_RD_OVERRUN)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_RD_OVERRUN!\n");
    }

    if(nEvent & HAL_AUDIO_DMA1_RD_UNDERRUN)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_RD_UNDERRUN!\n");
        DrvAudioDmaCtrlInt(BACH_DMA_READER1, FALSE, FALSE, FALSE);
    }

    /* Clear this IRQ one more time for garbage raw status */
    //MsClearInterrupt(MS_INT_NUM_IRQ_BACH);

    /* Unmask for auto-mask function */
    //MsUnmaskInterrupt(MS_INT_NUM_IRQ_BACH);
}
#endif

/**
* @brief
*
* @param[in]
*
* @retval
*
*/
#if 0
static void _hAudioTestDmaIsrProcData(void)
{
    u16 nEvent;

    DrvAudioDmaEventHandler(&nEvent);

    if(nEvent & HAL_AUDIO_DMA1_WR_FULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_WR_FULL!\n");
        DrvAudioDmaStop(BACH_DMA_WRITER1);
        DrvAudioDmaStart(BACH_DMA_WRITER1);
    }

    if(nEvent & HAL_AUDIO_DMA1_WR_OVERRUN)
    {

        //AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_WR_OVERRUN!\n"); //if print, Writer will full
        u32 nReadSize;
        u8 pBuf[192];

        DrvAudioDmaRecReadData(BACH_DMA_WRITER1, pBuf, sizeof(pBuf), &nReadSize);

        if((u32)_gMcuDmaBuf4+_gnReadTotalSize+nReadSize > (u32)_gMcuDmaBuf4 + sizeof(_gMcuDmaBuf4))
        {
            memcpy(_gMcuDmaBuf4+_gnReadTotalSize, pBuf,  sizeof(_gMcuDmaBuf4)-_gnReadTotalSize);
            //ptr = _gMcuDmaBuf4 + sizeof(_gMcuDmaBuf4);
            _gnReadTotalSize = sizeof(_gMcuDmaBuf4);
        }
        else
        {
            memcpy(_gMcuDmaBuf4+_gnReadTotalSize, pBuf, nReadSize);
            _gnReadTotalSize += nReadSize;
        }
        //_gnReadTotalSize += nReadSize;
        DrvAudioDmaCtrlInt(BACH_DMA_WRITER1, TRUE, TRUE, FALSE);

    }

    if(nEvent & HAL_AUDIO_DMA1_WR_UNDERRUN)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_WR_UNDERRUN!\n");
    }

    if(nEvent & HAL_AUDIO_DMA1_RD_EMPTY)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_RD_EMPTY!\n");
        DrvAudioDmaStop(BACH_DMA_READER1);
        DrvAudioDmaStart(BACH_DMA_READER1);
    }

    if(nEvent & HAL_AUDIO_DMA1_RD_OVERRUN)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_RD_OVERRUN!\n");
    }

    if(nEvent & HAL_AUDIO_DMA1_RD_UNDERRUN)
    {
        //AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_RD_UNDERRUN!\n"); // if print, Reader will empty
        u32  nActualSize;
        DrvAudioDmaPlayWriteData(BACH_DMA_READER1, tone_48k_2, sizeof(tone_48k_2), &nActualSize);
        _gnWriteTotalSize += nActualSize;
        DrvAudioDmaCtrlInt(BACH_DMA_READER1, TRUE, FALSE, TRUE);
    }

    /* Clear this IRQ one more time for garbage raw status */
    //MsClearInterrupt(MS_INT_NUM_IRQ_BACH);

    /* Unmask for auto-mask function */
    //MsUnmaskInterrupt(MS_INT_NUM_IRQ_BACH);
}
#endif

/**
* @brief
*
* @param[in]
*
* @retval
*
*/
#if 0
static void _hAudioTestDmaIsrFlag(void)
{
    u16 nEvent;

    DrvAudioDmaEventHandler(&nEvent);

    //if full and overrun happen together, firstly handle full
    if(nEvent & HAL_AUDIO_DMA1_WR_FULL)
    {
        DrvAudioDmaStop(BACH_DMA_WRITER1);
        MsFlagSetbits(&_tDmaFlag, HAL_AUDIO_DMA1_WR_FULL);
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_WR_FULL!\n");
    }
    else if(nEvent & HAL_AUDIO_DMA1_WR_OVERRUN)
    {
        DrvAudioDmaCtrlInt(BACH_DMA_WRITER1, FALSE, FALSE, FALSE);
        MsFlagSetbits(&_tDmaFlag, HAL_AUDIO_DMA1_WR_OVERRUN);
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_WR_OVERRUN!\n");
    }

    if(nEvent & HAL_AUDIO_DMA1_WR_UNDERRUN)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_WR_UNDERRUN!\n");
    }

    // if empty and underrun happen together, firstly handle empty
    if(nEvent & HAL_AUDIO_DMA1_RD_EMPTY)
    {
        DrvAudioDmaStop(BACH_DMA_READER1);
        MsFlagSetbits(&_tDmaFlag, HAL_AUDIO_DMA1_RD_EMPTY);
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_RD_EMPTY!\n");
    }
    else if(nEvent & HAL_AUDIO_DMA1_RD_UNDERRUN)
    {
        DrvAudioDmaCtrlInt(BACH_DMA_READER1, FALSE, FALSE, FALSE);
        MsFlagSetbits(&_tDmaFlag, HAL_AUDIO_DMA1_RD_UNDERRUN);
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_RD_UNDERRUN!\n");
    }

    if(nEvent & HAL_AUDIO_DMA1_RD_OVERRUN)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "HAL_AUDIO_DMA1_RD_OVERRUN!\n");
    }



    /* Clear this IRQ one more time for garbage raw status */
    //MsClearInterrupt(MS_INT_NUM_IRQ_BACH);

    /* Unmask for auto-mask function */
    //MsUnmaskInterrupt(MS_INT_NUM_IRQ_BACH);
}
#endif


int _AoRun(void* data)
{
    //
    _AO_Info_t* pstAoInfo = (_AO_Info_t*)data;

    //
    int size = 0;
    int flag = 1;
    int ret = 0;

    //
    int     dev;
    int     total_size;
    u8      *pDataArray;
    int     nArrSize;
    int     nStartThres;

    //
    dev = pstAoInfo->dev;
    total_size = pstAoInfo->total_size;
    pDataArray = pstAoInfo->pDataArray;
    nArrSize = pstAoInfo->nArrSize;
    nStartThres = pstAoInfo->nStartThres;

    //
    while(total_size>nArrSize)
    {
        if ( (size>=nStartThres) && (flag==1) )
        {
            ret = MHAL_AUDIO_StartPcmOut(dev);
            if ( ret )
            {
                CamOsPrintf("MHAL_AUDIO_StartPcmOut fail !!! ret = %d\n",ret);
                return 0;
            }
            else
            {
                CamOsPrintf("AO DMA Start ! current size = %d\n",size);
            }

            flag = 0;
        }

#if 0 //TEST_AO_EMPTY
        {
            static int c = 1;

            if ( flag == 1 )
            {
                c = 1;
            }
            else
            {
                c++;
                CamOsMsSleep(10*c);
            }
        }
#endif
        ret = MHAL_AUDIO_WriteDataOut(dev, pDataArray, nArrSize, TRUE); // FALSE
        if ( ret >= 0 )
        {
            size += nArrSize;
            total_size -= nArrSize;
        }
        else
        {
            CamOsPrintf("MHAL_AUDIO_WriteDataOut fail !!! ret = %d\n",ret);

            if ( MHAL_AUDIO_IsPcmOutXrun(dev) )
            {
                CamOsPrintf("AO Empty ! XRUN !!!\n");
                flag = 1;
                size = 0;
            }
        }

#if 0//TEST_AO_HOT_STOP
        {
            int nCurrLen = 0;

            if (size>=(nStartThres*4))
            {
                MHAL_AUDIO_GetPcmOutCurrDataLen(dev,&nCurrLen);

                CamOsPrintf("When Stop, nCurrLen = %d\n",nCurrLen);

                ret = MHAL_AUDIO_StopPcmOut(dev);
                if(ret)
                {
                    AUD_UT_PRINTF(ERROR_UT_LEVEL, "MHAL_AUDIO_StopPcmOut error=%d!!!\n",ret);
                    //return -EFAULT;
                }

                return 0;
            }
        }
#endif
    }

    if ( flag == 0 )
    {
        while( !MHAL_AUDIO_IsPcmOutXrun(dev) )
        {
            CamOsMsSleep(1);
        }
    }

    return 0;
}

int _AiRun(void* data)
{
    //
    _AI_Info_t* pstAiInfo = (_AI_Info_t*)data;

    //
#if 0 // dump file
    int size = 0;
#endif
    int ret = 0;
    int count = 0;
#if 0 // dump file
    int i = 0, j = 0;
#else
    int i = 0;
#endif
    //
    int dev;
    int total_size;
    u8 *tmpBuf;
    int tmpBufSize;
#if 0 // dump file
    int channels;
    int channelSize;
    int interleaved;
    int nSpecialDumpChannels;
#endif

    //
    void *pForDumpVirtPtr = NULL;

    pForDumpVirtPtr = pVirtPtr;

#if TEST_AI_FULL
    static int c = 1, pre_c = 0;
#endif

    //
    dev = pstAiInfo->dev;
    total_size = pstAiInfo->total_size;
    tmpBuf = pstAiInfo->tmpBuf;
    tmpBufSize = pstAiInfo->tmpBufSize;
#if 0 // dump file
    channels = pstAiInfo->channels;
    channelSize = pstAiInfo->channelSize;
    interleaved = pstAiInfo->interleaved;
    nSpecialDumpChannels = pstAiInfo->nSpecialDumpChannels;
#endif
    //
    ret = MHAL_AUDIO_StartPcmIn(dev);
    if(ret)
    {
        CamOsPrintf("MHAL_AUDIO_StartPcmIn fail !!! ret = %d\n",ret);
        return 0;
    }
    else
    {
        CamOsPrintf("AI DMA Start !\n");
    }

    while(count<total_size)
    {
        if (MHAL_AUDIO_IsPcmInXrun(dev))
        {
#if TEST_AI_FULL
            c = 1;
#endif
            CamOsPrintf("AI Full ! XRUN !!!, count = %d\n",count);

            ret = MHAL_AUDIO_StartPcmIn(dev);
            if(ret)
            {
                CamOsPrintf("MHAL_AUDIO_StartPcmIn fail !!! ret = %d\n",ret);
                return 0;
            }
            else
            {
                CamOsPrintf("AI DMA Start Re-Start !!!\n");
            }
        }

#if TEST_AI_FULL
        if ( pre_c < 100 )
        {
            pre_c++;
        }
        else
        {
            c++;
            CamOsMsSleep(c);
        }
#endif

        ret = MHAL_AUDIO_ReadDataIn(dev, tmpBuf, tmpBufSize, TRUE);
        if (ret<0)
        {
            CamOsPrintf("MHAL_AUDIO_ReadDataIn fail !!!, ret = %d\n",ret);
        }
        else
        {
#if 1
            for(i = 0; i < ret; i+=8)
            {
                memcpy(pForDumpVirtPtr, tmpBuf+i, 4);
                pForDumpVirtPtr += 4;
            }

            count += ret;
#else // dump file
            fs = get_fs();
            set_fs(KERNEL_DS);
            if(interleaved)
            {
                if ( nSpecialDumpChannels != 0 )
                {
                    for ( j = 0; (channelSize*j*nSpecialDumpChannels) < ret; j++ )
                    {
                        for ( i = 0; i < nSpecialDumpChannels; i++ )
                        {
                            size = vfs_write((*(fw+i)), (char *)tmpBuf+(channelSize*i)+(channelSize*j*nSpecialDumpChannels), channelSize, &(*(fw+i))->f_pos);
                            count += size;
                        }
                    }
                }
                else
                {
                    if(channels == 4)
                    {
                        for(i = 0; i < ret; i += channelSize*4)
                        {
                            size = vfs_write((*fw), (char *)tmpBuf+i, channelSize*2, &(*fw)->f_pos);
                            count += size;
                        }
                        for(i = 4; i < ret; i += channelSize*4)
                        {
                            size = vfs_write((*(fw+1)), (char *)tmpBuf+i, channelSize*2, &(*(fw+1))->f_pos);
                            count += size;
                        }
                    }
                    else
                    {
                        size = vfs_write((*fw), (char *)tmpBuf, ret, &(*fw)->f_pos);
                        count += size;
                    }
                }
            }
            else
            {
                if ( nSpecialDumpChannels != 0 )
                {
                    CamOsPrintf("SpecialDumpChannels dont support non-interleaved !!!\n");
                }
                else
                {
                    for(i=0; i<channels; i++)
                    {
                        size = vfs_write((*(fw+i)), (char *)tmpBuf+(ret/channels)*i, ret/channels, &(*(fw+i))->f_pos);
                        if ( size!=ret/channels )
                        {
                            CamOsPrintf("Dump file write data length is wrong !!!, size = %d, write = %d\n",size,ret/channels);
                        }
                    }
                    count += ret;
                }
            }
            set_fs(fs);
#endif
        }
    }

    CamOsPrintf("Dump file ok %d bytes!!!\n",count);

    return 0;
}


/**
* @brief
* Test Dma1 Reader
* @param[in]
*
* @retval
*
*/
static int _hDma1ReaderTest(void)
{
#if 1
    struct AUD_PcmCfg_s sPcmCfg;
    MHAL_AUDIO_PcmCfg_t tPcm;
    MHAL_AUDIO_I2sCfg_t tI2sCfg;
    //MHAL_AUDIO_SineGenCfg_t tSineGenCfg;
    //struct pcm_data *pPcmData = (struct pcm_data *)filp->private_data;
    int dev = 0;//pPcmData->dev_id;
    int ret;
    int total_size = 0;
    u8 *pDataArray;
    int nArrSize;
    int nStartThres;
    _AO_Info_t stAoInfo;
    CamOsThread TaskDma1RdHandle;

    // Config Parameter
    sPcmCfg.nRate = 48000;
    sPcmCfg.nBitWidth = 16;
    sPcmCfg.n16Channels = 2;
    sPcmCfg.nInterleaved = 1;
    sPcmCfg.n32PeriodSize = 1024;
    sPcmCfg.n32StartThres = 1<<30;
    sPcmCfg.nTimeMs = 5000; // 10000 -> 5000
    sPcmCfg.nI2sConfig = 0;
    sPcmCfg.sI2s.nTdmMode = 0;
    sPcmCfg.sI2s.nMsMode = 0;
    sPcmCfg.sI2s.nBitWidth = 0;
    sPcmCfg.sI2s.nFmt = 0;
    sPcmCfg.sI2s.u16Channels = 0;
    sPcmCfg.sI2s.e4WireMode = 0;
    sPcmCfg.sI2s.eMck = 0;

    unsigned char *phyPtrTmp = NULL;
    unsigned char *phyPtr = NULL;

    if ( CamOsDirectMemAlloc("aud_reader", DMA_BUF_SIZE, (void**)&buf_AO.area, (void**)&phyPtrTmp, (void**)&phyPtr) != CAM_OS_OK )
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "CamOsDirectMemAlloc fail\n");
    }

    if(buf_AO.area == NULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
        return -1;
    }

    buf_AO.addr = (dma_addr_t)phyPtr;
    buf_AO.bytes = DMA_BUF_SIZE;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "buf_AO.area = 0x%x\n",buf_AO.area);
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "buf_AO.addr = 0x%x\n",buf_AO.addr);
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "buf_AO.bytes = %d\n",buf_AO.bytes);

    memset(buf_AO.area, 0 ,DMA_BUF_SIZE);

    //
    if(sPcmCfg.nRate==8000)
        tPcm.eRate = E_MHAL_AUDIO_RATE_8K;
    else if(sPcmCfg.nRate==16000)
        tPcm.eRate = E_MHAL_AUDIO_RATE_16K;
    else if(sPcmCfg.nRate==32000)
        tPcm.eRate = E_MHAL_AUDIO_RATE_32K;
    else if(sPcmCfg.nRate==48000)
        tPcm.eRate = E_MHAL_AUDIO_RATE_48K;
    else
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "AUDRV_PCM_IOCTL sample rate %d not support!!!\n",sPcmCfg.nRate);
        return -EFAULT;
    }

    if(sPcmCfg.nBitWidth==16)
        tPcm.eWidth = E_MHAL_AUDIO_BITWIDTH_16;
    else if(sPcmCfg.nBitWidth==32)
        tPcm.eWidth = E_MHAL_AUDIO_BITWIDTH_32;
    else
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "AUDRV_PCM_IOCTL bitwidth %d not support!!!\n",sPcmCfg.nBitWidth);
        return -EFAULT;
    }

    if(sPcmCfg.n16Channels==1)
    {
        pDataArray = tone_48k_mono;
        nArrSize = sizeof(tone_48k_mono);
    }
    else if(sPcmCfg.n16Channels==2)
    {
        if(sPcmCfg.nInterleaved)
        {
            pDataArray = tone_48k;
            nArrSize = sizeof(tone_48k);
        }
        else
        {
            //pDataArray = tone_48k_noninterleave;
            //nArrSize = sizeof(tone_48k_noninterleave);
        }
    }
    else
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "AUDRV_PCM_IOCTL channel %d not support!!!\n",sPcmCfg.n16Channels);
        return -EFAULT;
    }

    tPcm.u16Channels = sPcmCfg.n16Channels;
    tPcm.bInterleaved = sPcmCfg.nInterleaved;
    tPcm.pu8DmaArea = buf_AO.area;
    tPcm.phyDmaAddr = buf_AO.addr-MIU_OFFSET;
    tPcm.u32BufferSize = buf_AO.bytes;
    tPcm.u32PeriodSize = sPcmCfg.n32PeriodSize;//1024*2*2*6;
    tPcm.u32StartThres = sPcmCfg.n32StartThres;//buf.bytes+1;//buf.bytes/2;

    if(sPcmCfg.nI2sConfig)
    {
        if(sPcmCfg.sI2s.nTdmMode==1)
        {
            if(sPcmCfg.sI2s.nMsMode==0)
            {
                tI2sCfg.eMode = E_MHAL_AUDIO_MODE_TDM_MASTER;
            }
            else
            {
                tI2sCfg.eMode = E_MHAL_AUDIO_MODE_TDM_SLAVE;
            }
        }
        else if(sPcmCfg.sI2s.nTdmMode==0)
        {
            if(sPcmCfg.sI2s.nMsMode==0)
            {
                tI2sCfg.eMode = E_MHAL_AUDIO_MODE_I2S_MASTER;
            }
            else
            {
                tI2sCfg.eMode = E_MHAL_AUDIO_MODE_I2S_SLAVE;
            }
        }
        else
        {
            AUD_UT_PRINTF(ERROR_UT_LEVEL, "AUDRV_PCM_IOCTL I2S mode not support!!!\n");
            return -EFAULT;
        }

        tI2sCfg.eWidth = (sPcmCfg.sI2s.nBitWidth?E_MHAL_AUDIO_BITWIDTH_32:E_MHAL_AUDIO_BITWIDTH_16);

        tI2sCfg.eFmt = (sPcmCfg.sI2s.nFmt?E_MHAL_AUDIO_I2S_FMT_LEFT_JUSTIFY:E_MHAL_AUDIO_I2S_FMT_I2S);

        tI2sCfg.u16Channels = sPcmCfg.sI2s.u16Channels;

        tI2sCfg.eMck = sPcmCfg.sI2s.eMck;

        tI2sCfg.e4WireMode = sPcmCfg.sI2s.e4WireMode;

        tI2sCfg.eRate = tPcm.eRate;

        ret=MHAL_AUDIO_ConfigI2sOut(dev, &tI2sCfg);
        if(ret)
        {
            AUD_UT_PRINTF(ERROR_UT_LEVEL,"MHAL_AUDIO_ConfigI2sOut error=%d!!!\n",ret);
            return -EFAULT;
        }
    }
    total_size = sPcmCfg.nTimeMs*(sPcmCfg.nRate/1000)*sPcmCfg.n16Channels*(sPcmCfg.nBitWidth/8);

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "AO total_size %d!!!\n",total_size);

    if(sPcmCfg.n16Channels==1)
    {
        nStartThres = (buf_AO.bytes/2) - (sPcmCfg.n32PeriodSize*4);
    }
    else
    {
        nStartThres = (buf_AO.bytes/1) - (sPcmCfg.n32PeriodSize*4);
    }

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "AO threshold size %d\n",nStartThres);

    ret = MHAL_AUDIO_ConfigPcmOut(dev, &tPcm);
    if(ret)
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "MHAL_AUDIO_ConfigPcmOut error=%d!!!\n",ret);
        return -EFAULT;
    }

    ret = MHAL_AUDIO_OpenPcmOut(dev);
    if(ret)
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "MHAL_AUDIO_OpenPcmOut error=%d!!!\n",ret);
        goto fail;
    }

    stAoInfo.dev = dev;
    stAoInfo.total_size = total_size;
    stAoInfo.pDataArray = pDataArray;
    stAoInfo.nArrSize = nArrSize;
    stAoInfo.nStartThres = nStartThres;

    if ( use_thread != 0 )
    {
        CamOsThreadCreate(&TaskDma1RdHandle, NULL, (void *)_AoRun, &stAoInfo);
    }
    else
    {
        //_AoRun( &stAoInfo );
    }

    // wait thread return
    CamOsThreadJoin(TaskDma1RdHandle);

fail:

    ret = MHAL_AUDIO_StopPcmOut(dev);
    if(ret)
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "MHAL_AUDIO_StopPcmOut error=%d!!!\n",ret);
        //return -EFAULT;
    }

    ret = MHAL_AUDIO_ClosePcmOut(dev);
    if(ret)
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "MHAL_AUDIO_ClosePcmOut error=%d!!!\n",ret);
        return -EFAULT;
    }

    // free memory
    CamOsDirectMemRelease(buf_AO.area, DMA_BUF_SIZE);

    //
    CamOsPrintf("Ao PCM Test Finish !\n");

    return 0;

#else
    u32   nOverrunTh, nUnderrunTh, nDataSize, nActualSize, nTimes;
    bool  bOverrun, bUnderrun;
    BachDmaChannel_e eDmaChannel;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Dma1Reader\n");

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Drv audio dma reset\n");
    DrvAudioDmaReset();//HalAudioDmaReset();

    eDmaChannel = BACH_DMA_READER1;

    u8* pMcuNonCacheDmaBuf1 = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);

    if(pMcuNonCacheDmaBuf1 == NULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
        return;
    }

    memset(pMcuNonCacheDmaBuf1, 0 ,DMA_BUF_SIZE);
    DrvAudioDmaSetBufAddr(eDmaChannel, pMcuNonCacheDmaBuf1, DMA_BUF_SIZE);

    nOverrunTh = DMA_BUF_SIZE / 4 * 3;
    nUnderrunTh = DMA_BUF_SIZE / 4;
    DrvAudioDmaSetThreshold(eDmaChannel, nOverrunTh, nUnderrunTh);

    DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
    while(bUnderrun)
    {
        DrvAudioDmaPlayWriteData(eDmaChannel, tone_48k_2, sizeof(tone_48k_2), &nActualSize);
        DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
    }

    //_hAudioTestEnableDmaIrq();
    DrvAudioRegDmaIrqCB(_hAudioTestDmaIsr);
    DrvAudioEnableDmaIrq();

    /* init and start */
    DrvAudioDmaStart(eDmaChannel);

    // DrvAudioDmaStart already Enable DMA interrupt
    //DrvAudioDmaCtrlInt(eDmaChannel, TRUE, FALSE, TRUE);

    nTimes = 0;
    while(1)
    {
        DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
        while(bOverrun)
        {
            SYS_UDELAY(100);
            DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
        }

        DrvAudioDmaPlayWriteData(eDmaChannel, tone_48k_2, sizeof(tone_48k_2), &nActualSize);

        nTimes++;
        if(nTimes > 30000)
        {
            while(!HalAudioDmaIsEmpty(eDmaChannel));
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "empty cnt %d\n",HalAudioDmaGetLevelCnt(eDmaChannel));
            DrvAudioDmaStop(eDmaChannel);
            break;
        }
    }
    // free non-cache memory
    MsReleaseMemory(pMcuNonCacheDmaBuf1);
#endif
}

/**
* @brief
* Test Dma1 Writer
* @param[in]
*
* @retval
*
*/
static int _hDma1WriterTest(void)
{
#if 1
    struct AUD_PcmCfg_s sPcmCfg;
    MHAL_AUDIO_PcmCfg_t tPcm;
    MHAL_AUDIO_I2sCfg_t tI2sCfg;
    //MHAL_AUDIO_SineGenCfg_t tSineGenCfg;
    //struct pcm_data *pPcmData = (struct pcm_data *)filp->private_data;
    int dev = 0;//pPcmData->dev_id;
    int ret;
    u8 tmpBuf[2048];
    u8 channelSize[2];
    int total_size;
    //int i;
    //char fn[50];
    _AI_Info_t stAiInfo;
    CamOsThread TaskDma1WrHandle;

    // Config Parameter
    sPcmCfg.nRate = 48000;
    sPcmCfg.nBitWidth = 16;
    sPcmCfg.n16Channels = 2;
    sPcmCfg.nInterleaved = 1;
    sPcmCfg.n32PeriodSize = 1024;
    sPcmCfg.n32StartThres = 1<<30;
    sPcmCfg.nTimeMs = 5000; // 10000 -> 5000
    sPcmCfg.nI2sConfig = 0;
    sPcmCfg.sI2s.nTdmMode = 0;
    sPcmCfg.sI2s.nMsMode = 0;
    sPcmCfg.sI2s.nBitWidth = 0;
    sPcmCfg.sI2s.nFmt = 0;
    sPcmCfg.sI2s.u16Channels = 0;
    sPcmCfg.sI2s.e4WireMode = 0;
    sPcmCfg.sI2s.eMck = 0;

    //
    sPcmCfg.nSpecialDumpChannels = 0;

    unsigned char *phyPtrTmp = NULL;
    unsigned char *phyPtr = NULL;

    //buf.area = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);

    if ( CamOsDirectMemAlloc("aud_writer", DMA_BUF_SIZE, (void**)&buf.area, (void**)&phyPtrTmp, (void**)&phyPtr) != CAM_OS_OK )
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "CamOsDirectMemAlloc aud_writer fail\n");
    }

    if(buf.area == NULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
        return -1;
    }

    buf.addr = (dma_addr_t)phyPtr;
    buf.bytes = DMA_BUF_SIZE;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "buf.area = 0x%x\n",buf.area);
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "buf.addr = 0x%x\n",buf.addr);
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "buf.bytes = %d\n",buf.bytes);

    memset(buf.area, 0 ,DMA_BUF_SIZE);

    //
    if ( CamOsDirectMemAlloc("DUMP BUFFER", DUMP_BUF_SIZE, &pVirtPtr, &pPhysAddr, &pMiuAddr) != CAM_OS_OK )
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "CamOsDirectMemAlloc DUMP BUFFER fail\n");
    }

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "[CAM_OS_WRAPPER_TEST:%d] CamOsDirectMemAlloc get addr:  pVirtPtr 0x%08X  pPhysAddr 0x%08X  pMiuAddr 0x%08X\n",
                __LINE__, (u32)pVirtPtr, (u32)pPhysAddr, (u32)pMiuAddr);

    memset(pVirtPtr, 0 ,DUMP_BUF_SIZE);

    if(((1 != sPcmCfg.n16Channels) && (0 != sPcmCfg.n16Channels%2)) || (4 < sPcmCfg.n16Channels))
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "Channels set error!!. \n");
        return -1;
    }

#if 0
    // File Open
    if ( sPcmCfg.nSpecialDumpChannels != 0 )
    {
        fw = (struct file**)kmalloc(sizeof(struct file*)*sPcmCfg.nSpecialDumpChannels,GFP_KERNEL);

        for(i=0; i<sPcmCfg.nSpecialDumpChannels; i++)
        {
            sprintf(fn,"/tmp/dump_%d_ch%d_%d.pcm", sPcmCfg.nRate, i, sPcmCfg.nBitWidth);
            *(fw+i) = filp_open(fn, O_RDWR | O_CREAT, 0644);

            if(IS_ERR(*(fw+i)))
            {
                AUD_UT_PRINTF(ERROR_UT_LEVEL, "Test: filp_open error!!. \n");
                return -1;
            }
        }
    }
    else
    {
        fw = (struct file**)kmalloc(sizeof(struct file*)*sPcmCfg.n16Channels,GFP_KERNEL);

        if(sPcmCfg.nInterleaved)
        {
            //fw = (struct file**)kmalloc(sizeof(struct file*),GFP_KERNEL);

            if(sPcmCfg.n16Channels == 4)
            {
                for(i=0; i<sPcmCfg.n16Channels/2; i++)
                {
                    sprintf(fn,"/tmp/rec_%d_ch%d%d_%d.pcm", sPcmCfg.nRate, i*2, (i*2)+1, sPcmCfg.nBitWidth);
                    *(fw+i) = filp_open(fn, O_RDWR | O_CREAT, 0644);

                    if(IS_ERR(*(fw+i)))
                    {
                        AUD_UT_PRINTF(ERROR_UT_LEVEL, "Test: filp_open error!!. \n");
                        return -1;
                    }
                }
            }
            else
            {
                sprintf(fn,"/tmp/rec_%d_%dch_%d.pcm",sPcmCfg.nRate,sPcmCfg.n16Channels,sPcmCfg.nBitWidth);
                *fw = filp_open(fn, O_RDWR | O_CREAT, 0644);

                if(IS_ERR(*fw))
                {
                    AUD_UT_PRINTF(ERROR_UT_LEVEL, "Test: filp_open error!!. \n");
                    return -1;
                }
            }
        }
        else
        {
            //fw = (struct file**)kmalloc(sizeof(struct file*)*sPcmCfg.n16Channels,GFP_KERNEL);
            for(i=0; i<sPcmCfg.n16Channels; i++)
            {
                sprintf(fn,"/tmp/rec_%d_ch%d_%d.pcm",sPcmCfg.nRate,i,sPcmCfg.nBitWidth);
                *(fw+i) = filp_open(fn, O_RDWR | O_CREAT, 0644);

                if(IS_ERR(*(fw+i)))
                {
                    AUD_UT_PRINTF(ERROR_UT_LEVEL, "Test: filp_open error!!. \n");
                    return -1;
                }
            }
        }
    }
#endif
    //
    if(sPcmCfg.nRate==8000)
        tPcm.eRate = E_MHAL_AUDIO_RATE_8K;
    else if(sPcmCfg.nRate==16000)
        tPcm.eRate = E_MHAL_AUDIO_RATE_16K;
    else if(sPcmCfg.nRate==32000)
        tPcm.eRate = E_MHAL_AUDIO_RATE_32K;
    else if(sPcmCfg.nRate==48000)
        tPcm.eRate = E_MHAL_AUDIO_RATE_48K;
    else
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "AUDRV_PCM_IOCTL sample rate %d not support!!!\n",sPcmCfg.nRate);
        return -EFAULT;
    }

    if(sPcmCfg.nBitWidth==16)
        tPcm.eWidth = E_MHAL_AUDIO_BITWIDTH_16;
    else if(sPcmCfg.nBitWidth==32)
        tPcm.eWidth = E_MHAL_AUDIO_BITWIDTH_32;
    else
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "AUDRV_PCM_IOCTL bitwidth %d not support!!!\n",sPcmCfg.nBitWidth);
        return -EFAULT;
    }
    tPcm.u16Channels = sPcmCfg.n16Channels;
    tPcm.bInterleaved = sPcmCfg.nInterleaved;
    tPcm.pu8DmaArea = buf.area;
    tPcm.phyDmaAddr = buf.addr-MIU_OFFSET;
    tPcm.u32BufferSize = buf.bytes;
    tPcm.u32PeriodSize = sPcmCfg.n32PeriodSize;//7680;//1024*2*2*6;
    tPcm.u32StartThres = sPcmCfg.n32StartThres;//buf.bytes+1;//buf.bytes/2;

    if(sPcmCfg.nI2sConfig)
    {
        if(sPcmCfg.sI2s.nTdmMode==1)
        {
            if(sPcmCfg.sI2s.nMsMode==0)
            {
                tI2sCfg.eMode = E_MHAL_AUDIO_MODE_TDM_MASTER;
            }
            else
            {
                tI2sCfg.eMode = E_MHAL_AUDIO_MODE_TDM_SLAVE;
            }
        }
        else if(sPcmCfg.sI2s.nTdmMode==0)
        {
            if(sPcmCfg.sI2s.nMsMode==0)
            {
                tI2sCfg.eMode = E_MHAL_AUDIO_MODE_I2S_MASTER;
            }
            else
            {
                tI2sCfg.eMode = E_MHAL_AUDIO_MODE_I2S_SLAVE;
            }
        }
        else
        {
            AUD_UT_PRINTF(ERROR_UT_LEVEL, "AUDRV_PCM_IOCTL I2S mode not support!!!\n");
            return -EFAULT;
        }

        tI2sCfg.eWidth = (sPcmCfg.sI2s.nBitWidth?E_MHAL_AUDIO_BITWIDTH_32:E_MHAL_AUDIO_BITWIDTH_16);

        tI2sCfg.eFmt = (sPcmCfg.sI2s.nFmt?E_MHAL_AUDIO_I2S_FMT_LEFT_JUSTIFY:E_MHAL_AUDIO_I2S_FMT_I2S);

        tI2sCfg.u16Channels = sPcmCfg.sI2s.u16Channels;

        tI2sCfg.eMck = sPcmCfg.sI2s.eMck;

        tI2sCfg.e4WireMode = sPcmCfg.sI2s.e4WireMode;

        tI2sCfg.eRate = tPcm.eRate;

        ret=MHAL_AUDIO_ConfigI2sIn(dev, &tI2sCfg);
        if(ret)
        {
            AUD_UT_PRINTF(ERROR_UT_LEVEL,"MHAL_AUDIO_ConfigI2sIn error=%d!!!\n",ret);
            return -EFAULT;
        }
    }

    //
    total_size = sPcmCfg.nTimeMs*(sPcmCfg.nRate/1000)*sPcmCfg.n16Channels*(sPcmCfg.nBitWidth/8);

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "AI total_size %d!!!\n",total_size);

    ret = MHAL_AUDIO_ConfigPcmIn(dev, &tPcm);
    if(ret)
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "MHAL_AUDIO_ConfigPcmIn error=%d!!!\n",ret);
        return -EFAULT;
    }
    ret = MHAL_AUDIO_OpenPcmIn(dev);
    if(ret)
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "MHAL_AUDIO_OpenPcmIn error=%d!!!\n",ret);
        goto fail;
    }

    stAiInfo.dev = dev;
    stAiInfo.total_size = total_size;
    stAiInfo.tmpBuf = tmpBuf;
    stAiInfo.tmpBufSize = sizeof(tmpBuf);
    stAiInfo.channels = sPcmCfg.n16Channels;
    stAiInfo.channelSize = sizeof(channelSize);
    stAiInfo.interleaved = sPcmCfg.nInterleaved;
    stAiInfo.nSpecialDumpChannels = sPcmCfg.nSpecialDumpChannels;

    if ( use_thread != 0 )
    {
        CamOsThreadCreate(&TaskDma1WrHandle, NULL, (void *)_AiRun, &stAiInfo);
    }
    else
    {
        //_AiRun( &stAiInfo );
    }

    // wait thread return
    CamOsThreadJoin(TaskDma1WrHandle);

fail:


#if 0
    // File Close
    if ( sPcmCfg.nSpecialDumpChannels != 0 )
    {
        for(i=0; i<sPcmCfg.nSpecialDumpChannels; i++)
        {
            filp_close(*(fw+i), NULL);
        }
        kfree(fw);
    }
    else
    {
        if(sPcmCfg.nInterleaved)
        {
            if(sPcmCfg.n16Channels == 4)
            {
                for(i=0; i<sPcmCfg.n16Channels/2; i++)
                {
                    filp_close(*(fw+i), NULL);
                }
                kfree(fw);
            }
            else
            {
                filp_close((*fw), NULL);
                kfree(fw);
            }
        }
        else
        {
            for(i=0; i<sPcmCfg.n16Channels; i++)
            {
                filp_close(*(fw+i), NULL);
            }
            kfree(fw);
        }
    }
#endif

    //
    ret = MHAL_AUDIO_StopPcmIn(dev);
    if(ret)
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "MHAL_AUDIO_StopPcmIn error=%d!!!\n",ret);
        //return -EFAULT;
    }
    ret = MHAL_AUDIO_ClosePcmIn(dev);
    if(ret)
    {
        AUD_UT_PRINTF(ERROR_UT_LEVEL, "MHAL_AUDIO_ClosePcmIn error=%d!!!\n",ret);
        return -EFAULT;
    }

    // free memory
    CamOsDirectMemRelease(buf.area, DMA_BUF_SIZE);
    CamOsDirectMemRelease(pVirtPtr, DUMP_BUF_SIZE);

    //
    CamOsPrintf("Ai PCM Test Finish !\n");

    return 0;

#else
    u32   nOverrunTh, nUnderrunTh, nDataSize, nReadSize;
    bool  bOverrun, bUnderrun;
    u8   *ptr, pBuf[192];
    BachDmaChannel_e eDmaChannel;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Dma1Writer\n");

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Drv audio dma reset\n");
    DrvAudioDmaReset();//HalAudioDmaReset();

    eDmaChannel = BACH_DMA_WRITER1;

    u8* pMcuNonCacheDmaBuf1 = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);
    if(pMcuNonCacheDmaBuf1 == NULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
        return;
    }
    memset(pMcuNonCacheDmaBuf1, 0 ,DMA_BUF_SIZE);

    //memset(_gMcuDmaBuf1, 0, sizeof(_gMcuDmaBuf1));
    //memset(_gMcuDmaBuf2, 0, sizeof(_gMcuDmaBuf2));
    memset(_gMcuDmaBuf4, 0, sizeof(_gMcuDmaBuf4));

    DrvAudioDmaSetBufAddr(eDmaChannel, pMcuNonCacheDmaBuf1, DMA_BUF_SIZE);

    nOverrunTh = DMA_BUF_SIZE / 4 * 3;
    nUnderrunTh = DMA_BUF_SIZE / 4;

    DrvAudioDmaSetThreshold(eDmaChannel, nOverrunTh, nUnderrunTh);


    //_hAudioTestEnableDmaIrq();
    DrvAudioRegDmaIrqCB(_hAudioTestDmaIsr);
    DrvAudioEnableDmaIrq();

    /* init and start */
    DrvAudioDmaStart(eDmaChannel);

    //DrvAudioDmaCtrlInt(eDmaChannel,  TRUE, TRUE,  FALSE);

    //ptr = _gMcuDmaBuf2;
    ptr = _gMcuDmaBuf4;

    while(1)
    {
        DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
        //HalAudioDmaRecGetBufInfo(&nDataSize, &bOverrun, &bUnderrun);
        while(bUnderrun)
        {
            SYS_UDELAY(100);
            DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
            //HalAudioDmaRecGetBufInfo(&nDataSize, &bOverrun, &bUnderrun);
        }

        DrvAudioDmaRecReadData(eDmaChannel, pBuf, sizeof(pBuf), &nReadSize);
        //HalAudioDmaRecReadData(pBuf, sizeof(pBuf), &nReadSize);

        if((u32)ptr+nReadSize > (u32)_gMcuDmaBuf4 + sizeof(_gMcuDmaBuf4))
        {
            memcpy(ptr, pBuf, _gMcuDmaBuf4 + sizeof(_gMcuDmaBuf4)-ptr);
            ptr = _gMcuDmaBuf4 + sizeof(_gMcuDmaBuf4);
        }
        else
        {
            memcpy(ptr, pBuf, nReadSize);
            ptr += nReadSize;
        }

        if((u32)ptr >= (u32)_gMcuDmaBuf4 + sizeof(_gMcuDmaBuf4))
        {
            break;
        }
    }

    while(!HalAudioDmaIsFull(eDmaChannel));
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "full cnt %d\n",HalAudioDmaGetLevelCnt(eDmaChannel));

    DrvAudioDmaStop(eDmaChannel);//HalAudioDmaRecStop();
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "DMA writer stop, output buffer=%x, size=%x\n", (u32)_gMcuDmaBuf4, sizeof(_gMcuDmaBuf4));

    MsReleaseMemory(pMcuNonCacheDmaBuf1);
#endif
}
/**
* @brief
* Test Dma1 Reader using DMA Isr to write data
* @param[in]
*
* @retval
*
*/
#if 0
static void _hDma1ReaderIsrTest(void)
{
    u32   nOverrunTh, nUnderrunTh, nDataSize, nActualSize;
    bool  bOverrun, bUnderrun;
    BachDmaChannel_e eDmaChannel;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Dma1Reader\n");

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Drv audio dma reset\n");
    DrvAudioDmaReset();//HalAudioDmaReset();

    eDmaChannel = BACH_DMA_READER1;

    u8* pMcuNonCacheDmaBuf1 = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);

    if(pMcuNonCacheDmaBuf1 == NULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
        return;
    }

    memset(pMcuNonCacheDmaBuf1, 0 ,DMA_BUF_SIZE);
    DrvAudioDmaSetBufAddr(eDmaChannel, pMcuNonCacheDmaBuf1, DMA_BUF_SIZE);

    nOverrunTh = DMA_BUF_SIZE / 4 * 3;
    nUnderrunTh = DMA_BUF_SIZE / 4;
    DrvAudioDmaSetThreshold(eDmaChannel, nOverrunTh, nUnderrunTh);

    DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
    while(bUnderrun)
    {
        DrvAudioDmaPlayWriteData(eDmaChannel, tone_48k_2, sizeof(tone_48k_2), &nActualSize);
        DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
    }

    //_hAudioTestEnableDmaIrq();
    DrvAudioRegDmaIrqCB(_hAudioTestDmaIsrProcData);
    DrvAudioEnableDmaIrq();

    /* init and start */
    DrvAudioDmaStart(eDmaChannel);

    // DrvAudioDmaStart already Enable DMA interrupt
    //DrvAudioDmaCtrlInt(eDmaChannel, TRUE, FALSE, TRUE);

    _gnWriteTotalSize = 0;
    while(1)
    {
        DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
        if(_gnWriteTotalSize > DMA_BUF_SIZE*20)
        {
            DrvAudioDmaStop(eDmaChannel);
            break;
        }

    }

    // free non-cache memory
   MsReleaseMemory(pMcuNonCacheDmaBuf1);
}
#endif

/**
* @brief
* Test Dma1 Reader using DMA Isr and creat thread to write data
* @param[in]
*
* @retval
*
*/
#if 0
void _hDma1ReaderIsrThreadTest(void)
{
    u32   nOverrunTh, nUnderrunTh, nDataSize;
    bool  bOverrun, bUnderrun;
    BachDmaChannel_e eDmaChannel;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Dma1Reader\n");

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Drv audio dma reset\n");
    DrvAudioDmaReset();//HalAudioDmaReset();

    eDmaChannel = BACH_DMA_READER1;

    u8* pMcuNonCacheDmaBuf1 = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);

    if(pMcuNonCacheDmaBuf1 == NULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
        return;
    }

    memset(pMcuNonCacheDmaBuf1, 0 ,DMA_BUF_SIZE);
    DrvAudioDmaSetBufAddr(eDmaChannel, pMcuNonCacheDmaBuf1, DMA_BUF_SIZE);

    nOverrunTh = DMA_BUF_SIZE / 4 * 3;
    nUnderrunTh = DMA_BUF_SIZE / 4;
    DrvAudioDmaSetThreshold(eDmaChannel, nOverrunTh, nUnderrunTh);

    //_hAudioTestDmaIsrFlag();
    DrvAudioRegDmaIrqCB(_hAudioTestDmaIsrFlag);
    DrvAudioEnableDmaIrq();

    // init the flag
    memset(&_tDmaFlag, 0, sizeof(Ms_Flag_t));
    MsFlagInit(&_tDmaFlag);

    //creat a thread to write data
    CamOsThread TaskDma1RdHandle;
    CamOsThreadCreate(&TaskDma1RdHandle, NULL, (void *)_CamOsThreadTestDma1Reader, NULL);

    /* init and start */
    //DrvAudioDmaStart(eDmaChannel);

    // DrvAudioDmaStart already Enable DMA interrupt
    //DrvAudioDmaCtrlInt(eDmaChannel, TRUE, FALSE, TRUE);

    _gnWriteTotalSize = 0;
    while(1)
    {
        DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
        if(_gnWriteTotalSize > DMA_BUF_SIZE*20)
        {
            DrvAudioDmaStop(eDmaChannel);
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "Drv audio dma1 reader stop\n");
            break;
        }

    }

    // wait thread return
    CamOsThreadJoin(TaskDma1RdHandle);

    // destroy the flag
    MsFlagDestroy(&_tDmaFlag);

    // free non-cache memory
    MsReleaseMemory(pMcuNonCacheDmaBuf1);
}
#endif

/**
* @brief
* Test Dma1 Writer using DMA Isr to read data
* @param[in]
*
* @retval
*
*/
#if 0
static void _hDma1WriterIsrTest(void)
{
    u32   nOverrunTh, nUnderrunTh, nDataSize;
    bool  bOverrun, bUnderrun;
    BachDmaChannel_e eDmaChannel;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Dma1Writer\n");

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Drv audio dma reset\n");
    DrvAudioDmaReset();//HalAudioDmaReset();

    eDmaChannel = BACH_DMA_WRITER1;

    u8* pMcuNonCacheDmaBuf1 = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);
    if(pMcuNonCacheDmaBuf1 == NULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
        return;
    }
    memset(pMcuNonCacheDmaBuf1, 0 ,DMA_BUF_SIZE);

    //memset(_gMcuDmaBuf1, 0, sizeof(_gMcuDmaBuf1));
    //memset(_gMcuDmaBuf2, 0, sizeof(_gMcuDmaBuf2));
    memset(_gMcuDmaBuf4, 0, sizeof(_gMcuDmaBuf4));

    DrvAudioDmaSetBufAddr(eDmaChannel, pMcuNonCacheDmaBuf1, DMA_BUF_SIZE);

    nOverrunTh = DMA_BUF_SIZE / 4 * 3;
    nUnderrunTh = DMA_BUF_SIZE / 4;

    DrvAudioDmaSetThreshold(eDmaChannel, nOverrunTh, nUnderrunTh);


    //_hAudioTestEnableDmaIrq();
    DrvAudioRegDmaIrqCB(_hAudioTestDmaIsrProcData);
    DrvAudioEnableDmaIrq();

    /* init and start */
    DrvAudioDmaStart(eDmaChannel);

    //DrvAudioDmaCtrlInt(eDmaChannel,  TRUE, TRUE,  FALSE);

    //ptr = _gMcuDmaBuf4;

    _gnReadTotalSize = 0;
    while(1)
    {
        DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
        if(_gnReadTotalSize >= sizeof(_gMcuDmaBuf4))
            break;
    }
    DrvAudioDmaStop(eDmaChannel);//HalAudioDmaRecStop();
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "DMA writer stop, output buffer=%x, size=%x\n", (u32)_gMcuDmaBuf4, sizeof(_gMcuDmaBuf4));

    MsReleaseMemory(pMcuNonCacheDmaBuf1);
}
#endif

/**
* @brief
* Test Dma1 Writer using DMA Isr and creat thread to read data
* @param[in]
*
* @retval
*
*/
#if 0
void _hDma1WriterIsrThreadTest(void)
{
    u32   nOverrunTh, nUnderrunTh, nDataSize;
    bool  bOverrun, bUnderrun;
    BachDmaChannel_e eDmaChannel;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Dma1Writer\n");

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Drv audio dma reset\n");
    DrvAudioDmaReset();//HalAudioDmaReset();

    eDmaChannel = BACH_DMA_WRITER1;

    u8* pMcuNonCacheDmaBuf1 = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);
    if(pMcuNonCacheDmaBuf1 == NULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
        return;
    }
    memset(pMcuNonCacheDmaBuf1, 0 ,DMA_BUF_SIZE);

    //memset(_gMcuDmaBuf1, 0, sizeof(_gMcuDmaBuf1));
    //memset(_gMcuDmaBuf2, 0, sizeof(_gMcuDmaBuf2));
    memset(_gMcuDmaBuf4, 0, sizeof(_gMcuDmaBuf4));

    DrvAudioDmaSetBufAddr(eDmaChannel, pMcuNonCacheDmaBuf1, DMA_BUF_SIZE);

    nOverrunTh = DMA_BUF_SIZE / 4 * 3;
    nUnderrunTh = DMA_BUF_SIZE / 4;

    DrvAudioDmaSetThreshold(eDmaChannel, nOverrunTh, nUnderrunTh);

    //_hAudioTestDmaIsrFlag();
    DrvAudioRegDmaIrqCB(_hAudioTestDmaIsrFlag);
    DrvAudioEnableDmaIrq();

    // init the flag
    memset(&_tDmaFlag, 0, sizeof(Ms_Flag_t));
    MsFlagInit(&_tDmaFlag);

    //creat a thread to read data
    CamOsThread TaskDma1WrHandle;
    CamOsThreadCreate(&TaskDma1WrHandle, NULL, (void *)_CamOsThreadTestDma1Writer, NULL);

    //DrvAudioDmaCtrlInt(eDmaChannel,  TRUE, TRUE,  FALSE);

    //ptr = _gMcuDmaBuf4;

    _gnReadTotalSize = 0;
    while(1)
    {
        DrvAudioDmaGetBufInfo(eDmaChannel, &nDataSize, &bOverrun, &bUnderrun);
        if(_gnReadTotalSize >= sizeof(_gMcuDmaBuf4))
            break;
    }
    DrvAudioDmaStop(eDmaChannel);//HalAudioDmaRecStop();
    AUD_UT_PRINTF(TRACE_UT_LEVEL, "DMA writer stop, output buffer=%x, size=%x\n", (u32)_gMcuDmaBuf4, sizeof(_gMcuDmaBuf4));

    // wait thread return
    CamOsThreadJoin(TaskDma1WrHandle);

    // destroy the flag
    MsFlagDestroy(&_tDmaFlag);

    MsReleaseMemory(pMcuNonCacheDmaBuf1);
}
#endif

#if 0
static void _CamOsThreadTestDma1Reader(void)
{
    u32  nActualSize, nDataSize, nBufSpaceSize;
    bool bOverrun, bUnderrun;
    rtk_flag_value_t nFlagValue = 0;

    // if disable this section, can test DMA1 reader empty
    DrvAudioDmaGetBufInfo(BACH_DMA_READER1, &nDataSize, &bOverrun, &bUnderrun);
    while(bUnderrun)
    {
        DrvAudioDmaPlayWriteData(BACH_DMA_READER1, tone_48k_2, sizeof(tone_48k_2), &nActualSize);
        DrvAudioDmaGetBufInfo(BACH_DMA_READER1, &nDataSize, &bOverrun, &bUnderrun);
    }

    // Start DMA1 Reader
    DrvAudioDmaStart(BACH_DMA_READER1);

    while(1)
    {
        // wait flag
        //MsFlagWait(&_tDmaFlag, HAL_AUDIO_DMA1_RD_UNDERRUN, RTK_FLAG_WAITMODE_AND);
        nFlagValue = MsFlagWait(&_tDmaFlag, HAL_AUDIO_DMA1_RD_UNDERRUN | HAL_AUDIO_DMA1_RD_EMPTY, RTK_FLAG_WAITMODE_OR);

        if(nFlagValue & HAL_AUDIO_DMA1_RD_EMPTY)
        {
            DrvAudioDmaGetBufInfo(BACH_DMA_READER1, &nDataSize, &bOverrun, &bUnderrun);
            while(bUnderrun)
            {
                DrvAudioDmaPlayWriteData(BACH_DMA_READER1, tone_48k_2, sizeof(tone_48k_2), &nActualSize);
                DrvAudioDmaGetBufInfo(BACH_DMA_READER1, &nDataSize, &bOverrun, &bUnderrun);
            }
            // Start DMA1 Reader
            DrvAudioDmaStart(BACH_DMA_READER1);
        }


        //write data to Dma1Reader Dram
        DrvAudioDmaGetBufInfo(BACH_DMA_READER1, &nDataSize, &bOverrun, &bUnderrun);
        nBufSpaceSize =  DMA_BUF_SIZE - nDataSize;
        while(nBufSpaceSize > 2 * sizeof(tone_48k_2))
        {
            DrvAudioDmaPlayWriteData(BACH_DMA_READER1, tone_48k_2, sizeof(tone_48k_2), &nActualSize);
            _gnWriteTotalSize += nActualSize;
            DrvAudioDmaGetBufInfo(BACH_DMA_READER1, &nDataSize, &bOverrun, &bUnderrun);
            nBufSpaceSize =  DMA_BUF_SIZE - nDataSize;
        }

        DrvAudioDmaCtrlInt(BACH_DMA_READER1, TRUE, FALSE, TRUE);

        if(_gnWriteTotalSize > DMA_BUF_SIZE*20)
            break;
    }
}
#endif

#if 0
static void _CamOsThreadTestDma1Writer(void)
{
    u32 nReadSize, nDataSize;
    bool bOverrun, bUnderrun;
    u8 pBuf[192];
    rtk_flag_value_t nFlagValue = 0;

    /* init and start */
    DrvAudioDmaStart(BACH_DMA_WRITER1);


    while(1)
    {
        // wait flag
        //MsFlagWait(&_tDmaFlag, HAL_AUDIO_DMA1_WR_OVERRUN, RTK_FLAG_WAITMODE_AND);
        nFlagValue  = MsFlagWait(&_tDmaFlag, HAL_AUDIO_DMA1_WR_OVERRUN | HAL_AUDIO_DMA1_WR_FULL, RTK_FLAG_WAITMODE_OR);

        if(nFlagValue & HAL_AUDIO_DMA1_WR_FULL)
        {
            DrvAudioDmaStart(BACH_DMA_WRITER1);
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "DMA1 writer restart for buffer full \n");
        }

        //Read data from Dma1Writer
        DrvAudioDmaGetBufInfo(BACH_DMA_WRITER1, &nDataSize, &bOverrun, &bUnderrun);

        while(nDataSize > 2*sizeof(pBuf))
        {
            DrvAudioDmaRecReadData(BACH_DMA_WRITER1, pBuf, sizeof(pBuf), &nReadSize);

            if((u32)_gMcuDmaBuf4+_gnReadTotalSize+nReadSize > (u32)_gMcuDmaBuf4 + sizeof(_gMcuDmaBuf4))
            {
                memcpy(_gMcuDmaBuf4+_gnReadTotalSize, pBuf,  sizeof(_gMcuDmaBuf4)-_gnReadTotalSize);
                //ptr = _gMcuDmaBuf4 + sizeof(_gMcuDmaBuf4);
                _gnReadTotalSize = sizeof(_gMcuDmaBuf4);
            }
            else
            {
                memcpy(_gMcuDmaBuf4+_gnReadTotalSize, pBuf, nReadSize);
                _gnReadTotalSize += nReadSize;
            }
            //_gnReadTotalSize += nReadSize;
            DrvAudioDmaGetBufInfo(BACH_DMA_WRITER1, &nDataSize, &bOverrun, &bUnderrun);
        }

        DrvAudioDmaCtrlInt(BACH_DMA_WRITER1, TRUE, TRUE, FALSE);


        if(_gnReadTotalSize >= sizeof(_gMcuDmaBuf4))
            break;
    }
}
#endif

/**
* @brief
* Test Dma1 Reader/Writer using thread to Write/Read data together
* @param[in]
*
* @retval
*
*/
#if 0
static void _hDma1ReaderWriterIsrThreadTest(void)
{
    u32   nOverrunTh, nUnderrunTh, nDataSize, nActualSize;
    bool  bOverrun, bUnderrun;
    BachDmaChannel_e eDmaChannel1, eDmaChannel2;

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Dma1 Reader/Writer \n");

    AUD_UT_PRINTF(TRACE_UT_LEVEL, "Drv audio dma reset\n");
    DrvAudioDmaReset();//HalAudioDmaReset();

    eDmaChannel1 = BACH_DMA_READER1;
    eDmaChannel2 = BACH_DMA_WRITER1;

    u8* pMcuNonCacheDmaBuf1 = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);
    u8* pMcuNonCacheDmaBuf2 = MsAllocateNonCacheMemExt(DMA_BUF_SIZE, 6);

    if(pMcuNonCacheDmaBuf1 == NULL || pMcuNonCacheDmaBuf2 == NULL)
    {
        AUD_UT_PRINTF(TRACE_UT_LEVEL, "Allocate memory fail\n");
        return;
    }

    memset(pMcuNonCacheDmaBuf1, 0 ,DMA_BUF_SIZE);
    DrvAudioDmaSetBufAddr(eDmaChannel1, pMcuNonCacheDmaBuf1, DMA_BUF_SIZE);

    memset(pMcuNonCacheDmaBuf2, 0 ,DMA_BUF_SIZE);
    DrvAudioDmaSetBufAddr(eDmaChannel2, pMcuNonCacheDmaBuf2, DMA_BUF_SIZE);

    memset(_gMcuDmaBuf4, 0, sizeof(_gMcuDmaBuf4));

    nOverrunTh = DMA_BUF_SIZE / 4 * 3;
    nUnderrunTh = DMA_BUF_SIZE / 4;
    DrvAudioDmaSetThreshold(eDmaChannel1, nOverrunTh, nUnderrunTh);
    DrvAudioDmaSetThreshold(eDmaChannel2, nOverrunTh, nUnderrunTh);

    //_hAudioTestDmaIsrFlag();
    DrvAudioRegDmaIrqCB(_hAudioTestDmaIsrFlag);
    DrvAudioEnableDmaIrq();

    // init the flag
    memset(&_tDmaFlag, 0, sizeof(Ms_Flag_t));
    MsFlagInit(&_tDmaFlag);

    //creat two thread to write data and Read data
    CamOsThread TaskDma1RdHandle, TaskDma1WrHandle;
    CamOsThreadCreate(&TaskDma1RdHandle, NULL, (void *)_CamOsThreadTestDma1Reader, NULL);
    CamOsThreadCreate(&TaskDma1WrHandle, NULL, (void *)_CamOsThreadTestDma1Writer, NULL);

    /* init and start */
    //DrvAudioDmaStart(eDmaChannel1);
    //DrvAudioDmaStart(eDmaChannel2);

    // DrvAudioDmaStart already Enable DMA interrupt
    //DrvAudioDmaCtrlInt(eDmaChannel, TRUE, FALSE, TRUE);

    _gnWriteTotalSize = 0;
    _gnReadTotalSize = 0;
    while(1)
    {
        DrvAudioDmaGetBufInfo(eDmaChannel1, &nDataSize, &bOverrun, &bUnderrun);
        if(_gnWriteTotalSize > DMA_BUF_SIZE*20 && _gnReadTotalSize >= sizeof(_gMcuDmaBuf4))
        {
            DrvAudioDmaStop(eDmaChannel1);
            DrvAudioDmaStop(eDmaChannel2);
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "DMA1 Reader reader stop\n");
            AUD_UT_PRINTF(TRACE_UT_LEVEL, "DMA1 Writer stop, output buffer=%x, size=%x\n", (u32)_gMcuDmaBuf4, sizeof(_gMcuDmaBuf4));
            break;
        }

    }

    // wait thread return
    CamOsThreadJoin(TaskDma1RdHandle);
    CamOsThreadJoin(TaskDma1WrHandle);

    // destroy the flag
    MsFlagDestroy(&_tDmaFlag);

    // free non-cache memory
    MsReleaseMemory(pMcuNonCacheDmaBuf1);
    MsReleaseMemory(pMcuNonCacheDmaBuf2);
}
#endif