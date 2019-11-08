//
//  Playdate.h
//  PlaydateNative
//
//  Created by Max Weisel on 10/22/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

// TODO: Move all method implementations to cpp file so pd_api isn't included when the C++ API is included.
#include "pd_api.h"

#include <memory>

// TODO: Can we move this inside of the namespace?
extern PlaydateAPI *_pd;

namespace Playdate {
    void SetPlaydateAPI(PlaydateAPI *pd) { _pd = pd; }
    
    namespace System {
        void *malloc(size_t size) { return _pd->system->realloc(NULL, size); }
        void *realloc(void *pointer, size_t size) { return _pd->system->realloc(pointer, size); }
        void  free(void *pointer) { _pd->system->realloc(pointer, 0); }
        
        void SetUpdateCallback(PDCallbackFunction *update, void *userData) { _pd->system->setUpdateCallback(update, userData); }
        void ClearUpdateCallback() { _pd->system->setUpdateCallback(NULL, NULL); }
        
        // TODO: Implement
        void GetButtonState(PDButtons *current, PDButtons *pushed, PDButtons *released) { _pd->system->getButtonState(current, pushed, released); }
        //void (*setPeripheralsEnabled)(PDPeripherals mask);
        //void (*getAccelerometer)(float* accelerometer, float* magnetometer);
        //float (*getCrankChange)(void);
        float GetCrankAngle() { return _pd->system->getCrankAngle(); }
        //void (*logToConsole)(char* fmt, ...);
        //void (*error)(const char* fmt, ...);
        //int (*formatString)(char **ret, const char *fmt, ...);
        //void (*setMenuImage)(LCDBitmap* bitmap, int xOffset);
        //PDLanguage (*getLanguage)(void);
        //unsigned int (*getCurrentTimeMilliseconds)(void);
        //unsigned int (*getSecondsSinceEpoch)(unsigned int *milliseconds);
        void DrawFPS(int x, int y) { _pd->system->drawFPS(x, y); }
    }
    
    // File
    // TODO: Implement
    
    // Graphics
    namespace Graphics {
        // TODO: Implement
        uint8_t *GetFrame() { return _pd->graphics->getFrame(); }; // row stride = LCD_ROWSIZE
        //uint8_t* (*getDisplayFrame)(void); // row stride = LCD_ROWSIZE
        //LCDBitmap* (*getDebugImage)(void); // valid in simulator only, function is NULL on device
        //LCDBitmap* (*getFrameBufferBitmap)(void);
        
        void MarkUpdatedRows(int start, int end) { _pd->graphics->markUpdatedRows(start, end); }
        //void (*display)(void);
    
        //void (*setDrawOffset)(int dx, int dy);
        
        //LCDBitmap* (*newBitmap)(int width, int height, LCDColor bgcolor);
        //void (*freeBitmap)(LCDBitmap*);
        //LCDBitmap* (*loadBitmap)(const char* path, const char** outerr);
        //LCDBitmap* (*copyBitmap)(LCDBitmap* bitmap);
        //void (*loadIntoBitmap)(const char* path, LCDBitmap* bitmap, const char** outerr);
        //void (*getBitmapData)(LCDBitmap* bitmap, int* width, int* height, int* rowbytes, int* hasmask, uint8_t** data);
        //LCDBitmapTable* (*loadBitmapTable)(const char* path, const char** outerr);
        //LCDBitmap* (*getTableBitmap)(LCDBitmapTable* table, int idx);
        void Clear(LCDSolidColor color) { _pd->graphics->clear(color); }
        //void (*setColorToPattern)(LCDColor* color,LCDBitmap* bitmap, int x, int y);
    
        // if target is NULL, draws in display frame buffer
        //void (*drawBitmap)(LCDBitmap* bitmap, LCDBitmap* target, LCDBitmap* stencil, int x, int y, LCDBitmapDrawMode mode, LCDBitmapFlip flip, LCDRect clip);
        //void (*tileBitmap)(LCDBitmap* bitmap, LCDBitmap* target, LCDBitmap* stencil, int x, int y, int width, int height, LCDBitmapDrawMode mode, LCDBitmapFlip flip, LCDRect clip);
        //void (*drawLine)(LCDBitmap* target, LCDBitmap* stencil, int x1, int y1, int x2, int y2, int width, LCDColor color, LCDLineCapStyle endCapStyle, LCDRect clip);
        //void (*fillTriangle)(LCDBitmap* target, LCDBitmap* stencil, int x1, int y1, int x2, int y2, int x3, int y3, LCDColor color, LCDRect clip);
        //void (*drawRect)(LCDBitmap* target, LCDBitmap* stencil, int x, int y, int width, int height, LCDColor color, LCDRect clip);
        //void (*fillRect)(LCDBitmap* target, LCDBitmap* stencil, int x, int y, int width, int height, LCDColor color, LCDRect clip);
        void DrawEllipse(LCDBitmap *target, LCDBitmap *stencil, int x, int y, int width, int height, int lineWidth, float startAngle, float endAngle, LCDColor color, LCDRect clip) { _pd->graphics->drawEllipse(target, stencil, x, y, width, height, lineWidth, startAngle, endAngle, color, clip); }
        //void (*fillEllipse)(LCDBitmap* target, LCDBitmap* stencil, int x, int y, int width, int height, float startAngle, float endAngle, LCDColor color, LCDRect clip);
    
