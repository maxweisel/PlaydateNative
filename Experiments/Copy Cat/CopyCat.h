//
//  CopyCatApplication.h
//  PlaydateNative
//
//  Created by Max Weisel on 11/11/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

// TODO: This is gross. Why doesn't "Playdate/Playdate.h" work?
#include "../../Playdate/Playdate.h"
#include "math.h"

using namespace Playdate;

#define BUFFER_SIZE 1024*200

class CopyCat : public Application {
public:
    CopyCat() : _state(Recording), _recordStartCursor(0), _recordEndCursor(0), _volumeGate(false), _pitch(1.0f), _playbackCursor(0), _playbackGain(1.0f), _image("face", nullptr) {
        // Clear the buffer
        memset(_audioBuffer, 0, BUFFER_SIZE*sizeof(int16_t));
        
        // Recording
        Audio::SetMicrophoneCallback([this](int16_t *audioData, int audioDataLength, int channels) {
            if (_state != Recording)
                return;
            
            // Measure volume level
            float volumeLevel = CalculateBufferAudioLevel(audioData, audioDataLength, channels);
            float threshold = 0.2f;
            
            bool belowThreshold = volumeLevel <= 0.2f;
            bool aboveThreshold = !belowThreshold;
            bool currentlyRecording = _volumeGate;
            bool hasBeenBelowThresholdForDelay = System::CurrentSteadyClockTime() - _lastAudioTime >= 500;
            
            if (aboveThreshold) _lastAudioTime = System::CurrentSteadyClockTime();
            
            if (!currentlyRecording && aboveThreshold) {
                // Start recording if we went above the threshold
                _volumeGate = true;
                _recordStartCursor = _recordEndCursor;
            }
            if (currentlyRecording && belowThreshold && hasBeenBelowThresholdForDelay) {
                StartPlayback();
            }
            
            // Record audio
            // Audio input callback is interleaved, but we only want the first channel.
            int singleChannelLength = audioDataLength / channels;
            
            int startWriteIndex = _recordEndCursor;
            int   endWriteIndex = _recordEndCursor + singleChannelLength * _pitch;
            int      writeLength = endWriteIndex - startWriteIndex;
            
            for (int i = 0; i < writeLength; i++) {
                float progress = ((float)i)/writeLength;
                int writeIndex = BufferWrap(_recordEndCursor + i, BUFFER_SIZE);
                
                float readPosition = progress * singleChannelLength;
                int   readIndexStart = floorf(readPosition);
                int   readIndexEnd   = readIndexStart + 1;
                
                int16_t startSample =                                      audioData[readIndexStart * channels];
                int16_t   endSample = readIndexEnd < singleChannelLength ? audioData[readIndexEnd   * channels] : startSample;
                
                // Inverse Lerp
                float t = (readPosition - readIndexStart) / 1.0f;
                float range = endSample - startSample;
                float value = startSample + t*range;
                int16_t sample = (int16_t)value;
                                
                _audioBuffer[writeIndex] = sample;
            }
            _recordEndCursor += writeLength;
        });
        
        // Playback
        Audio::SetSpeakerCallback([this](int16_t *leftData, int16_t *rightData, int audioDataLength) {
            if (_state != Playback) {
                for (int i = 0; i < audioDataLength; i++)
                    leftData[i] = rightData[i] = 0;
                return;
            }
            
            // Play back audio
            int recordEndIndex = BufferWrap(_recordEndCursor, BUFFER_SIZE);
            for (int i = 0; i < audioDataLength; i++) {
                int  readIndex = BufferWrap(_playbackCursor + i, BUFFER_SIZE);
                int writeIndex = i;
                
                leftData[writeIndex] = rightData[writeIndex] = _audioBuffer[readIndex] * 2.5f;
                
                // Stop playback if we've played back the whole buffer
                if (readIndex == recordEndIndex) {
                    StartRecording();
                    break;
                }
            }
            _playbackCursor += audioDataLength;
        });
    }
    
    ~CopyCat() {
        // Tear down
        
    }
    
    void StartRecording() {
        _volumeGate = false;
        _recordEndCursor = BufferWrap(_recordEndCursor, BUFFER_SIZE);
        _state = Recording;
    }
    
    void StartPlayback() {
        _playbackCursor = _recordStartCursor;
        CalulcateGainAmount();
        
        _state = Playback;
    }
    
