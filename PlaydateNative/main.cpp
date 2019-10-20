//
//  main.cpp
//  Playdate Native
//
//  Created by Max Weisel on 10/19/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#include <memory>

extern "C" {
    #include "pd_api.h"
    #include "Config.h"
    #include "Application.h"
    
    using namespace std;
    
    static unique_ptr<APPLICATION_CLASS> __application;

#if TARGET_PLAYDATE
    // Sets up the BSS segment, clearing global variables
    void SetupExtension(void);
#endif
    
    int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t argument) {
        switch (event) {
            case kEventGetHeapPosition: // device only--use template code for this! :)
#if TARGET_PLAYDATE
                extern char bssend asm("__bss_end__");
                return (int)&bssend;
#else
                return 0;
#endif
            case kEventGetVersion:    // return API_VERSION
                return API_VERSION;
            case kEventInit:            // should call SetupExtension() on the device to intialize globals
#if TARGET_PLAYDATE
                SetupExtension();
#endif
                
                // Create application
                __application = make_unique<APPLICATION_CLASS>(playdate);
                return 0;
            case kEventTerminate:
                __application = nullptr;
                return 0;
            default:
                // Dispatch event to application
                if (__application)
                    return __application->HandleEvent(event, argument);
                else
                    return 0;
        }
    }
    
#if TARGET_PLAYDATE
    PDEventHandler* PD_eventHandler __attribute__((section(".extension_reg"))) = &eventHandler;
#endif
}
