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


#include "dsVideoPort.h"
#include "dsDisplay.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <dlfcn.h>
#include "dsError.h"
#include "dsUtl.h"
#include "dsTypes.h"
#include "pthread.h"
#include <pthread.h>
#include "libIARM.h"
#include "iarmUtil.h"
#include "libIBusDaemon.h"
#include "libIBus.h"
#include "dsRpc.h"
#include "dsMgr.h"
#include <iostream>
#include <string.h>
#include "hostPersistence.hpp"
#include "dsserverlogger.h"
#include "dsTypes.h"
#include "dsVideoPortSettings.h"

#ifdef DEVICESETTINGS_DEFAULT_RESOLUTION
  #define DEFAULT_RESOLUTION DEVICESETTINGS_DEFAULT_RESOLUTION
#else
  #define DEFAULT_RESOLUTION "720p"
#endif

static int m_isInitialized = 0;
static int m_isPlatInitialized = 0;
static pthread_mutex_t dsLock = PTHREAD_MUTEX_INITIALIZER;
static std::string _dsHDMIResolution(DEFAULT_RESOLUTION);
static std::string _dsCompResolution(DEFAULT_RESOLUTION);
static dsHdcpStatus_t _hdcpStatus = dsHDCP_STATUS_UNAUTHENTICATED;

#define NULL_HANDLE 0
#define IARM_BUS_Lock(lock) pthread_mutex_lock(&dsLock)
#define IARM_BUS_Unlock(lock) pthread_mutex_unlock(&dsLock)

IARM_Result_t _dsVideoPortInit(void *arg);
IARM_Result_t _dsGetVideoPort(void *arg);
IARM_Result_t _dsIsVideoPortEnabled(void *arg);
IARM_Result_t _dsIsDisplayConnected(void *arg);
IARM_Result_t _dsIsDisplaySurround(void *arg);
IARM_Result_t _dsEnableVideoPort(void *arg);
IARM_Result_t _dsSetResolution(void *arg);
IARM_Result_t _dsGetResolution(void *arg);
IARM_Result_t _dsVideoPortTerm(void *arg);
IARM_Result_t _dsEnableHDCP(void *arg);
IARM_Result_t _dsIsHDCPEnabled(void *arg);
IARM_Result_t _dsGetHDCPStatus(void *arg);
IARM_Result_t _dsIsVideoPortActive(void *arg);


static dsVideoPortType_t _GetVideoPortType(int handle);
static int  _dsVideoPortPreResolutionCall(dsVideoPortResolution_t *resolution);
static int  _dsSendVideoPortPostResolutionCall(dsVideoPortResolution_t *resolution);
void _dsHdcpCallback(int handle, dsHdcpStatus_t event);
static void persistResolution(dsVideoPortSetResolutionParam_t *param);


#define IsHDCompatible(p)  (((p) >= dsVIDEO_PIXELRES_1280x720 ) && ((p) < dsVIDEO_PIXELRES_MAX))
static  std::string getCompatibleResolution(dsVideoPortResolution_t *SrcResn);
static bool    IsCompatibleResolution(dsVideoResolution_t pixelResolution1,dsVideoResolution_t pixelResolution2);
static dsVideoResolution_t getPixelResolution(std::string &resolution);

