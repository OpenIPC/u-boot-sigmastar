/*
 * mhal_jpd.h- Sigmastar
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

/**
 * @file          mhal_jpd.h
 * @brief         JPD Driver API
 * @details       Details
 */
#ifndef __MHAL_JPD_H__
#define __MHAL_JPD_H__

#include <mhal_common.h>

/** MHAL_JPD_DbgLevel_e */
typedef enum
{
    E_MHAL_JPD_DBG_NONE = 0, /**< debug level of None*/
    E_MHAL_JPD_DBG_INFO,     /**< debug level of Info*/
    E_MHAL_JPD_DBG_DBG,      /**< debug level of Debug*/
    E_MHAL_JPD_DBG_WARN,     /**< debug level of Warn*/
    E_MHAL_JPD_DBG_ERR,      /**< debug level of Error*/
} MHAL_JPD_DbgLevel_e;

/** MHAL_JPD_ScaleDownMode_e */
typedef enum
{
    E_MHAL_JPD_SCALE_DOWN_ORG    = 0, /**< scale down off*/
    E_MHAL_JPD_SCALE_DOWN_HALF   = 2, /**< scale down 1/2*/
    E_MHAL_JPD_SCALE_DOWN_FOURTH = 4, /**< scale down 1/4*/
    E_MHAL_JPD_SCALE_DOWN_EIGHTH = 8, /**< scale down 1/8*/
} MHAL_JPD_ScaleDownMode_e;

/** MHAL_JPD_OutputMode_e */
typedef enum
{
    E_MHAL_JPD_OUTPUT_FRAME = 0, /**< Output Mode Frame*/
    E_MHAL_JPD_OUTPUT_HANDSHAKE, /**< Output Mode HandShake.Unused*/
    E_MHAL_JPD_OUTPUT_RING,      /**< Output Mode Hw Ring.Unused*/
    E_MHAL_JPD_OUTPUT_IMI,       /**< Output Mode IMI*/
    E_MHAL_JPD_OUTPUT_EMI,       /**< Output Mode EMI*/
} MHAL_JPD_OutputMode_e;

/** MHAL_JPD_Status_e */
typedef enum
{
    E_MHAL_JPD_STATUS_FAIL = 0, /**< Irq CallBack status.failed*/
    E_MHAL_JPD_STATUS_DONE,     /**< Irq CallBack status.Done*/
    E_MHAL_JPD_STATUS_ERROR,    /**< Irq CallBack status.Error*/
    E_MHAL_JPD_STATUS_DECODING, /**< Irq CallBack status.Decodeing*/
} MHAL_JPD_Status_e;

/** MHAL_JPD_OutputFmt_e */
typedef enum
{
    E_MHAL_JPD_OUTPUT_FMT_YUV420 = 0, /**< output YUV420 format.Unused*/
    E_MHAL_JPD_OUTPUT_FMT_YUV422,     /**< output YUV422 format.JPD output format only support YUV422*/
} MHAL_JPD_OutputFmt_e;

/** MHAL_JPD_DecodeMode_e */
typedef enum
{
    E_MHAL_JPD_DECODE_MAIN = 0,   /**< Decode of whole picture*/
    E_MHAL_JPD_DECODE_THHUMBNAIL, /**< Decode of thumbnail*/
    E_MHAL_JPD_DECODE_MJPEG       /**< Decode of motion jpeg*/
} MHAL_JPD_DecodeMode_e;

/** MHAL_JPD_PARAM_CMD_e */
typedef enum
{
    E_JPD_PARAM_MAX_DEC_RES = 0,  /**< param of maximum resolution in progressive */
    E_JPD_PARAM_FPGA_STOP_DECODE, /**< Debug of decode.will be while 1 before trig jpd*/
} MHAL_JPD_PARAM_CMD_e;

/** MHAL_JPD_DevConfig_e */
typedef enum
{
    E_MHAL_JPD_DEV_CONFIG_CMDQ_BUF_SIZE = 0, /**< param of get device cmdq bufer size*/
    E_MHAL_JPD_DEV_CONFIG_EXTRA_CAPABILITY,  /**< param of get device extra capability*/
    E_MHAL_JPD_DEV_CONFIG_ISR_CB,            /**< param of register callback*/
    E_MHAL_JPD_DEV_CONFIG_MMA_CB             /**< param of set device MMACB function*/
} MHAL_JPD_DevConfig_e;

