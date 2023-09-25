//
//  Resize.cpp
//  Resize
//
//  Created by くま on 2017/07/16.
//  Copyright © 2017年 くま. All rights reserved.
//
#include "Resize.hpp"
#include <assert.h>
#include <string>
#include <string.h>
#include <memory>
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#define RESIZE_DEBUG 1

namespace {
    
    




    int fileExist(const char *filename)
    {
#ifdef __APPLE__
        struct stat   buffer;
        return (stat (filename, &buffer) == 0);
#endif
        return 0;
    }
    
    /*
     バッファーを示す
     */
    class Data
    {
    public:
        
        Data(const Data& data)
        {
            length_ = data.GetLength();
            bytes_  = data.GetBytes();
            is16bit_ = data.GetIs16bit();
        }
        
        Data(uint8_t* bytes, uint32_t length, bool is16bit) : bytes_(bytes), length_(length), is16bit_(is16bit)
        {
            
        }

        bool GetIs16bit() const
        {
            return is16bit_;
        }
        
        uint32_t GetLength() const
        {
            return length_;
        }
        
        uint8_t* GetBytes() const
        {
            return bytes_;
        }
        
        void Set(int index, uint16_t val)
        {
            if ( length_ > index) {
                 if (is16bit_)
                {
                    uint16_t* pos = (uint16_t*)&bytes_[index];
                    *pos = val;
                }else
                {
                    bytes_[index] = val;
                }
            }
        }
        
        uint16_t Get(int index) const
        {
            if ( length_ > index) {
                if (is16bit_)
                {
                    uint16_t* pos = (uint16_t*)&bytes_[index];
                    return *pos;
                }else
                {
                    return bytes_[index];
                }
            }
            return 0;
        }
            

        void SetBit (uint32_t nBitIndex, bool bTrue)
        {
           uint32_t nOctet = nBitIndex / 8;
           uint8_t  un8Bit	 = 7 - (nBitIndex % 8);

            if (bTrue) {
                bytes_[nOctet] |=  (1 << un8Bit);
            } else {
                bytes_[nOctet] &= ~(1 << un8Bit);
            }   
        }


        bool GetBit (uint32_t nBitIndex) const
        {
            uint32_t nOctet = nBitIndex / 8;
            uint8_t  un8Mask = 1 << (7 - (nBitIndex % 8));

           if( bytes_[nOctet] & un8Mask ){
                 return true;
            }

            return false;
        }
        
        void Free()
        {
           // delete[] bytes_;
            //bytes_ = nullptr;
        }
        
        virtual ~Data()
        {
        }
        
    private:
        uint32_t length_;
        uint8_t *bytes_;
        bool    freeOnDestruct_;
        bool    is16bit_;
    };
    
    /*
     画像を示す
     */
    class Image
    {
    public:
        Image(uint32_t width,
              uint32_t height,
              uint32_t rowBytes,
              uint32_t resolution,
              uint32_t samplesPerPixel,
              uint32_t bitsPerSample,
              Data    inData
              ) :
                width_(width),
                height_(height),
                rowBytes_(rowBytes),
                resolution_(resolution),
                samplesPerPixel_(samplesPerPixel),
                bitsPerSample_(bitsPerSample),
                data_(inData),
                freeOnDealloc_(false)
        {
        }

        
        uint32_t GetWidth() const
        {
            return width_;
        }
        
        uint32_t GetHeight() const
        {
            return height_;
        }
        
        uint32_t GetRowBytes() const
        {
            return rowBytes_;
        }
        
        uint32_t GetResolution() const
        {
            return resolution_;
        }
        
        uint32_t GetSamplesPerPixel() const
        {
            return samplesPerPixel_;
        }

        uint32_t GetBitsPerSample() const 
        {
            return bitsPerSample_;
        }
        
        uint16_t GetR(uint32_t x, uint32_t y) const
        {
            if (samplesPerPixel_ == 1) {
                return Get(x, y);
            }

            if (bitsPerSample_ == 16)
            {
                return data_.Get(y * rowBytes_ + x * samplesPerPixel_ * 2);
            }        

            return data_.Get(y * rowBytes_ + x * samplesPerPixel_);
        }
        
        uint16_t GetG(uint32_t x, uint32_t y) const
        {
            if (samplesPerPixel_ == 1) {
                return Get(x, y);
            }

            if (bitsPerSample_ == 16)
            {
                return data_.Get(y * rowBytes_ + x * samplesPerPixel_ * 2 + 1*2);
            }        
            return data_.Get(y * rowBytes_ + x * samplesPerPixel_ + 1);
        }
        
