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


#include "dsAudio.h"

#include <sys/types.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <dlfcn.h>
#include "dsError.h"
#include "dsUtl.h"
#include "dsTypes.h"
#include "pthread.h"
#include "libIARM.h"
#include "libIBus.h"
#include "iarmUtil.h"
#include "dsRpc.h"
#include "dsMgr.h"
#include "hostPersistence.hpp"
#include "dsserverlogger.h"
#include "dsAudioSettings.h"

static int m_isInitialized = 0;
static int m_isPlatInitialized = 0;

static pthread_mutex_t dsLock = PTHREAD_MUTEX_INITIALIZER;
int _srv_AudioAuto  = 0;
dsAudioStereoMode_t _srv_HDMI_Audiomode = dsAUDIO_STEREO_STEREO;
dsAudioStereoMode_t _srv_SPDIF_Audiomode = dsAUDIO_STEREO_STEREO;

#define IARM_BUS_Lock(lock) pthread_mutex_lock(&dsLock)
#define IARM_BUS_Unlock(lock) pthread_mutex_unlock(&dsLock)

IARM_Result_t _dsAudioPortInit(void *arg);
IARM_Result_t _dsGetAudioPort(void *arg);
IARM_Result_t _dsSetStereoMode(void *arg);
IARM_Result_t _dsSetStereoAuto(void *arg);
IARM_Result_t _dsGetStereoAuto(void *arg);
IARM_Result_t _dsSetAudioMute(void *arg);
IARM_Result_t _dsIsAudioMute(void *arg);
IARM_Result_t _dsAudioPortTerm(void *arg);
IARM_Result_t _dsGetStereoMode(void *arg);
IARM_Result_t _dsGetEncoding(void *arg);
IARM_Result_t _dsIsAudioMSDecode(void *arg);
IARM_Result_t _dsIsAudioPortEnabled(void *arg);
IARM_Result_t _dsEnableAudioPort(void *arg);



static void _GetAudioModeFromPersistent(void *arg);
static dsAudioPortType_t _GetAudioPortType(int handle);


IARM_Result_t dsAudioMgr_init()
{
   try
	{
		/* Get the AudioModesettings FOR HDMI from Persistence */
		std::string _AudioModeSettings("STEREO");
		_AudioModeSettings = device::HostPersistence::getInstance().getProperty("HDMI0.AudioMode",_AudioModeSettings);
		__TIMESTAMP();printf("The HDMI Audio Mode Setting on startup  is %s \r\n",_AudioModeSettings.c_str());
		if (_AudioModeSettings.compare("MONO") == 0)
		{
			_srv_HDMI_Audiomode = dsAUDIO_STEREO_MONO;
		}
		else if (_AudioModeSettings.compare("SURROUND") == 0)
		{
			_srv_HDMI_Audiomode = dsAUDIO_STEREO_SURROUND;
		}
		else if (_AudioModeSettings.compare("PASSTHRU") == 0)
		{
			_srv_HDMI_Audiomode = dsAUDIO_STEREO_PASSTHRU;
		}
                else 
                {
			_srv_HDMI_Audiomode = dsAUDIO_STEREO_STEREO;
                }


		/* Get the AutoModesettings FOR HDMI from Persistence */
                /* If HDMI persistence is surround, Auto defaults to true */
            std::string _AudioModeAuto("FALSE");

            #if 0
               /* 
                Commenting this to fix the persistent settings
                Audio mode should not be forced to Auto 
                To enabale this we need to change the DS Mgr implementation 
                which reads the _dsGetStereoMode to  know the persistent value...*/
                if (_srv_HDMI_Audiomode == dsAUDIO_STEREO_SURROUND) 
                {
                    _AudioModeAuto = "TRUE";
                }
            #endif
          
	    _AudioModeAuto = device::HostPersistence::getInstance().getProperty("HDMI0.AudioMode.AUTO",_AudioModeAuto);
	    if (_AudioModeAuto.compare("TRUE") == 0)
	    {
	        _srv_AudioAuto = 1;
	    }
        else 
        {
			_srv_AudioAuto = 0;
        }
		__TIMESTAMP();printf("The HDMI Audio Auto Setting on startup  is %s \r\n",_AudioModeAuto.c_str());
		
		/* Get the AudioModesettings for SPDIF from Persistence */
		std::string _SPDIFModeSettings("STEREO");
		_SPDIFModeSettings = device::HostPersistence::getInstance().getProperty("SPDIF0.AudioMode",_SPDIFModeSettings);
		__TIMESTAMP();printf("The SPDIF Audio Mode Setting on startup  is %s \r\n",_SPDIFModeSettings.c_str());
		if (_SPDIFModeSettings.compare("MONO") == 0)
		{
			_srv_SPDIF_Audiomode = dsAUDIO_STEREO_MONO;
		}
		else if (_SPDIFModeSettings.compare("SURROUND") == 0)
		{
			_srv_SPDIF_Audiomode = dsAUDIO_STEREO_SURROUND;
		}
		else if (_SPDIFModeSettings.compare("PASSTHRU") == 0)
		{
			_srv_SPDIF_Audiomode = dsAUDIO_STEREO_PASSTHRU;
		}
        else 
        {
			_srv_SPDIF_Audiomode = dsAUDIO_STEREO_STEREO;
        }
	}
	catch(...) 
	{
		printf("Exception in Getting the Audio  settings on Startup..... \r\n");
	}
    	if (!m_isPlatInitialized) {
    		dsAudioPortInit();
	   	}
        /*coverity[missing_lock]  CID-19380 using Coverity Annotation to ignore error*/
        m_isPlatInitialized ++;

	IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsAudioPortInit, _dsAudioPortInit);
    return IARM_RESULT_SUCCESS;
}



