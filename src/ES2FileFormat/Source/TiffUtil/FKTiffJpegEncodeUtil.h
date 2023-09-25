//
//  FKTiffEncodeUtil.h
//  ESImageFormatKit
//
//  Created by Yumoto Atsushi on 2013/12/05.
//  Copyright (c) 2013年 EPSON. All rights reserved.
//
#pragma once
#include "FKDestination.h"
#include "tiffio.h"
#include "FKTiffEncodeUtil.h"
/**
  TIFF圧縮のための基底クラス
*/

class FKTiffJpegEncodeUtil : public FKTiffEncodeUtil
{
public:
	FKTiffJpegEncodeUtil();
	virtual ~FKTiffJpegEncodeUtil();
	virtual BOOL WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cData, ENUM_FK_ERROR_CODE& eError) override;
protected:
	virtual uint32_t GetRowsPerStrip(uint32_t compression) override;
	virtual uint32_t GetRgbColorSpaceForCompression(uint32_t compression) override;
	virtual BOOL StartWriting() override;
	virtual BOOL FinalizeWriting() override;
private:

	std::vector<uint8_t> m_pageBuffer;

};
