#pragma once
#include <EpsonScan2.h>
#include <stdint.h>
#include <stdio.h>
#include <Platform.h>
#include <ES2Command/ES2CommandAPI.h>
#include <thread>
#include "KeyMgr.hpp"
#include "Utility.hpp"
#include "ESImageInfo.h"
#include "ModelInfo.hpp"

namespace epsonscan {

	enum SDIImageDataType {
		kSDIImageDataTypeNone,
		kSDIImageDataTypePipe,
		kSDIImageDataTypeDummy,
		kSDIImageDataTypeFile,
		kSDIImageDataTypeBlank,
	};

	enum SDIImageDataSourceRepresentation {
		kSDIImageDataSourceRepresentationNone,
		kSDIImageDataSourceRepresentationRaw,
		kSDIImageDataSourceRepresentationJpeg,
		kSDIImageDataSourceRepresentationTiff,
		kSDIImageDataSourceRepresentationTiffG4,
		kSDIImageDataSourceRepresentationTiffJpeg,
		kSDIImageDataSourceRepresentationTiffLZW,
		kSDIImageDataSourceRepresentationPNG,
		kSDIImageDataSourceRepresentationPDF,
		kSDIImageDataSourceRepresentationBMP,
		kSDIImageDataSourceRepresentationPNM
	}; 

	class Image
	{
public :

	Image(IESScannedImage* imageHandle) : refcount_(1), isBlankPage_(false)
	{
		if (imageHandle) {
			imageInfo_[ES_IMAGE_INFO::kESImageInfoWidthKey]			 = (ESNumber)imageHandle->GetWidth();
			imageInfo_[ES_IMAGE_INFO::kESImageInfoHeightKey]		  	 = (ESNumber)imageHandle->GetHeight();;
			imageInfo_[ES_IMAGE_INFO::kESImageInfoSamplesPerPixelKey] = (ESNumber)imageHandle->GetSamplesPerPixel();
			imageInfo_[ES_IMAGE_INFO::kESImageInfoBitsPerSampleKey]	 = (ESNumber)imageHandle->GetBitsPerSample();
			imageInfo_[ES_IMAGE_INFO::kESImageInfoPageCountKey]		 = (ESNumber)imageHandle->GetSerialNumber();
			imageInfo_[ES_IMAGE_INFO::kESImageInfoPaperCountKey]      = (ESNumber)imageHandle->GetPaperSerialNumber();
			imageInfo_[ES_IMAGE_INFO::kESImageInfoAutoCroppedKey]    =  (ESNumber)imageHandle->IsAutoCropped();
			imageInfo_[ES_IMAGE_INFO::kESImageInfoUpsideDownKey]     = (ESNumber)imageHandle->IsUpsideDown();
			imageInfo_[ES_IMAGE_INFO::kESImageInfoBlankPageKey]		= (ESNumber)imageHandle->IsAutoDetectBlankPage();
			imageInfo_[ES_IMAGE_INFO::kESImageInfoMonoPageKey]		= (ESNumber)imageHandle->IsAutoDetectMonoPage();

			if (imageHandle->GetSurfaceType() == kESSurfaceTypeBackSide)
			{
				imageInfo_[ES_IMAGE_INFO::kESImageInfoSurfaceTypeKey] = (ESNumber)ES_IMAGE_INFO::kESImageInfoSurfaceTypeBackSide;
			}else{
				imageInfo_[ES_IMAGE_INFO::kESImageInfoSurfaceTypeKey] = (ESNumber)ES_IMAGE_INFO::kESImageInfoSurfaceTypeFrontSide;
			}
			
			if (imageHandle->GetImageFormat() == kESImageFormatJPEG)
			{
				imageInfo_[ES_IMAGE_INFO::kESImageInfoDataTypeKey] = (ESNumber)ES_IMAGE_INFO::kESImageDataTypeJpeg;
			}else{
				imageInfo_[ES_IMAGE_INFO::kESImageInfoDataTypeKey] =(ESNumber)ES_IMAGE_INFO::kESImageDataTypeRaw;
			}

			if (imageHandle->GetDocumentType() == kESDocumentTypeCarrierSheet)
			{
				imageInfo_[ES_IMAGE_INFO::kESImageInfoCarrierSheetTypeKey] = (ESNumber)ES_IMAGE_INFO::kESImageInfoCarrierSheetTypeON;
			}else{
				imageInfo_[ES_IMAGE_INFO::kESImageInfoCarrierSheetTypeKey] = (ESNumber)ES_IMAGE_INFO::kESImageInfoCarrierSheetTypeOFF;
			}

			imageHandle->ReadDataToEnd(dataBuf_);
		}
	}
	
