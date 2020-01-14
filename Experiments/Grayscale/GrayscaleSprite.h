//
//  GrayscaleSprite.h
//  PlaydateNative
//
//  Created by Max Weisel on 1/13/20.
//  Copyright © 2020 Max Weisel. All rights reserved.
//

#pragma once

#include "../../Playdate/Playdate.h"
#include "math.h"

#include "Sprite.h"

using namespace Playdate;

class GrayscaleSprite : public Application {
public:
    GrayscaleSprite() {
        Graphics::Clear(kColorWhite);
    }
    
    ~GrayscaleSprite() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        uint8_t *displayBuffer = Graphics::GetFrame();
        for (int h = 0; h < 240; h++) {
            for (int w = 0; w < 400/8; w++) {
                uint8_t byte = 0;
                for (int i = 0; i < 8; i++) {
                    int colorIndex = i + w*8 + h*400;
                    int color = sprite[colorIndex];
                    
                    int frameNumber = (_frameNumber + h) % 4;
                    bool evenPixel = (i + w*8) % 2 == 0;

                    bool error = false;
                    bool pixelShouldBeOn = GetPixelShouldBeOn(color, frameNumber, evenPixel, &error);
                    if (error)
                        return;
                    
                    if (pixelShouldBeOn)
                        byte |= 1 << (7-i);
                }
                displayBuffer[w + h*52] = byte;
            }
        }
                
        Graphics::MarkUpdatedRows(0, LCD_ROWS-1);
        
        _frameNumber++;
    }
    
    static bool GetPixelShouldBeOn(int color, int frameNumber, bool evenPixel, bool *error) {
        switch (color) {
            case 0:
                return false;
            case 1:
                switch (frameNumber) {
                    case 0:
                        return false;
                    case 1:
                        return evenPixel ? false : true;
                    case 2:
                        return false;
                    case 3:
                        return evenPixel ? true : false;
                }
                break;
            case 2:
                switch (frameNumber) {
                    case 0:
                        return evenPixel ? true : false;
                    case 1:
                        return evenPixel ? false : true;
                    case 2:
                        return evenPixel ? true : false;
                    case 3:
                        return evenPixel ? false : true;
                }
                break;
            case 3:
                switch (frameNumber) {
                    case 0:
                        return true;
                    case 1:
                        return evenPixel ? true : false;
                    case 2:
                        return true;
                    case 3:
                        return evenPixel ? false : true;
                }
                break;
            case 4:
                return true;
        }
        
        *error = true;
        return false;
    }
private:
    int _frameNumber = 0;
};
