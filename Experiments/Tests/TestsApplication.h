//
//  TestsApplication.h
//  PlaydateNative
//
//  Created by Max Weisel on 10/20/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "Playdate.h"

using namespace Playdate;

#include "math.h"
static int frame = 0;


#define MIN(a, b) ((a > b) ? a : b)

static int __lastSpeakerLength = 0;
static int __lastMicrophoneLength = 0;
static float __crankAngle = 0.0f;

static int __test = 0;
static AudioSample *__sample;

static int16_t audioData[1024];

int TestAudioSource(void* context, int16_t* left, int16_t* right, int len) {
    for (int i = 0; i < len; i++)
         left[i] = right[i] = audioData[i*2];
    
    return 1;
}

int TestMicrophoneInput(void* context, int16_t* data, int len) {
    memcpy(audioData, data, len*2*sizeof(int16_t));
    
    return 1;
}

class TestsApplication : public Application {
public:
    LCDFont *_font;
    TestsApplication() {
        // Set up
        _font = _pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);
        _pd->audio->addSource(TestAudioSource, NULL, 1);
        _pd->audio->setMicCallback(TestMicrophoneInput, NULL);
    }
    
    ~TestsApplication() {
        // Tear down
        
    }
    
    void Update() override {
        // Update
        
        // Clear screen
        _pd->graphics->clear(kColorWhite);
        
        // Draw FPS indicator
        _pd->system->drawFPS(10, 10);
        
        __crankAngle = _pd->system->getCrankAngle()/360.0f;
        
        char *buff = NULL;
        int length = _pd->system->formatString(&buff, "%i", __lastSpeakerLength);
        _pd->graphics->drawText(_font, NULL, NULL, buff, length, kASCIIEncoding, 40, 10, kDrawModeCopy, 0, LCDMakeRect(0,0,0,0));
        
        length = _pd->system->formatString(&buff, "%f", (1.0f + __crankAngle));
        _pd->graphics->drawText(_font, NULL, NULL, buff, length, kASCIIEncoding, 80, 10, kDrawModeCopy, 0, LCDMakeRect(0,0,0,0));
        
        //if (__test % 2 == 0 && __test < 20) {
        //    __sample = _pd->audio->sample->load("musicbox");
        //    SamplePlayer *player = _pd->audio->sampleplayer->newSamplePlayer();
        //    _pd->audio->sampleplayer->setSample(player, __sample);
        //    _pd->audio->sampleplayer->play(player, 1, 1.0f + (__test/20.0f));
        //}
        //
        //__test++;
    }
private:
    
};