typedef VOID *MHAL_JPD_DEV_HANDLE;  /**< JPD device handle*/
typedef VOID *MHAL_JPD_INST_HANDLE; /**< JPD instance handle*/
typedef VOID  MHAL_JPD_Param_t;     /**< JPD dynamic parameter*/

/** MHAL_JPD_Buffer_t */
typedef struct MHAL_JPD_Buffer_s
{
    void * pVirtAddr;  /**< buf virtual addr*/
    MS_U64 u64PhyAddr; /**< buf physical addr*/
    MS_U32 u32Size;    /**< buf size*/
    MS_U32 u32Srtide;  /**< buf stride*/
} MHAL_JPD_Buffer_t;

/** MHAL_JPD_InputBuf_t */
typedef struct MHAL_JPD_InputBuf_s
{
    void * pVirtBufAddr1;  /**< JPD input buf viraddr2.for support ring buffer management must not zero*/
    MS_PHY u64PhyBufAddr1; /**< JPD input buf phyaddr1.for support ring buffer management must not zero*/
    MS_U32 u32BufSize1;    /**< JPD input buf size1.must not zero*/
    void * pVirtBufAddr2;  /**< JPD input buf viraddr2.for support ring buffer management must not zero*/
    MS_PHY u64PhyBufAddr2; /**< JPD input buf phyaddr2.for support ring buffer management must not zero*/
    MS_U32 u32BufSize2;    /**< JPD input buf size1.must not zero*/
} MHAL_JPD_InputBuf_t;

/** MHAL_JPD_OutputBuf_t */
typedef struct MHAL_JPD_OutputBuf_s
{
    void *                   pCmdQ;            /**< cmdq instance handle*/
    void *                   pVirtBufAddr[3];  /**< JPD output buf viraddr.for support ring buffer management*/
    MS_PHY                   u64PhyBufAddr[3]; /**< JPD output buf phyaddr.for support ring buffer management*/
    MS_U32                   u32BufStride[3];  /**< JPD output format stride*/
    MS_U32                   u32BufSize;       /**< JPD output buf size*/
    MS_U32                   u32RingBufHeight; /**< valid in case of E_MHAL_JPD_OUTPUT_RING*/
    MS_U32                   u32RingTagId;     /**< valid in case of E_MHAL_JPD_OUTPUT_RING*/
    MS_U32                   u32LineNum;       /**< valid in case of E_MHAL_JPD_OUTPUT_IMI/EMI*/
    MHAL_JPD_OutputFmt_e     eOutputFmt;       /**< JPD output format info*/
    MHAL_JPD_OutputMode_e    eOutputMode;      /**< JPD output mode info*/
    MHAL_JPD_ScaleDownMode_e eScaleDownMode;   /**< JPD output scale down mode*/
    MS_BOOL                  bProtectEnable;   /**< JPD output write protect*/
} MHAL_JPD_OutputBuf_t;

/** MHAL_JPD_JpgFrame_t */
typedef struct MHAL_JPD_JpgFrame_s
{
    MHAL_JPD_DecodeMode_e eDecodeMode; /**< JPD decode mode info*/
    MHAL_JPD_InputBuf_t   stInputBuf;  /**< JPD input buf info*/
    MHAL_JPD_OutputBuf_t  stOutputBuf; /**< JPD output buf info*/
} MHAL_JPD_JpgFrame_t;

/** MHAL_JPD_MaxDecRes_t */
typedef struct MHAL_JPD_MaxDecRes_s
{
    MS_U16 u16MaxWidth;     /**< Max width of JPD baseline mode*/
    MS_U16 u16MaxHeight;    /**< Max height of JPD baseline mode*/
    MS_U16 u16ProMaxWidth;  /**< Max width of JPD progressive mode*/
    MS_U16 u16ProMaxHeight; /**< Max height of JPD progressive mode*/
} MHAL_JPD_MaxDecRes_t;

/** MHAL_JPD_JpgInfo_t */
typedef struct MHAL_JPD_JpgInfo_s
{
    MS_U16 u16AlignedWidth;   /**< The aligned width by MCU size of jpeg frame*/
    MS_U16 u16AlignedHeight;  /**< The aligned height by MCU size of jpeg frame*/
    MS_U16 u16OriginWidth;    /**< The origin width from jpeg SOF*/
    MS_U16 u16OriginHeight;   /**< The origin height from jpeg SOF*/
    MS_U8  u8SampleFactorHor; /**< The horizontal sampling factor of jpeg source image*/
    MS_U8  u8SampleFactorVer; /**< The vertical sampling factor of jpeg source image*/
} MHAL_JPD_JpgInfo_t;

