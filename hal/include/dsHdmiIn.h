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
 * @file dsHdmiIn.h
 *
 * @brief Device Settings HAL HDMI Input Public API.
 * This API defines the HAL for the Device Settings HDMI Input interface.
 *
 * @par Document
 * Document reference.
 *
 * @par Open Issues (in no particular order)
 * -# None
 *
 * @par Assumptions
 * -# None
 *
 * @par Abbreviations
 * - BE:      Big-Endian.
 * - cb:      Callback function (suffix).
 * - DS:      Device Settings.
 * - FPD:     Front-Panel Display.
 * - HAL:     Hardware Abstraction Layer.
 * - LE:      Little-Endian.
 * - LS:      Least Significant.
 * - MBZ:     Must be zero.
 * - MS:      Most Significant.
 * - RDK:     Reference Design Kit.
 * - _t:      Type (suffix).
 *
 * @par Implementation Notes
 * -# None
 *
 */



/**
* @defgroup devicesettings
* @{
* @defgroup hal
* @{
**/


#ifndef _DS_dsHdmiInH_
#define _DS_dsHdmiInH_

#include <sys/types.h>
#include "dsError.h"
#include "dsTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup DSHAL_HdmiIn_API Device Settings HAL HDMI Input Public APIs
 *  @ingroup devicesettingshalapi
 *  @{
 */

/**
 * @brief Initialize the underlying HDMI Input sub-system.
 * 
 * This function must initialize the HDMI Input module and any associated data
 * structures.
 *
 * @param None
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInInit (void);

/**
 * @brief Terminate the underlying HDMI Input sub-system.
 * 
 * This function must terminate the HDMI Input module and any associated data
 * structures.
 *
 * @param None
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInTerm (void);

/**
 * @brief Get the number of HDMI Input ports on the set-top.
 * 
 * This function is used to get the number of HDMI Input ports on the set-top.
 *
 * @param [in]pNumberOfInputs   number of HDMI Input ports.
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInGetNumberOfInputs (uint8_t *pNumberOfInputs);

/**
 * @brief Get the HDMI Inpuut Status.
 * 
 * This function is used to get the current HDMI Input Status.
 *
 * @param [in]pStatus       HDMI Input enabled, HDMI Input port connected,
 *                          Active HDMI Input port, and HW Pass-Through enabled.
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInGetStatus (dsHdmiInStatus_t *pStatus);

/**
 * @brief Select the HDMI Input port to be presented.
 * 
 * This function is used to select the HDMI Input port for presentation.
 *
 * @param [in]ePort       HDMI Input port to be presented,
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInSelectPort (dsHdmiInPort_t Port);

/**
 * @brief Scale the HDMI In video
 * This function is used to scale the HDMI In video.
 *
 * @param[in] x      : x coordinate for the video
 * @param[in] y      : y coordinate for the video
 * @param[in] width  : width of the video
 * @param[in] height : height of the video
 *
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInScaleVideo (int32_t x, int32_t y, int32_t width, int32_t height);

/**
 * @brief Select the HDMI Input zoom mode
 * 
 * This function is used to select the HDMI Input zoom mode.
 *
 * @param [in]requestedZoomMode       HDMI Input zoom mode,
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInSelectZoomMode (dsVideoZoom_t requestedZoomMode);

/**
 * @brief Stop the output of HDMI Input audio.
 *
 * This function stops the HDMI Input audio from being output via HDMI Out.
 *
 * @param None
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInPauseAudio (void);

/**
 * @brief Start the output of HDMI Input audio.
 *
 * This function presents the HDMI Input audio via HDMI Out.
 *
 * @param None
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInResumeAudio (void);

/**
 * @brief Get the current HDMI Input video mode.
 *
 * This function gets the current HDMI Input video mode.
 *
 * @param None
 * @return Current HDMI Input video mode (resolution)
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInGetCurrentVideoMode (dsVideoPortResolution_t *resolution);

/**
 * @brief Callback function used to notify applications of HDMI In hot plug status
 *
 * HAL Implementation should call this method to deliver HDMI In hot plug status
 * to the application (e.g. Connect/Disconnect for Port 0/1).
 *
 * @param Port                HDMI Input port.
 * @param isPortConnected     Connection state of HDMI In Port.
 *
 *
 * @return None.
 */
typedef void (*dsHdmiInConnectCB_t)(dsHdmiInPort_t Port, bool isPortConnected);

/**
 * @brief Register for the HDMI Input hot plug event.
 * 
 * This function is used to register for the HDMI Input hot plug event.
 *
 * @param[in] CBFunc HDMI Input hot plug callback function.
 * @return Device Settings error code
 * @retval    ::dsError_t
 */
dsError_t dsHdmiInRegisterConnectCB (dsHdmiInConnectCB_t CBFunc);

/* End of DSHAL_HdmiIn_API doxygen group */
/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* _DS_dsHdmiInH_ */


/** @} */
/** @} */
