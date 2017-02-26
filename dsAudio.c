/*
 * ============================================================================
 * COMCAST C O N F I D E N T I A L AND PROPRIETARY
 * ============================================================================
 * This file and its contents are the intellectual property of Comcast.  It may
 * not be used, copied, distributed or otherwise disclosed in * whole or in part
 * without the express written permission of Comcast.
 * ============================================================================
 * Copyright (c) 2011 – 2012 Comcast. All rights reserved.
 * ============================================================================
 */

#include <sys/types.h>
#include "dsAudio.h"
#include <stdint.h>
#include "dsError.h"
#include "dsUtl.h"



typedef struct  _audioOutputPortData_t
{
  float audioGain;
  float audioLevel;
  float audioDb;
  bool  loopThru;
  bool  muted;
  float maxDb;
  float minDb;
  dsAudioEncoding_t  encoding;
  dsAudioCompression_t compression;
  dsAudioStereoMode_t  stereomode;
  dsAudioTypeConfig_t audioPortConfig;
  dsAudioPortId_t portId;
}audioOutputPortData_t;


static audioOutputPortData_t *audioOutputPorts = NULL;
static unsigned int *dsAudioHandlers = NULL;
static size_t dsAudio_port_max = 0;

void print(audioOutputPortData_t* port)
{
    printf("\nName: %s\n",port->audioPortConfig.name);
    printf("Type: %d\n",port->audioPortConfig.typeId);
    printf("gain = %f\n",port->audioGain);
    printf("Level = %f\n",port->audioLevel);
    printf("maxDB = %f\n",port->maxDb);
    printf("minDb = %f\n",port->minDb);
    printf("Supported Modes = %d\n",port->audioPortConfig.numSupportedStereoModes);
    int i =0;
    for(;i< port->audioPortConfig.numSupportedStereoModes ;i++)
        printf("Modes: %d \t",port->audioPortConfig.stereoModes[i]);
    for(i = 0;i< port->audioPortConfig.numSupportedEncodings ;i++)
        printf("EnCodings: %d \t",port->audioPortConfig.encodings[i]);
    for(i = 0;i< port->audioPortConfig.numSupportedCompressions ;i++)
        printf("Compressions: %d \t",port->audioPortConfig.compressions[i]);
}


bool dsIsValidHandle(unsigned int uHandle)
{
    size_t index ;
    bool retValue = false;
    for (index = 0;index < dsAudio_port_max;index++) {
        if (dsAudioHandlers[index] == uHandle) {
            retValue = true;
            break;
        }
    }
    return retValue;
}
//###################################################################################################
dsError_t dsAudioPortInit()
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t  dsGetAudioPort(dsAudioPortType_t type, int index, int *handle)
{
	dsError_t ret = dsERR_NONE;
    	if ( index >= 0 && index < dsAudio_port_max) {
	    if ( (audioOutputPorts + index)->portId.type == type && (audioOutputPorts + index)->portId.index == index) {
       *handle = (int)(audioOutputPorts + index);
        ret =  dsERR_NONE;
        	}
    	}
	return ret;
}

dsError_t dsGetAudioEncoding(int handle, dsAudioEncoding_t *encoding)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsGetAudioCompression(int handle, dsAudioCompression_t *compression)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsGetStereoMode(int handle, dsAudioStereoMode_t *stereoMode)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t  dsIsAudioPortEnabled(int handle, bool *enabled)
{
    dsError_t ret = dsERR_NONE;
    return ret;
}

dsError_t  dsEnableAudioPort(int handle, bool enabled)
{
    dsError_t ret = dsERR_NONE;
    return ret;
}

dsError_t dsGetAudioGain(int handle, float *gain)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsGetAudioDB(int handle, float *db)
{
	dsError_t ret = dsERR_NONE;
    	if (dsIsValidHandle(handle)) 
	{
        	*db = ((audioOutputPortData_t*)handle)->audioDb;
	        ret = dsERR_NONE;
    	}
	return ret;
}

dsError_t dsGetAudioLevel(int handle, float *level)
{
    dsError_t ret = dsERR_NONE;
    return ret;
}

dsError_t dsGetAudioMaxDB(int handle, float *maxDb)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsGetAudioMinDB(int handle, float *minDb)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsGetAudioOptimalLevel(int handle, float *optimalLevel)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t  dsIsAudioLoopThru(int handle, bool *loopThru)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsIsAudioMute(int handle, bool *muted)
{
	dsError_t ret = dsERR_NONE;
    return ret;
}

dsError_t dsSetAudioEncoding(int handle, dsAudioEncoding_t encoding)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsSetAudioCompression(int handle, dsAudioCompression_t compression)
{
	dsError_t ret = dsERR_OPERATION_NOT_SUPPORTED;
	/* This is a empty operation in RNG150 */
	return ret;
}

dsError_t dsSetStereoMode(int handle, dsAudioStereoMode_t mode) {

	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsSetAudioGain(int handle, float gain)
{
	dsError_t ret = dsERR_OPERATION_NOT_SUPPORTED;
	return ret;
}

dsError_t dsSetAudioDB(int handle, float db)
{
	dsError_t ret = dsERR_OPERATION_NOT_SUPPORTED;
	return ret;
}

dsError_t dsSetAudioLevel(int handle, float level)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsEnableLoopThru(int handle, bool loopThru)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsSetAudioMute(int handle, bool mute)
{
	dsError_t ret = dsERR_NONE;
	return ret;
}

dsError_t dsAudioPortTerm()
{
	dsError_t ret = dsERR_NONE;
	return ret;
}
