#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdlib.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"

#include <ubi_uboot.h>

#if defined(CONFIG_FS_LITTLEFS)
#include "littlefs.h"
#elif defined(CONFIG_FS_FIRMWAREFS)
#include "firmwarefs.h"
#else
#error "CONFIG_FS_LITTLEFS or CONFIG_FS_FIRMWAREFS must be defined"
#endif

#if defined(CONFIG_CMD_MTDPARTS)
#include <jffs2/jffs2.h>
/* partition handling routines */
int mtdparts_init(void);
int find_dev_and_part(const char *id, struct mtd_device **dev, u8 *part_num, struct part_info **part);
#endif

extern void Chip_Flush_Memory(void);

#include "mhal_audio.h"

#define MIU_OFFSET 0x20000000

static char bStartPcmOut = FALSE;

typedef struct WAVE_FORMAT
{
    signed short wFormatTag;
    signed short wChannels;
    unsigned int dwSamplesPerSec;
    unsigned int dwAvgBytesPerSec;
    signed short wBlockAlign;
    signed short wBitsPerSample;
} WaveFormat_t;

typedef struct WAVEFILEHEADER
{
    char chRIFF[4];
    unsigned int  dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    unsigned int  dwFMTLen;
    WaveFormat_t wave;
    char chDATA[4];
    unsigned int  dwDATALen;
} WaveFileHeader_t;


static int getBootMusicReservedMem(u32* pu32Addr, u32* pu32Size)
{
    char* pEnvStr = NULL;
    char* pHandle = NULL;
    s32 s32Miu = 0;
    char szSize[128];
    char startAddr[128];
    char endAddr[128];

    *pu32Addr = 0;
    *pu32Size = 0;

    pEnvStr = getenv("bootargs");
    if (!pEnvStr)
    {
        return -1;
    }

    pHandle = strstr(pEnvStr, "mmap_reserved=ao");
    if (!pHandle)
    {
        return -1;
    }

    sscanf(pHandle, "mmap_reserved=ao,miu=%d,sz=%[^,],max_start_off=%[^,],max_end_off=%[^ ]", &s32Miu, szSize, startAddr, endAddr);
    *pu32Size = simple_strtoul(szSize, NULL, 16);
    *pu32Addr = simple_strtoul(startAddr, NULL, 16);
    printf("Parsing bootargs size 0x%x addr 0x%x\n", *pu32Size, *pu32Addr);

    flush_dcache_range(*pu32Addr, *pu32Addr + *pu32Size);
    Chip_Flush_Memory();

    return 0;
}

static S32 fs_mount(char *partition, char *mnt_path)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_mount(partition, mnt_path);
#else
    return firmwarefs_mount(partition, mnt_path);
#endif
}

static void fs_unmount(void)
{
#if defined(CONFIG_FS_LITTLEFS)
    littlefs_unmount();
#else
    firmwarefs_unmount();
#endif
}

static void *fs_open(char *filename, U32 flags, U32 mode)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_open(filename, flags, mode);
#else
    return firmwarefs_open(filename, flags, mode);
#endif
}

static S32 fs_close(void *fd)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_close(fd);
#else
    return firmwarefs_close(fd);
#endif
}

static S32 fs_read(void *fd, void *buf, U32 count)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_read(fd, buf, count);
#else
    return firmwarefs_read(fd, buf, count);
#endif
}

#if 0
static S32 fs_write(void *fd, void *buf, U32 count)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_write(fd, buf, count);
#else
    return firmwarefs_write(fd, buf, count);
#endif
}

static S32 fs_lseek(void *fd, S32 offset, S32 whence)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_lseek(fd, offset, whence);
#else
    return firmwarefs_lseek(fd, offset, whence);
#endif
}
#endif

static int openWavFile(void **pFd)
{
    char partitionName[] = "MISC";
    void *fd = NULL;

#if defined(CONFIG_CMD_MTDPARTS) || defined(CONFIG_MS_SPINAND)
    struct mtd_device *pDev;
    struct part_info *pPart;
    u8 u8PartNum;
    s32 s32Ret;

    s32Ret = mtdparts_init();
    if (s32Ret)
    {
        return -1;
    }

    s32Ret = find_dev_and_part(partitionName, &pDev, &u8PartNum, &pPart);
    if (s32Ret)
    {
        return -1;
    }

    if (pDev->id->type != MTD_DEV_TYPE_NAND)
    {
        printf("BootMusic:not a NAND device\n");
        return -1;
    }

#elif defined(CONFIG_MS_PARTITION)
    mxp_record stRec;
    u32 u32Idx;
    mxp_load_table();
    u32Idx = mxp_get_record_index(partitionName);
    if (u32Idx < 0)
    {
        printf("BootMusic:can not found mxp record: %s\n", partitionName);
        return -1;
    }

    if (0 != mxp_get_record_by_index(u32Idx, &stRec))
    {
        printf("BootMusic:failed to get MXP record with name: %s\n", partitionName);
        return -1;
    }
#else
    return -1;
#endif

    if (fs_mount(partitionName, NULL) < 0)
    {
        return -1;
    }

    fd = fs_open("bootmusic.wav", O_RDONLY, 0);
    if (fd == NULL)
    {
        printf("BootMusic:open file bootmusic.wav error!\n");
        fs_unmount();
        return -1;
    }
    *pFd = fd;
    return 0;
}

