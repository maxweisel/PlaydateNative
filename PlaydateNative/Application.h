//
//  Application.h
//  Playdate Native
//
//  Created by Max Weisel on 10/20/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "pd_api.h"
#include <memory>

using namespace std;

class Application {
public:
    Application(PlaydateAPI *pd) : pd(pd) {
        pd->display->setRefreshRate(50);
        pd->system->setUpdateCallback(Update, this);
    }
    
    ~Application() {
        pd->system->setUpdateCallback(NULL, NULL);
    }
    
    static int Update(void *context) {
        Application *application = reinterpret_cast<Application *>(context);
        application->Update();
        return 1;
    }
    
    int HandleEvent(PDSystemEvent event, uint32_t argument) {
        switch (event) {
            case kEventLock:        Lock();                return 0;
            case kEventUnlock:      Unlock();              return 0;
            case kEventPause:       Pause();               return 0;
            case kEventResume:      Resume();              return 0;
            case kEventKeyPressed:  KeyPressed(argument);  return 0;
            case kEventKeyReleased: KeyReleased(argument); return 0;
            default: return 0;
        }
    }
    
    virtual void Update() { };
    
    virtual void Lock()   { }
    virtual void Unlock() { }
    
    virtual void Pause()  { }
    virtual void Resume() { }
    
    virtual void KeyPressed(uint32_t keycode) { }
    virtual void KeyReleased(uint32_t keycode) { }
protected:
    PlaydateAPI *pd;
};

