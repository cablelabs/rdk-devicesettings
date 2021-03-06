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
/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright ARRIS Enterprises, Inc. 2015.
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
 * limitations under
*/


/**
 * @file hdmiIn.cpp
 * @brief Configuration of HDMI Input
 */
 


/**
* @defgroup devicesettings
* @{
* @defgroup ds
* @{
**/


#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include "hdmiIn.hpp"
#include "illegalArgumentException.hpp"
#include "host.hpp"

#include "dslogger.h"
#include "dsError.h"
#include "dsTypes.h"
#include "dsHdmiIn.h"
#include "dsUtl.h"

// Enable for debug tracing
// #define HDMI_IN_DEBUG
#ifdef HDMI_IN_DEBUG
   #define HDMI_IN_TRACE( m ) printf m
#else
   #define HDMI_IN_TRACE( m )
#endif

namespace device 
{


/**
 * @fn  HdmiInput::HdmiInput()
 * @brief default constructor
 *
 * @param None
 *
 * @return None
 * @callergraph
 */
HdmiInput::HdmiInput() 
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));
    
    HDMI_IN_TRACE(("%s - invoking dsHdmiInInit()\n", __PRETTY_FUNCTION__));
    dsHdmiInInit();
    
    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));    
}

/**
 * @fn  HdmiInput::~HdmiInput()
 * @brief destructor
 *
 * @param None
 *
 * @return None
 * @callergraph
 */
HdmiInput::~HdmiInput() 
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));

    HDMI_IN_TRACE(("%s - invoking dsHdmiInTerm()\n", __PRETTY_FUNCTION__));
    dsHdmiInTerm();

    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}

/**
 * @fn  HdmiInput::getInstance()
 * @brief This API is used to get the instance of the HDMI Input
 *
 * @param None
 *
 * @return Reference to the instance of HDMI Input class instance
 * @callergraph
 */
HdmiInput & HdmiInput::getInstance()
{
    static HdmiInput _singleton;

    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));
    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
    
	return _singleton;
}

/**
 * @fn  HdmiInput::getNumberOfInputs()
 * @brief This API is used to get the number of HDMI Input ports on the set-top
 *
 * @param[in] None
 *
 * @return number of HDMI Inputs
 * @callergraph
 */
uint8_t HdmiInput::getNumberOfInputs() const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));

    HDMI_IN_TRACE(("%s - invoking dsHdmiInGetNumberOfInputs()\n", __PRETTY_FUNCTION__));
    
    uint8_t numHdmiInputs;
    dsError_t eError = dsHdmiInGetNumberOfInputs (&numHdmiInputs);

	// Throw an exception if there was an error
	if (dsERR_NONE != eError) 
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}
	
    HDMI_IN_TRACE(("%s <--- Number of inputs=%d\n", __PRETTY_FUNCTION__, numHdmiInputs));
    
    return (numHdmiInputs);
}

/**
 * @fn  HdmiInput::isPresented()
 * @brief This API is used to specify if HDMI Input is being
 *        presented via HDMI Out
 *
 * @param[in] None
 *
 * @return true if HDMI Input is being presetned.
 * @callergraph
 */
bool HdmiInput::isPresented() const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));
    
    HDMI_IN_TRACE(("%s - invoking dsHdmiInGetStatus()\n", __PRETTY_FUNCTION__));
    dsHdmiInStatus_t Status;
    dsError_t eError = dsHdmiInGetStatus (&Status);

	// Throw an exception if there was an error
	if (dsERR_NONE != eError) 
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}
	
	return (Status.isPresented);
	
	
    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}

/**
 * @fn  HdmiInput::isActivePort()
 * @brief This API is used to specify if the provided HDMI Input port is
 *        active (i.e. communicating with the set-top)
 *
 * @param[in] HDMI Input port
 *
 * @return true if the provided HDMI Input port is active.
 * @callergraph
 */
bool HdmiInput::isActivePort(int8_t Port) const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));
    
    HDMI_IN_TRACE(("%s - invoking dsHdmiInGetStatus()\n", __PRETTY_FUNCTION__));
    dsHdmiInStatus_t Status;
    dsError_t eError = dsHdmiInGetStatus (&Status);

	// Throw an exception if there was an error
	if (dsERR_NONE != eError) 
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}
	
	return (Status.activePort == Port);
	
	
    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}

/**
 * @fn  HdmiInput::getActivePort()
 * @brief This API is used to specify the active (i.e. communicating with
 *        the set-top) HDMI Input port
 *
 * @param[in] None
 *
 * @return the HDMI Input port which is currently active.
 * @callergraph
 */