IARM_Result_t dsVideoPortMgr_init()
{
	
	std::string _Resolution(DEFAULT_RESOLUTION);
	
	try
	{
		/*TBD - Get the Device type Dynamically*/
		/*
			* Read the HDMI,Component or Composite
			* Next is to browse through all supported ports  i.e kPorts to differentiate between 
			* Component and Composite. TBD - Remove HAS_ONLY_COMPOSITE
		*/
		_dsHDMIResolution = device::HostPersistence::getInstance().getProperty("HDMI0.resolution",_Resolution);
		__TIMESTAMP();printf("The Persistent HDMI resolution read is %s \r\n",_dsHDMIResolution.c_str());
		#ifdef HAS_ONLY_COMPOSITE
			_Resolution = DEFAULT_RESOLUTION;
			_dsCompResolution = device::HostPersistence::getInstance().getProperty("Baseband0.resolution",_Resolution);
		#else
			_Resolution = DEFAULT_RESOLUTION;
			_dsCompResolution = device::HostPersistence::getInstance().getProperty("COMPONENT0.resolution",_Resolution);
		#endif
		__TIMESTAMP();printf("The Persistent Component/Composite resolution read is %s \r\n",_dsCompResolution.c_str());
					
		if (!m_isPlatInitialized) 
		{
			/*Initialize the Video Ports */
			dsVideoPortInit();
		}
		/*coverity[missing_lock]  CID-18497 using Coverity Annotation to ignore error*/
		m_isPlatInitialized ++;
	}
	catch(...) 
	{
		printf("Error in Getting the Video Resolution on Startup..... \r\n");
	}
	IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsVideoPortInit, _dsVideoPortInit);
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t dsVideoPortMgr_term()
{
   return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsVideoPortInit(void *arg)
{
    IARM_BUS_Lock(lock);

    if (!m_isInitialized) {
		
		#ifdef HAS_HDCP_CALLBACK
			dsRegisterHdcpStatusCallback(NULL,_dsHdcpCallback);
		#endif
		
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetVideoPort,_dsGetVideoPort);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsIsVideoPortEnabled,_dsIsVideoPortEnabled);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsIsDisplayConnected,_dsIsDisplayConnected);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsIsDisplaySurround,_dsIsDisplaySurround);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsEnableVideoPort,_dsEnableVideoPort);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetResolution,_dsSetResolution);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetResolution,_dsGetResolution);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsVideoPortTerm,_dsVideoPortTerm);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsEnableHDCP ,_dsEnableHDCP);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsIsHDCPEnabled,_dsIsHDCPEnabled);
	    IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetHDCPStatus ,_dsGetHDCPStatus); 
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsIsVideoPortActive ,_dsIsVideoPortActive); 
	
        m_isInitialized = 1;
    }

    if (!m_isPlatInitialized) {
    	/* Nexus init, if any here */
    	dsVideoPortInit();
    }
    m_isPlatInitialized++;

    IARM_BUS_Unlock(lock);
	
	return IARM_RESULT_SUCCESS;

}

IARM_Result_t _dsGetVideoPort(void *arg)
{
    _DEBUG_ENTER();
    
	IARM_BUS_Lock(lock);
    
	dsVideoPortGetHandleParam_t *param = (dsVideoPortGetHandleParam_t *)arg;
    dsGetVideoPort(param->type, param->index, &param->handle);

	IARM_BUS_Unlock(lock);
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsIsVideoPortEnabled(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);
    
	dsVideoPortIsEnabledParam_t *param = (dsVideoPortIsEnabledParam_t *)arg;
    dsIsVideoPortEnabled(param->handle, &param->enabled);
   
    IARM_BUS_Unlock(lock);
	
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsIsVideoPortActive(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);
    
	dsVideoPortIsActiveParam_t *param = (dsVideoPortIsActiveParam_t *)arg;
    
    dsVideoPortType_t _VPortType = _GetVideoPortType(param->handle);

	if (_VPortType == dsVIDEOPORT_TYPE_HDMI)
	{
		param->result = dsIsVideoPortActive(param->handle, &param->active);
	}
	else if (_VPortType == dsVIDEOPORT_TYPE_COMPONENT)
	{
		param->active = true;
        param->result =  dsERR_NONE;
	}

    IARM_BUS_Unlock(lock);
	
	return IARM_RESULT_SUCCESS;
}