IARM_Result_t dsAudioMgr_term()
{
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsAudioPortInit(void *arg)
{
    IARM_BUS_Lock(lock);

    if (!m_isInitialized) {

        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetAudioPort,_dsGetAudioPort);
        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetStereoMode,_dsSetStereoMode);
        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetStereoMode,_dsGetStereoMode);
        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetStereoAuto,_dsSetStereoAuto);
        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetStereoAuto,_dsGetStereoAuto);
        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetAudioMute,_dsSetAudioMute);
        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsIsAudioMute,_dsIsAudioMute);
        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetEncoding,_dsGetEncoding);
        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsIsAudioMSDecode,_dsIsAudioMSDecode);

        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsIsAudioPortEnabled,_dsIsAudioPortEnabled);
        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsEnableAudioPort,_dsEnableAudioPort);

        IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsAudioPortTerm,_dsAudioPortTerm);

        m_isInitialized = 1;
    }

    if (!m_isPlatInitialized) {
        /* Nexus init, if any here */
        dsAudioPortInit();
   }
   m_isPlatInitialized++;


    IARM_BUS_Unlock(lock);

 return IARM_RESULT_SUCCESS;

}

IARM_Result_t _dsGetAudioPort(void *arg)
{
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);

    dsAudioGetHandleParam_t *param = (dsAudioGetHandleParam_t *)arg;

    if (param != NULL)
    {
        printf("%s..%d-%d \r\n",__func__,param->type,param->index);
        dsGetAudioPort(param->type, param->index, &param->handle);
    }

    IARM_BUS_Unlock(lock);

    return IARM_RESULT_SUCCESS;
}