int8_t HdmiInput::getActivePort() const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));
    
    HDMI_IN_TRACE(("%s - invoking dsHdmiInGetStatus()\n", __PRETTY_FUNCTION__));
    dsHdmiInStatus_t Status;
    dsError_t eError = dsHdmiInGetStatus (&Status);

	// Throw an exception if there was an error
	if (dsERR_NONE != eError) 
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}
	
	return (Status.activePort);
	
	
    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}

/**
 * @fn  HdmiInput::isPortConnected()
 * @brief This API is used to specify if the prvided HDMI Input port is
 *        connected (i.e. HDMI Input devie is plugged into the set-top).
 *
 * @param[in] HDMI Input port
 *
 * @return true if the HDMI Input port is connected
 * @callergraph
 */
bool HdmiInput::isPortConnected(int8_t Port) const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));
    
    HDMI_IN_TRACE(("%s - invoking dsHdmiInGetStatus()\n", __PRETTY_FUNCTION__));
    dsHdmiInStatus_t Status;
    dsError_t eError =  dsHdmiInGetStatus (&Status);

	// Throw an exception if there was an error
	if (dsERR_NONE != eError) 
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}
	
	return (Status.isPortConnected[Port]);
	
	
    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}

/**
 * @fn  HdmiInput::selectPort()
 * @brief This API is used to select the HDMI In port to be presented
 *
 * @param[in] int8_t Port : -1 for No HDMI Input port to be presented
 *                           0..n for HDMI Input port (n) to be presented 
 *
 * @return None
 * @callergraph
 */
void HdmiInput::selectPort (int8_t Port) const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));

    HDMI_IN_TRACE(("%s - invoking dsHdmiInSelectPort()\n", __PRETTY_FUNCTION__));
    dsError_t eError = dsHdmiInSelectPort ((dsHdmiInPort_t)Port);

	// Throw an exception if there was an error
	if (dsERR_NONE != eError) 
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}
	
    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}

/**
 * @fn  HdmiInput::scaleVideo()
 * @brief This API is used to scale the HDMI In video
 *
 * @param[in] int32_t x      : x coordinate for the video
 * @param[in] int32_t y      : y coordinate for the video
 * @param[in] int32_t width  : width of the video
 * @param[in] int32_t height : height of the video
 *
 * @return None
 * @callergraph
 */
void HdmiInput::scaleVideo (int32_t x, int32_t y, int32_t width, int32_t height) const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));

    HDMI_IN_TRACE(("%s - invoking dsHdmiInScaleVideo(%d, %d, %d, %d)\n", __PRETTY_FUNCTION__, x, y, width, height));
    dsError_t eError = dsHdmiInScaleVideo (x, y, width, height);

	// Throw an exception if there was an error
	if (dsERR_NONE != eError) 
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}
	
    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}

/**
 * @fn  HdmiInput::selectZoomMode()
 * @brief This API is used to select the HDMI In video zoom mode
 *
 * @param[in] int8_t zoomMoode : 0 for NONE
 *                               1 for FULL
 *
 * @return None
 * @callergraph
 */
void HdmiInput::selectZoomMode (int8_t zoomMode) const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));

    HDMI_IN_TRACE(("%s - invoking dsHdmiInSelectPort()\n", __PRETTY_FUNCTION__));
    dsError_t eError = dsHdmiInSelectZoomMode ((dsVideoZoom_t)zoomMode);

	// Throw an exception if there was an error
	if (dsERR_NONE != eError)
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}

    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}

/**
 * @fn  HdmiInput::pauseAudio()
 * @brief This API is used to select the HDMI In video zoom mode
 *
 * @param[in] None
 *
 * @return None
 * @callergraph
 */
void HdmiInput::pauseAudio () const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));

    HDMI_IN_TRACE(("%s - invoking dsHdmiInPauseAudio()\n", __PRETTY_FUNCTION__));
    dsError_t eError = dsHdmiInPauseAudio ();

	// Throw an exception if there was an error
	if (dsERR_NONE != eError)
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}

    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}

/**
 * @fn  HdmiInput::resumeAudio()
 * @brief This API is used to select the HDMI In video zoom mode
 *
 * @param[in] None
 *
 * @return None
 * @callergraph
 */
void HdmiInput::resumeAudio () const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));

    HDMI_IN_TRACE(("%s - invoking dsHdmiInResumeAudio()\n", __PRETTY_FUNCTION__));
    dsError_t eError = dsHdmiInResumeAudio ();

	// Throw an exception if there was an error
	if (dsERR_NONE != eError)
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}

    HDMI_IN_TRACE(("%s <--- \n", __PRETTY_FUNCTION__));
}