IARM_Result_t _dsIsHDCPEnabled(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);
    
    dsVideoPortIsHDCPEnabledParam_t *param = (dsVideoPortIsHDCPEnabledParam_t *)arg;
    dsIsHDCPEnabled(param->handle, &param->enabled);
   
    if(param->enabled){
    	__TIMESTAMP();printf("isHDCP =true !!!!!!..\r\n");
	}
    else{
    	__TIMESTAMP();printf("isHDCP =false !!!!!!..\r\n");
    }	
    IARM_BUS_Unlock(lock);
	
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsIsDisplayConnected(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);
   
	dsVideoPortIsDisplayConnectedParam_t *param = (dsVideoPortIsDisplayConnectedParam_t *)arg;
    dsIsDisplayConnected(param->handle,&param->connected);

	IARM_BUS_Unlock(lock);
	
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsIsDisplaySurround(void *arg)
{

#ifndef RDK_DSHAL_NAME
#warning   "RDK_DSHAL_NAME is not defined"
#define RDK_DSHAL_NAME "RDK_DSHAL_NAME is not defined"
#endif
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);

    printf("dsSRV::_dsIsDisplaySurround \r\n");

    typedef dsError_t (*dsIsDisplaySurround_t)(int handle, bool *surround);
    static dsIsDisplaySurround_t func = 0;
    if (func == 0) {
        void *dllib = dlopen(RDK_DSHAL_NAME, RTLD_LAZY);
        if (dllib) {
            func = (dsIsDisplaySurround_t) dlsym(dllib, "dsIsDisplaySurround");
            if (func) {
                printf("dsIsDisplaySurround_t(int, bool*) is defined and loaded\r\n");
            }   
            else {
                printf("dsIsDisplaySurround_t(int, bool*) is not defined\r\n");
            }   
            dlclose(dllib);
        }   
        else {
            printf("Opening RDK_DSHAL_NAME [%s] failed\r\n", RDK_DSHAL_NAME);
        }   
    }   

	dsVideoPortIsDisplaySurroundParam_t *param = (dsVideoPortIsDisplaySurroundParam_t *)arg;

    if (func != 0) {
        dsError_t ret = func(param->handle, &param->surround);
        printf("dsSRV ::isDisplaySurround() returns %d %d\r\n", ret, param->surround);
    }
    else {
        param->surround = false;
    }

	IARM_BUS_Unlock(lock);
	
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsEnableVideoPort(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);
   
	dsVideoPortSetEnabledParam_t *param = (dsVideoPortSetEnabledParam_t *)arg;
    dsEnableVideoPort(param->handle, param->enabled);
   
    IARM_BUS_Unlock(lock);
	
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsGetResolution(void *arg)
{
    _DEBUG_ENTER();
   
	IARM_BUS_Lock(lock);

	
	std::string _Resolution(DEFAULT_RESOLUTION);
	dsVideoPortGetResolutionParam_t *param = (dsVideoPortGetResolutionParam_t *)arg;
	dsVideoPortResolution_t *resolution = &param->resolution;	

	dsVideoPortType_t _VPortType = _GetVideoPortType(param->handle);

	if (_VPortType == dsVIDEOPORT_TYPE_HDMI)
	{
		if(param->toPersist)
		{
			_Resolution = device::HostPersistence::getInstance().getProperty("HDMI0.resolution",_Resolution);
			__TIMESTAMP();printf("Reading HDMI  persistent resolution %s\r\n",_Resolution.c_str());
		}
		else{
			_Resolution = _dsHDMIResolution;
		}
	}
	else if (_VPortType == dsVIDEOPORT_TYPE_COMPONENT)
	{
		if(param->toPersist){
			#ifdef HAS_ONLY_COMPOSITE
				_Resolution = device::HostPersistence::getInstance().getProperty("Baseband0.resolution",_Resolution);
			#else
				_Resolution = device::HostPersistence::getInstance().getProperty("COMPONENT0.resolution",_Resolution);
			#endif
			__TIMESTAMP();printf("Reading Component persistent resolution %s\r\n",_Resolution.c_str());
		}
		else{
			_Resolution = _dsCompResolution; 
		}
	}
	
	strcpy(resolution->name,_Resolution.c_str());

	IARM_BUS_Unlock(lock);
	
	return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsSetResolution(void *arg)
{
    _DEBUG_ENTER();
	dsError_t ret = dsERR_NONE;
    
	IARM_BUS_Lock(lock);
	

	dsVideoPortSetResolutionParam_t *param = (dsVideoPortSetResolutionParam_t *)arg;
	
	if (param != NULL)
	{
	
		dsVideoPortResolution_t resolution = param->resolution;
		std::string resolutionName(resolution.name);

		/* * Check the Platform Resolution 
		   * If the platform Resolution is same as requested , Do not set new resolution
		   * Needed to avoid setting resolution during multiple hot plug  
		 */

		dsVideoPortResolution_t platresolution;
		memset(platresolution.name,'\0',sizeof(platresolution.name));
		dsGetResolution(param->handle,&platresolution);
		__TIMESTAMP();printf("Resolution Requested ..%s Platform Resolution - %s\r\n",resolution.name,platresolution.name);
#if 1 // GENERIC RDK output resolution set
        if ( NULL == getenv("BRCMDEBUG_OUTPUT_RES"))
#endif
		if ((strcmp(resolution.name,platresolution.name) == 0 ))
		{
			
			printf("Same Resolution ..Ignoring Resolution Request------\r\n");
                        _dsHDMIResolution = platresolution.name;
			IARM_BUS_Unlock(lock);
			return IARM_RESULT_SUCCESS;
		}
	
		/*!< Resolution Pre Change Event  - IARM_BUS_DSMGR_EVENT_RES_POSTCHANGE */
		_dsVideoPortPreResolutionCall(&param->resolution);

		/*!< Set Platform Resolution  */
		ret = dsSetResolution(param->handle, &param->resolution);
		
		/*!< Resolution Post Change Event  - IARM_BUS_DSMGR_EVENT_RES_POSTCHANGE */
		_dsSendVideoPortPostResolutionCall(&param->resolution);
		
		if (ret == dsERR_NONE)
		{
			/*!< Persist Resolution Settings */
			persistResolution(param);
		}
	}
		
    IARM_BUS_Unlock(lock);
	
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsVideoPortTerm(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);

    m_isPlatInitialized--;
	
	if (0 == m_isPlatInitialized)
	{
		dsVideoPortTerm();
	}
    
	IARM_BUS_Unlock(lock);
 
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsEnableHDCP(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);
   
    dsError_t ret = dsERR_NONE;

    __TIMESTAMP();printf("Enable HDCP in Platform !! \r\n");

    dsEnableHDCPParam_t *param = (dsEnableHDCPParam_t *)arg;
    ret = dsEnableHDCP(param->handle, param->contentProtect, param->hdcpKey, param->keySize);
    param->rpcResult = ret;
     
    IARM_BUS_Unlock(lock);
	
	return IARM_RESULT_SUCCESS;
}


static int  _dsSendVideoPortPostResolutionCall(dsVideoPortResolution_t *resolution)
{
	dsError_t ret = dsERR_NONE;

	if (resolution == NULL)
	{
		ret = dsERR_INVALID_PARAM;
	}

	if (ret == dsERR_NONE){
		//IARM_Bus_DSMgr_EventData_t eventData;
		  IARM_Bus_CommonAPI_ResChange_Param_t param;

			switch(resolution->pixelResolution) {
			case dsVIDEO_PIXELRES_720x480:
				param.width =  720;
				param.height = 480;
			break;
			case dsVIDEO_PIXELRES_720x576:
				param.width =  720;
				param.height = 576;
			break;

			case dsVIDEO_PIXELRES_1280x720:
				param.width =  1280;
				param.height = 720;
			break;
			
			case dsVIDEO_PIXELRES_1920x1080:
				param.width =  1920;
				param.height = 1080;
			break;

            case dsVIDEO_PIXELRES_3840x2160:
                param.width =  3840;
                param.height = 2160;
                break;
            case dsVIDEO_PIXELRES_4096x2160:
                param.width =  4096;
                param.height = 2160;
                break;

			case dsVIDEO_PIXELRES_MAX: //to mute compiler warning
			default:
				param.width =  1280;
				param.height = 720;
			break;
			}
			IARM_BusDaemon_ResolutionPostchange(param);
			//IARM_Bus_BroadcastEvent(IARM_BUS_DSMGR_NAME,(IARM_EventId_t)IARM_BUS_DSMGR_EVENT_RES_POSTCHANGE,(void *)&eventData, sizeof(eventData));
		}
	return ret;
}
static int  _dsVideoPortPreResolutionCall(dsVideoPortResolution_t *resolution)
{
	dsError_t ret = dsERR_NONE;
	if (resolution == NULL)
	{
		ret = dsERR_INVALID_PARAM;
	}

	if (ret == dsERR_NONE){
			IARM_Bus_CommonAPI_ResChange_Param_t param;

			switch(resolution->pixelResolution) {
			case dsVIDEO_PIXELRES_720x480:
				param.width =  720;
				param.height = 480;
			break;
			case dsVIDEO_PIXELRES_720x576:
				param.width =  720;
				param.height = 576;
			break;

			case dsVIDEO_PIXELRES_1280x720:
				param.width =  1280;
				param.height = 720;
			break;
			
			case dsVIDEO_PIXELRES_1920x1080:
				param.width =  1920;
				param.height = 1080;
			break;
            case dsVIDEO_PIXELRES_3840x2160:
                param.width =  3840;
                param.height = 2160;
                break;
            case dsVIDEO_PIXELRES_4096x2160:
                param.width =  4096;
                param.height = 2160;
                break;
			case dsVIDEO_PIXELRES_MAX: //to mute compiler warning
			default:
				param.width =  1280;
				param.height = 720;
			break;
			}
		IARM_BusDaemon_ResolutionPrechange(param);
		}
	return ret;
}

/*HDCP Status  Call back */
void _dsHdcpCallback (int handle, dsHdcpStatus_t status)
{
	IARM_Bus_DSMgr_EventData_t hdcp_eventData;
	

	if (handle == NULL_HANDLE)
	{
		printf("Err:HDMI Hot plug back has NULL Handle... !!!!!!..\r\n");
	}
	IARM_BUS_Lock(lock);
	switch(status)
	{
		case dsHDCP_STATUS_AUTHENTICATED:
			__TIMESTAMP();printf("DS HDCP Authenticated Event!!!!!!..\r\n");
			hdcp_eventData.data.hdmi_hdcp.hdcpStatus =  dsHDCP_STATUS_AUTHENTICATED;
			_hdcpStatus = status;
			break;

		case dsHDCP_STATUS_AUTHENTICATIONFAILURE:
			__TIMESTAMP();printf("DS HDCP Failure Event!!!!!!..\r\n");
			 hdcp_eventData.data.hdmi_hdcp.hdcpStatus =  dsHDCP_STATUS_AUTHENTICATIONFAILURE;
			_hdcpStatus = status;
			break;
		/* Based on discussion with Steve, we may handle the unpowered and unauthenticated 
		cases in a different manner. Logging this events for now.*/
		case dsHDCP_STATUS_UNPOWERED:
		case dsHDCP_STATUS_UNAUTHENTICATED:
		default:
			__TIMESTAMP();printf("HDCP Event Status from HAL is ...%d\n",status);
			return;
	}
	
	IARM_Bus_BroadcastEvent(IARM_BUS_DSMGR_NAME,(IARM_EventId_t)IARM_BUS_DSMGR_EVENT_HDCP_STATUS,(void *)&hdcp_eventData, sizeof(hdcp_eventData));
	IARM_BUS_Unlock(lock);
}

IARM_Result_t _dsGetHDCPStatus (void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);
    
    dsVideoPortGetHDCPStatus_t *param = (dsVideoPortGetHDCPStatus_t *)arg;

    if (param != NULL) {
      param->hdcpStatus = _hdcpStatus;
    }
   
    IARM_BUS_Unlock(lock);
	
    return IARM_RESULT_SUCCESS;
}