IARM_Result_t _dsGetStereoMode(void *arg)
{
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);

    dsAudioSetStereoModeParam_t *param = (dsAudioSetStereoModeParam_t *)arg;

    if (param != NULL && param->toPersist) {
        _GetAudioModeFromPersistent(arg);
    }
    else if (param != NULL)
    {
        /* In Auto Mode, get the effective mode */
        dsAudioStereoMode_t stereoMode = dsAUDIO_STEREO_UNKNOWN;
        dsGetStereoMode(param->handle, &stereoMode);
        param->mode = stereoMode;
        printf("The Audio Stereo Mode obtained is %d \r\n",param->mode);
   }
    if(param->mode == dsAUDIO_STEREO_PASSTHRU)
    {
        param->mode = dsAUDIO_STEREO_SURROUND;  
        printf("The Audio Stereo Mode updated to %d \r\n",param->mode);
    }

    IARM_BUS_Unlock(lock);    

    return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsSetStereoMode(void *arg)
{
    _DEBUG_ENTER();
    IARM_Bus_DSMgr_EventData_t eventData;

    IARM_BUS_Lock(lock);

    dsError_t ret = dsERR_NONE;
    dsAudioSetStereoModeParam_t *param = (dsAudioSetStereoModeParam_t *)arg;

    ret = dsSetStereoMode(param->handle, param->mode);
    param->rpcResult = ret;

    if (ret == dsERR_NONE)
    {
        dsAudioPortType_t _APortType = _GetAudioPortType(param->handle);
        try
        {
            if(param->mode == dsAUDIO_STEREO_MONO)
            {
                __TIMESTAMP();printf("Setting Audio Mode MONO with persistent value : %d \r\n",param->toPersist);

                if (_APortType == dsAUDIOPORT_TYPE_HDMI)
                {
                    if (param->toPersist)
                    device::HostPersistence::getInstance().persistHostProperty("HDMI0.AudioMode","MONO");

                    _srv_HDMI_Audiomode = dsAUDIO_STEREO_MONO;
                }
                else if (_APortType == dsAUDIOPORT_TYPE_SPDIF)
                {
                    if (param->toPersist)
                    device::HostPersistence::getInstance().persistHostProperty("SPDIF0.AudioMode","MONO");

                    _srv_SPDIF_Audiomode = dsAUDIO_STEREO_MONO;
                }
                eventData.data.Audioport.mode = dsAUDIO_STEREO_MONO;
                eventData.data.Audioport.type = _APortType;
                IARM_Bus_BroadcastEvent(IARM_BUS_DSMGR_NAME,(IARM_EventId_t)IARM_BUS_DSMGR_EVENT_AUDIO_MODE,(void *)&eventData, sizeof(eventData));

            }
            else if(param->mode == dsAUDIO_STEREO_STEREO)
            {
                __TIMESTAMP();printf("Setting Audio Mode STEREO with persistent value : %d \r\n",param->toPersist);

                if (_APortType == dsAUDIOPORT_TYPE_HDMI)
                {
                    if (param->toPersist)
                    device::HostPersistence::getInstance().persistHostProperty("HDMI0.AudioMode","STEREO");
                
                    _srv_HDMI_Audiomode = dsAUDIO_STEREO_STEREO;
                }
                else if (_APortType == dsAUDIOPORT_TYPE_SPDIF)
                {
                    if (param->toPersist)
                    device::HostPersistence::getInstance().persistHostProperty("SPDIF0.AudioMode","STEREO");
                
                    _srv_SPDIF_Audiomode = dsAUDIO_STEREO_STEREO;
                }
                eventData.data.Audioport.mode = dsAUDIO_STEREO_STEREO;
                eventData.data.Audioport.type = _APortType;
                IARM_Bus_BroadcastEvent(IARM_BUS_DSMGR_NAME,(IARM_EventId_t)IARM_BUS_DSMGR_EVENT_AUDIO_MODE,(void *)&eventData, sizeof(eventData));

            }
            else if(param->mode == dsAUDIO_STEREO_SURROUND)
            {
                __TIMESTAMP();printf("Setting Audio Mode SURROUND with persistent value %d \r\n",param->toPersist);

                if (_APortType == dsAUDIOPORT_TYPE_HDMI)
                {
                    if (param->toPersist)
                    device::HostPersistence::getInstance().persistHostProperty("HDMI0.AudioMode","SURROUND");
                
                    _srv_HDMI_Audiomode = dsAUDIO_STEREO_SURROUND;
                }
                else if (_APortType == dsAUDIOPORT_TYPE_SPDIF)
                {
                    if (param->toPersist)
                    device::HostPersistence::getInstance().persistHostProperty("SPDIF0.AudioMode","SURROUND");

                    _srv_SPDIF_Audiomode = dsAUDIO_STEREO_SURROUND;
                }

                eventData.data.Audioport.mode = dsAUDIO_STEREO_SURROUND;
                eventData.data.Audioport.type = _APortType;
                IARM_Bus_BroadcastEvent(IARM_BUS_DSMGR_NAME,(IARM_EventId_t)IARM_BUS_DSMGR_EVENT_AUDIO_MODE,(void *)&eventData, sizeof(eventData));
            }
            else if(param->mode == dsAUDIO_STEREO_PASSTHRU)
            {
                __TIMESTAMP();printf("Setting Audio Mode PASSTHRU with persistent value %d \r\n",param->toPersist);

                if (_APortType == dsAUDIOPORT_TYPE_HDMI)
                {
                    if (param->toPersist)
                    device::HostPersistence::getInstance().persistHostProperty("HDMI0.AudioMode","PASSTHRU");
                    _srv_HDMI_Audiomode = dsAUDIO_STEREO_PASSTHRU;
                }
                else if (_APortType == dsAUDIOPORT_TYPE_SPDIF)
                {
                    if (param->toPersist)
                    device::HostPersistence::getInstance().persistHostProperty("SPDIF0.AudioMode","PASSTHRU");
                    _srv_SPDIF_Audiomode = dsAUDIO_STEREO_PASSTHRU;
                }

                eventData.data.Audioport.mode = dsAUDIO_STEREO_PASSTHRU;
                eventData.data.Audioport.type = _APortType;
                IARM_Bus_BroadcastEvent(IARM_BUS_DSMGR_NAME,(IARM_EventId_t)IARM_BUS_DSMGR_EVENT_AUDIO_MODE,(void *)&eventData, sizeof(eventData));

            }
        }
        catch(...)
        {
            printf("Error in Setting audio mode... \r\n");
        }

    }

    IARM_BUS_Unlock(lock);

    return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsGetStereoAuto(void *arg)
{
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);

    dsAudioSetStereoAutoParam_t *param = (dsAudioSetStereoAutoParam_t *)arg;

    if (param != NULL)
    {
        param->autoMode = (_srv_AudioAuto ? 1 : 0);
    }

    IARM_BUS_Unlock(lock);

    return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsSetStereoAuto(void *arg)
{
    _DEBUG_ENTER();
    IARM_Bus_DSMgr_EventData_t eventData;

    IARM_BUS_Lock(lock);

    dsAudioSetStereoAutoParam_t *param = (dsAudioSetStereoAutoParam_t *)arg;

    if (param->autoMode)  {
        device::HostPersistence::getInstance().persistHostProperty("HDMI0.AudioMode.AUTO","TRUE");
        _srv_AudioAuto = 1;
    }
    else {
        device::HostPersistence::getInstance().persistHostProperty("HDMI0.AudioMode.AUTO","FALSE");
        _srv_AudioAuto = 0;
    }

    IARM_BUS_Unlock(lock);
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsSetAudioMute(void *arg)
{
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);

    dsAudioSetMutedParam_t *param = (dsAudioSetMutedParam_t *)arg;
    bool enabled = false;

    dsError_t ret = dsIsAudioPortEnabled(param->handle, &enabled);
    if(enabled)
    {
        dsSetAudioMute(param->handle, param->mute);
    }
    else
    {
       __TIMESTAMP();printf("Do not Mute/unmute Audio when Audio ports are disabled \r\n");
    }

    IARM_BUS_Unlock(lock);

    return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsIsAudioMute(void *arg)
{
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);

    dsAudioSetMutedParam_t *param = (dsAudioSetMutedParam_t *)arg;
    bool muted = false;
    
    dsError_t ret = dsIsAudioMute(param->handle, &muted);
    if (ret == dsERR_NONE) {
        param->mute = muted;
    }

    IARM_BUS_Unlock(lock);

    return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsIsAudioPortEnabled(void *arg)
{
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);

    dsAudioPortEnabledParam_t *param = (dsAudioPortEnabledParam_t *)arg;
    bool enabled = false;
    
    dsError_t ret = dsIsAudioPortEnabled(param->handle, &enabled);
    if (ret == dsERR_NONE) {
        param->enabled = enabled;
    }

    IARM_BUS_Unlock(lock);

    return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsEnableAudioPort(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);
   
    dsAudioPortEnabledParam_t *param = (dsAudioPortEnabledParam_t *)arg;
    dsEnableAudioPort(param->handle, param->enabled);
   
    IARM_BUS_Unlock(lock);
    
    return IARM_RESULT_SUCCESS;
}



IARM_Result_t _dsAudioPortTerm(void *arg)
{
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);

    m_isPlatInitialized--;

    if (0 == m_isPlatInitialized)
    {
        dsAudioPortTerm();
    }

    IARM_BUS_Unlock(lock);

    return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsGetEncoding(void *arg)
{
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);

    dsAudioGetEncodingModeParam_t *param = (dsAudioGetEncodingModeParam_t *)arg;

    if (param != NULL)
    {

        dsAudioEncoding_t _encoding = dsAUDIO_ENC_NONE;
        dsGetAudioEncoding(param->handle, &_encoding);
        param->encoding = _encoding;


     __TIMESTAMP();printf("param->encoding = %d\r\n",_encoding);
    
    }

    IARM_BUS_Unlock(lock);

    return IARM_RESULT_SUCCESS;
}