        uint16_t GetB(uint32_t x, uint32_t y) const
        {
            if (samplesPerPixel_ == 1) {
                return Get(x, y);
            }
            if (bitsPerSample_ == 16)
            {
                return data_.Get(y * rowBytes_ + x * samplesPerPixel_ * 2 + 2*2);
            }        
            return data_.Get(y * rowBytes_ + x * samplesPerPixel_ + 2);
        }
        
        uint16_t Get(uint32_t x, uint32_t y) const
        {
            if (bitsPerSample_ == 1)
            {
                return data_.GetBit(y * rowBytes_ * 8 + x);
            }
            if (bitsPerSample_ == 16)
            {
                return data_.Get(y * rowBytes_ + x * samplesPerPixel_ * 2);
            } 
            return data_.Get(y * rowBytes_ + x * samplesPerPixel_);
        }
        
        void SetR(uint32_t x, uint32_t y, uint16_t val)
        {
            if (samplesPerPixel_ == 1) {
                Set(x, y, val);
            }else{
                if (bitsPerSample_ == 16)
                {
                    data_.Set(y * rowBytes_ + x * samplesPerPixel_ * 2, val);
                }else{
                    data_.Set(y * rowBytes_ + x * samplesPerPixel_, val);
                }
            }
        }
        
        void SetG(uint32_t x, uint32_t y, uint16_t val)
        {
            if (samplesPerPixel_ == 1) {
                Set(x, y, val);
            }else{
                if (bitsPerSample_ == 16)
                {
                    data_.Set(y * rowBytes_ + x * samplesPerPixel_ * 2 + 1 * 2, val);
                }else{
                    data_.Set(y * rowBytes_ + x * samplesPerPixel_ + 1, val);
                }
            }
        }
        
        void SetB(uint32_t x, uint32_t y, uint16_t val)
        {
            if (samplesPerPixel_ == 1) {
                Set(x, y, val);
            }else{
                if (bitsPerSample_ == 16)
                {
                    data_.Set(y * rowBytes_ + x * samplesPerPixel_*2 + 2*2, val);
                }else{
                    data_.Set(y * rowBytes_ + x * samplesPerPixel_ + 2, val);
                }
            }
        }
        
        void Set(uint32_t x, uint32_t y, uint16_t val)
        {
            if (bitsPerSample_ == 1)
            {
                if (val > 1) {
                    val = true;
                }else{
                    val = false;
                }
                data_.SetBit(y * rowBytes_ * 8 + x * samplesPerPixel_, val);
            }else{
               if (bitsPerSample_ == 16)
                {
                    data_.Set(y * rowBytes_ +  x * samplesPerPixel_ * 2, val);
                }else{
                    data_.Set(y * rowBytes_ +  x * samplesPerPixel_, val);
                }
           }
        }
        
        
        const Data& inData() const
        {
            return data_;
        }
        
        /*
         画像をダンプする ( デバッグ )
         */
        void DebugDump(std::string savePath) const
        {
#ifdef __APPLE__
            CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data_.GetBytes(), data_.GetLength(), NULL);
            CGImageRef cgImage = NULL;
            
            if (samplesPerPixel_ == 3) {
                CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
                
                cgImage = CGImageCreate(width_,
                                        height_,
                                        8,
                                        samplesPerPixel_ * 8,
                                        rowBytes_,
                                        rgb,
                                        kCGBitmapByteOrderDefault,
                                        provider,
                                        nullptr,
                                        false,
                                        kCGRenderingIntentDefault);
                
                CFRelease(rgb);
                rgb = NULL;
                
            }else if (samplesPerPixel_ == 1) {
                
                CGColorSpaceRef gray = CGColorSpaceCreateDeviceGray();
                
                cgImage = CGImageCreate(width_,
                                        height_,
                                        8,
                                        samplesPerPixel_ * 8,
                                        rowBytes_,
                                        gray,
                                        kCGBitmapByteOrderDefault,
                                        provider,
                                        nullptr,
                                        false,
                                        kCGRenderingIntentDefault);
                
                CFRelease(gray);
                gray = NULL;
            }
            
            if (cgImage == NULL) {
                CFRelease(provider);
                provider = NULL;
                return;
            }
            
            CFMutableStringRef path = CFStringCreateMutable(NULL, 1000);
            CFStringRef str = CFStringCreateWithCString(NULL, savePath.c_str(), kCFStringEncodingUTF8);
            CFStringAppend(path, str);
            CFStringAppend(path, CFSTR(".png"));
            CFURLRef url = CFURLCreateWithFileSystemPath(nullptr, path, kCFURLPOSIXPathStyle, false);
            CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
            
            CFRelease(path);
            path = NULL;
            
            CFRelease(url);
            url = NULL;
            
            CFRelease(str);
            str = NULL;
            
