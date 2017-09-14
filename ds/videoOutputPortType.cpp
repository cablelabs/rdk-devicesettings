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


#include "videoOutputPortType.hpp"
#include "videoOutputPort.hpp"
#include "list.hpp"
#include "dsUtl.h"
#include "dsError.h"
#include "dsVideoPort.h"
#include "illegalArgumentException.hpp"
#include "videoOutputPortConfig.hpp"
#include "dslogger.h"



#include <iostream>
#include <string.h>
#include <sstream>


/**
 * @file videoOutputPortType.cpp
 * @brief VideoOutputPortType objects are instantiated by the Device Settings module upon initialization.
 * Applications do not need to create any such objects on its own.
 * References to these objects can be retrieved using a VideoOutputPort object invoking VideoOutputPort::getType().
 * A VideoOutputPortType object represent the shared properties of all output ports of same type.
 * Control over a specific instance of Video Output Port is access over a Video Output Port object
 */

using namespace std;


extern "C" dsError_t dsEnableHDCP(int handle, bool contentProtect, char *hdcpKey, size_t keySize);
extern "C" dsError_t dsSDR2HDR(dsHdmiDrmEotf  source);
extern "C" dsError_t dsSetHDRGfxColorSpace(int16_t *y, int16_t *cr, int16_t *cb);
extern "C" dsError_t dsGetHDRGfxColorSpace(int16_t *y, int16_t *cr, int16_t *cb);

namespace {
	const char *_names[] = {
			"RF",
			"Baseband",
			"SVideo",
			"1394",
			"DVI",
			"Component",
			"HDMI",
			"HDMIInput",
			"Internal",
			"SCART",
	};

	inline const bool isValid(int id) {
		return dsVideoPortType_isValid(id);
	}

}