static dsAudioPortType_t _GetAudioPortType(int handle)
{
    int numPorts,i;
    int halhandle = 0;

    numPorts = dsUTL_DIM(kSupportedPortTypes);
    
    for(i=0; i< numPorts; i++)
    {
        dsGetAudioPort(kPorts[i].id.type, kPorts[i].id.index, &halhandle);
        if (handle == halhandle)
        {
            return kPorts[i].id.type;
        }
    }
    __TIMESTAMP();printf("Error: The Requested Audio Port is not part of Platform Port Configuration \r\n");
    return dsAUDIOPORT_TYPE_MAX;
}


IARM_Result_t _dsIsAudioMSDecode(void *arg)
{

    IARM_BUS_Unlock(lock);

#ifndef RDK_DSHAL_NAME
    #warning   "RDK_DSHAL_NAME is not defined"
    #define RDK_DSHAL_NAME "RDK_DSHAL_NAME is not defined"
#endif
    _DEBUG_ENTER();

    IARM_BUS_Lock(lock);
    
    typedef dsError_t  (*dsIsAudioMSDecode_t)(int handle, bool *HasMS11Decode);
    static dsIsAudioMSDecode_t func = NULL;
    if (func == NULL) {
        void *dllib = dlopen(RDK_DSHAL_NAME, RTLD_LAZY);
        if (dllib) {
            func = (dsIsAudioMSDecode_t) dlsym(dllib, "dsIsAudioMSDecode");
            if (func) {
                __TIMESTAMP();printf("dsIsAudioMSDecode(int, bool*) is defined and loaded\r\n");
            }   
            else {
                __TIMESTAMP();printf("dsIsAudioMSDecode(int, bool*) is not defined\r\n");
            }   
            dlclose(dllib);
        }   
        else {
            __TIMESTAMP();printf("Opening RDK_DSHAL_NAME [%s] failed\r\n", RDK_DSHAL_NAME);
        }   
    }   

    dsAudioGetMS11Param_t *param = (dsAudioGetMS11Param_t *)arg;
    if (func != NULL) {
        bool HasMS11Decode = false;
        dsError_t ret = func(param->handle, &HasMS11Decode);
        if (ret == dsERR_NONE) {
            param->ms11Enabled = HasMS11Decode;
        }
    }else {
        param->ms11Enabled = false;
    }

    IARM_BUS_Unlock(lock);

    return IARM_RESULT_SUCCESS;
}


