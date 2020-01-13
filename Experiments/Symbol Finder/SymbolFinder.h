//
//  SymbolFinder.hpp
//  PlaydateNative Simulator
//
//  Created by Max Weisel on 11/17/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "../../Playdate/Playdate.h"

#include <string.h>

using namespace Playdate;

extern "C" {
    //void (*startAccelerometer)(void) = (void (*)(void))0x8014130;
    //void (*readAccelerometer)(float* outX, float* outY, float* outZ) = (void (*)(float* outX, float* outY, float* outZ))0x8014210;
}

class SymbolFinder : public Application {
public:
    SymbolFinder() {
        // Load font
        // TODO: Do we need to free this font? How do you free a font?
        _font = _pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        
        // Start search
        //Search();
    }
    
    ~SymbolFinder() {
        // Tear down
        
    }
    
    void Update() override {
        Graphics::Clear(kColorWhite);
        
        // Update offset
        PDButtons current, pressed, released;
        System::ButtonState(&current, &pressed, &released);
        
        if ((pressed & kButtonUp) == kButtonUp)
            _memoryPointer++;
        if ((pressed & kButtonDown) == kButtonDown)
            _memoryPointer--;
        
        // Display value
        char *buff = NULL;
        char value = *_memoryPointer;
        int length = _pd->system->formatString(&buff, "P: %010p - 0x%X", _memoryPointer, value);
        _pd->graphics->drawText(_font, NULL, NULL, buff, length, kASCIIEncoding, 40, 10, kDrawModeCopy, 0, LCDMakeRect(0,0,0,0));
        System::free(buff);
        buff = NULL;
    }
    
    void DrawStatus(float percent, void *memoryAddress) {
        Graphics::Clear(kColorWhite);
        
        char *buff = NULL;
        // TODO: This leaks formatted strings
        int length = _pd->system->formatString(&buff, "Searching:  %0.2f%% - %010P", percent*100.0f, memoryAddress);
        
        _pd->graphics->drawText(_font, NULL, NULL, buff, length, kASCIIEncoding, 40, 10, kDrawModeCopy, 0, LCDMakeRect(0,0,0,0));
        
    }
    
    void Search() {
        void   *start  = (void *)0x08000000;
        size_t  length = 0x100000;
        
        unsigned char symbolAssembly[] = { 0xF8, 0xB5, 0x17, 0x4C, 0x06, 0x46, 0x15, 0x46, 0x0F, 0x46, 0x23, 0x78, 0x94, 0xF9, 0x01, 0x00, 0x43, 0xEA, 0x00, 0x20, 0x87, 0xF0, 0xA4, 0xFD, 0x00, 0x22, 0x12, 0x4B, 0x87, 0xF0, 0x0A, 0xFE, 0x88, 0xF0, 0x00, 0xF9, 0x38, 0x60, 0xA3, 0x78, 0x94, 0xF9, 0x03, 0x00, 0x43, 0xEA, 0x00, 0x20 };
        
        void *memoryAddress = memmem(start, length, symbolAssembly, sizeof(symbolAssembly));
        
        DrawStatus(1.0f, memoryAddress);
    }
    
    void *memmem(const void *haystack_, size_t hlen, const void *needle, size_t nlen) {
        int needle_first;
        const unsigned char *haystack = (unsigned char *)haystack_;
        unsigned char *p = (unsigned char *)haystack;
        size_t plen = hlen;

        if (nlen <= 0)
            return 0;

        needle_first = *(unsigned char *)needle;

        while (plen >= nlen && (p = (unsigned char *)memchr(p, needle_first, plen - nlen + 1))) {
            DrawStatus(0.0f, p);
            
            if (!memcmp(p, needle, nlen))
                return (void *)p;

            p++;
            plen = hlen - (p - haystack);
        }

        return NULL;
    }
private:
    LCDFont *_font;
    unsigned char *_memoryPointer = (unsigned char *)0x08014140;
};