        //int (*checkMaskCollision)(LCDBitmap* bitmap1, int x1, int y1, LCDBitmapFlip flip1, LCDBitmap* bitmap2, int x2, int y2, LCDBitmapFlip flip2, LCDRect rect);
        
        //LCDBitmapTable* (*newBitmapTable)(int count, int width, int height);
        //void (*loadIntoBitmapTable)(const char* path, LCDBitmapTable* table, const char** outerr);
        
        //LCDBitmap* (*transformedBitmap)(LCDBitmap* bitmap, float rotation, float xscale, float yscale, int* allocedSize);
        
        //void (*drawScaledBitmap)(LCDBitmap* bitmap, LCDBitmap* target, LCDBitmap* stencil, int x, int y, float xscale, float yscale, LCDBitmapDrawMode mode, LCDRect clip);
        //void (*clearBitmap)(LCDBitmap* bitmap, LCDColor bgcolor);
        
        //LCDFont* (*loadFont)(const char* path, const char** outErr);
        //LCDBitmap* (*getFontGlyph)(LCDFont* font, uint16_t c, unsigned int* advance);
        //int (*getFontKerning)(LCDFont* font, uint16_t c1, uint16_t c2);
        //int (*getTextWidth)(LCDFont* font, const void* text, size_t len, PDStringEncoding encoding, int tracking);
        //int (*drawText)(LCDFont* font, LCDBitmap* target, LCDBitmap* stencil, const void* text, size_t len, PDStringEncoding encoding, int x, int y, LCDBitmapDrawMode mode, int tracking, LCDRect clip);

    }

    // Sprite
    // TODO: Implement
    
    // Display
    namespace Display {
        int  Width()  { return _pd->display->getWidth();  }
        int  Height() { return _pd->display->getHeight(); }
        void SetInverted(bool inverted) { _pd->display->setInverted(inverted ? 1 : 0); }
        void SetScaleFactor(int scaleFactor) { _pd->display->setScale(scaleFactor); }
        void SetMosaic(int x, int y) { _pd->display->setMosaic(x, y); }
        void SetRefreshRate(float refreshRate) { _pd->display->setRefreshRate(refreshRate); }
    };
    
    // Audio
    namespace Audio {
        // TODO: It would be cool to be able to use a lambda or somenthing here.
        void AddSource(AudioSourceFunction* callback, void *context, bool stereo) { _pd->audio->addSource(callback, context, stereo ? 1 : 0); }
        void SetMicCallback(AudioInputFunction* callback, void *context) { _pd->audio->setMicCallback(callback, context); }
    
        void Start() { _pd->audio->start(); }
        void Stop() { _pd->audio->stop(); }
    
        class Sample {
        public:
            Sample(int length) {
                _sample = _pd->audio->sample->newAudioSample(length);
            }
            // TODO: Switch to std::string?
            Sample(const char *path) {
                _sample = _pd->audio->sample->load(path);
            }
            ~Sample() {
                _pd->audio->sample->unload(_sample);
            }
        
            // Load a new file into this sample
            // TODO: Switch to std::string?
            void Load(const char *path) {
                _pd->audio->sample->loadIntoSample(_sample, path);
            }
        
            // Length in seconds
            float Length() {
                return _pd->audio->sample->getLength(_sample);
            }
            
            ::AudioSample *GetNativeSample() { return _sample; }
        private:
            ::AudioSample *_sample;
        };
    
        class SamplePlayer {
        public:
            SamplePlayer() {
                _samplePlayer = _pd->audio->sampleplayer->newSamplePlayer();
            }
            ~SamplePlayer() {
                _pd->audio->sampleplayer->unload(_samplePlayer);
            }
            
            void SetSample(std::shared_ptr<Sample> sample) {
                _pd->audio->sampleplayer->setSample(_samplePlayer, sample->GetNativeSample());
            }
            
            // TODO: What is the return value of this for? Is it supposed to be a bool?
            int Play(int loopCount = 1, float rate = 1.0f) {
                return _pd->audio->sampleplayer->play(_samplePlayer, loopCount, rate);
            }
            
            bool isPlaying() {
                return _pd->audio->sampleplayer->isPlaying(_samplePlayer) != 0;
            }
            
            void Stop() {
                _pd->audio->sampleplayer->stop(_samplePlayer);
            }
            
            void SetVolume(float volume) {
                _pd->audio->sampleplayer->setVolume(_samplePlayer, volume);
            }
        
            // TODO: Implement
            //void (*setFinishCallback)(SamplePlayer* player, sndCallbackProc callback);
            //float (*getLength)(SamplePlayer* player);
            //void (*setOffset)(SamplePlayer* player, float offset);
            //void (*setRate)(SamplePlayer* player, float rate);
            //void (*setPlayRange)(SamplePlayer* player, int start, int end);
        private:
            ::SamplePlayer *_samplePlayer;
        };
    
        // TODO: Implement
        //struct playdate_audio_fileplayer* fileplayer;
    }
    
    // Lua
    // TODO: Implement
    
    // JSON
    // TODO: Implement

    class Application {
    public:
        Application();
        virtual ~Application();
    
        static int Update(void *context);
        
        int HandleEvent(PDSystemEvent event, uint32_t argument);
    
        virtual void Update() { };
    
        virtual void Lock()   { }
        virtual void Unlock() { }
    
        virtual void Pause()  { }
        virtual void Resume() { }
    
        virtual void KeyPressed(uint32_t keycode) { }
        virtual void KeyReleased(uint32_t keycode) { }
    };
};