            if (!destination) {
                
                CGImageRelease(cgImage);
                cgImage = NULL;
                
                CFRelease(provider);
                provider = NULL;
                
                return ;
            }
            
            CGImageDestinationAddImage(destination, cgImage, nil);
            
            if (!CGImageDestinationFinalize(destination)) {
                
                CGImageRelease(cgImage);
                cgImage = NULL;
                
                CFRelease(provider);
                provider = NULL;
                
                CFRelease(destination);
                destination  = NULL;
                
                return;
            }
            
            CFRelease(destination);
            
            CGImageRelease(cgImage);
            cgImage = NULL;
            
            CFRelease(provider);
            provider = NULL;
#endif
        }
        
        virtual ~Image() {
            if (freeOnDealloc_)
            {
                data_.Free();
            }
        }
        
    private:
        uint32_t width_;
        uint32_t height_;
        uint32_t rowBytes_;
        uint32_t resolution_;
        uint32_t samplesPerPixel_;
        uint32_t bitsPerSample_;
        Data     data_;
        bool     freeOnDealloc_;
    };
    
    class ResizeImpl
    {
    public:
        
        ResizeImpl(int32_t fromWidth,
               int32_t fromHeight,
               int32_t fromRowBytes,
               int32_t bitsPerSample,
               int32_t samplesPerPixel,
               int32_t toWidth,
               int32_t toHeight,
               int32_t toRowBytes,
               uint8_t *inBuffer,
               uint8_t *outBuffer) :
               inImage_(fromWidth, fromHeight, fromRowBytes, 0, samplesPerPixel, bitsPerSample, Data(inBuffer, fromRowBytes * fromHeight, bitsPerSample == 16)),
               outImage_(toWidth, toHeight, toRowBytes, 0, samplesPerPixel, bitsPerSample, Data(outBuffer, toRowBytes * toHeight, bitsPerSample == 16))
        {
            
        }
        
        virtual void Process() = 0;
       
        const Image& GetOutImage()
        {
            return outImage_;
        }
        
        const Image& GetInImage()
        {
            return inImage_;
        }
        
    protected:
        Image inImage_;
        Image outImage_;
    };

    class Bicubic : public ResizeImpl
    {
        public:
       
        static double Weight(double d)
        {
            double a = -1.0f;
            
            if (d <= 1.0)
            {
                return ((a + 2.0) * d * d * d) - ((a + 3.0) * d * d) + 1;
            }
            
            if (d <= 2.0)
            {
                return (a * d * d * d) - (5.0 * a * d * d) + (8.0 * a * d) - (4.0 * a);
            }
            
            return 0.0;
        }
        
        Bicubic(int32_t fromWidth,
                int32_t fromHeight,
                int32_t fromRowBytes,
                int32_t bitsPerSample,
                int32_t samplesPerPixel,
                int32_t toWidth,
                int32_t toHeight,
                int32_t toRowBytes,
                uint8_t *inBuffer,
                uint8_t *outBuffer) :
                ResizeImpl(fromWidth, fromHeight, fromRowBytes, bitsPerSample, samplesPerPixel, toWidth, toHeight, toRowBytes, inBuffer, outBuffer)
        {
            
        }
        
        void Process() override
        {

            double scaleX = (double)inImage_.GetWidth() / (double)outImage_.GetWidth();
            double scaleY = (double)inImage_.GetHeight() / (double)outImage_.GetHeight();
            
            for (int32_t y = 0; y < outImage_.GetHeight(); y++)
            {
                for (int32_t x = 0; x  < outImage_.GetWidth(); x++)
                {
                   
                    int32_t toX = scaleX * x + 0.4999f;
                    int32_t toY = scaleY * y + 0.4999f;
                    
                    double  r = 0;
                    double  g = 0;
                    double  b = 0;
                    
                    for (int32_t areaY = toY - 1; areaY <= toY + 2; areaY++)
                    {
                        for (int32_t areaX = toX - 1; areaX <= toX + 2; areaX++)
                        {
                            if (areaX < 0 || areaY < 0)
                            {
                                continue;
                            }
                            
                            double w = Weight(abs(toX - areaX)) * Weight(abs(toY - areaY));
                            if (w == 0) continue;
                            
                            int32_t posY = 0;
                            if (areaY < inImage_.GetHeight())
                            {
                                posY = areaY;
                            }else{
                                posY = inImage_.GetHeight() - 1;
                            }
                           
                            int32_t posX = 0;
                            if (areaX < inImage_.GetWidth())
                            {
                                posX = areaX;
                            }else{
                                posX = inImage_.GetWidth() - 1;
                            }
                            
                            if (inImage_.GetSamplesPerPixel() == 1)
                            {
                                g += (inImage_.Get(posX, posY) * w);
                            }
                           
                            if (inImage_.GetSamplesPerPixel() == 3)
                            {
                                r += (inImage_.GetR(posX, posY) * w);
                                g += (inImage_.GetG(posX, posY) * w);
                                b += (inImage_.GetB(posX, posY) * w);
                            }
                        }
                    }
                    
                    if (inImage_.GetBitsPerSample() == 16)
                    {
                        if(r > 65535) r = 65535;
                        if(g > 65535) g = 65535;
                        if(b > 65535) b = 65535;
                    }else{
                        if(r > 255) r = 255;
                        if(g > 255) g = 255;
                        if(b > 255) b = 255;
                    }

                    if (inImage_.GetSamplesPerPixel() == 1)
                    {
                        outImage_.Set(x, y, g);
                    }
                    
                    if (inImage_.GetSamplesPerPixel() == 3)
                    {
                        outImage_.SetR(x, y, r);
                        outImage_.SetG(x, y, g);
                        outImage_.SetB(x, y, b);
                    }
                    
                }
            }
        }
        
    };
    
    class NearestNeighbor : public ResizeImpl
    {
        public:
 
        NearestNeighbor(int32_t fromWidth,
                        int32_t fromHeight,
                        int32_t fromRowBytes,
                        int32_t bitsPerSample,
                        int32_t samplesPerPixel,
                        int32_t toWidth,
                        int32_t toHeight,
                        int32_t toRowBytes,
                        uint8_t *inBuffer,
                        uint8_t *outBuffer) :
                        ResizeImpl(fromWidth, fromHeight, fromRowBytes, bitsPerSample, samplesPerPixel, toWidth, toHeight, toRowBytes, inBuffer, outBuffer)
        {

        
        }
        
        void Process() override
        {
            double scaleX = (double)inImage_.GetWidth() / (double)outImage_.GetWidth();
            double scaleY = (double)inImage_.GetHeight() / (double)outImage_.GetHeight();
            
            for (int32_t i = 0; i < outImage_.GetHeight(); i++)
            {
                int32_t fromPixelY = i * scaleY + 0.4999f;
                
                if (fromPixelY >= inImage_.GetHeight())
                {
                    fromPixelY = inImage_.GetHeight() - 1;
                }
                
                if (fromPixelY < 0)
                {
                    fromPixelY = 0;
                }
                
                for (int32_t j = 0; j < outImage_.GetWidth(); j++)
                {
                    int32_t fromPixelX = j * scaleX + 0.4999f;
                    if (fromPixelX >= inImage_.GetWidth())
                    {
                        fromPixelX = inImage_.GetWidth() - 1;
                    }
                    
                    if (fromPixelX < 0)
                    {
                        fromPixelX = 0;
                    }
                    
                    if (outImage_.GetSamplesPerPixel() == 1)
                    {
                        outImage_.Set(j, i , inImage_.Get(fromPixelX , fromPixelY));
                    }else
                    {
                        outImage_.SetR(j, i , inImage_.GetR(fromPixelX , fromPixelY));
                        outImage_.SetG(j, i , inImage_.GetG(fromPixelX , fromPixelY));
                        outImage_.SetB(j, i , inImage_.GetB(fromPixelX , fromPixelY));
                    }
                }
            }
        }
    };
}

