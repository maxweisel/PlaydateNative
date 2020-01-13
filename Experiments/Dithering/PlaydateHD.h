//
//  PlaydateHD.h
//  PlaydateNative
//
//  Created by Max Weisel on 11/28/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "../../Playdate/Playdate.h"
#include <math.h>

using namespace Playdate;

class PlaydateHD {
public:
    PlaydateHD() {
        _displaySize      = Display::Size();
        _displayRowStride = CalculateRowStride(_displaySize.x);
        _displayBuffer    = Graphics::GetFrame();
        
        int framebufferSize = _displaySize.x * _displaySize.y;
        _framebuffer = (uint8_t *)System::malloc(framebufferSize);
        memset(_framebuffer, 0, framebufferSize);
        
        // Test pattern
        for (int h = 0; h < _displaySize.y; h++) {
            float hProgress = ((float)h)/_displaySize.y;
            float b = (sinf(hProgress*1.0f*3.141592653f*2.0f) + 1.0f)/2.0f;
            for (int w = 0; w < _displaySize.x; w++) {
                float wProgress = ((float)w)/_displaySize.x;
                float a = (sinf(wProgress*1.1f*3.141592653f*2.0f) + 1.0f)/2.0f;
                _framebuffer[w + h*_displaySize.x] = (uint8_t)((a*0.5f + b*0.5f)*256.0f);
            }
        }
    }
    
    // Round width (in bis) up to nearest multiple of 32 bits. Return as number of bytes.
    static int CalculateRowStride(int width) {
        int rowStrideBits  = (width + 32 - 1) / 32 * 32;
        int rowStrideBytes = rowStrideBits / 8;
        return rowStrideBytes;
    }
    
    ~PlaydateHD() {
        System::free(_framebuffer);
    }
    
    static int index(int x, int y, int w) {
        return x + y * w;
    }
    
    void ApplyError(uint16_t x, uint8_t y, int16_t error, uint8_t amount) {
        _framebuffer[index(x, y, _displaySize.x)] += error * amount >> 4;
    }
    
    float test = 0.0f;
    void Render() {
        test += 0.01f;
        
        // Test pattern
        //for (int h = 0; h < _displaySize.y; h++) {
        //    float hProgress = ((float)h)/_displaySize.y;
        //    for (int w = 0; w < _displaySize.x; w++) {
        //        float wProgress = ((float)w)/_displaySize.x;
        //        _framebuffer[w + h*_displaySize.x] = (uint8_t)((wProgress*0.5f + hProgress*0.5f)*256.0f);
        //    }
        //}
        
        // Dither frame buffer an blit
        for (int y = 0; y < _displaySize.y; y++) {
            for (int z = 0; z < _displaySize.x/8; z++) {
                uint8_t byte = 0;
                for (int i = 0; i < 8; i++) {
                    int x = z*8 + i;
                    
                    int16_t  inputColor = _framebuffer[index(x, y, _displaySize.x)];
                    uint8_t closestColor = 0;
                    if (inputColor > 96) {
                        closestColor = 255;
                        byte |= 1 << (7-i);
                    }
                    _framebuffer[index(x, y, _displaySize.x)] = closestColor;
                    
                    int16_t error = inputColor - closestColor;
                    _framebuffer[(x + 1) + (y    ) * _displaySize.x] += error * 7 >> 4; //ApplyError(x + 1, y    , error, 7);
                    _framebuffer[(x - 1) + (y + 1) * _displaySize.x] += error * 3 >> 4; //ApplyError(x - 1, y + 1, error, 3);
                    _framebuffer[(x    ) + (y + 1) * _displaySize.x] += error * 5 >> 4; //ApplyError(x    , y + 1, error, 5);
                    _framebuffer[(x + 1) + (y + 1) * _displaySize.x] += error * 1 >> 4; //ApplyError(x + 1, y + 1, error, 1);
                }
                _displayBuffer[z + y*_displayRowStride] = byte;
            }
        }
        
        // Blit to display buffer
        //for (int h = 0; h < _displaySize.y; h++) {
        //    for (int w = 0; w < _displaySize.x/8; w++) {
        //        uint8_t byte = 0;
        //        for (int i = 0; i < 8; i++) {
        //            uint8_t pixel = _framebuffer[i + w*8 + h*_displaySize.x];
        //
        //            // If the pixel is bright enough, flip it to white
        //            if (pixel != 0)
        //                byte |= 1 << (7-i);
        //        }
        //        _displayBuffer[w + h*_displayRowStride] = byte;
        //    }
        //}
        Graphics::MarkUpdatedRows(0, LCD_ROWS-1);
    }
private:
    // Display
    Vector2i _displaySize;
    int      _displayRowStride;
    uint8_t *_displayBuffer; // 52 byte row stride. Last two bits are ignored.
    
    // Framebuffer
    uint8_t *_framebuffer;
};
