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


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include "dsFPD.h"
#include "dsRpc.h"
#include "dsTypes.h"
#include "dsFPDSettings.h"
#include "dsserverlogger.h"
#include "dsMgr.h"
	
#include "iarmUtil.h"
#include "libIARM.h"
#include "libIBus.h"

#include <iostream>
#include "hostPersistence.hpp"
#include <sstream>
#include <vector>


#define direct_list_top(list) ((list))
#define IARM_BUS_Lock(lock) pthread_mutex_lock(&fpLock)
#define IARM_BUS_Unlock(lock) pthread_mutex_unlock(&fpLock)

static int m_isInitialized = 0;
static int m_isPlatInitialized=0;
static pthread_mutex_t fpLock = PTHREAD_MUTEX_INITIALIZER;

IARM_Result_t dsFPDMgr_init();
IARM_Result_t dsFPDMgr_term();
IARM_Result_t _dsFPInit(void *arg);
IARM_Result_t _dsFPTerm(void *arg);
IARM_Result_t _dsSetFPText(void *arg);
IARM_Result_t _dsSetFPTime(void *arg);
IARM_Result_t _dsSetFPScroll(void *arg);
IARM_Result_t _dsSetFPBlink(void *arg);
IARM_Result_t _dsGetFPBrightness(void *arg);
IARM_Result_t _dsSetFPBrightness(void *arg);
IARM_Result_t _dsSetFPState(void *arg);
IARM_Result_t _dsGetFPState(void *arg);
IARM_Result_t _dsSetFPColor(void *arg);
IARM_Result_t _dsGetFPColor(void *arg);
IARM_Result_t _dsSetFPTextBrightness(void *arg);
IARM_Result_t _dsGetFPTextBrightness(void *arg);
IARM_Result_t _dsFPEnableCLockDisplay(void *arg);
IARM_Result_t _dsGetTimeFormat(void *arg);
IARM_Result_t _dsSetTimeFormat(void *arg);

#ifdef HAS_CLOCK_DISPLAY
static  dsFPDBrightness_t _dsTextBrightness  = dsFPD_BRIGHTNESS_MAX ;
#endif

/** Structure that defines internal data base for the FP */
typedef struct _dsFPDSettings_t_
{   
    dsFPDBrightness_t brightness;
    dsFPDState_t state;
    dsFPDColor_t color;
}_FPDSettings_t;

using namespace std;

/** Variable that stores the brightness and State for FP */
static _FPDSettings_t srvFPDSettings[dsFPD_INDICATOR_MAX];
const std::string default_led_color("White");
vector<dsFPDColors_t> allColors (arrayAllColors, arrayAllColors + sizeof(arrayAllColors) / sizeof(arrayAllColors[0]) );

std::string numberToString (int number);
int stringToNumber (std::string text);
std::string enumToColor (dsFPDColor_t enumColor);

std::string numberToString (int number)
{
    stringstream convert;
    convert << number;
    return convert.str();
}


int stringToNumber (std::string text)
{
    int number;
    stringstream convert (text);

    if (!(convert >> number) )
    number = 0;

	return number;
}

std::string enumToColor (dsFPDColor_t enumColor)
{
	std::string color;
  stringstream convert;
  unsigned int i;

  for (i=0; i < allColors.size(); i++)
  {
    if (allColors[i].color == enumColor)
    {
      color = std::string(allColors[i].name);
      break;
    }
  }
  if (i < allColors.size())
  {
    return color;
  }
  else
  {
    convert << enumColor;
    return convert.str();
  }
}

dsFPDColor_t colorToEnum(std::string &colorName)
{
  dsFPDColor_t color = 0;
  unsigned int i;

  for (i = 0; i < allColors.size(); i++)
  {
    if (strcmp(allColors[i].name, colorName.c_str()) == 0)
    {
      color = allColors[i].color;
      break;
    }
  }
  return color;
}