    void CalulcateGainAmount() {
        float max = 0;
        for (int i = _recordStartCursor; i < _recordEndCursor; i++) {
            int16_t value = _audioBuffer[i];
            
            // Always grab the positive value
            if (value < 0) value *= -1;
            
            if (value > max) max = value;
        }
        
        _playbackGain = max > 0.0f ? INT16_MAX/max : 1.0f;
    }
    
    void Update() override {
        // Clear screen
        Graphics::Clear(kColorWhite);
        
        PDButtons current, pressed, released;
        System::ButtonState(&current, &pressed, &released);
        
        if ((pressed & kButtonA) == kButtonA) {
            if (_state == Recording) {
                _recordStartCursor = _recordEndCursor - BUFFER_SIZE/2;
                _playbackCursor = _recordStartCursor;
                _state = Playback;
            }
        }
        
        float crankAngle = System::CrankAngle();
        _pitch = crankAngle / 360.0f + 0.5f;
        
        // Draw face
        int screenWidth  = Display::Width();
        int screenHeight = Display::Height();
        int imageWidth   = _image.Width();
        int imageHeight  = _image.Height();
        int positionX = (screenWidth  - imageWidth)/2;
        int positionY = (screenHeight - imageHeight)/2;
        
        if (_state == Playback)
            _image.Draw(positionX, positionY);
        else
            _image.Draw(0, 0);
        
        // Draw debug Line
        //_audioLevel = Lerp(_audioLevel, _lastFrameAudioLevel, _lastFrameAudioLevel > _audioLevel ? 0.1f : 0.1f);
        //_threshold  = Lerp(_threshold, _lastFrameAudioLevel, _lastFrameAudioLevel > _threshold ? 0.008f : 0.008f);
        Graphics::DrawLine(200, 20, 200, 21.0f + _lastFrameAudioLevel * 200.0f, 20);
        //Graphics::DrawLine(220, 20, 220, 21.0f +  _audioLevel * 200.0f, 20);
        //Graphics::DrawLine(240, 20, 240, 21.0f +  _threshold*1.2f * 200.0f, 20);
    }
    
    // Treat a buffer as a cirular buffer by wrapping the index if it goes off either end of the buffer
    static int BufferWrap(int index, int length) {
        int value = index % length;
        if (value < 0) value += length;
        return value;
    }
    
    static float CalculateBufferAudioLevel(int16_t *audioBuffer, int length, int stride = 1) {
        float averageDbSample = 0.0f;
        for (int i = 0; i < length; i += stride) {
            float audioSample = ((float)audioBuffer[i])/INT16_MAX;
            averageDbSample += audioSample * audioSample;
        }

        averageDbSample = sqrtf(averageDbSample / length);
        averageDbSample = LinearToDb(averageDbSample);
        //averageDbSample = Mathf.Exp(-2.0f * averageDbSample) * averageDbSample;
        return NormalizeDbVolume(averageDbSample);
    }
    
    static float LinearToDb(float linear) {
        float db = -100.0f;

        if (linear != 0.0f)
            db = 10.0f * log10f(linear);

        return db;
    }
    
    static float NormalizeDbVolume(float volumeDb) {
        float averageDbSample = volumeDb;

        // These are arbitrary values I picked from my own testing.
        float volumeMinDb = -30.0f;
        float volumeMaxDb = -10.0f;
        float volumeRange = volumeMaxDb - volumeMinDb;

        float normalizedVolume = (averageDbSample - volumeMinDb) / volumeRange;
        if (normalizedVolume < 0.0f)
            normalizedVolume = 0.0f;
        if (normalizedVolume > 1.0f)
            normalizedVolume = 1.0f;

        return normalizedVolume;
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
private:
    enum State {
        Recording,
        Playback
    };
    State _state;
    
    int16_t _audioBuffer[BUFFER_SIZE];
    
    // Recording
    int     _recordStartCursor;
    int     _recordEndCursor;
    bool    _volumeGate;
    float   _pitch;
    
    // Playback
    int     _playbackCursor;
    float   _playbackGain;
    
    float _threshold = 0.2f;
    float _lastFrameAudioLevel = 0.0f;
    float _audioLevel = 0.0f;
    unsigned int _lastAudioTime = 0;
    
    Graphics::Image _image;
};
