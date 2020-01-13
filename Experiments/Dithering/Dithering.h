//
//  Dithering.h
//  PlaydateNative Simulator
//
//  Created by Max Weisel on 11/12/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "../../Playdate/Playdate.h"

#include "PlaydateHD.h"

using namespace Playdate;

class Dithering : public Application {
public:
    Dithering() {
        
    }
    
    ~Dithering() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        
        // Clear screen
        //Graphics::Clear(kColorWhite);
        _playdateHD.Render();
        
        // Draw FPS indicator
        System::DrawFPS(10, 10);
    }
private:
    PlaydateHD _playdateHD;
};