	virtual ~Image()
	{	
		try{
			SDI_TRACE_LOG("Enter Destroy Image");

			//WaitUntilFinishImageProc();

			SDI_TRACE_LOG("Leave");
		}catch(...)
		{
			
		}
	}

	static CritSec m_ThreadCriticSec;

	void AddThreadCount()
	{
		CBlockCriticalSection cValueBlock(m_ThreadCriticSec);
		threadCount_++;
	}

	void SubThreadCount()
	{
		CBlockCriticalSection cValueBlock(m_ThreadCriticSec);
		threadCount_ --;
	}

	void WaitUntilTheradLimit(int resolution)
	{
		int maxThreadCount = 5;
		if (resolution >= 600) {
			maxThreadCount = 1;
		}

		while(1)
		{
			CBlockCriticalSection cValueBlock(m_ThreadCriticSec);
			if(threadCount_ >= maxThreadCount)
			{
				usleep(100);
			}else{
				break;
			}
		}
	}

	static int threadCount_;

	//----------------------------------------=----------------------------------------┐
	/*!@brief	インターフェースの参照カウンタをインクリメント
	@return			参照カウンタの現在値を返す。
	*/
	virtual uint32_t AddReference() 
	{
		refcount_ ++;
		return refcount_;
	}


	//----------------------------------------=----------------------------------------┐
	/*!@brief	インターフェースの参照カウンタの現在値の取得
	@return			参照カウンタの現在値を返す。
	*/
	virtual uint32_t GetReferenceCount() 
	{
		return refcount_;	
	}

	//----------------------------------------=----------------------------------------┐
	/*!@brief	インターフェースの参照カウンタをデクリメント
	@details		AddReferenceとReleaseは対で呼び出す。
					Releaseにより参照カウンタが0となった場合、オブジェクトを削除する。
					ISDIScannerDriver::NextImageData呼び出しによりインターフェースを取得
					した場合、参照カウンタをインクリメントする。
					そのため、ISDIScannerDriver::NextImageData呼び出し後はReleaseを呼び出す
					必要がある。
	*/
	virtual void	Release() 
	{
		refcount_ --;
		if (refcount_ == 0)
		{
			delete this;
		}
	}


	//----------------------------------------=----------------------------------------┐
	/*!@brief	型情報の取得
	*/
	virtual SDIImageDataType	GetType() const
	{
		return kSDIImageDataTypeFile;
	}

	virtual int32_t		GetWidth()const 
	{
		return  ES_IMAGE_INFO::GetESImageWidth(imageInfo_);
	}

	virtual int32_t		GetHeight()const 
	{
		return  ES_IMAGE_INFO::GetESImageHeight(imageInfo_);
	}

	virtual int32_t		GetBitsPerSample()const 
	{
		return  ES_IMAGE_INFO::GetESImageBitsPerSample(imageInfo_);
	}
	virtual int32_t		GetSamplesPerPixel()const 
	{
		return  ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo_);
	}

	virtual int32_t		GetResolution()const 
	{
		return  ES_IMAGE_INFO::GetESImageOutputResolution(imageInfo_);
	}

	virtual int32_t		GetBytesPerRow() const 
	{	
		return  ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo_);
	}

	virtual bool IsBlankPage() const
	{
		return isBlankPage_;
	}

	virtual std::string GetPath() const 
	{
		return filePath_;
	}

	virtual SDIImageDataSourceRepresentation GetSourceRepresentation()const {
		SDIImageDataSourceRepresentation	eSourceRepresentation = kSDIImageDataSourceRepresentationNone;
		return eSourceRepresentation;
	}

	virtual SDIError applyImageProc(std::shared_ptr<KeyMgr> keyMgr, std::shared_ptr<ModelInfo> modelInfo);

	virtual void WaitUntilFinishImageProc()
	{
		if (imageProcThread_)
		{
			SDI_TRACE_LOG("join Start");
			if (imageProcThread_->joinable())
			{
				imageProcThread_->join();
			}
			SDI_TRACE_LOG("join End");
		}
	}

	virtual SDIError GetLastError()
	{
		return lastError_;
	}

	bool IsHardwareImageProcessSupported(std::shared_ptr<KeyMgr> keyMgr);

protected:
		ESImageInfo imageInfo_;
		std::string filePath_;
		ES_CMN_FUNCS::BUFFER::CESHeapBuffer dataBuf_;
		int32_t refcount_;
		std::shared_ptr<std::thread> imageProcThread_;

		bool isBlankPage_;
		SDIError lastError_ = kSDIErrorNone;
	};
}