IARM_Result_t dsFPDMgr_init()
{

	try
	{
		/* Init the Power and Clock Brightness */
		string value;
		int maxBrightness = dsFPD_BRIGHTNESS_MAX;	

        for (int i = 0; i < dsFPD_INDICATOR_MAX; i++)
        {
            srvFPDSettings[i].brightness = dsFPD_BRIGHTNESS_MAX;
            srvFPDSettings[i].state = dsFPD_STATE_OFF;
            srvFPDSettings[i].color = 0;
        }

		value = device::HostPersistence::getInstance().getProperty("Power.brightness", numberToString(maxBrightness));
		srvFPDSettings[dsFPD_INDICATOR_POWER].brightness = stringToNumber(value);
		INFO("Power brightness initialized to        %s percent.\n", value.c_str());

		value = device::HostPersistence::getInstance().getProperty("Network.brightness", numberToString(maxBrightness));
		srvFPDSettings[dsFPD_INDICATOR_NETWORK].brightness = stringToNumber(value);
		INFO("Network brightness initialized to      %s percent.\n", value.c_str());

		value = device::HostPersistence::getInstance().getProperty("Wifi.brightness", numberToString(maxBrightness));
		srvFPDSettings[dsFPD_INDICATOR_WIFI].brightness = stringToNumber(value);
		INFO("Wifi brightness initialized to         %s percent.\n", value.c_str());

		value = device::HostPersistence::getInstance().getProperty("Power.color", default_led_color);
		srvFPDSettings[dsFPD_INDICATOR_POWER].color = colorToEnum(value);
		INFO("Power indicator color initialized to   %s\n", value.c_str());

		value = device::HostPersistence::getInstance().getProperty("Network.color", default_led_color);
		srvFPDSettings[dsFPD_INDICATOR_NETWORK].color = colorToEnum(value);
		INFO("Network indicator color initialized to %s\n", value.c_str());

		value = device::HostPersistence::getInstance().getProperty("Wifi.color", default_led_color);
		srvFPDSettings[dsFPD_INDICATOR_WIFI].color = colorToEnum(value);
		INFO("Wifi indicator color initialized to    %s\n", value.c_str());

	#ifdef HAS_CLOCK_DISPLAY
		value = device::HostPersistence::getInstance().getProperty("Text.brightness", numberToString(maxBrightness));
		_dsTextBrightness = stringToNumber(value);
		string _TimeFormat("12_HOUR");
		/* Get the Time Format from Persistence */
		_TimeFormat = device::HostPersistence::getInstance().getProperty("Text.timeformat",_TimeFormat);
		if (_TimeFormat.compare("12_HOUR") == 0)
		{
			_dsTextTimeFormat = dsFPD_TIME_12_HOUR;
			device::HostPersistence::getInstance().persistHostProperty("Text.timeformat","12_HOUR");
		}
		else if (_TimeFormat.compare("24_HOUR") == 0)
		{
			_dsTextTimeFormat = dsFPD_TIME_24_HOUR;
			device::HostPersistence::getInstance().persistHostProperty("Text.timeformat","24_HOUR");
		}
		INFO("Clock Time Format Read from Persistent is %s \r\n",_TimeFormat.c_str());
	#endif
	}
	catch(...)
	{
		ERROR("Error in Reading Brightness Value On Startup.. Use Default value \r\n");
	}

	if (!m_isPlatInitialized) {
	  dsFPInit();
	  m_isPlatInitialized = 1;
	  for(int i=0; i<dsFPD_INDICATOR_MAX; i++)
	  {
	    dsSetFPBrightness(i, srvFPDSettings[i].brightness);
	    dsSetFPColor(i, srvFPDSettings[i].color);
	  }
	}
	IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsFPInit,_dsFPInit);

	return IARM_RESULT_SUCCESS;
}