/** MHAL_JPD_ParamInt_t */
typedef struct MHAL_JPD_ParamInt_s
{
    MS_U32 u32Val; /**< unsigned int value */
} MHAL_JPD_ParamInt_t;

/** MHAL_JPD_ExtraCapability_t */
typedef struct MHAL_JPD_ExtraCapability_s
{
    MS_BOOL bImiSupport;     /**< support imi mode or not*/
    MS_BOOL bCmdqSupport;    /**< support cmdq or not*/
    MS_U32  u32MaxDeviceNum; /**< max device num */
} MHAL_JPD_ExtraCapability_t;

/** MHAL_JPD_MMA_CB_t */
typedef struct MHAL_JPD_MMA_CB_s
{
    MS_S32(*AllocMma)
    (MHAL_JPD_DEV_HANDLE hInst, MS_U8 *pu8MMAHeapName, MS_U8 *pu8BufName, MS_U32 u32Size,
     MS_PHYADDR *phyAddr);                                            /**< function pointer to alloc memory*/
    MS_S32 (*FreeMma)(MHAL_JPD_DEV_HANDLE hInst, MS_PHYADDR phyAddr); /**< function pointer to free memory*/
    void *(*Map)(MS_PHYADDR phyAddr, MS_U32 u32Size, MS_BOOL bCache); /**< function pointer to map memory*/
    void (*Unmap)(void *pVirtAddr);                                   /**< function pointer to unmap memory*/
    MS_S32 (*Flush)(void *pVirtAddr, MS_U32 u32Size);                 /**< function pointer to flush memory*/
} MHAL_JPD_MMA_CB_t;

typedef struct MHAL_JPD_IsrCallback_s
{
    void (*funcIsrCB)(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_Status_e eStatus);
} MHAL_JPD_IsrCallback_t;

/**
 * @brief        Create Device Handle
 * @details      The MHAL_JPD_CreateDevice() function to create device
 * @param[in]    u32DevId : The device id
 * @param[out]   ppstDevHandle : Pointer to device handle
 * @note         Before create devide,must to call MHAL_JPD_Init.
 * @return       MS_S32
 * @retval       0                successful
 * @retval       Other            failed
 * @par Sample
 * @code
 *               MHAL_JPD_Init(NULL);
 *               if (0 != MHAL_JPD_CreateDevice(devnum, &g_JPD_Dev[devnum], &st_JPD_DevParam))
 *               {
 *                   CamOsPrintf("%s:%d fail\n",__FUNCTION__,__LINE__);
 *                   return -1;
 *               }
 * @endcode
 */
MS_S32 MHAL_JPD_CreateDevice(MS_U32 u32DevId, MHAL_JPD_DEV_HANDLE *ppstDevHandle);

/**
 * @brief       Destroy Device Handle
 * @details     The MHAL_JPD_DestroyDevice() function to destroy device handle
 * @param[in]   pstDevHandle : The device handle
 * @note        Destory device and release resources.before call the func should call MHAL_JPD_DestroyInstance func
 * @return      MS_S32
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              MHAL_JPD_DestroyInstance(g_JPD_Ctx[devid]);
 *              MHAL_JPD_DestroyDevice(g_JPD_Dev[devid]);
 * @endcode
 */
MS_S32 MHAL_JPD_DestroyDevice(MHAL_JPD_DEV_HANDLE pstDevHandle);

/**
 * @brief       Set Config of Device
 * @details     The MHAL_JPD_SetDevConfig() function to set device config
 * @param[in]   pstDevHandle: Pointer to device handle
 * @param[in]   eType: Config Type
 * @param[in]   pstParam: Config Info that set
 * @note        This function can set a variety of parameters, reference structure MHAL_JPD_DevConfig_e
 * @return      MHAL_ErrCode_e
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              no eType for MHAL_JPD_SetDevConfig yet
 * @endcode
 */
MS_S32 MHAL_JPD_SetDevConfig(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_DevConfig_e eType, MHAL_JPD_Param_t *pstParam);