namespace epsonscan
{
        void Resize::ProcessResize(int32_t fromWidth,
                               int32_t fromHeight,
                               int32_t fromRowBytes,
                              int32_t bitsPerSample,
                              int32_t samplesPerPixel,
                              int32_t toWidth,
                              int32_t toHeight,
                              int32_t toRowBytes,
                              uint8_t *inBuffer,
                              uint8_t *outBuffer
                             )
    {
        
        if (bitsPerSample == 8 || bitsPerSample == 16)
        {
            Bicubic cubic(fromWidth,
                          fromHeight,
                          fromRowBytes,
                          bitsPerSample,
                          samplesPerPixel,
                          toWidth,
                          toHeight,
                          toRowBytes,
                          inBuffer,
                          outBuffer);
            
            cubic.Process();

#if RESIZE_DEBUG
            
            const Image& outImage = cubic.GetOutImage();
            outImage.DebugDump("/tmp/result");
#endif

            
        }
        else if (bitsPerSample == 1)
        {
            NearestNeighbor neighbor(fromWidth,
                                     fromHeight,
                                     fromRowBytes,
                                     bitsPerSample,
                                     samplesPerPixel,
                                     toWidth,
                                     toHeight,
                                     toRowBytes,
                                     inBuffer,
                                     outBuffer);
            neighbor.Process();
        }
        else
        {
            // paramerter error
            assert(false);
        }
    }
}
