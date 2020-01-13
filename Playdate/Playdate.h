//
//  Playdate.h
//  PlaydateNative
//
//  Created by Max Weisel on 10/22/19.
//  Copyright © 2019 Max Weisel. All rights reserved.
//

#pragma once

#include "pd_api.h"
#include <memory>
#include <functional>

// TODO: Remove this from the header so it's private at some point
extern PlaydateAPI *_pd;
void SetPlaydateAPI(PlaydateAPI *pd);

namespace Playdate {
    template <typename T>
    struct Vector2 {
        T x;
        T y;
        
        Vector2() : x(0.0f), y(0.0f) { }
        Vector2(T x, T y) : x(x), y(y) { }
        
        Vector2 operator + (const Vector2 v) const { return Vector2(x + v.x, y + v.y); }
        Vector2 operator - (const Vector2 v) const { return Vector2(x - v.x, y - v.y); }
        Vector2 operator * (const T v)       const { return Vector2(x * v, y / v);     }
        Vector2 operator / (const T v)       const { return Vector2(x / v, y / v);     }

        Vector2& operator += (const Vector2& v) { x += v.x; y += v.y; return *this; }
        Vector2& operator -= (const Vector2& v) { x -= v.x; y -= v.y; return *this; }
        Vector2& operator *= (const T v)        { x *= v;   y *= v;   return *this; }
        Vector2& operator /= (const T v)        { x /= v;   y /= v;   return *this; }
        
        operator Vector2<float>() const { return Vector2<float>(x, y); }
        
        static Vector2<T> zero;
        static Vector2<T> one;
    };

    template <typename T> Vector2<T> Vector2<T>::zero = Vector2<T>(0, 0);
    template <typename T> Vector2<T> Vector2<T>::one  = Vector2<T>(1, 1);

    template <typename T>
    struct Vector3 {
        T x;
        T y;
        T z;
        
        Vector3() : x(0.0f), y(0.0f), z(0.0f) { }
        Vector3(float x, float y, float z) : x(x), y(y), z(z) { }
        