IARM_Result_t dsFPDMgr_term()
{
   return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsFPInit(void *arg)
{
    IARM_BUS_Lock(lock);
    int i;


    if (!m_isInitialized) {

    	INFO("<<<<< called _dsFPInit >>>>>>>>\r\n");

		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsFPTerm,_dsFPTerm);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetFPText,_dsSetFPText);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetFPTime,_dsSetFPTime);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetFPScroll,_dsSetFPScroll);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetFPBlink,_dsSetFPBlink);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetFPBrightness,_dsGetFPBrightness);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetFPState,_dsSetFPState);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetFPState,_dsGetFPState);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetFPBrightness,_dsSetFPBrightness);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetFPColor,_dsSetFPColor);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetFPColor,_dsGetFPColor);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetFPTextBrightness,_dsGetFPTextBrightness);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetFPTextBrightness,_dsSetFPTextBrightness);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsFPEnableCLockDisplay,_dsFPEnableCLockDisplay);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsGetTimeFormat,_dsGetTimeFormat);
		IARM_Bus_RegisterCall(IARM_BUS_DSMGR_API_dsSetTimeFormat,_dsSetTimeFormat);

        m_isInitialized = 1;
    }

    if (!m_isPlatInitialized) {
        dsFPInit();
        m_isPlatInitialized = 1;
        for(i=0; i<dsFPD_INDICATOR_MAX; i++)
        {
          dsSetFPBrightness(i, srvFPDSettings[i].brightness);
          dsSetFPColor(i, srvFPDSettings[i].color);
        }
    }

    IARM_BUS_Unlock(lock);
	
    return IARM_RESULT_SUCCESS;
}
IARM_Result_t _dsFPTerm(void *arg)
{
    /*@TODO: Deregister the calls, if necessary */
    _DEBUG_ENTER();


	IARM_BUS_Lock(lock);

	if (m_isPlatInitialized) {
        dsFPTerm();
        m_isPlatInitialized = 0;
    }

    IARM_BUS_Unlock(lock);
	
    return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsSetFPText(void *arg)
{
    _DEBUG_ENTER();
    
     #ifdef HAS_CLOCK_DISPLAY
    IARM_BUS_Lock(lock);

	dsSetFPText((char *) arg);

    IARM_BUS_Unlock(lock);
    #endif
	
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsSetFPTime(void *arg)
{
    _DEBUG_ENTER();
    
    #ifdef HAS_CLOCK_DISPLAY
    IARM_BUS_Lock(lock);

	dsFPDTimeParam_t *param = (dsFPDTimeParam_t *)arg;
    dsSetFPTime(_dsTextTimeFormat, param->nHours, param->nMinutes);

    IARM_BUS_Unlock(lock);
    #endif
	
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsSetFPScroll(void *arg)
{
    _DEBUG_ENTER();
   	
   	#ifdef HAS_CLOCK_DISPLAY
    IARM_BUS_Lock(lock);

	dsFPDScrollParam_t *param = (dsFPDScrollParam_t *)arg;
    dsSetFPScroll(param->nScrollHoldOnDur, param->nHorzScrollIterations, param->nVertScrollIterations);

    IARM_BUS_Unlock(lock);
    #endif

	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsSetFPBlink(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);

	dsFPDBlinkParam_t *param = (dsFPDBlinkParam_t *)arg;
    dsSetFPBlink(param->eIndicator, param->nBlinkDuration, param->nBlinkIterations);

    IARM_BUS_Unlock(lock);
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsGetFPBrightness(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);

	dsFPDBrightParam_t *param = (dsFPDBrightParam_t *)arg;

	if (isIndicatorValid(param->eIndicator))
	  param->eBrightness = srvFPDSettings[param->eIndicator].brightness;


	INFO("_dsGetFPBrightness brightness is %d for indicator %d\n", param->eBrightness, param->eIndicator);

	IARM_BUS_Unlock(lock);
	return IARM_RESULT_SUCCESS;

}



IARM_Result_t _dsSetFPBrightness(void *arg)
{
  dsError_t res = dsERR_NONE;
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);

	dsFPDBrightParam_t *param = (dsFPDBrightParam_t *)arg;

	
	if (isIndicatorValid(param->eIndicator) && param->eBrightness <= dsFPD_BRIGHTNESS_MAX)
    {
	  res = dsSetFPBrightness(param->eIndicator, param->eBrightness);
		if (dsERR_NONE != res)
		{
		  ERROR("Something went wrong when calling dsSetFPBrightness(), ret=%d\n", res);
		  IARM_BUS_Unlock(lock);
		  return IARM_RESULT_SUCCESS;
		}
		srvFPDSettings[param->eIndicator].brightness = param->eBrightness;

		if(param->toPersist)
		{
		  try{
        switch (param->eIndicator)
        {
          case dsFPD_INDICATOR_POWER:
            device::HostPersistence::getInstance().persistHostProperty("Power.brightness", numberToString(srvFPDSettings[param->eIndicator].brightness));
            break;
            /* Network and Wifi indicators has always the same brightness because they are on one PWM channel */
          case dsFPD_INDICATOR_NETWORK:
          case dsFPD_INDICATOR_WIFI:
            device::HostPersistence::getInstance().persistHostProperty("Network.brightness", numberToString(srvFPDSettings[param->eIndicator].brightness));
            device::HostPersistence::getInstance().persistHostProperty("Wifi.brightness", numberToString(srvFPDSettings[param->eIndicator].brightness));
            break;
          default:
            break;
        }

		  }
      catch(...)
      {
        ERROR("Error in Persisting the Power Brightness Value \r\n");
      }
		}
	}

    IARM_BUS_Unlock(lock);
	return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsGetFPTextBrightness(void *arg)
{
    _DEBUG_ENTER();

	#ifdef HAS_CLOCK_DISPLAY
		IARM_BUS_Lock(lock);
		
		dsFPDTextBrightParam_t *param = (dsFPDTextBrightParam_t *)arg;
		/*
			* Text Display Indicator Brightness
		*/
		param->eBrightness = _dsTextBrightness;
		INFO("_dsGetFPTextBrightness Brightness is %d \r\n",param->eBrightness);

		IARM_BUS_Unlock(lock);
	#endif
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsSetFPTextBrightness(void *arg)
{
    _DEBUG_ENTER();
	#ifdef HAS_CLOCK_DISPLAY
    IARM_BUS_Lock(lock);

	dsFPDTextBrightParam_t *param = (dsFPDTextBrightParam_t *)arg;

	
	if (param->eBrightness <= dsFPD_BRIGHTNESS_MAX)
    {
		dsSetFPTextBrightness(param->eIndicator, param->eBrightness);
		try
		{
			switch (param->eIndicator)
			{
				case dsFPD_TEXTDISP_TEXT:
				{	
					 INFO("_dsSetFPTextBrightness Brightness frm App is %d \r\n",param->eBrightness);
					_dsTextBrightness = param->eBrightness;
					device::HostPersistence::getInstance().persistHostProperty("Text.brightness",numberToString(_dsTextBrightness));
					break;
				}
				default:
				{	
					break;
				}
			}
		}
		catch(...)
		{
			ERROR("Error in Persisting the Text Brightness Value \r\n");
		}
		
	}
	
	IARM_BUS_Unlock(lock);

	#endif
	return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsGetFPColor(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);

	dsFPDColorParam_t *param = (dsFPDColorParam_t *)arg;
	if (isIndicatorValid(param->eIndicator))
	  param->eColor = srvFPDSettings[param->eIndicator].color;
	IARM_BUS_Unlock(lock);
	return IARM_RESULT_SUCCESS;

}

IARM_Result_t _dsSetFPColor(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);

	dsFPDColorParam_t *param = (dsFPDColorParam_t *)arg;
	std::string persist_token;

    if (!isIndicatorValid(param->eIndicator))
    {
      IARM_BUS_Unlock(lock);
      return IARM_RESULT_INVALID_PARAM;
    }

    dsSetFPColor(param->eIndicator, param->eColor);
    srvFPDSettings[param->eIndicator].color = param->eColor;
    INFO("_dsSetFPColor Value  From  App is %d for Indicator %d \r\n",param->eColor,param->eIndicator);
    if(param->toPersist)
    {
      try{
        switch (param->eIndicator)
        {
          case dsFPD_INDICATOR_POWER:
            persist_token = "Power.color";
            break;

          case dsFPD_INDICATOR_NETWORK:
            persist_token = "Network.color";
            break;

          case dsFPD_INDICATOR_WIFI:
            persist_token = "Wifi.color";
            break;
          default:
            break;
        }
        device::HostPersistence::getInstance().persistHostProperty(persist_token, enumToColor(param->eColor));
      }
      catch(...)
      {
        ERROR("Error in Persisting the Color  Value \r\n");
      }
    }
    IARM_BUS_Unlock(lock);
	return IARM_RESULT_SUCCESS;
}



IARM_Result_t _dsFPEnableCLockDisplay(void *arg)
{

#ifdef HAS_CLOCK_DISPLAY
	IARM_BUS_Lock(lock);
	int *enable = (int *)arg;
	int lenable = *enable;
    dsFPEnableCLockDisplay(lenable);
	IARM_BUS_Unlock(lock);
#endif
	return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsSetFPState(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);

	dsFPDStateParam_t *param = (dsFPDStateParam_t *)arg;

	if (!isIndicatorValid(param->eIndicator))
  {
	  IARM_BUS_Unlock(lock);
    return IARM_RESULT_INVALID_PARAM;
  }

	dsSetFPState(param->eIndicator, param->state);
	srvFPDSettings[param->eIndicator].state = param->state;

    IARM_BUS_Unlock(lock);
	return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsGetFPState(void *arg)
{
    _DEBUG_ENTER();
    IARM_BUS_Lock(lock);

	dsFPDStateParam_t *param = (dsFPDStateParam_t *)arg;

	if (isIndicatorValid(param->eIndicator))
	{
	  param->state = srvFPDSettings[param->eIndicator].state;
	}
	
    IARM_BUS_Unlock(lock);
	return IARM_RESULT_SUCCESS;
}


IARM_Result_t _dsGetTimeFormat(void *arg)
{
    _DEBUG_ENTER();
    
#ifdef HAS_CLOCK_DISPLAY
    IARM_BUS_Lock(lock);

   	dsFPDTimeFormatParam_t *param = (dsFPDTimeFormatParam_t *)arg;

	if( param == NULL)
	{
		INFO("Error:_dsGetTimeFormat : NULL Param ... \r\n");
		IARM_BUS_Unlock(lock);
		return IARM_RESULT_INVALID_PARAM;
	}	

   	param->eTime = _dsTextTimeFormat;
	
    IARM_BUS_Unlock(lock);
  #endif 
	return IARM_RESULT_SUCCESS;
}

IARM_Result_t _dsSetTimeFormat(void *arg)
{
    _DEBUG_ENTER();
 	
	#ifdef HAS_CLOCK_DISPLAY
	    
	    IARM_BUS_Lock(lock);

	    dsFPDTimeFormatParam_t *param = (dsFPDTimeFormatParam_t *)arg;

	    if( param == NULL)
	    {
	    	INFO("Error:_dsSetTimeFormat : NULL Param ... \r\n");
	    	IARM_BUS_Unlock(lock);
	    	return IARM_RESULT_INVALID_PARAM;
	    }	

	    if (param->eTime != _dsTextTimeFormat)
	    {
	    	_dsTextTimeFormat = param->eTime;
			
			if (param->eTime == dsFPD_TIME_12_HOUR)
			{
				device::HostPersistence::getInstance().persistHostProperty("Text.timeformat","12_HOUR");
				INFO("Clock Time Format Updated to 12_HOUR ... \r\n");
			}
			else if (param->eTime == dsFPD_TIME_24_HOUR)
			{
				device::HostPersistence::getInstance().persistHostProperty("Text.timeformat","24_HOUR");
				INFO("Clock Time Format Updated to 24_HOUR ... \r\n");
			}
	    
		   	/* Send Time Format Event */
		   	IARM_Bus_DSMgr_EventData_t _eventData;
	    	IARM_Bus_DSMgr_EventId_t _eventId;

	    	_eventData.data.FPDTimeFormat.eTimeFormat =  _dsTextTimeFormat;
	    	_eventId = IARM_BUS_DSMGR_EVENT_TIME_FORMAT_CHANGE;
	    	
	    	IARM_Bus_BroadcastEvent(IARM_BUS_DSMGR_NAME,(IARM_EventId_t)_eventId,(void *)&_eventData, sizeof(_eventData));
		    
		    INFO("Sent Clock IARM_BUS_DSMGR_EVENT_TIME_FORMAT_CHANGE event ... \r\n");
		}

	    IARM_BUS_Unlock(lock);

    #endif
	
	return IARM_RESULT_SUCCESS;
}

/** @} */
/** @} */
