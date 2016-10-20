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


#ifndef _DS_VIDEODEVICE_HPP_
#define _DS_VIDEODEVICE_HPP_

#include "dsConstant.hpp"
#include "videoDFC.hpp"
#include "list.hpp"
#include <string>


/**
 * @file videoDevice.hpp
 * @brief It contains class referenced by videoDevice.cpp file.
 */
namespace device {


/**
 * @class VideoDevice
 * @brief class extending DSConstant to implement the videoDevice interface.
 * @ingroup devicesettingsclass
 */
class VideoDevice : public DSConstant {
	int _handle;
	int _dfc; //!< Decoder format converter variable.
	List<VideoDFC> _supportedDFCs; //!< List of supported dfc's.
	void setDFC(const VideoDFC & dfc);
	void setDFC();

	static const char * kPropertyDFC;

public:
	static VideoDevice & getInstance(int id);
	static VideoDevice & getInstance(const std::string &name);

	VideoDevice(int id);
	void setDFC(const std::string & name);
	void setDFC(int id);
	void setPlatformDFC();
	const VideoDFC & getDFC();
	const List <VideoDFC>  getSupportedDFCs() const;
	void addDFC(const VideoDFC &dfc);
	virtual ~VideoDevice();
	void getHDRCapabilities(int *capabilities);
};

}

#endif /* _DS_VIDEODEVICE_HPP_ */


/** @} */
/** @} */