        Vector3 operator + (const Vector3 v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
        Vector3 operator - (const Vector3 v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
        Vector3 operator * (const T v)   const { return Vector3(x * v, y / v, z * v);       }
        Vector3 operator / (const T v)   const { return Vector3(x / v, y / v, z / v);       }

        Vector3& operator += (const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
        Vector3& operator -= (const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
        Vector3& operator *= (const T v)        { x *= v;   y *= v;   z *= v;   return *this; }
        Vector3& operator /= (const T v)        { x /= v;   y /= v;   z /= v;   return *this; }

        operator Vector3<float>() const { return Vector3<float>(x, y, z); }
        operator Vector2<T>()     const { return Vector2<T>(x, y); }
        
        static Vector3<T> zero;
        static Vector3<T> one;
    };

    template <typename T> Vector3<T> Vector3<T>::zero = Vector3<T>(0, 0, 0);
    template <typename T> Vector3<T> Vector3<T>::one  = Vector3<T>(1, 1, 1);

    typedef Vector2<float> Vector2f;
    typedef Vector2<int>   Vector2i;
    typedef Vector3<float> Vector3f;
    typedef Vector3<int>   Vector3i;

    class System {
    public:
        static void *malloc(size_t size);
        static void *realloc(void *pointer, size_t size);
        static void  free(void *pointer);
        
        static void SetUpdateCallback(PDCallbackFunction *update, void *userData);
        static void ClearUpdateCallback();
        
        static void ButtonState(PDButtons *current, PDButtons *pushed, PDButtons *released);
        static void SetPeripheralsEnabled(PDPeripherals mask);
        static Vector3f Accelerometer();
        //float (*getCrankChange)(void);
        static float CrankAngle();
        //void (*logToConsole)(char* fmt, ...);
        //void (*error)(const char* fmt, ...);
        //int (*formatString)(char **ret, const char *fmt, ...);
        //void (*setMenuImage)(LCDBitmap* bitmap, int xOffset);
        //PDLanguage (*getLanguage)(void);
        static unsigned int CurrentSteadyClockTime(); // Measured in milliseconds
        //unsigned int (*getSecondsSinceEpoch)(unsigned int *milliseconds);
        static void DrawFPS(int x, int y);
    };
    
    // File
    // TODO: Implement
    
    // Graphics
    class Graphics {
    public:
        class Image;
        
        static uint8_t *GetFrame(); // row stride = LCD_ROWSIZE
        //uint8_t* (*getDisplayFrame)(void); // row stride = LCD_ROWSIZE
        //LCDBitmap* (*getDebugImage)(void); // valid in simulator only, function is NULL on device
        //LCDBitmap* (*getFrameBufferBitmap)(void);
        
        static void MarkUpdatedRows(int start, int end);
        //void (*display)(void);
    
        //void (*setDrawOffset)(int dx, int dy);
        
        //LCDBitmapTable* (*loadBitmapTable)(const char* path, const char** outerr);
        //LCDBitmap* (*getTableBitmap)(LCDBitmapTable* table, int idx);
        static void Clear(LCDSolidColor color);
        //void (*setColorToPattern)(LCDColor* color, LCDBitmap* bitmap, int x, int y);
    
        void (*drawLine)(LCDBitmap* target, LCDBitmap* stencil, int x1, int y1, int x2, int y2, int width, LCDColor color, LCDLineCapStyle endCapStyle, LCDRect clip);
        static void DrawLine(int x1, int y1, int x2, int y2,
                             int width                        = 1,
                             LCDColor color                   = kColorBlack,
                             LCDLineCapStyle endCapStyle      = kLineCapStyleButt,
                             LCDRect clip                     = LCDMakeRect(0.0f, 0.0f, 0.0f, 0.0f),
                             std::shared_ptr<Image> target    = nullptr,
                             std::shared_ptr<Image> stencil   = nullptr);
        //void (*fillTriangle)(LCDBitmap* target, LCDBitmap* stencil, int x1, int y1, int x2, int y2, int x3, int y3, LCDColor color, LCDRect clip);
        //void (*drawRect)(LCDBitmap* target, LCDBitmap* stencil, int x, int y, int width, int height, LCDColor color, LCDRect clip);
        //void (*fillRect)(LCDBitmap* target, LCDBitmap* stencil, int x, int y, int width, int height, LCDColor color, LCDRect clip);
        static void DrawEllipse(int x, int y, int width, int height, float startAngle, float endAngle,
                                int                    lineWidth = 1,
                                LCDColor               color     = kColorBlack,
                                LCDRect                clip      = LCDMakeRect(0.0f, 0.0f, 0.0f, 0.0f),
                                std::shared_ptr<Image> target    = nullptr,
                                std::shared_ptr<Image> stencil   = nullptr);
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
        
        class Image {
        public:
            Image(int width, int height, LCDColor backgroundColor);
            Image(const char *path, const char **error);
            Image(Image& bitmap);
            
            ~Image();
            
            int      Width()  const { return _width;  }
            int      Height() const { return _height; }
            Vector2i Size()   const { return Vector2i(_width, _height); }
            
            // TODO: What does this represent? Is this essentially Width/8?
            int RowBytes() const { return _rowBytes; }
            
            // TODO: What does this represent? Should it be a bool?
            int HasMask() const { return _hasMask; }
            
            uint8_t *Data() const { return _data; }
            
            void LoadImageAtPath(const char *path, const char **error);
            
            void GetImageData(int *width, int *height, int *rowbytes, int *hasmask, uint8_t **data);
            
            void Draw(int x, int y,
                      LCDBitmapDrawMode      mode    = kDrawModeCopy,
                      LCDBitmapFlip          flip    = kBitmapUnflipped,
                      LCDRect                clip    = LCDMakeRect(0.0f, 0.0f, 0.0f, 0.0f),
                      std::shared_ptr<Image> target  = nullptr,
                      std::shared_ptr<Image> stencil = nullptr);
            
            void DrawTiled(int x, int y, int width, int height,
                           LCDBitmapDrawMode      mode    = kDrawModeCopy,
                           LCDBitmapFlip          flip    = kBitmapUnflipped,
                           LCDRect                clip    = LCDMakeRect(0.0f, 0.0f, 0.0f, 0.0f),
                           std::shared_ptr<Image> target  = nullptr,
                           std::shared_ptr<Image> stencil = nullptr);
            
            LCDBitmap *GetNativeLCDBitmap() { return _bitmap; }
        private:
            ::LCDBitmap *_bitmap;
            int _width;
            int _height;
            int _rowBytes; // TODO: Can we calculate this ourselves when needed?
            int _hasMask;  // TODO: Can this be a bool?
            uint8_t *_data;
            
            void SyncMetadata();
        };
    };

    // Sprite
    // TODO: Implement
    
    // Display
    class Display {
    public:
        static int      Width()                           { return _pd->display->getWidth();                    }
        static int      Height()                          { return _pd->display->getHeight();                   }
        static Vector2i Size()                            { return Vector2i(Width(), Height());                 }
        static void SetInverted(bool inverted)            {        _pd->display->setInverted(inverted ? 1 : 0); }
        static void SetScaleFactor(int scaleFactor)       {        _pd->display->setScale(scaleFactor);         }
        static void SetMosaic(int x, int y)               {        _pd->display->setMosaic(x, y);               }
        static void SetRefreshRate(float refreshRate)     {        _pd->display->setRefreshRate(refreshRate);   }
    };
    
    // Audio
    class Audio {
    public:
        typedef std::function<void(int16_t *leftData, int16_t *rightData, int audioDataLength)> AudioOutputCallback;
        static void SetSpeakerCallback(AudioOutputCallback speakerCallback) {
            // Start audio callback, or stop it if microphoneCallback is empty
            // TODO: How do we remove the source? Do we pass null?
            _pd->audio->addSource(speakerCallback ? &SpeakerCallback : NULL, NULL, 1);
            
            _speakerCallback = speakerCallback;
            
            //_pd->audio->addSource(callback, context, stereo ? 1 : 0);
        }
        
        typedef std::function<void(int16_t *audioData, int audioDataLength, int channels)> AudioInputCallback;
        static void SetMicrophoneCallback(AudioInputCallback microphoneCallback) {
            // Start audio callback, or stop it if microphoneCallback is empty
            _pd->audio->setMicCallback(microphoneCallback ? &MicrophoneCallback : NULL, NULL);
            
            _microphoneCallback = microphoneCallback;
        }
        
        static void Start() { _pd->audio->start(); }
        static void Stop()  { _pd->audio->stop();  }
    
        class Sample {
        public:
            Sample(int length);
            Sample(const char *path); // TODO: Switch to std::string?
            ~Sample();
        
            // Load a new file into this sample
            void Load(const char *path); // TODO: Switch to std::string?
        
            // Length in seconds
            float Length() const { return _length; }
            
            ::AudioSample *GetNativeAudioSample() { return _sample; }
        private:
            ::AudioSample *_sample;
            float          _length;
            
            void SyncMetadata();
        };
    
        class SamplePlayer {
        public:
             SamplePlayer();
            ~SamplePlayer();
            
            void SetSample(std::shared_ptr<Sample> sample);
            
            // TODO: What is the return value of this for? Is it supposed to be a bool?
            int  Play(int loopCount = 1, float rate = 1.0f);
            bool IsPlaying();
            void Stop();
            
            void SetVolume(float volume);
        
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
    private:
        static AudioOutputCallback _speakerCallback;
        static int SpeakerCallback(void *context, int16_t *leftData, int16_t *rightData, int audioDataLength);
        
        static AudioInputCallback _microphoneCallback;
        static int MicrophoneCallback(void *context, int16_t *audioData, int audioDataLength);
    };
    
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
