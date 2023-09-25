#pragma once
#include <stdio.h>
#include <stdint.h>

typedef struct 
{   
    int32_t width;
    int32_t height;
    int32_t samplesPerPixel;
    int32_t bitsPerSample;
    uint8_t* imageData;
    uint32_t resolution;
    int32_t bytesPerRow;
}OfdImage;

#define OFD_OPTION_MAX_PATH 256

typedef struct 
{
    int8_t targetFile[OFD_OPTION_MAX_PATH + 1];
    int8_t tempPDFFile[OFD_OPTION_MAX_PATH + 1];
}OfdOption;


typedef enum : int32_t
{
    kOfdErrorNone = 0,
    kOfdErrorGeneric = 1
} OfdError;

class IOfdPlugin
{
    public:

    virtual OfdError Initialize(OfdOption* option) = 0;
    virtual OfdError Add( OfdImage* image) = 0;
    virtual OfdError Fin() = 0;
};

#if __cplusplus
extern "C" {
#endif

OfdError OFDPluginCreate(IOfdPlugin** plugin);
OfdError OFDPluginFree(IOfdPlugin* plugin);


typedef OfdError (*OFDPluginCreatePtr)(IOfdPlugin** plugin);
typedef OfdError (*OFDPluginFreePtr)(IOfdPlugin* plugin);


#ifdef __cplusplus
}
#endif