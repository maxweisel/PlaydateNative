//
//  Playdate.cpp
//  PlaydateNative
//
//  Created by Max Weisel on 10/22/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#include "Playdate.h"

#include <memory>

using namespace std;
using namespace Playdate;

#if TARGET_PLAYDATE
#define _THROW_BAD_ALLOC
#define _NOEXCEPT
#endif

// C++ Allocator
PlaydateAPI *_pd;

void *operator new(size_t s) _THROW_BAD_ALLOC {
    void *const p = System::malloc(s);
    return p;
}

void operator delete(void *p) _NOEXCEPT {
    System::free(p);
}


// Application
Application::Application() {
    Display::SetRefreshRate(50);
    System::SetUpdateCallback(Update, this);
}

Application::~Application() {
    System::ClearUpdateCallback();
}

int Application::Update(void *context) {
    Application *application = reinterpret_cast<Application *>(context);
    application->Update();
    return 1;
}

int Application::HandleEvent(PDSystemEvent event, uint32_t argument) {
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
