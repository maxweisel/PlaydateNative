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
void SetPlaydateAPI(PlaydateAPI *pd) {
    _pd = pd;
}

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


// System
void *System::malloc(size_t size)                                                        { return _pd->system->realloc(NULL, size);                       }
void *System::realloc(void *pointer, size_t size)                                        { return _pd->system->realloc(pointer, size);                    }
void  System::free(void *pointer)                                                        {        _pd->system->realloc(pointer, 0);                       }
void  System::SetUpdateCallback(PDCallbackFunction *update, void *userData)              {        _pd->system->setUpdateCallback(update, userData);       }
void  System::ClearUpdateCallback()                                                      {        _pd->system->setUpdateCallback(NULL, NULL);             }
void  System::GetButtonState(PDButtons *current, PDButtons *pushed, PDButtons *released) {        _pd->system->getButtonState(current, pushed, released); }
float System::GetCrankAngle()                                                            { return _pd->system->getCrankAngle();                           }
void  System::DrawFPS(int x, int y)                                                      {        _pd->system->drawFPS(x, y);                             }


// Graphics
uint8_t *Graphics::GetFrame()                                      { return _pd->graphics->getFrame(); }; // row stride = LCD_ROWSIZE
void     Graphics::MarkUpdatedRows(int start, int end)             {        _pd->graphics->markUpdatedRows(start, end); }
void     Graphics::Clear(LCDSolidColor color)                      {        _pd->graphics->clear(color); }
void     Graphics::DrawLine(int x1, int y1, int x2, int y2,
                            int width,
                            LCDColor color,
                            LCDLineCapStyle endCapStyle,
                            LCDRect clip,
                            std::shared_ptr<Image> target,
                            std::shared_ptr<Image> stencil)        {        _pd->graphics->drawLine( target ?  target->GetNativeLCDBitmap() : NULL,
                                                                                                    stencil ? stencil->GetNativeLCDBitmap() : NULL,
                                                                                                    x1, y1, x2, y2,
                                                                                                    width, color, endCapStyle, clip);                     }

void     Graphics::DrawEllipse(int x, int y,int width, int height,
                               float                  startAngle,
                               float                  endAngle,
                               int                    lineWidth,
                               LCDColor               color,
                               LCDRect                clip,
                               std::shared_ptr<Image> target,
                               std::shared_ptr<Image> stencil)     {        _pd->graphics->drawEllipse( target ?  target->GetNativeLCDBitmap() : NULL,
                                                                                                       stencil ? stencil->GetNativeLCDBitmap() : NULL,
                                                                                                       x, y, width, height,
                                                                                                       lineWidth, startAngle, endAngle,
                                                                                                       color, clip);                                      }

Graphics::Image::Image(int width, int height, LCDColor backgroundColor) {
    _bitmap = _pd->graphics->newBitmap(width, height, backgroundColor);
    SyncMetadata();
}

Graphics::Image::Image(const char *path, const char **error) {
    _bitmap = _pd->graphics->loadBitmap(path, error);
    SyncMetadata();
}

Graphics::Image::Image(Image& bitmap) {
    _bitmap = _pd->graphics->copyBitmap(bitmap._bitmap);
    SyncMetadata();
}

Graphics::Image::~Image() {
    _pd->graphics->freeBitmap(_bitmap);
}

void Graphics::Image::LoadImageAtPath(const char *path, const char **error) {
    _pd->graphics->loadIntoBitmap(path, _bitmap, error);
    SyncMetadata();
}

void Graphics::Image::GetImageData(int *width, int *height, int *rowbytes, int *hasmask, uint8_t **data) {
    _pd->graphics->getBitmapData(_bitmap, width, height, rowbytes, hasmask, data);
}

void Graphics::Image::Draw(int x, int y, LCDBitmapDrawMode mode, LCDBitmapFlip flip, LCDRect clip, std::shared_ptr<Image> target, std::shared_ptr<Image> stencil) {
    _pd->graphics->drawBitmap(_bitmap, target ? target->_bitmap : NULL, stencil ? stencil->_bitmap : NULL, x, y, mode, flip, clip);
}

void Graphics::Image::DrawTiled(int x, int y, int width, int height, LCDBitmapDrawMode mode, LCDBitmapFlip flip, LCDRect clip, std::shared_ptr<Image> target, std::shared_ptr<Image> stencil) {
    _pd->graphics->tileBitmap(_bitmap, target ? target->_bitmap : NULL, stencil ? stencil->_bitmap : NULL, x, y, width, height, mode, flip, clip);
}

void Graphics::Image::SyncMetadata() {
    _pd->graphics->getBitmapData(_bitmap, &_width, &_height, &_rowBytes, &_hasMask, &_data);
}


// Audio
Audio::AudioOutputCallback Audio::_speakerCallback;

int Audio::SpeakerCallback(void *context, int16_t *leftData, int16_t *rightData, int audioDataLength) {
    if (_speakerCallback) {
        _speakerCallback(leftData, rightData, audioDataLength);
        return 1;
    } else {
        return 0;
    }
}

Audio::AudioInputCallback Audio::_microphoneCallback;

int Audio::MicrophoneCallback(void *context, int16_t *audioData, int audioDataLength) {
    const int channels = 2;
    if (_microphoneCallback) {
        _microphoneCallback(audioData, audioDataLength*channels, channels);
        return 1;
    } else {
        return 0;
    }
}

Audio::Sample::Sample(int length) {
    _sample = _pd->audio->sample->newAudioSample(length);
    SyncMetadata();
}

Audio::Sample::Sample(const char *path) {
    _sample = _pd->audio->sample->load(path);
    SyncMetadata();
}

Audio::Sample::~Sample() {
    _pd->audio->sample->unload(_sample);
}

void Audio::Sample::Load(const char *path) {
    _pd->audio->sample->loadIntoSample(_sample, path);
    SyncMetadata();
}

void Audio::Sample::SyncMetadata() {
    _length = _pd->audio->sample->getLength(_sample);
}

Audio::SamplePlayer::SamplePlayer() {
    _samplePlayer = _pd->audio->sampleplayer->newSamplePlayer();
}
Audio::SamplePlayer::~SamplePlayer() {
    _pd->audio->sampleplayer->unload(_samplePlayer);
}

void Audio::SamplePlayer::SetSample(std::shared_ptr<Sample> sample) {
    _pd->audio->sampleplayer->setSample(_samplePlayer, sample->GetNativeAudioSample());
}

int Audio::SamplePlayer::Play(int loopCount, float rate) {
    return _pd->audio->sampleplayer->play(_samplePlayer, loopCount, rate);
}

bool Audio::SamplePlayer::IsPlaying() {
    return _pd->audio->sampleplayer->isPlaying(_samplePlayer) != 0;
}

void Audio::SamplePlayer::Stop() {
    _pd->audio->sampleplayer->stop(_samplePlayer);
}

void Audio::SamplePlayer::SetVolume(float volume) {
    _pd->audio->sampleplayer->setVolume(_samplePlayer, volume);
}