static dsVideoPortType_t _GetVideoPortType(int handle)
{
    int numPorts,i;
    int halhandle = 0;
    
    numPorts = dsUTL_DIM(kSupportedPortTypes);
    for(i=0; i< numPorts; i++)
    {
		dsGetVideoPort(kPorts[i].id.type, kPorts[i].id.index, &halhandle);
		if (handle == halhandle)
		{
			return kPorts[i].id.type;
		}
	}
	printf("Error: The Requested Video Port is not part of Platform Port Configuration \r\n");
	return dsVIDEOPORT_TYPE_MAX;
}


static void persistResolution(dsVideoPortSetResolutionParam_t *param)
{
	dsVideoPortResolution_t resolution = param->resolution;
	std::string resolutionName(resolution.name);

	try
	{
		dsVideoPortType_t _VPortType = _GetVideoPortType(param->handle);
		if (_VPortType == dsVIDEOPORT_TYPE_HDMI)
		{
			if(param->toPersist){
				device::HostPersistence::getInstance().persistHostProperty("HDMI0.resolution",resolutionName);
			}
			
			__TIMESTAMP();printf("Set Resolution on HDMI Port!!!!!!..\r\n");
			_dsHDMIResolution = resolutionName;

			if (false == IsCompatibleResolution(resolution.pixelResolution,getPixelResolution(_dsCompResolution)))
			{
				__TIMESTAMP();printf("HDMI Resolution is not Compatible with Analog ports..\r\n");
				_dsCompResolution = getCompatibleResolution(&resolution);
				__TIMESTAMP();printf("New Compatible resolution is %s  \r\n",_dsCompResolution.c_str());
				
				if(param->forceCompatible)
				{
					#ifdef HAS_ONLY_COMPOSITE
						device::HostPersistence::getInstance().persistHostProperty("Baseband0.resolution",_dsCompResolution);
					#else
						device::HostPersistence::getInstance().persistHostProperty("COMPONENT0.resolution",_dsCompResolution);
					#endif	
				}
			}
			else
			{
				__TIMESTAMP();printf("HDMI and Analog Ports Resolutions are  Compatible \r\n");
			}

		}
		else if (_VPortType == dsVIDEOPORT_TYPE_COMPONENT)
		{

			if(param->toPersist){
				#ifdef HAS_ONLY_COMPOSITE
					device::HostPersistence::getInstance().persistHostProperty("Baseband0.resolution",resolutionName);
				#else
					device::HostPersistence::getInstance().persistHostProperty("COMPONENT0.resolution",resolutionName);
				#endif	
			}

			__TIMESTAMP();printf("Set Resolution on Component/Composite Ports!!!!!!..\r\n");
			_dsCompResolution = resolutionName;
			if (false == IsCompatibleResolution(resolution.pixelResolution,getPixelResolution(_dsHDMIResolution)))
			{
				__TIMESTAMP();printf("HDMI Resolution is not Compatible with Analog ports..\r\n");
				
				_dsHDMIResolution = getCompatibleResolution(&resolution);
				if (_dsHDMIResolution.compare("480i") == 0)
					_dsHDMIResolution = "480p";

				__TIMESTAMP();printf("New Compatible resolution is %s  \r\n",_dsHDMIResolution.c_str());
				if(param->forceCompatible)
				{
					device::HostPersistence::getInstance().persistHostProperty("HDMI0.resolution",_dsHDMIResolution);
				}
			}
			else
			{
				__TIMESTAMP();printf("HDMI and Analog Ports Resolutions are  Compatible \r\n");
			}
		}
	}
	catch(...) 
	{
		printf("Error in Persisting the Video Resolution..... \r\n");
	}
}