/**
 * @brief        Get Config of Device
 * @details      The MHAL_JPD_GetDevConfig() function to get device config
 * @param[in]    pstDevHandle: Pointer to device handle
 * @param[in]    eType: Config Type
 * @param[out]   pstParam: Config Info that get
 * @note         This function can get a variety of parameters, reference structure MHAL_JPD_DevConfig_e
 * @return       MHAL_ErrCode_e
 * @retval       0                successful
 * @retval       Other            failed
 * @par Sample
 * @code
 *               MHAL_JPD_GetDevConfig(g_JPD_Dev[devnum], E_MHAL_JPD_DEV_CONFIG_CMDQ_BUF_SIZE, &stCmdqParam);
 * @endcode
 */
MS_S32 MHAL_JPD_GetDevConfig(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_DevConfig_e eType, MHAL_JPD_Param_t *pstParam);

/**
 * @brief       Create instance Handle
 * @details     The MHAL_JPD_CreateInstance() function to instance device
 * @param[in]   pstDevHandle : The device handle
 * @param[in]   phInst : The instance handle
 * @note        Create a instance.maximum 16 instance of channel.before call the func a device handle must be create
 * @return      MS_S32
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              if (0 != MHAL_JPD_CreateDevice(devnum, &g_JPD_Dev[devnum], &st_JPD_DevParam))
 *              {
 *                  CamOsPrintf("%s:%d fail\n",__FUNCTION__,__LINE__);
 *                  return -1;
 *              }
 *              if (0 != MHAL_JPD_CreateInstance(g_JPD_Dev[devnum], &g_JPD_Ctx[devnum]))
 *              {
 *                  CamOsPrintf("%s:%d fail\n",__FUNCTION__,__LINE__);
 *                  return -1;
 *              }
 * @endcode
 */
MS_S32 MHAL_JPD_CreateInstance(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_INST_HANDLE *ppstInstHandle);

/**
 * @brief       Destroy instance Handle
 * @details     The MHAL_JPD_DestroyInstance() function to destroy instance handle
 * @param[in]   pstInstHandle : The instance handle
 * @note        Destory instance and release resources of channel.
 * @return      MS_S32
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              MHAL_JPD_DestroyInstance(g_JPD_Ctx[devid]);
 * @endcode
 */
MS_S32 MHAL_JPD_DestroyInstance(MHAL_JPD_INST_HANDLE pstInstHandle);

/**
 * @brief       Reset JPD HW when an error occurs
 * @details     The MHAL_JPD_ResetDev() function to Reset Hw
 * @param[in]   pstDevHandle : The Device handle
 * @note        Reset HW to restart decode work,param should be device handle.
 * @return      MS_S32
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              MHAL_JPD_ResetDev(g_JPD_Dev[devnum],NULL);
 * @endcode
 */
MS_S32 MHAL_JPD_ResetDev(MHAL_JPD_DEV_HANDLE pstDevHandle);

/**
 * @brief       Set parameters through this interface
 * @details     The MHAL_JPD_SetParam() function to set parameters
 * @param[in]   pstInstHandle : The instance handle
 * @param[in]   eParamCmd : The command of set parameters
 * @param[in]   pstParam : Data corresponding to the command
 * @note        This function can set a variety of parameters, reference structure MHAL_JPD_PARAM_CMD_e
 * @return      MS_S32
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              MHAL_JPD_SetParam(g_JPD_Ctx[u32DevId], E_JPD_PARAM_MAX_DEC_RES, &stJpdMaxinfo);
 *              MHAL_JPD_SetParam(g_JPD_Ctx[u32DevId], E_JPD_PARAM_PRO_MAX_DEC_RES, &stJpdProinfo);
 * @endcode
 */
MS_S32 MHAL_JPD_SetParam(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_PARAM_CMD_e eParamCmd,
                         MHAL_JPD_Param_t *pstParam);

/**
 * @brief       Get parameters through this interface
 * @details     The MHAL_JPD_GetParam() function to get parameters
 * @param[in]   pstInstHandle : The instance handle
 * @param[in]   eParamCmd : The command of set parameters
 * @param[in]   pstParam : Data corresponding to the command
 * @note        This function can get a variety of parameters, reference structure MHAL_JPD_PARAM_CMD_e
 * @return      MS_S32
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              MHAL_JPD_GetParam(g_JPD_Ctx[u32DevId], E_JPD_PARAM_MAX_DEC_RES, &stJpdMaxinfo);
 *              MHAL_JPD_GetParam(g_JPD_Ctx[u32DevId], E_JPD_PARAM_PRO_MAX_DEC_RES, &stJpdProinfo);
 * @endcode
 */
