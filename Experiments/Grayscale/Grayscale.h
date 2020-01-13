//
//  Grayscale.h
//  PlaydateNative
//
//  Created by Max Weisel on 1/12/20.
//  Copyright © 2020 Max Weisel. All rights reserved.
//

#pragma once

#include "../../Playdate/Playdate.h"
#include "math.h"

using namespace Playdate;

class Grayscale : public Application {
public:
    Grayscale() {
        Graphics::Clear(kColorWhite);
    }
    
    ~Grayscale() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        
        int brightness = floorf(System::CrankAngle()/360.0f*5.0f); // 1 - 5
        
        uint8_t *displayBuffer = Graphics::GetFrame();
        for (int i = 0; i < LCD_ROWS; i++) {
            uint8_t shadeValue = 0x0;
            
            int frameNumber = (_frameNumber + i) % 4;
            
            if (brightness < 1) {
                shadeValue = 0x0;
            } else if (brightness < 2) {
                // 3 black 1 white
                // Pattern A: 0001
                // Pattern B: 0100
                switch (frameNumber) {
                    case 0:
                        shadeValue = 0b00000000;
                        break;
                    case 1:
                        shadeValue = 0b01010101;
                        break;
                    case 2:
                        shadeValue = 0b00000000;
                        break;
                    case 3:
                        shadeValue = 0b10101010;
                        break;
                }
            } else if (brightness < 3) {
                // Pattern A: 1010
                // Pattern B: 0101
                switch (frameNumber) {
                    case 0:
                        shadeValue = 0b10101010;
                        break;
                    case 1:
                        shadeValue = 0b01010101;
                        break;
                    case 2:
                        shadeValue = 0b10101010;
                        break;
                    case 3:
                        shadeValue = 0b01010101;
                        break;
                }
            } else if (brightness < 4) {
                // Pattern A: 1110
                // Pattern B: 1011
                switch (frameNumber) {
                    case 0:
                        shadeValue = 0b11111111;
                        break;
                    case 1:
                        shadeValue = 0b10101010;
                        break;
                    case 2:
                        shadeValue = 0b11111111;
                        break;
                    case 3:
                        shadeValue = 0b01010101;
                        break;
                }
            } else if (brightness < 5)
                shadeValue = 0xff;
            
            memset(&displayBuffer[i*52], shadeValue, 52);
        }
        
        Graphics::MarkUpdatedRows(0, LCD_ROWS-1);
        
        _frameNumber++;
    }
private:
    int _frameNumber = 0;
};