#ifdef HAS_INIT_RESN_SETTINGS
IARM_Result_t _dsInitResolution(void *arg)
{
	_DEBUG_ENTER();
	dsError_t ret = dsERR_NONE;
	IARM_BUS_Lock(lock);

	dsVideoPortSetResolutionParam_t *param = (dsVideoPortSetResolutionParam_t *)arg;	
	if (param == NULL)
	{
		return IARM_RESULT_INVALID_STATE;
	}

	ret = dsInitResolution(&param->resolution);
	if (ret == dsERR_NONE)
	{
		persistResolution(param);
	}

	IARM_BUS_Unlock(lock);	
	return IARM_RESULT_SUCCESS;
}
#endif


static  std::string getCompatibleResolution(dsVideoPortResolution_t *SrcResn)
{
   dsError_t ret = dsERR_NONE;
   std::string resolution("720p");

   if (SrcResn == NULL)
   {
      ret = dsERR_INVALID_PARAM;
   }

   if (ret == dsERR_NONE){
      switch(SrcResn->pixelResolution) {
      
         case dsVIDEO_PIXELRES_720x480:
          	return resolution.assign(SrcResn->name);
         break;

         case dsVIDEO_PIXELRES_1280x720:
         case dsVIDEO_PIXELRES_1920x1080:           
         case dsVIDEO_PIXELRES_3840x2160:
         case dsVIDEO_PIXELRES_4096x2160:
         case dsVIDEO_PIXELRES_MAX: 
         default:
       		  return resolution.assign(kResolutions[kDefaultResIndex].name);
         break;
      }
   }
   return resolution;
}

static bool  IsCompatibleResolution(dsVideoResolution_t pixelResolution1,dsVideoResolution_t pixelResolution2)
{
   bool bret = false;

 	if( pixelResolution1 == pixelResolution2) {
            bret = true;
        }
	else if((IsHDCompatible(pixelResolution1)) && (IsHDCompatible(pixelResolution2))) {
            bret = true;
         }
    return  bret;
}

static dsVideoResolution_t getPixelResolution(std::string &resolution )
{
  	dsVideoPortResolution_t *Resn = &kResolutions[kDefaultResIndex]; 
	
	for (unsigned int i = 0; i < dsUTL_DIM(kResolutions); i++)
	{
		Resn = &kResolutions[i];
		if (resolution.compare(Resn->name) == 0 )
		{
			break;
		}
	}
	return Resn->pixelResolution;
}



/** @} */
/** @} */