namespace device {

typedef int _SafetyCheck[(dsUTL_DIM(_names) == dsVIDEOPORT_TYPE_MAX) ? 1 : -1];

const int VideoOutputPortType::kRF 			= dsVIDEOPORT_TYPE_RF;
const int VideoOutputPortType::kBaseband 	= dsVIDEOPORT_TYPE_BB;
const int VideoOutputPortType::kSVideo 		= dsVIDEOPORT_TYPE_SVIDEO;
const int VideoOutputPortType::k1394 		= dsVIDEOPORT_TYPE_1394;
const int VideoOutputPortType::kDVI 		= dsVIDEOPORT_TYPE_DVI;
const int VideoOutputPortType::kComponent 	= dsVIDEOPORT_TYPE_COMPONENT;
const int VideoOutputPortType::kHDMI 		= dsVIDEOPORT_TYPE_HDMI;
const int VideoOutputPortType::kInternal 	= dsVIDEOPORT_TYPE_INTERNAL;
const int VideoOutputPortType::kScart 	= dsVIDEOPORT_TYPE_SCART;
const int VideoOutputPortType::kMax 		= dsVIDEOPORT_TYPE_MAX;


/**
 * @fn  VideoOutputPortType::getInstance(int id)
 * @brief This function is used to get the instance of the video output port type based on the port id. If the 'id'
 * passed is invalid then it throws an IllegalArgumentException.
 *
 * @param[in] id port id
 *
 * @return Reference to the instance of the port type id
 */
VideoOutputPortType & VideoOutputPortType::getInstance(int id)
{
	if (::isValid(id)) {
		return VideoOutputPortConfig::getInstance().getPortType(id);
	}
	else {
		throw IllegalArgumentException();
	}
}



/**
 * @fn  VideoOutputPortType::getInstance(const std::string &name)
 * @brief This function is used to get the instance of the video output port type based on the port name. If the "name"
 * passed is invalid then it throws an IllegalArgumentException.
 *
 * @param[in] name port name
 *
 * @return Reference to the instance of the port type name
 */
VideoOutputPortType & VideoOutputPortType::getInstance(const std::string &name)
{
	for (size_t i = 0; i < dsUTL_DIM(_names); i++) {
		if (name.compare(_names[i]) == 0) {
			return VideoOutputPortConfig::getInstance().getPortType(i);
		}
	}
	throw IllegalArgumentException();
}


/*
 * @fn VideoOutputPortType::VideoOutputPortType(const int id)
 * @brief This function is a parameterised constructor. It initialises the VideoOutputPortType instance
 * with the id and name corresponding to the id passed. If the 'id' passed is invalid then it throws an
 * IllegalArgumentException.
 *
 * @param[in] id Videooutput port type id.
 * @return Reference to the instance of the videooutputport type id.
 */
VideoOutputPortType::VideoOutputPortType(const int id)
 {
	if (::isValid(id)) {
		_id = id;
		_name = std::string(_names[id]);
	}
	else {
		throw IllegalArgumentException();
	}

}


/**
 * @fn VideoOutputPortType::~VideoOutputPortType()
 * @brief This is a default destructor of the class VideoOutputPortType.
 *
 * @return None.
 */
VideoOutputPortType::~VideoOutputPortType()
{
}


/**
 * @fn const List<VideoOutputPort> VideoOutputPortType::getPorts() const
 * @brief This function is used to get the list of videooutputporttype.
 *
 * @return List of videooutputport type.
 */
const List<VideoOutputPort> VideoOutputPortType::getPorts() const
{
	return _vPorts;
}


/**
 * @fn void VideoOutputPortType::enabledDTCP()
 * @brief This function is used to enable the DTCP for videooutputport type.
 *
 * @return None.
 */
void VideoOutputPortType::enabledDTCP()
{
	_dtcpSupported = true;
}


/**
 * @fn void VideoOutputPortType::enabledHDCP(bool contentProtect , char *hdcpKey , size_t keySize )
 * @brief This function is used to enable the HDCP content protection.
 *
 * @param[in] contentProtect True If Content protection needs to be enabled, false otherwise.
 * @param[in] hdcpKey HDCP key.
 * @param[in] keySize HDCP key size.
 *
 * @return None.
 */
void VideoOutputPortType::enabledHDCP(bool contentProtect , char *hdcpKey , size_t keySize )
{
  	dsError_t ret = dsERR_NONE;
	int handle=0;

	ret = dsGetVideoPort(dsVIDEOPORT_TYPE_HDMI, 0, &handle);
	if (ret != dsERR_NONE || handle == 0)
	{
		throw IllegalArgumentException();
	}

	ret = dsEnableHDCP(handle, contentProtect, hdcpKey, keySize);
  	if (ret != dsERR_NONE)
  	{
  		throw IllegalArgumentException();
  	}
 	_hdcpSupported = true;
}

void VideoOutputPortType::SDR2HDR(dsHdmiDrmEotf  source )
{
  	dsError_t ret = dsERR_NONE;
  	ret = dsSDR2HDR(source);
  	if (ret != dsERR_NONE)
  	{
  		throw IllegalArgumentException();
  	}
}

void VideoOutputPortType::SetHDRGfxColorSpace(int16_t *y, int16_t *cr, int16_t *cb)
{
  	dsError_t ret = dsERR_NONE;
  	ret = dsSetHDRGfxColorSpace(y,cr,cb);
  	if (ret != dsERR_NONE)
  	{
  		throw IllegalArgumentException();
  	}
}

void VideoOutputPortType::GetHDRGfxColorSpace(int16_t *y, int16_t *cr, int16_t *cb)
{
  	dsError_t ret = dsERR_NONE;

  	ret = dsGetHDRGfxColorSpace(y,cr,cb);
  	if (ret != dsERR_NONE)
  	{
  		throw IllegalArgumentException();
  	}
}


/**
 * @fn void VideoOutputPortType::setRestrictedResolution(int resolution)
 * @brief This function is used to set the restricted resolution of the videooutput port type.
 *
 * @param[in] resolution Restricted resolution.
 * @return None.
 */
void VideoOutputPortType::setRestrictedResolution(int resolution)
{
	_restrictedResolution = resolution;
}


/**
 * @fn void VideoOutputPortType::addPort(const VideoOutputPort & port)
 * @brief This function is used to add video output port type in the videooutputport list.
 *
 * @param[in] port Videooutputport type.
 * @return None.
 */
void VideoOutputPortType::addPort(const VideoOutputPort & port)
{
	_vPorts.push_back(port);
}


/**
 * @fn VideoOutputPortType::isDTCPSupported() const
 * @brief This API is used to query if DTCP is supported by the port type.
 *
 * @return  True or False
 * @retval 1 if DTCP is supported
 * @retval 0 if DTCP is not supported
 */
bool VideoOutputPortType::isDTCPSupported() const
{
	return _dtcpSupported;
}


/**
 * @fn bool VideoOutputPortType::isHDCPSupported() const
 * @brief This API is used to query if HDCP is supported by the port type.
 *
 * @return  True or False
 * @retval 1 if HDCP is supported
 * @retval 0 if HDCP is not supported
 */
bool VideoOutputPortType::isHDCPSupported() const
{
	return _hdcpSupported;
}


/**
 * @fn bool VideoOutputPortType::isDynamicResolutionsSupported() const
 * @brief This function is used to query if dynamic resolutions are supported by the port type.
 *
 * @return  True or False
 * @retval 1 If dynamic resolutions is supported.
 * @retval 0 if dynamic resolutions is not supported.
 */
bool VideoOutputPortType::isDynamicResolutionsSupported() const
{
	return true;
}


/**
 * @fn int VideoOutputPortType::getRestrictedResolution() const
 * @brief This function is used to get the resolution type which has been restricted from usage.
 *
 * @return _restrictedResolution Any Restricted resolution type.
 */
int VideoOutputPortType::getRestrictedResolution() const
{
	return _restrictedResolution;
}


/**
 * @fn  const List<VideoResolution >  VideoOutputPortType::getSupportedResolutions() const
 * @brief This API is used to get a list of supported Video Resolutions by the port type.
 *
 * @return A list of video resolutions supported
 */

const List<VideoResolution >  VideoOutputPortType::getSupportedResolutions() const
{
	return VideoOutputPortConfig::getInstance().getSupportedResolutions();
}

}


/** @} */
/** @} */
