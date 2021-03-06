
/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/



/**
* @defgroup devicesettings
* @{
* @defgroup rpc
* @{
**/


#ifndef _IARM_RPDS_H_
#define _IARM_RPDS_H_

#include "dsTypes.h"
#include "dsError.h"

#ifdef __cplusplus
extern "C" {
#endif



/*
 * Declare RPC dsAudio API names
 */
#define  IARM_BUS_DSMGR_API_dsAudioPortInit 	"dsAudioPortInit"
#define  IARM_BUS_DSMGR_API_dsGetAudioPort		"dsGetAudioPort"
#define  IARM_BUS_DSMGR_API_dsSetStereoMode		"dsSetStereoMode"
#define  IARM_BUS_DSMGR_API_dsGetStereoMode		"dsGetStereoMode"
#define  IARM_BUS_DSMGR_API_dsSetStereoAuto		"dsSetStereoAuto"
#define  IARM_BUS_DSMGR_API_dsGetStereoAuto		"dsGetStereoAuto"
#define IARM_BUS_DSMGR_API_dsGetEncoding                 "dsGetEncoding"
#define  IARM_BUS_DSMGR_API_dsSetAudioEncoding		"dsSetAudioEncoding"
#define  IARM_BUS_DSMGR_API_dsSetAudioMute		"dsSetAudioMute"
#define  IARM_BUS_DSMGR_API_dsIsAudioMute        "dsIsAudioMute"
#define  IARM_BUS_DSMGR_API_dsIsAudioMSDecode    "dsIsAudioMSDecode"
#define  IARM_BUS_DSMGR_API_dsIsAudioPortEnabled  "dsIsAudioPortEnabled"
#define  IARM_BUS_DSMGR_API_dsEnableAudioPort      "dsEnableAudioPort"
#define  IARM_BUS_DSMGR_API_dsAudioPortTerm		"dsAudioPortTerm"
#define  IARM_BUS_DSMGR_API_dsGetAudioGain		"dsGetAudioGain"
#define  IARM_BUS_DSMGR_API_dsSetAudioGain		"dsSetAudioGain"
#define  IARM_BUS_DSMGR_API_dsGetAudioDB		"dsGetAudioDB"
#define  IARM_BUS_DSMGR_API_dsSetAudioDB		"dsSetAudioDB"
#define  IARM_BUS_DSMGR_API_dsGetAudioLevel		"dsGetAudioLevel"
#define  IARM_BUS_DSMGR_API_dsSetAudioLevel		"dsSetAudioLevel"
#define  IARM_BUS_DSMGR_API_dsEnableLEConfig            "dsEnableLEConfig"
#define  IARM_BUS_DSMGR_API_dsEnableMS12Config	"dsEnableMS12Config"



/*
 * Declare RPC dsDisplay API names
 */
#define IARM_BUS_DSMGR_API_dsDisplayInit				"dsDisplayInit"
#define IARM_BUS_DSMGR_API_dsGetDisplay					"dsGetDisplay"
#define IARM_BUS_DSMGR_API_dsGetDisplayAspectRatio		"dsGetDisplayAspectRatio"
#define IARM_BUS_DSMGR_API_dsGetEDID					"dsGetEDID"
#define IARM_BUS_DSMGR_API_dsGetEDIDBytes               "dsGetEDIDBytes"
#define IARM_BUS_DSMGR_API_dsDisplayTerm				"dsDisplayTerm"


/*
 * Declare RPC dsVideo Device API names
 */
#define IARM_BUS_DSMGR_API_dsVideoDeviceInit		"dsVideoDeviceInit"
#define IARM_BUS_DSMGR_API_dsGetVideoDevice			"dsGetVideoDevice"
#define IARM_BUS_DSMGR_API_dsSetDFC					"dsSetDFC"
#define IARM_BUS_DSMGR_API_dsGetDFC					"dsGetDFC"
#define IARM_BUS_DSMGR_API_dsVideoDeviceTerm		"dsVideoDeviceTerm"

/*
 * Declare RPC dsVideo Port API names
 */

#define IARM_BUS_DSMGR_API_dsVideoPortInit			 "dsVideoPortInit"
#define IARM_BUS_DSMGR_API_dsGetVideoPort			"dsGetVideoPort"
#define IARM_BUS_DSMGR_API_dsIsVideoPortEnabled		"dsIsVideoPortEnabled"
#define IARM_BUS_DSMGR_API_dsIsDisplayConnected		 "dsIsDisplayConnected"
#define IARM_BUS_DSMGR_API_dsIsDisplaySurround 		 "dsIsDisplaySurround"
#define IARM_BUS_DSMGR_API_dsGetSurroundMode 		 "dsGetSurroundMode"  
#define IARM_BUS_DSMGR_API_dsEnableVideoPort		 "dsEnableVideoPort"
#define IARM_BUS_DSMGR_API_dsEnableAllVideoPort		 "dsEnableAllVideoPort"
#define IARM_BUS_DSMGR_API_dsSetResolution			"dsSetResolution"
#define IARM_BUS_DSMGR_API_dsGetResolution			"dsGetResolution"
#define IARM_BUS_DSMGR_API_dsGetPlatformResolution		"dsGetPlatformResolution"
#define IARM_BUS_DSMGR_API_dsVideoPortTerm			"dsVideoPortTerm"
#define IARM_BUS_DSMGR_API_dsEnableHDCP    		"dsEnableHDCP"
#define IARM_BUS_DSMGR_API_dsIsHDCPEnabled    		"dsIsHDCPEnabled"
#define IARM_BUS_DSMGR_API_dsGetHDCPStatus    		"dsGetHDCPStatus"
#define IARM_BUS_DSMGR_API_dsGetHDCPProtocol	        "dsGetHDCPProtocol"
#define IARM_BUS_DSMGR_API_dsGetHDCPReceiverProtocol	"dsGetHDCPReceiverProtocol"
#define IARM_BUS_DSMGR_API_dsGetHDCPCurrentProtocol	"dsGetHDCPCurrentProtocol"
#define IARM_BUS_DSMGR_API_dsIsVideoPortActive		"dsIsVideoPortActive"
#define IARM_BUS_DSMGR_API_dsGetHDRCapabilities     "dsGetHDRCapabilities"
#define IARM_BUS_DSMGR_API_dsGetTVHDRCapabilities     "dsGetTVHDRCapabilities"
#define IARM_BUS_DSMGR_API_dsGetSupportedTVResolution     "dsGetSupportedTVResolution"
#define IARM_BUS_DSMGR_API_dsGetSupportedVideoCodingFormats "dsGetSupportedVideoCodingFormats"
#define IARM_BUS_DSMGR_API_dsGetVideoCodecInfo "dsGetVideoCodecInfo"
#define IARM_BUS_DSMGR_API_dsSetForceDisableHDR "dsForceDisableHDR"
#define IARM_BUS_DSMGR_API_dsSetForceDisable4K "dsSetForceDisable4K"
#define IARM_BUS_DSMGR_API_dsGetForceDisable4K "dsGetForceDisable4K"
#define IARM_BUS_DSMGR_API_dsSetScartParameter     "dsSetScartParameter"
#define IARM_BUS_DSMGR_API_dsEnableHDRDVSupport "dsEnableHDRDVSupport"

/*
 * Declare RPC FP  API names
 */

#define IARM_BUS_DSMGR_API_dsFPInit				"dsFPInit"
#define IARM_BUS_DSMGR_API_dsFPTerm				"dsFPTerm"
#define IARM_BUS_DSMGR_API_dsSetFPText			"dsSetFPText"
#define IARM_BUS_DSMGR_API_dsSetFPTime			"dsSetFPTime"
#define IARM_BUS_DSMGR_API_dsSetFPScroll		"dsSetFPScroll"
#define IARM_BUS_DSMGR_API_dsSetFPBlink			"dsSetFPBlink"
#define IARM_BUS_DSMGR_API_dsGetFPBrightness	 "dsGetFPBrightness"
#define IARM_BUS_DSMGR_API_dsSetFPBrightness	 "dsSetFPBrightness"
#define IARM_BUS_DSMGR_API_dsGetFPState           "dsGetFPState"
#define IARM_BUS_DSMGR_API_dsSetFPState			 "dsSetFPState"
#define IARM_BUS_DSMGR_API_dsSetFPColor			 "dsSetFPColor"
#define IARM_BUS_DSMGR_API_dsGetFPColor			  "dsGetFPColor"
#define IARM_BUS_DSMGR_API_dsGetFPTextBrightness "dsGetFPTextBrightness"
#define IARM_BUS_DSMGR_API_dsSetFPTextBrightness "dsSetFPTextBrightness"
#define IARM_BUS_DSMGR_API_dsFPEnableCLockDisplay "dsFPEnableCLockDisplay"
#define IARM_BUS_DSMGR_API_dsGetTimeFormat         "dsGetTimeFormat"
#define IARM_BUS_DSMGR_API_dsSetTimeFormat          "dsSetTimeFormat"


/*
 * Declare RPC HDMI API names
 */
#define IARM_BUS_DSMGR_API_dsHdmiInInit                 "dsHdmiInInit"
#define IARM_BUS_DSMGR_API_dsHdmiInTerm                 "dsHdmiInTerm"
#define IARM_BUS_DSMGR_API_dsHdmiInGetNumberOfInputs    "dsHdmiInGetNumberOfInputs"
#define IARM_BUS_DSMGR_API_dsHdmiInGetStatus            "dsHdmiInGetStatus"
#define IARM_BUS_DSMGR_API_dsHdmiInSelectPort           "dsHdmiInSelectPort"
#define IARM_BUS_DSMGR_API_dsHdmiInScaleVideo           "dsHdmiInScaleVideo"
#define IARM_BUS_DSMGR_API_dsHdmiInSelectZoomMode       "dsHdmiInSelectZoomMode"
#define IARM_BUS_DSMGR_API_dsHdmiInPauseAudio           "dsHdmiInPauseAudio"
#define IARM_BUS_DSMGR_API_dsHdmiInResumeAudio          "dsHdmiInResumeAudio"
#define IARM_BUS_DSMGR_API_dsHdmiInGetCurrentVideoMode  "dsHdmiInGetCurrentVideoMode"

/*
 * Declare RPC Host Interface  API names
 */

#define IARM_BUS_DSMGR_API_dsHostInit				"dsHostInit"
#define IARM_BUS_DSMGR_API_dsHostTerm				"dsHostTerm"
#define IARM_BUS_DSMGR_API_dsSetPreferredSleepMode  "dsSetPreferredSleepMode"
#define IARM_BUS_DSMGR_API_dsGetPreferredSleepMode  "dsGetPreferredSleepMode"
#define IARM_BUS_DSMGR_API_dsGetCPUTemperature 		"dsGetCPUTemperature"
#define IARM_BUS_DSMGR_API_dsGetVersion				"dsGetVersion"
#define IARM_BUS_DSMGR_API_dsSetVersion				"dsSetVersion"


typedef struct _dsAudioGetHandleParam_t {
	dsAudioPortType_t type;
	int index;
	int handle;
} dsAudioGetHandleParam_t;

typedef struct _dsAudioSetStereoModeParam_t {
	int handle;
	dsAudioStereoMode_t mode;
    dsError_t rpcResult;
    bool toPersist;
} dsAudioSetStereoModeParam_t;

typedef struct _dsAudioSetStereoAutoParam_t {
	int handle;
    int autoMode;
} dsAudioSetStereoAutoParam_t;

typedef struct _dsAudioSetMutedParam_t {
	int handle;
	bool mute;
} dsAudioSetMutedParam_t;

typedef struct _dsAudioGetEncodingModeParam_t {
	int handle;
	dsAudioEncoding_t encoding;
} dsAudioGetEncodingModeParam_t, dsAudioSetEncodingModeParam_t;

typedef struct _dsAudioGetMS11Param_t {
    int handle;
    bool ms11Enabled;
} dsAudioGetMS11Param_t;

typedef struct _dsAudioValueParam_t {
    int handle;
    float value;
} dsAudioGainParam_t, dsAudioDBParam_t, dsAudioLevelParam_t;

typedef struct _dsVideoPortGetHandleParam_t {
	dsVideoPortType_t type;
	int index;
	int handle;
} dsVideoPortGetHandleParam_t;

typedef struct _dsVideoPortEnabledParam_t {
	int handle;
	bool enabled;
} dsVideoPortIsEnabledParam_t, dsVideoPortSetEnabledParam_t, dsVideoPortIsHDCPEnabledParam_t,dsAudioPortEnabledParam_t;


typedef struct _dsVideoPortIsActiveParam_t {
	int handle;
	bool active;
    dsError_t   result;
} dsVideoPortIsActiveParam_t;

typedef struct _dsVideoPortGetHDCPStatus_t {
	int handle;
	dsHdcpStatus_t hdcpStatus;
} dsVideoPortGetHDCPStatus_t;

typedef struct _dsVideoPortGetHDCPProtocolVersion_t {
	int handle;
	dsHdcpProtocolVersion_t protocolVersion;
} dsVideoPortGetHDCPProtocolVersion_t;


typedef struct _dsVideoPortIsDisplayConnectedParam_t {
	int handle;
	bool connected;
} dsVideoPortIsDisplayConnectedParam_t;

typedef struct _dsVideoPortIsDisplaySurroundParam_t {
	int handle;
	bool surround;
} dsVideoPortIsDisplaySurroundParam_t;

typedef struct _dsVideoPortGetSurroundModeParam_t {
	int handle;
	int surround;
} dsVideoPortGetSurroundModeParam_t;


typedef struct _dsVideoPortGetResolutionParam_t {
	int handle;
    bool toPersist;
	dsVideoPortResolution_t resolution;
} dsVideoPortGetResolutionParam_t;


typedef struct _dsVideoPortSetResolutionParam_t {
	int handle;
	bool toPersist;
    bool forceCompatible;
	dsVideoPortResolution_t resolution;
} dsVideoPortSetResolutionParam_t;

typedef struct _dsVideoDeviceGetHandleParam_t {
	int index;
	int handle;
} dsVideoDeviceGetHandleParam_t;

typedef struct _dsVideoDeviceSetDFCParam_t {
	int handle;
	dsVideoZoom_t dfc;
} dsVideoDeviceSetDFCParam_t;

typedef struct _dsDisplayGetHandleParam_t {
	dsVideoPortType_t type;
	int index;
	int handle;
} dsDisplayGetHandleParam_t;

typedef struct _dsDisplayGetAspectRatioParam_t {
	int handle;
    dsVideoAspectRatio_t aspectRatio;
} dsDisplayGetAspectRatioParam_t;

typedef struct _dsDisplayGetEDIDParam_t {
	int handle;
    dsDisplayEDID_t edid;
} dsDisplayGetEDIDParam_t;

typedef struct _dsSupportedResolutionParam_t {
    dsError_t result;
    int       handle;
    int       resolutions;
}dsSupportedResolutionParam_t;

typedef struct _dsDisplayGetEDIDBytesParam_t {
    int result;
	int handle;
    int length;
    unsigned char bytes[1024];
} dsDisplayGetEDIDBytesParam_t;

typedef struct _dsFPDTimeParam
{
    dsFPDTimeFormat_t eTime;
    unsigned int nHours;
    unsigned int nMinutes;
}dsFPDTimeParam_t;


typedef struct _dsFPDTimeFormatParam
{
    dsFPDTimeFormat_t eTime;
}dsFPDTimeFormatParam_t;

typedef struct _dsFPDScrollParam
{
    unsigned int nScrollHoldOnDur;
    unsigned int nHorzScrollIterations;
    unsigned int nVertScrollIterations;
}dsFPDScrollParam_t;

typedef struct _dsFPDBlinkParam
{
    dsFPDIndicator_t eIndicator;
    unsigned int nBlinkDuration;
    unsigned int nBlinkIterations;
}dsFPDBlinkParam_t;

typedef struct _dsFPDBrightParam
{
    dsFPDIndicator_t eIndicator;
    dsFPDBrightness_t eBrightness;
    bool toPersist;
}dsFPDBrightParam_t;


typedef struct _dsFPDTextBrightParam
{
    dsFPDTextDisplay_t eIndicator;
    dsFPDBrightness_t eBrightness;
}dsFPDTextBrightParam_t;


typedef struct _dsFPDStateParam
{
    dsFPDIndicator_t eIndicator;
    dsFPDState_t state;
}dsFPDStateParam_t;

typedef struct _dsFPDColorParam
{
    dsFPDIndicator_t eIndicator;
    dsFPDColor_t eColor;
    bool toPersist;
}dsFPDColorParam_t;

typedef struct _dsEnableHDCPParam
{
    int handle;
    bool contentProtect;
    char hdcpKey[HDCP_KEY_MAX_SIZE];
    size_t keySize;
    dsError_t rpcResult;
} dsEnableHDCPParam_t;

typedef struct _dsPreferredSleepMode
{
    dsSleepMode_t mode;
} dsPreferredSleepMode;

typedef struct _dsCPUThermalParam
{
	float  temperature;
} dsCPUThermalParam;

typedef struct _dsVesrionParam
{
	uint32_t  versionNumber;
} dsVesrionParam;

typedef struct _dsVideoRect
{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} dsVideoRect_t;

typedef struct _dsHdmiInGetNumberOfInputsParam_t
{
    dsError_t   result;
    uint8_t     numHdmiInputs;
} dsHdmiInGetNumberOfInputsParam_t;

typedef struct _dsHdmiInGetStatusParam_t
{
    dsError_t           result;
    dsHdmiInStatus_t    status;
} dsHdmiInGetStatusParam_t;

typedef struct _dsHdmiInSelectPortParam_t
{
    dsError_t       result;
    dsHdmiInPort_t  port;
} dsHdmiInSelectPortParam_t;

typedef struct _dsHdmiInScaleVideoParam_t
{
    dsError_t       result;
    dsVideoRect_t   videoRect;
} dsHdmiInScaleVideoParam_t;

typedef struct _dsHdmiInSelectZoomModeParam_t
{
    dsError_t      result;
    dsVideoZoom_t  zoomMode;
} dsHdmiInSelectZoomModeParam_t;

typedef struct _dsHdmiInGetResolutionParam_t
{
	dsError_t               result;
	dsVideoPortResolution_t resolution;
} dsHdmiInGetResolutionParam_t;

typedef struct _dsGetHDRCapabilitiesParam_t
{
    dsError_t               result;
    int                     handle;
    int                     capabilities;
} dsGetHDRCapabilitiesParam_t;

typedef struct _dsLEConfigParam_t
{
    int                handle;
    bool               enable;
} dsLEConfigParam_t;

typedef struct _dsMS12ConfigParam_t
{
    int                handle;
    dsMS12FEATURE_t    feature;
    bool               enable;
} dsMS12ConfigParam_t;

typedef struct
{
    dsError_t result;
    int handle;
    unsigned int supported_formats;
} dsGetSupportedVideoCodingFormatsParam_t;

typedef struct
{
    dsError_t result;
	int handle;
    dsVideoCodingFormat_t format;
    dsVideoCodecInfo_t info;
} dsGetVideoCodecInfoParam_t;

typedef struct
{
	dsError_t result;
	int handle;
	bool disable;
} dsForceDisableHDRParam_t;

typedef struct
{
	dsError_t result;
	int handle;
	bool disable;
} dsForceDisable4KParam_t;
  
typedef struct
{
        dsError_t result;
        int handle;
        bool enable;
} dsEnableHDRDVSupportParam_t;  

#define DSSCART_PARAM_LEN_MAX 1024
#define DSSCART_VALUE_LEN_MAX 1024

typedef struct _dsScartParamParam_t {
    int result;
    int handle;
    char param_bytes[DSSCART_PARAM_LEN_MAX];
    char value_bytes[DSSCART_VALUE_LEN_MAX];
} dsScartParamParam_t;
  
#ifdef __cplusplus
}
#endif

#endif /* RPDS_H_ */


/** @} */
/** @} */