static void _GetAudioModeFromPersistent(void *arg)
{
    _DEBUG_ENTER();

    dsAudioSetStereoModeParam_t *param = (dsAudioSetStereoModeParam_t *)arg;

    if (param != NULL)
    {
        dsAudioPortType_t _APortType = _GetAudioPortType(param->handle);
        std::string _AudioModeSettings("STEREO");

        if (_APortType == dsAUDIOPORT_TYPE_SPDIF)
        {   
           _AudioModeSettings = device::HostPersistence::getInstance().getProperty("SPDIF0.AudioMode",_AudioModeSettings);
           __TIMESTAMP();printf("The SPDIF Audio Mode Setting From Persistent is %s \r\n",_AudioModeSettings.c_str());
        }
        else if (_APortType == dsAUDIOPORT_TYPE_HDMI) {
            _AudioModeSettings = device::HostPersistence::getInstance().getProperty("HDMI0.AudioMode",_AudioModeSettings);
            __TIMESTAMP();printf("The HDMI Audio Mode Setting From Persistent is %s \r\n",_AudioModeSettings.c_str());
        }

        if (_AudioModeSettings.compare("MONO") == 0)
        {
            param->mode = dsAUDIO_STEREO_MONO;
        }
        else if (_AudioModeSettings.compare("SURROUND") == 0)
        {
            param->mode = dsAUDIO_STEREO_SURROUND;
        }
        else if (_AudioModeSettings.compare("PASSTHRU") == 0)
        {
            param->mode = dsAUDIO_STEREO_PASSTHRU;
        }
        else 
        {
            param->mode = dsAUDIO_STEREO_STEREO;
        } 
    }
}


/** @} */
/** @} */