MS_S32 MHAL_JPD_GetParam(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_PARAM_CMD_e eParamCmd,
                         MHAL_JPD_Param_t *pstParam);

/**
 * @brief       Analyze the header information of jpg to get the width and height of the picture
 * @details     The MHAL_JPD_ExtractJpgInfo() function to get the width and height of the picture
 * @param[in]   pstInstHandle : The instance handle
 * @param[in]   pstInputBuf : input buf information
 * @param[out]  pstJpgInfo : Get the width and height of the picture
 * @note        This function analyze the header information of jpg,Only cpu is involved
 * @return      MS_S32
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              if (0 != MHAL_JPD_ExtractJpgInfo(g_JPD_Ctx[devnum],&st_extra.stInputBuf,&stJpgInfo))
 *              {
 *                  CamOsPrintf("%s:%d fail\n",__FUNCTION__,__LINE__);
 *                  return -1;
 *              }
 * @endcode
 */
MS_S32 MHAL_JPD_ExtractJpgInfo(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_InputBuf_t *pstInputBuf,
                               MHAL_JPD_JpgInfo_t *pstJpgInfo);

/**
 * @brief       Start decoding one frame using header information
 * @details     The MHAL_JPD_StartDecodeOneFrame() function to start decode one frame
 * @param[in]   pstInstHandle : The instance handle
 * @param[in]   pstJpgFrame : Outputbuf information and output format setting
 * @note        This function will trig hw decode using header information,so should call MHAL_JPD_ExtractJpgInfo before
 * @return      MS_S32
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              if (0 != MHAL_JPD_StartDecodeOneFrame(g_JPD_Ctx[devnum], &stframe))
 *              {
 *                  CamOsPrintf("%s:%d fail\n",__FUNCTION__,__LINE__);
 *                  return -1;
 *              }
 * @endcode
 */
MS_S32 MHAL_JPD_StartDecodeOneFrame(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_JpgFrame_t *pstJpgFrame);

/**
 * @brief       Barrier one jpeg frame
 * @details     The MHAL_JPD_BarrierOneFrame() function to barrier one frame
 * @param[in]   pstInstHandle: Pointer to instance handle
 * @note        This function will barrier one frame in cmdq after start one frame, only used for cmdq case
 * @return      MHAL_JPD_ErrCode_e
 * @retval      0                successful
 * @retval      Other            failed
 * @par Sample
 * @code
 *              if (0 != MHAL_JPD_StartDecodeOneFrame(g_JPD_Ctx[devnum], &stframe))
 *              {
 *                  CamOsPrintf("%s:%d fail\n",__FUNCTION__,__LINE__);
 *                  return -1;
 *              }
 *              if (0 != MHAL_JPD_BarrierOneFrame(g_JPD_Ctx[devnum]))
 *              {
 *                  CamOsPrintf("%s:%d fail\n",__FUNCTION__,__LINE__);
 *                  return -1;
 *              }
 * @endcode
 */
MS_S32 MHAL_JPD_BarrierOneFrame(MHAL_JPD_INST_HANDLE pstInstHandle);

/**
 * @brief       polling to get the information of jpd HW status
 * @details     The MHAL_JPD_CheckDecodeStatus() function to Get the information of jpd HW status
 * @param[in]   pstInstHandle : The instance handle
 * @param[out]  peDecodeStatus : Pointer to jpd status
 * @note        This function will get jpd hw status.so after trig jpd,call this func will get hw status.
 * @return      MS_S32
 * @retval      0                decode pass
 * @retval      Other            failed
 * @par Sample
 * @code
 *              return MHAL_JPD_CheckDecodeStatus( hinst, &JPD_Status_Flag);
 * @endcode
 */
MS_S32 MHAL_JPD_CheckDecodeStatus(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_Status_e *peDecodeStatus);

/**
 * @brief       JPD Decode Parts time
 * @details     The MHAL_JPD_Decodetimes() function is to get jpd decode FPS
 * @param[in]   pstInstHandle : The instance handle.
 * @note        This func will get jpd decode times of cpu and jpd HW.
 * @return      0
 * @retval  0   for all
 * @par Sample
 * @code
 *              MHAL_JPD_Decodetimes(g_JPD_Ctx[devnum]);
 * @endcode
 */
MS_S32 MHAL_JPD_Decodetimes(MHAL_JPD_INST_HANDLE pstInstHandle);

#endif //__MHAL_JPD_H__