static std::string getResolutionStr (dsVideoResolution_t resolution)
{
    printf("%s ---> \n", __PRETTY_FUNCTION__);

    std::string resolutionStr;

    switch (resolution)
    {
        case dsVIDEO_PIXELRES_720x480:
            resolutionStr = "480";
            break;

        case dsVIDEO_PIXELRES_720x576:
            resolutionStr = "576";
            break;

        case dsVIDEO_PIXELRES_1280x720:
            resolutionStr = "720";
            break;

        case dsVIDEO_PIXELRES_1920x1080:
            resolutionStr = "1080";
            break;


        case dsVIDEO_PIXELRES_3840x2160:
            resolutionStr = "3840x2160";
            break;

        case dsVIDEO_PIXELRES_4096x2160:
            resolutionStr = "4096x2160";
            break;

        default:
            resolutionStr = "unknown";
            break;
    }

    printf ("%s <--- %s\n", __PRETTY_FUNCTION__, resolutionStr.c_str());
    return resolutionStr;
}

static std::string getFrameRateStr (dsVideoFrameRate_t frameRate)
{
    printf("%s ---> \n", __PRETTY_FUNCTION__);

    std::string FrameRateStr;

    switch (frameRate)
    {
        case dsVIDEO_FRAMERATE_24:
            FrameRateStr = "24";
            break;

        case dsVIDEO_FRAMERATE_25:
            FrameRateStr = "25";
            break;

        case dsVIDEO_FRAMERATE_30:
            FrameRateStr = "30";
            break;

        case dsVIDEO_FRAMERATE_60:
            FrameRateStr = "60";
            break;

        case dsVIDEO_FRAMERATE_23dot98:
            FrameRateStr = "23.98";
            break;

        case dsVIDEO_FRAMERATE_29dot97:
            FrameRateStr = "29.97";
            break;

        case dsVIDEO_FRAMERATE_50:
            FrameRateStr = "50";
            break;

        case dsVIDEO_FRAMERATE_59dot94:
            FrameRateStr = "59.94";
            break;

         default:
            // Not all video formats have a specified framerate.
            break;
    }

    printf ("%s <--- %s\n", __PRETTY_FUNCTION__, FrameRateStr.c_str());
    return FrameRateStr;
}

static std::string getInterlacedStr (bool interlaced)
{
    printf("%s ---> \n", __PRETTY_FUNCTION__);

    std::string InterlacedStr = (interlaced) ? "i" : "p";

    printf ("%s <--- %s\n", __PRETTY_FUNCTION__, InterlacedStr.c_str());
    return InterlacedStr;
}

static std::string CreateResolutionStr (const dsVideoPortResolution_t &resolution)
{
    printf("%s ---> \n", __PRETTY_FUNCTION__);

    std::string resolutionStr = getResolutionStr(resolution.pixelResolution) +
                                getInterlacedStr(resolution.interlaced) +
                                getFrameRateStr(resolution.frameRate);

    printf ("%s <--- %s\n", __PRETTY_FUNCTION__, resolutionStr.c_str());
    return resolutionStr;
}

/**
 * @fn  HdmiInput::getCurrentVideoMode()
 * @brief This API is used to get the current HDMI In video mode (resolution)
 *
 * @param[in] None
 *
 * @return HDMI Input video resolution string
 * @callergraph
 */
std::string HdmiInput::getCurrentVideoMode () const
{
    HDMI_IN_TRACE(("%s ---> \n", __PRETTY_FUNCTION__));

	dsVideoPortResolution_t resolution;
	memset(&resolution, 0, sizeof(resolution));

    HDMI_IN_TRACE(("%s - invoking dsHdmiInGetCurrentVideoMode()\n", __PRETTY_FUNCTION__));
    dsError_t eError = dsHdmiInGetCurrentVideoMode (&resolution);

	// Throw an exception if there was an error
	if (dsERR_NONE != eError)
	{
        HDMI_IN_TRACE(("%s <--- exception error=%d \n", __PRETTY_FUNCTION__, eError));
		throw Exception(eError);
	}

    std::string resolutionStr = CreateResolutionStr (resolution);
    HDMI_IN_TRACE(("%s <--- resolution =%s\n", __PRETTY_FUNCTION__, resolutionStr.c_str()));
    return resolutionStr;
}


}


/** @} */
/** @} */