static void closeWavFile(void *fd)
{
    fs_close(fd);
    fs_unmount();
}

static int initMhal(s16 s16Channels, u32 u32SampleRate, s16 s16BitWidth, s32 s32Volume, u32 u32DmaBufAddr, u32 u32DmaBufSize)
{
    MHAL_AUDIO_PcmCfg_t stPcmCfg;
    s32 s32Ret = 0;

    switch(u32SampleRate)
    {
        case 8000:
            stPcmCfg.eRate = E_MHAL_AUDIO_RATE_8K;
            break;
        case 11025:
            stPcmCfg.eRate = E_MHAL_AUDIO_RATE_11K;
            break;
        case 12000:
            stPcmCfg.eRate = E_MHAL_AUDIO_RATE_12K;
            break;
        case 16000:
            stPcmCfg.eRate = E_MHAL_AUDIO_RATE_16K;
            break;
        case 22050:
            stPcmCfg.eRate = E_MHAL_AUDIO_RATE_22K;
            break;
        case 24000:
            stPcmCfg.eRate = E_MHAL_AUDIO_RATE_24K;
            break;
        case 32000:
            stPcmCfg.eRate = E_MHAL_AUDIO_RATE_32K;
            break;
        case 44100:
            stPcmCfg.eRate = E_MHAL_AUDIO_RATE_44K;
            break;
        case 48000:
            stPcmCfg.eRate = E_MHAL_AUDIO_RATE_48K;
            break;
        default:
            printf("sample rate %d not support!!!\n", u32SampleRate);
            return -1;
            break;
    }

    if (s16BitWidth == 16)
    {
        stPcmCfg.eWidth = E_MHAL_AUDIO_BITWIDTH_16;
    }
    else
    {
        printf("bit width %d not support!!!\n", s16BitWidth);
        return -1;
    }

    if ((1 != s16Channels) && (2 != s16Channels))
    {
        printf("channel count %d not support!!!\n", s16Channels);
        return -1;
    }
    else
    {
        stPcmCfg.u16Channels = s16Channels;
    }

    stPcmCfg.bInterleaved = TRUE;
    stPcmCfg.pu8DmaArea = (MS_U8 *)(u32DmaBufAddr + MIU_OFFSET);
    stPcmCfg.phyDmaAddr = u32DmaBufAddr;
    stPcmCfg.u32BufferSize = u32DmaBufSize;
    stPcmCfg.u32PeriodSize = s16Channels * s16BitWidth / 8;
    stPcmCfg.u32StartThres = s16Channels * s16BitWidth / 8;

    MHAL_AUDIO_Init(NULL);
    s32Ret |= MHAL_AUDIO_SetGainOut(0, s32Volume, 0, E_MHAL_AUDIO_GAIN_FADING_7);
    s32Ret |= MHAL_AUDIO_SetGainOut(0, s32Volume, 1, E_MHAL_AUDIO_GAIN_FADING_7);
    if (FALSE == bStartPcmOut)
    {
        s32Ret |= MHAL_AUDIO_ConfigPcmOut(0, &stPcmCfg);
        s32Ret |= MHAL_AUDIO_OpenPcmOut(0);
        if (s32Ret)
        {
            MHAL_AUDIO_StopPcmOut(0);
            MHAL_AUDIO_ClosePcmOut(0);
            printf("failed to init lineout.\n");
            return -1;
        }
    }

    return 0;
}

