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
* @defgroup ds
* @{
**/


#include "videoOutputPort.hpp"
#include "videoResolution.hpp"
#include "audioOutputPortType.hpp"
#include "videoOutputPortType.hpp"
#include "videoOutputPortConfig.hpp"
#include "audioOutputPortConfig.hpp"
#include "host.hpp"
#include "dslogger.h"
#include "dsVideoPort.h"
#include "dsDisplay.h"

#include "illegalArgumentException.hpp"
#include "unsupportedOperationException.hpp"

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sstream>
using namespace std;


/**
 * @file videoOutputPort.cpp
 * @brief VideoOutputPort objects are instantiated by the Device Settings module upon initialization.
 * Applications do not need to create any such objects on its own.
 * References to the preallocated objects can be retrieved by applications via Host::getVideoOutputPort(const std::string &name).
 * Each VideoOutputPort is associated with an instance of VideoOutputPortType.
 */


namespace device {

const char * VideoOutputPort::kPropertyResolution = ".resolution";


/**
 * @addtogroup dssettingsvidoutportapi
 * @{
 */
/**
 * @fn  VideoOutputPort::getInstance(int id)
 * @brief This API is used to get the instance of the video output port based on the port id returned by the getsupported videooutput port.
 *
 * @param[in] id port id
 *
 * @return Reference to the instance of the port id
 */
VideoOutputPort & VideoOutputPort::getInstance(int id)
{
	return VideoOutputPortConfig::getInstance().getPort(id);
}


/**
 * @fn  VideoOutputPort::getInstance(const std::string &name)
 * @brief This API is used to get the instance of the video output port based on the port name returned by the getsupported videooutput port.
 *
 * @param[in] name Name of the port
 *
 * @return Reference to the instance of the name of the port
 */
VideoOutputPort & VideoOutputPort::getInstance(const std::string &name)
{
	return VideoOutputPortConfig::getInstance().getPort(name);

}


/**
 * @fn VideoOutputPort::VideoOutputPort(const int type, const int index, const int id, int audioPortId, const std::string &resolution)
 * @brief This function is a parameterised constructor for videooutputport. It initialises the data members of
 * VideoOutputPort instance with the parameters passed. It also updates the details about the video port
 * to indicate if the video port is enabled or not and also if its connected to the display or not. An
 * IllegalArgumentException is thrown if any error occurs while accessing the information about the video
 * port.
 *
 * @param[in] type Type of video output port (HDMI).
 * @param[in] index Index of video output port (0,1,...)
 * @param[in] id Videooutput port id.
 * @param[in] audioPortId AudioOutput Porttype id.
 * @param[in] resolution Supported videooutput port resolution.
 *
 * @return None
 */
VideoOutputPort::VideoOutputPort(const int type, const int index, const int id, int audioPortId, const std::string &resolution) :
										_type(type), _index(index), _id(id),
										_handle(-1), _enabled(true), _contentProtected(false),
										_displayConnected(false), _aPortId(audioPortId),
										_defaultResolution(resolution),
										_resolution(resolution),
										_display(*this)
{
	dsError_t ret = dsGetVideoPort((dsVideoPortType_t)_type, _index, &_handle);

	{
		std::stringstream out;
		out << getType().getName() << _index;
		_name = std::string(out.str());
	}

	if (ret == dsERR_NONE) {
		bool enabled = false;
		ret = dsIsVideoPortEnabled(_handle, &enabled);
		if (ret == dsERR_NONE) {
			_enabled = enabled;
			_contentProtected = false;

			bool connected = false;
			ret = dsIsDisplayConnected(_handle, &connected);
			if (ret == dsERR_NONE) {
				_displayConnected = connected;
			}
			else {
				throw IllegalArgumentException();
			}
		}
		else {
		}
	}
	else {
		throw IllegalArgumentException();
	}
}

bool VideoOutputPort::setScartParameter(const std::string parameter, const std::string value)
{
	return dsSetScartParameter(_handle, parameter.c_str(), value.c_str()) == dsERR_NONE;
}


/**
 * @fn VideoOutputPort::~VideoOutputPort()
 * @brief This is a default destructor of class VideoOutputPort.
 *
 * @return None.
 */
VideoOutputPort::~VideoOutputPort()
{
}


/**
 * @fn const VideoOutputPortType & VideoOutputPort::getType() const
 * @brief This API is used to get the type of the video output port.
 * A type of the video output port represent the general capabilities of the port.
 *
 * @return An instance to the type of the video output port
 */
const VideoOutputPortType & VideoOutputPort::getType() const
{
	return VideoOutputPortConfig::getInstance().getPortType(_type);
}


/**
 * @fn AudioOutputPort &VideoOutputPort::getAudioOutputPort()
 * @brief This API is used to get the audio output port connected to the video output port.
 * This connection is established during library initialization and cannot be modified.
 * An Video port is connected to at most one Audio port. An Audio port however can be connected to multiple Video ports.
 * UnsupportedOperationException will be thrown if the this Video Output Port is not connected to any Audio Output Port (i.e. on a Video-only device)
 *
 * @return Reference to the audio output port connected
 */
AudioOutputPort &VideoOutputPort::getAudioOutputPort()
{
	return AudioOutputPortConfig::getInstance().getPort(_aPortId);
}


/**
 * @fn  const VideoResolution & VideoOutputPort::getResolution()
 * @brief This API is used to get the current video resolution output from the video output port.
 * Upon library initialization, the resolution from the persistence storage will be loaded as the initial Resolution for the video output port.
 *
 * @return Reference to the output resolution
 */
const VideoResolution & VideoOutputPort::getResolution() 
{
	dsVideoPortResolution_t resolution;
	memset(resolution.name,'\0',sizeof(resolution.name));
	
	dsGetResolution(_handle,&resolution);

	/*Copy onto Temp - Safe*/
	std::string temp( resolution.name,strlen(resolution.name));
	_resolution = string(temp);
	
	return VideoResolution::getInstance(_resolution);
}

/**
 * @fn  const VideoResolution & VideoOutputPort::getPlatformResolution()
 * @brief This API is used to get the plarform video resolution output from the video output port.
 *
 * @return Reference to the output resolution
 */
const VideoResolution & VideoOutputPort::getPlatformResolution()
{
        dsVideoPortResolution_t resolution;
        memset(resolution.name,'\0',sizeof(resolution.name));

        dsGetPlatformResolution(_handle,&resolution);

        /*Copy onto Temp - Safe*/
        std::string temp( resolution.name,strlen(resolution.name));
        _resolution = string(temp);

        return VideoResolution::getInstance(_resolution);
}

/**
 * @fn  const VideoOutputPort::getDefaultResolution() const
 * @brief This API is used to get the default resolution supported by the video output port.
 *
 * @return Reference to the default output resolution
 */
const VideoResolution & VideoOutputPort::getDefaultResolution() const
{
	return VideoResolution::getInstance(_defaultResolution);
}


/**
 * @fn const VideoOutputPort::Display & VideoOutputPort::getDisplay()
 * @brief This API is used to get the display device information currently connected to the output port.
 * Application can use isDisplayConnected() to query the display connection status before trying to get an instance of the display.
 *
 * @return Returns a reference to the connected display device. If the vedio port is not connected to the display then
 * it throws an IllegalStateException.
 */
const VideoOutputPort::Display &VideoOutputPort::getDisplay()
{
	if (isDisplayConnected()) {
		dsVideoAspectRatio_t aspect;
		dsDisplayEDID_t edid;
		dsError_t dsError = dsERR_NONE;
		if (_display._handle != 0) {
			dsGetDisplayAspectRatio(_display._handle, &aspect);
			_display._aspectRatio = aspect;
			dsError = dsGetEDID(_display._handle, &edid);
			if(dsError == dsERR_NONE)
			{
			 _display._productCode = edid.productCode;
			 _display._serialNumber = edid.serialNumber;
			 _display._manufacturerYear = edid.manufactureYear;
			 _display._manufacturerWeek = edid.manufactureWeek;
			 _display._hdmiDeviceType = edid.hdmiDeviceType;
			 /* The EDID  physical Address */
			_display._physicalAddressA = edid.physicalAddressA;
			_display._physicalAddressB = edid.physicalAddressB;
			_display._physicalAddressC = edid.physicalAddressC;
			_display._physicalAddressD = edid.physicalAddressD;


			}
			else
			{
				printf("VideoOutputPort::Display::dsGetEDID has dsError: %d\r\n", dsError); 
			}
			return _display;
		}
		else {
			throw Exception(dsERR_INVALID_PARAM);
		}
	}
	else {
		throw Exception(dsERR_INVALID_STATE);
	}
}


/**
 * @fn bool VideoOutputPort::isDisplayConnected() const
 * @brief This API is used to Check if the port is currently connected to any display device.
 *
 * If ret is not equal to dsERR_NONE, it will throw the ret to exception handler and it will pass message as "No message for this Exception".
 *
 * @return True or False
 * @retval 1 if display is connected
 * @retval 0 if display is not connected
 */
bool VideoOutputPort::isDisplayConnected() const
{
    bool connected = false;
    dsError_t ret = dsIsDisplayConnected(_handle, &connected);
	if (ret != dsERR_NONE) {
		throw Exception(ret);
	}
    return connected;
}


/**
 * @fn VideoOutputPort::isEnabled() const
 * @brief This API is used to check whether this Video output port is enabled or not.
 *
 * @return None
 */
bool VideoOutputPort::isEnabled() const
{
	bool enabled = false;
	dsError_t ret = dsIsVideoPortEnabled(_handle, &enabled);
	if (ret != dsERR_NONE) {
		throw Exception(ret);
	}
	return enabled;
}


/**
 * @fn VideoOutputPort::isActive() const
 * @brief This API is used to check if Port is connected to active port of Sink device.
 *
 * @return None
 */
bool VideoOutputPort::isActive() const
{
	bool active = false;
	dsError_t ret = dsIsVideoPortActive(_handle, &active);
	if (ret != dsERR_NONE) {
        if (ret == dsERR_OPERATION_NOT_SUPPORTED) {
            throw UnsupportedOperationException("RxSense is Not Supported");
        }
		throw Exception(ret);
	}
	return active;
}



/**
 * @fn VideoOutputPort::isDynamicResolutionSupported() const
 * @brief This API is used to check whether the video output port supports the dynamic super resolution or not.
 *
 * @return True or False
 * @retval True If video output port supports Dynamic super resolution
 * @retval False If video output port doesnot support Dynamic super resolution
 */
bool VideoOutputPort::isDynamicResolutionSupported() const
{
	return VideoOutputPortType::getInstance(_type).isDynamicResolutionsSupported();
}


/**
 * @fn VideoOutputPort::setResolution(const std::string &resolutionName)
 * @brief This API is used to set the output resolution of the port by ID or its Name.
 * The specified resolution must be one of the resolutions supported by the port.
 * The list of supported resolutions can be retrieved via VideoOutputPortType:: getSupportedResolutions(); IllegalArgumentException will be thrown
 * if the specified name or ID of the resolution is not recognized.
 *
 * @param[in] resolutionName Name of the Resoultion
 *
 * @return None
 */
void VideoOutputPort::setResolution(const std::string &resolutionName)
{

	if (0 && resolutionName.compare(_resolution) == 0) {
		return;
	}

	VideoResolution newResolution = VideoResolution::getInstance(resolutionName);

	dsVideoPortResolution_t resolution;
	resolution.aspectRatio 		= (dsVideoAspectRatio_t)		newResolution.getAspectRatio().getId();
	resolution.frameRate 		= (dsVideoFrameRate_t) 	 	newResolution.getFrameRate().getId();
	resolution.interlaced 		= (bool)					newResolution.isInterlaced();
	resolution.pixelResolution 	= (dsVideoResolution_t)	newResolution.getPixelResolution().getId();
	resolution.stereoScopicMode = (dsVideoStereoScopicMode_t)newResolution.getStereoscopicMode().getId();
	strcpy(resolution.name,resolutionName.c_str());

	dsError_t ret = dsSetResolution(_handle, &resolution);

	if (ret != dsERR_NONE) {
		throw Exception(ret);
	}
	
	_resolution = newResolution.getName();
}


/**
 * @fn VideoOutputPort::setDisplayConnected(const bool connected)
 * @brief This API is used to set the video output port display to be connected.
 *
 * @param connected True if display is connected  or False if display is not connencted
 *
 * @return None
 */
void VideoOutputPort::setDisplayConnected(const bool connected)
{
	_displayConnected = connected;
	throw UnsupportedOperationException();
}


/**
 * @fn bool VideoOutputPort::isContentProtected() const
 * @brief This API is used to Check if the port or the content output on the port has DTCP or HDCP in use.
 *
 * @param  None
 *
 * @return True or False
 * @retval 1 if content from this port is protected by DTCP or HDCP.Otherwise
 * @retval 0 if content from this port is not protected by DTCP or HDCP
 */
bool VideoOutputPort::isContentProtected() const
{
    bool isProtected = false;
    dsIsHDCPEnabled (_handle, &isProtected);
    return isProtected;
}


/**
 * @fn VideoOutputPort::enable()
 * @brief This API is used to enable the video output port.
 *
 * @return None
 */
void VideoOutputPort::enable()
{
	if (dsEnableVideoPort(_handle, true) == dsERR_NONE)
	{
		_enabled = true;
	}
}


/**
 * @fn VideoOutputPort::disable()
 * @brief This API is used to disable the video output port.
 *
 * @return None
 */
void VideoOutputPort::disable()
{
	if (dsEnableVideoPort(_handle, false) == dsERR_NONE)
	{
		_enabled = false;
	}
}


/**
 * @fn VideoOutputPort::Display::Display(VideoOutputPort &vPort)
 * @brief This function is used to get the display handle of a given video device.
 *
 * @param[in] vPort Video port handle associated with connected display.
 * @return None.
 */
VideoOutputPort::Display::Display(VideoOutputPort &vPort)
{
	dsError_t ret = dsERR_NONE;
	ret = dsGetDisplay((dsVideoPortType_t)vPort.getType().getId(), vPort.getIndex(), &_handle);
	if (ret != dsERR_NONE) {
		throw Exception(ret);
	}
}

/**
 * @fn bool VideoOutputPort::Display::hasSurround(void) const
 * @brief This function returns true if connected display supports surround audio 
 *
 * @param None.
 * @return true if the connected sink has surround capability.
 */
bool VideoOutputPort::Display::hasSurround(void) const
{
    printf("VideoOutputPort::Display::hasSurround\r\n");

    dsError_t ret = dsERR_NONE;
    bool surround = false;
    int vopHandle = 0;
    ret = dsGetVideoPort(dsVIDEOPORT_TYPE_HDMI, 0, &vopHandle);
    if (ret == dsERR_NONE) {
	ret = dsIsDisplaySurround(vopHandle, &surround);
	printf("VideoOutputPort::Display::hasSurround ret %d\r\n", ret);
    }

    if (ret != dsERR_NONE) {
                surround = false;
		printf("VideoOutputPort::Display::hasSurround return default value false");
    }

    return surround;
}
  
/**
 * @fn bool VideoOutputPort::Display::getSurroundMode(void) const
 * @brief This function returns the supports surround audio Mode
 *
 * @param None.
 * @return enum for the surround capability.
 */
int VideoOutputPort::Display::getSurroundMode(void) const
{
    printf("VideoOutputPort::Display::getSurroundMode\r\n");

    dsError_t ret = dsERR_NONE;
    int surroundMode = dsSURROUNDMODE_NONE;
    int vopHandle = 0;
    ret = dsGetVideoPort(dsVIDEOPORT_TYPE_HDMI, 0, &vopHandle);
    if (ret == dsERR_NONE) {
	ret = dsGetSurroundMode(vopHandle, &surroundMode);
	printf("VideoOutputPort::Display::getSurroundMode ret %d\r\n", ret);
    }

    if (ret != dsERR_NONE) {
                surroundMode = dsSURROUNDMODE_NONE;
		printf("VideoOutputPort::Display::getSurroundMode return default value dsSURROUNDMODE_NONE");
    }

    return surroundMode;
}


/**
 * @fn void VideoOutputPort::Display::getEDIDBytes(std::vector<uint8_t> &edid) const
 * @brief This function is used to get the EDID information of the connected video display.
 * After it gets the EDID information , it clears the old edid information and inserts the
 * If ret is not equal to dsERR_NONE, it will throw the ret to exception handler and it will pass message as "No message for this Exception".
 *
 * @param edid The EDID raw buffer of the display. The HAL implementation should
 *                      malloc() the buffer and return it to the application. The
 *                      application is required to free() the buffer after using it;
 *                      If HDMI is not connected  no data should be returned,
 *                      and the API returns dsERR_INVALID_STATE.
 * @return None.
 */
void VideoOutputPort::Display::getEDIDBytes(std::vector<uint8_t> &edid) const
{
    printf("VideoOutputPort::Display::getEDIDBytes \r\n");

    dsError_t ret = dsERR_NONE;
    unsigned char *edidBytes = NULL;
    int length = 0;
    ret = dsGetEDIDBytes(_handle, &edidBytes, &length);
    printf("VideoOutputPort::Display::getEDIDBytes has ret %d\r\n", ret);
    if (ret == dsERR_NONE) {
        if (length <= 1024) {
            printf("VideoOutputPort::Display::getEDIDBytes has %d bytes\r\n", length);
            edid.clear();
            edid.insert(edid.begin(), edidBytes, edidBytes + length); 
            free(edidBytes);
        }
        else {
            ret = dsERR_OPERATION_NOT_SUPPORTED;
        }
    }

    if (ret != dsERR_NONE) {
		throw Exception(ret);
    }
}


/**
 * @fn VideoOutputPort::Display::~Display()
 * @brief This function is a default destructor of class Display.
 *
 * @return None.
 */
VideoOutputPort::Display::~Display()
{
}


int VideoOutputPort::getHDCPStatus() 
{
    dsHdcpStatus_t hdcpStatus;
    int _hdcpStatus = 0;

    dsError_t ret = dsGetHDCPStatus(_handle, &hdcpStatus);
    
    if (ret != dsERR_NONE) {
        throw Exception(ret);
    }

    _hdcpStatus = hdcpStatus;
    return _hdcpStatus;
}

int VideoOutputPort::getHDCPProtocol()
{
    dsHdcpProtocolVersion_t hdcpProtocol;
    dsError_t ret = dsGetHDCPProtocol(_handle, &hdcpProtocol);

    if (ret != dsERR_NONE) {
        throw Exception(ret);
    }

    return hdcpProtocol;
}

int VideoOutputPort::getHDCPReceiverProtocol()
{
    dsHdcpProtocolVersion_t hdcpProtocol;

    dsError_t ret = dsGetHDCPReceiverProtocol(_handle, &hdcpProtocol);

    if (ret != dsERR_NONE) {
        throw Exception(ret);
    }

    return hdcpProtocol;
}

int VideoOutputPort::getHDCPCurrentProtocol()
{
    dsHdcpProtocolVersion_t hdcpProtocol;

    dsError_t ret = dsGetHDCPCurrentProtocol(_handle, &hdcpProtocol);

    if (ret != dsERR_NONE) {
        throw Exception(ret);
    }

    return hdcpProtocol;
}

/**
 * @fn  const void VideoOutputPort::getTVHDRCapabilities(int *capabilities) const
 * @brief This API is used to get HDR format supported by TV.
 *
 * @return HDR capabilities supported by TV
 */
void VideoOutputPort::getTVHDRCapabilities(int *capabilities) const
{
    dsGetTVHDRCapabilities(_handle, capabilities);
}

/**
 * @fn  const void VideoOutputPort::getSupportedTvResolutions(int *resolutions) const
 * @brief This API is used to get TV supported Video Resolutions.
 *
 * @return Resolutions supported by TV
 */
void VideoOutputPort::getSupportedTvResolutions(int *resolutions) const
{
    dsSupportedTvResolutions(_handle,resolutions);
}

int VideoOutputPort::forceDisable4KSupport(bool disable)
{
	dsSetForceDisable4KSupport(_handle, disable);
	return 0;
}
}

/** @} */ //End of Doxygen tag


/** @} */
/** @} */
