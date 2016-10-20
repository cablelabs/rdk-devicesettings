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
#include "videoOutputPortConfig.hpp"
#include "audioOutputPortConfig.hpp"
#include "illegalArgumentException.hpp"
#include "dsVideoPortSettings.h"
#include "dsVideoResolutionSettings.h"
 #include "dsDisplay.h"
#include "dsUtl.h"
#include "dsError.h"
#include "illegalArgumentException.hpp"
#include "list.hpp"
#include "videoResolution.hpp"
#include "dslogger.h"

#include <iostream>
#include <string.h>

using namespace std;

namespace device {


VideoOutputPortConfig::VideoOutputPortConfig() {
	// TODO Auto-generated constructor stub
}

VideoOutputPortConfig::~VideoOutputPortConfig() {
	// TODO Auto-generated destructor stub
}

VideoOutputPortConfig & VideoOutputPortConfig::getInstance() {
    static VideoOutputPortConfig _singleton;
	return _singleton;
}

const PixelResolution &VideoOutputPortConfig::getPixelResolution(int id) const
{
	return _vPixelResolutions.at(id);
}

const AspectRatio &VideoOutputPortConfig::getAspectRatio(int id) const
{
	return _vAspectRatios.at(id);
}

const StereoScopicMode &VideoOutputPortConfig::getSSMode(int id) const
{
	return _vStereoScopieModes.at(id);
}

const VideoResolution &VideoOutputPortConfig::getVideoResolution (int id) const
{
	return _supportedResolutions.at(id);
}

const FrameRate &VideoOutputPortConfig::getFrameRate(int id) const
{
	return _vFrameRates.at(id);
}

VideoOutputPortType &VideoOutputPortConfig::getPortType(int id)
{
	return _vPortTypes.at(id);
}

VideoOutputPort &VideoOutputPortConfig::getPort(int id)
{
	return _vPorts.at(id);
}

VideoOutputPort &VideoOutputPortConfig::getPort(const std::string & name)
{
	for (size_t i = 0; i < _vPorts.size(); i++) {
		if (name.compare(_vPorts.at(i).getName()) == 0) {
			return _vPorts.at(i);
		}
	}

	throw IllegalArgumentException();
}

List<VideoOutputPort> VideoOutputPortConfig::getPorts()
{
	List <VideoOutputPort> rPorts;

	for (size_t i = 0; i < _vPorts.size(); i++) {
		rPorts.push_back(_vPorts.at(i));
	}

	return rPorts;
}

List<VideoOutputPortType>  VideoOutputPortConfig::getSupportedTypes()
{
	List<VideoOutputPortType> supportedTypes;
	for (std::vector<VideoOutputPortType>::const_iterator it = _vPortTypes.begin(); it != _vPortTypes.end(); it++) {
		if (it->isEnabled()) {
			supportedTypes.push_back(*it);
		}
	}

	return supportedTypes;
}

List<VideoResolution>  VideoOutputPortConfig::getSupportedResolutions()
{
	List<VideoResolution> supportedResolutions;
	int isDynamicList = 0;
	
	_supportedResolutions.clear(); /*Clear the Vector */

	try {

		device::VideoOutputPort vPort = VideoOutputPortConfig::getInstance().getPort("HDMI0");
		if (vPort.isDisplayConnected())
		{
			dsDisplayEDID_t edid;
			int _handle;
			
			/*Initialize the struct*/
			memset(&edid, 0, sizeof(edid));

			edid.numOfSupportedResolution = 0;
			dsGetDisplay((dsVideoPortType_t)vPort.getType().getId(), vPort.getIndex(), &_handle);
			dsGetEDID(_handle, &edid);
	
			cout <<" EDID Num of Resolution ......."<< edid.numOfSupportedResolution << endl;	
			for (size_t i = 0; i < edid.numOfSupportedResolution; i++)
			{
				dsVideoPortResolution_t *resolution = &edid.suppResolutionList[i];
				isDynamicList = 1;

				_supportedResolutions.push_back(
							VideoResolution(
							i, /* id */
							std::string(resolution->name),
							resolution->pixelResolution,
							resolution->aspectRatio,
							resolution->stereoScopicMode,
							resolution->frameRate,
							resolution->interlaced));
			}	
		}
	}catch (...) 
		{
			isDynamicList = 0;
			cout << "VideoOutputPortConfig::getSupportedResolutions Thrown. Exception..."<<endl;
		}
	
	if (0 == isDynamicList )
	{
		size_t numResolutions = dsUTL_DIM(kResolutions);
		for (size_t i = 0; i < numResolutions; i++) 
		{
			dsVideoPortResolution_t *resolution = &kResolutions[i];
			_supportedResolutions.push_back(
					VideoResolution(
					i, /* id */
					std::string(resolution->name),
					resolution->pixelResolution,
					resolution->aspectRatio,
					resolution->stereoScopicMode,
					resolution->frameRate,
					resolution->interlaced));
		}
	}
	
	for (std::vector<VideoResolution>::iterator it = _supportedResolutions.begin(); it != _supportedResolutions.end(); it++) {
		if (it->isEnabled()) {
			supportedResolutions.push_back(*it);
		}
	}
	return supportedResolutions;
}



void VideoOutputPortConfig::load()
{
	try {
		/*
		 * Load Constants First.
		 */
		for (size_t i = 0; i < dsVIDEO_PIXELRES_MAX; i++) {
			_vPixelResolutions.push_back(PixelResolution(i));
		}
		for (size_t i = 0; i < dsVIDEO_ASPECT_RATIO_MAX; i++) {
			_vAspectRatios.push_back(AspectRatio(i));
		}
		for (size_t i = 0; i < dsVIDEO_SSMODE_MAX; i++) {
			_vStereoScopieModes.push_back(StereoScopicMode(i));
		}
		for (size_t i = 0; i < dsVIDEO_FRAMERATE_MAX; i++) {
			_vFrameRates.push_back(FrameRate((int)i));
		}

		for (size_t i = 0; i < dsVIDEOPORT_TYPE_MAX; i++) {
			_vPortTypes.push_back(VideoOutputPortType((int)i));
		}

		/* Initialize a set of supported resolutions
		 *
		 */
		size_t numResolutions = dsUTL_DIM(kResolutions);
		for (size_t i = 0; i < numResolutions; i++) {
			dsVideoPortResolution_t *resolution = &kResolutions[i];
			_supportedResolutions.push_back(
									VideoResolution(
										i, /* id */
										std::string(resolution->name),
										resolution->pixelResolution,
										resolution->aspectRatio,
										resolution->stereoScopicMode,
										resolution->frameRate,
										resolution->interlaced));
		}


	/*
	 * Initialize Video portTypes (Only Enable POrts)
	 * and its port instances (curr resolution)
	 */
		for (size_t i = 0; i < dsUTL_DIM(kConfigs); i++)
		{
			const dsVideoPortTypeConfig_t *typeCfg = &kConfigs[i];
			VideoOutputPortType &vPortType = VideoOutputPortType::getInstance(typeCfg->typeId);
			vPortType.enable();
			vPortType.setRestrictedResolution(typeCfg->restrictedResollution);
		}

		/*
		 * set up ports based on kPorts[]
		 */
		for (size_t i = 0; i < dsUTL_DIM(kPorts); i++) {
			const dsVideoPortPortConfig_t *port = &kPorts[i];

			_vPorts.push_back(
					VideoOutputPort((port->id.type), port->id.index, i,
							AudioOutputPortType::getInstance(kPorts[i].connectedAOP.type).getPort(kPorts[i].connectedAOP.index).getId(),
							std::string(port->defaultResolution)));

			_vPortTypes.at(port->id.type).addPort(_vPorts.at(i));

		}

	}
	catch (...) {
		cout << "VIdeo Outport Exception Thrown. ..."<<endl;
	}

}

void VideoOutputPortConfig::release()
  {
	try {
              _vPixelResolutions.clear();
              _vAspectRatios.clear();
              _vStereoScopieModes.clear();
              _vFrameRates.clear();
              _vPortTypes.clear();                            
              _supportedResolutions.clear();
              _vPorts.clear();
	}
	catch (const Exception &e) {
		throw e;
	}
  }
}


/** @} */
/** @} */