int play_bootmusic(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    void *fd = NULL;
    s32 s32Volume = 0;
    s32 s32Ret = 0;
    u32 u32DmaBufAddr = 0, u32DmaBufSize = 0, u32TmpBufSize = 0;
    WaveFileHeader_t stWavHeader;
    s16 s16FormatTag;
    s16 s16Channels;
    u32 u32SampleRate;
    s16 s16BitWidth;
    u8  *pu8PcmBuf = NULL;

    // check AO volume
    if ((argc != 1) && (argc != 2))
    {
        printf("usage: bootmusic [volume dB]\n");
        return 0;
    }

    if (2 == argc)
    {
        s32Volume = simple_strtol(argv[1], NULL, 0);
    }
    s32Volume = s32Volume;

    // set boot stage
    bootstage_mark_name(BOOTSTAGE_ID_ALLOC, "play_bootmusic++");

    // get reserved memory for ao
    s32Ret = getBootMusicReservedMem(&u32DmaBufAddr, &u32DmaBufSize);
    if (s32Ret)
    {
        printf("BootMusic:failed to get reserved memory of ao form bootargs\n");
        return 0;
    }
    else
    {
        printf("BootMusic:reserved memory size[0x%x] addr[0x%x]\n", u32DmaBufSize, u32DmaBufAddr);
    }

    // open bootmusic.wav
    s32Ret = openWavFile(&fd);
    if ((0 != s32Ret) || ((0 == s32Ret) && (NULL == fd)))
    {
        printf("BootMusic:failed to open bootmusic.wav\n");
        return 0;
    }

    // read wave file
    memset(&stWavHeader, 0, sizeof(stWavHeader));
    s32Ret = fs_read(fd, (void *)(&stWavHeader), sizeof(stWavHeader));
    if (s32Ret != sizeof(stWavHeader))
    {
        closeWavFile(fd);
        return 0;
    }
    else
    {
        // parse wave file header
        s16FormatTag = stWavHeader.wave.wFormatTag;
        s16Channels = stWavHeader.wave.wChannels;
        u32SampleRate = stWavHeader.wave.dwSamplesPerSec;
        s16BitWidth = stWavHeader.wave.wBitsPerSample;

        printf("format[%d] chn[%d] rate[%d] bitwidth[%d]\n", s16FormatTag, s16Channels, u32SampleRate, s16BitWidth);
        printf("pcm data size: %u\n", stWavHeader.dwDATALen);

        if ((s16FormatTag != 0x01)
            || ((1 != s16Channels) && (2 != s16Channels))
            || ((8000 != u32SampleRate) && (11025 != u32SampleRate) && (12000 != u32SampleRate)
                && (16000 != u32SampleRate) && (22050 != u32SampleRate) && (24000 != u32SampleRate)
                && (32000 != u32SampleRate) && (44100 != u32SampleRate) && (48000 != u32SampleRate))
            || (16 != s16BitWidth)
        )
        {
            printf("BootMusic:invalid wav format\n");
            closeWavFile(fd);
            return 0;
        }
        else
        {
            u32TmpBufSize = u32DmaBufSize / 2;
            pu8PcmBuf = (u8*)malloc(u32TmpBufSize);
            if (NULL == pu8PcmBuf)
            {
                printf("BootMusic:failed to alloc memory for pcm data, size[%u]\n", u32TmpBufSize);
                closeWavFile(fd);
                return 0;
            }
        }
    }

    // init audio codec
    s32Ret = initMhal(s16Channels, u32SampleRate, s16BitWidth, s32Volume, u32DmaBufAddr, u32DmaBufSize);
    if (s32Ret)
    {
        closeWavFile(fd);
        return 0;
    }

    while(1)
    {
        memset(pu8PcmBuf, 0, u32TmpBufSize);
        flush_dcache_range((MS_U32)pu8PcmBuf, (MS_U32)pu8PcmBuf + u32TmpBufSize);
        Chip_Flush_Memory();
        s32Ret = fs_read(fd, pu8PcmBuf, u32TmpBufSize);
        if (s32Ret <= 0)
        {
            break;
        }
        flush_dcache_range((MS_U32)pu8PcmBuf, (MS_U32)pu8PcmBuf + u32TmpBufSize);
        Chip_Flush_Memory();

        // write pcm data to audio codec
        s32Ret = MHAL_AUDIO_WriteDataOut(0, pu8PcmBuf, s32Ret, TRUE);
        if (s32Ret < 0)
        {
            printf("failed to write pcm data.\n");
            break;
        }

        if ((FALSE == bStartPcmOut) || ((TRUE == bStartPcmOut) && MHAL_AUDIO_IsPcmOutXrun(0)))
        {
            s32Ret = MHAL_AUDIO_StartPcmOut(0);
            if (s32Ret)
            {
                printf("failed to start pcm out.\n");
                break;
            }
            bStartPcmOut = TRUE;
        }
    }

    // free pcm data memory
    free(pu8PcmBuf);

    // close file
    closeWavFile(fd);

    // set boot stage
    bootstage_mark_name(BOOTSTAGE_ID_ALLOC, "play_bootmusic--");

    return 0;
}

U_BOOT_CMD(
    bootmusic, CONFIG_SYS_MAXARGS, 1,    play_bootmusic,
    "play bootmusic",
    NULL
);