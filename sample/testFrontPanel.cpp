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
 
// TODO: Include your class to test here.


/**
* @defgroup devicesettings
* @{
* @defgroup sample
* @{
**/


#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include "manager.hpp"

#include "frontPanelConfig.hpp"
#include <stdlib.h>  


#include "libIBus.h"




int main(int argc, char *argv[]) 
{
   
    int i = 1;
	
	IARM_Bus_Init("SampleDSClient");
	IARM_Bus_Connect();

    device::Manager::Initialize();
    
    if (argc != 2) {
        printf("%s : <Text Message [3 Chars]>\n", argv[0]);
        return 0;
    }
    char *Message = argv[1];
  device::FrontPanelIndicator::Color color_red(device::FrontPanelIndicator::Color::kRed);
  device::FrontPanelIndicator::Color color_blue(device::FrontPanelIndicator::Color::kBlue);
  device::FrontPanelIndicator::Color color_green(device::FrontPanelIndicator::Color::kGreen);
  device::FrontPanelIndicator::Color color_orange(device::FrontPanelIndicator::Color::kOrange);

	int bright = device::FrontPanelConfig::getInstance().getIndicator("Power").getBrightness();
	device::FrontPanelConfig::getInstance().getIndicator("Power").setColor(color_blue);
	printf("Power : brightness is %d\n",bright);
	bright = device::FrontPanelConfig::getInstance().getIndicator("Network").getBrightness();
	device::FrontPanelConfig::getInstance().getIndicator("Network").setColor(color_orange);
	printf("Network : brightness is %d\n",bright);
	bright = device::FrontPanelConfig::getInstance().getIndicator("Wifi").getBrightness();
	device::FrontPanelConfig::getInstance().getIndicator("Wifi").setColor(color_red);
	printf("Wifi : brightness is %d\n",bright);
//	bright = device::FrontPanelConfig::getInstance().getTextDisplay("Text").getTextBrightness();
//	printf("Text : brightness is %d\n",bright);

	printf("Wait 5s...\n");
	sleep(5);
	
	printf("Set Power indicator brightness to %d\n", i);
	device::FrontPanelConfig::getInstance().getIndicator("Power").setBrightness(i);
	device::FrontPanelConfig::getInstance().getIndicator("Power").setState(true);
	printf("Set Network indicator brightness to %d\n", i);
	device::FrontPanelConfig::getInstance().getIndicator("Network").setBrightness(i);
	device::FrontPanelConfig::getInstance().getIndicator("Network").setState(true);
	printf("Set Wifi indicator brightness to %d\n", i);
	device::FrontPanelConfig::getInstance().getIndicator("Wifi").setBrightness(i);
	device::FrontPanelConfig::getInstance().getIndicator("Wifi").setState(true);
//	device::FrontPanelConfig::getInstance().getTextDisplay("Text").setTextBrightness(i);

	printf("Wait 5s...\n");
	sleep(5);

	bright = device::FrontPanelConfig::getInstance().getIndicator("Power").getBrightness();
	printf("Power : brightness is %d\n",bright);
	bright = device::FrontPanelConfig::getInstance().getIndicator("Network").getBrightness();
	printf("Network : brightness is %d\n",bright);
	bright = device::FrontPanelConfig::getInstance().getIndicator("Wifi").getBrightness();
	printf("Wifi : brightness is %d\n",bright);
//	bright = device::FrontPanelConfig::getInstance().getTextDisplay("Text").getTextBrightness();
//	printf("Text : brightness is %d\n",bright);
    
    try {
//		   printf("Sample Application: set text display------- %s\n",Message);
//           device::FrontPanelConfig::getInstance().getTextDisplay("Text").setText(Message);
		 	 
           for (i=10;i < 100 ; )
           {
				printf("Sample Application: set brightness------- %d\n",i);
//				device::FrontPanelConfig::getInstance().getTextDisplay("Text").setTextBrightness(i);
				device::FrontPanelConfig::getInstance().getIndicator("Power").setBrightness(i);
				device::FrontPanelConfig::getInstance().getIndicator("Network").setBrightness(i);
				device::FrontPanelConfig::getInstance().getIndicator("Wifi").setBrightness(i);
				i = i+ 10;
				printf("Wait 2s...\n");
				sleep(2);
		   }
       
			bright = device::FrontPanelConfig::getInstance().getIndicator("Power").getBrightness();
			printf("Power : brightness is %d\n",bright);
			bright = device::FrontPanelConfig::getInstance().getIndicator("Network").getBrightness();
			printf("Network : brightness is %d\n",bright);
			bright = device::FrontPanelConfig::getInstance().getIndicator("Wifi").getBrightness();
			printf("Wifi : brightness is %d\n",bright);
//			bright = device::FrontPanelConfig::getInstance().getTextDisplay("Text").getTextBrightness();
//			printf("Text : brightness is %d\n",bright);
		}
    catch (...) {
    	printf("Exception Caught during [%s]\r\n", argv[0]);
    }


    printf("Start blink on all indicators for 20 iterations and intervals: Power=200ms, Network=150ms, Wifi=1000ms\n");
    device::FrontPanelConfig::getInstance().getIndicator("Power").setBlink(device::FrontPanelIndicator::Blink(200, 20));
    device::FrontPanelConfig::getInstance().getIndicator("Network").setBlink(device::FrontPanelIndicator::Blink(150, 20));
    device::FrontPanelConfig::getInstance().getIndicator("Wifi").setBlink(device::FrontPanelIndicator::Blink(1000, 20));

    device::Manager::DeInitialize();

    printf("Sample Application: Finished.\n");

	IARM_Bus_Disconnect();
	IARM_Bus_Term();
	
    return 0;
}


/** @} */
/** @} */
