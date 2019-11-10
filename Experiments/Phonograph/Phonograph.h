//
//  Phonograph.h
//  PlaydateNative
//
//  Created by Max Weisel on 11/7/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

// TODO: This is gross. Why doesn't "Playdate/Playdate.h" work?
#include "../../Playdate/Playdate.h"

#include <math.h>

using namespace Playdate;

#define FRAME_SIZE 1024
#define NUM_FRAMES 128
#define BUFFER_SIZE (FRAME_SIZE*NUM_FRAMES)
#define MIN(x, y) (x < y ? x : y)

class Phonograph : public Application {
public:
    Phonograph() : _recording(false), _cursor(0), _crankAngle(System::GetCrankAngle()), _crankSpeed(0.0f) {
        // Clear the buffer
        memset(_audioBuffer, 0, BUFFER_SIZE*sizeof(int16_t));
        
        // Set audio callbacks
        Audio::AddSource(AudioCallback, this, 1);
        Audio::SetMicCallback(MicrophoneCallback, this);
    }
    
    static int AudioCallback(void *context, int16_t *left, int16_t *right, int length) { return reinterpret_cast<Phonograph *>(context)->AudioCallback(left, right, length); }
    int AudioCallback(int16_t *left, int16_t *right, int length) {
        if (_recording) {
            for (int i = 0; i < length; i++)
                left[i] = right[i] = 0;
            return 1;
        }
        
        int startReadPosition = _cursor;
        int   endReadPosition = _cursor = _cursor+_crankSpeed*length;
        int      readLength   = endReadPosition - startReadPosition;
        
        int startWritePosition = 0;
        int   endWritePosition = length;
        int      writeLength   = endWritePosition - startWritePosition;
        
        for (int i = 0; i < writeLength; i++) {
            float      position = ((float)i)/writeLength;
            int    readPosition = startReadPosition + position*readLength;
            int   writePosition = startWritePosition + i;
            
            left[writePosition] = right[writePosition] = _audioBuffer[BufferWrap(readPosition, BUFFER_SIZE)];
        }
        
        return 1;
    }

    static int MicrophoneCallback(void *context, int16_t *data, int length) { return reinterpret_cast<Phonograph *>(context)->MicrophoneCallback(data, length); }
    int MicrophoneCallback(int16_t *data, int length) {
        if (!_recording) {
            return 1;
        }
        
        // Record audio
        int channels = 2; // Microphone is stereo interleaved, but we only want the first channel.
        
        int startWritePosition = _cursor;
        int   endWritePosition = _cursor = _cursor+_crankSpeed*length;
        int      writeLength   = endWritePosition - startWritePosition;
        
        int startReadPosition = 0;
        int   endReadPosition = length;
        int      readLength   = endReadPosition - startReadPosition;
        
        for (int i = 0; i < writeLength; i++) {
            float      position = ((float)i)/writeLength;
            int    readPosition = startReadPosition + position*readLength;
            int   writePosition = startWritePosition + i;
            
            _audioBuffer[BufferWrap(writePosition, BUFFER_SIZE)] = data[readPosition*channels];
        }
                
        return 1;
    }
    
    void Update() override {
        // Clear
        Graphics::Clear(kColorWhite);
        
        PDButtons current, pressed, released;
        System::GetButtonState(&current, &pressed, &released);
        
        if ((pressed & kButtonA) == kButtonA)
            _recording = !_recording;
        
        if (_recording)
            System::DrawFPS(10, 10);
        
        // Calculate crank delta since the last frame
        float crankAngle = System::GetCrankAngle();
        float delta = crankAngle-_crankAngle;
        while (delta >  180.0f) delta -= 360.0f;
        while (delta < -180.0f) delta += 360.0f;
        _crankAngle = crankAngle;
        
        // Lerp towards target crank speed
        float targetCrankSpeed = delta/360.0f * 10.0f;
        _crankSpeed = Lerp(_crankSpeed, targetCrankSpeed, 0.05f);
        
        // Draw circle to represent buffer
        DrawCenteredCircle(0, 360, 4, 20);
        
        // Draw cursor
        float position = ((float)_cursor)/BUFFER_SIZE;
        int startAngle = position*360+5;
        int endAngle = position*360-5;
        DrawCenteredCircle(startAngle, endAngle, 4, 10);
    }
    
    void DrawCenteredCircle(int startAngle, int endAngle, int width, int padding) {
        int screenWidth  = Display::Width();
        int screenHeight = Display::Height();
        int diameter = MIN(screenWidth, screenHeight)-(padding*2);
        int paddingX = (screenWidth  - diameter)/2;
        int paddingY = (screenHeight - diameter)/2;
        Graphics::DrawEllipse(NULL, NULL, paddingX, paddingY, diameter, diameter, width, startAngle, endAngle, kColorBlack, LCDMakeRect(0,0,0,0));
    }
    
    static float Lerp(float a, float b, float t) {
        // Lerp
        float range = b - a;
        float value = a + t*range;
        
        // Clamp
        float min = a < b ? a : b;
        float max = a > b ? a : b;
        if (value < min) value = min;
        if (value > max) value = max;
        
        return value;
    }
    
    // Treat a buffer as a cirular buffer by wrapping the index if it goes off either end of the buffer
    static int BufferWrap(int index, int length) {
        int value = index % length;
        if (value < 0) value += length;
        return value;
    }
private:
    bool    _recording;
    int16_t _audioBuffer[BUFFER_SIZE];
    float   _cursor;
    
    float _crankAngle;
    float _crankSpeed;
};
