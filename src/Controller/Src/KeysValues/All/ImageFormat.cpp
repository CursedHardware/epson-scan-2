#include "ImageFormat.hpp"
#include "Scanner.hpp"
#include <ES2Command/ES2CommandAPI.h>

namespace epsonscan {

void ImageFormat::SetValue(const SDIInt& intVal)
{
	current_ = intVal;
}	
void ImageFormat::Reset()
{
	current_  = kSDIImageFormatColorJpegMonoPNM;
}

void ImageFormat::GetValue(SDIInt& intVal)
{
	intVal = current_;
}

void ImageFormat::GetCapability(SDICapability &capability)
{
	SetDefaultListCapability(capability);
    capability.supportLevel = kSDISupportLevelAvailable;

	AddList(capability, kSDIImageFormatRaw);
	AddAllList(capability, kSDIImageFormatRaw);

	AddList(capability, kSDIImageFormatPNM);
	AddAllList(capability, kSDIImageFormatPNM);

	AddList(capability, kSDIImageFormatJPEG);
	AddAllList(capability, kSDIImageFormatJPEG);

	AddList(capability, kSDIImageFormatPNG);
	AddAllList(capability, kSDIImageFormatPNG);

	AddList(capability, kSDIImageFormatTIFF);
	AddAllList(capability, kSDIImageFormatTIFF);

	AddList(capability, kSDIImageFormatColorJpegMonoPNM);
	AddAllList(capability, kSDIImageFormatColorJpegMonoPNM);
	
}
	
ESDictionary ImageFormat::GetEngineKeyValues()
{
	auto dict =  ESDictionary();
	return dict;
}

	
}//namespace epsonscan
