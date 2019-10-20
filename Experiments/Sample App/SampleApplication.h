//
//  SampleApplication.h
//  PlaydateNative
//
//  Created by Max Weisel on 10/20/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "Application.h"

class SampleApplication : public Application {
public:
    SampleApplication(PlaydateAPI *pd) : Application(pd) {
        // Set up
        
    }
    
    ~SampleApplication() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        
        // Clear screen
        pd->graphics->clear(kColorWhite);
        
        // Draw FPS indicator
        pd->system->drawFPS(10, 10);
    }
private:
    
};


