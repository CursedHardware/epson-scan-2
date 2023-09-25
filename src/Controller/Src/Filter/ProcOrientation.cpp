#include "ProcOrientation.hpp"


namespace epsonscan
{

static void SetBit (UINT8* un8Bytes, UINT32 nBitIndex, BOOL bTrue)
{
    UINT32 nOctet = nBitIndex / 8;
    UINT8  un8Bit	 = 7 - (nBitIndex % 8);

    if (bTrue) {
        un8Bytes[nOctet] |=  (1 << un8Bit);
    } else {
        un8Bytes[nOctet] &= ~(1 << un8Bit);
    }
}

static BOOL GetBit (UINT8* un8Bytes, UINT32 nBitIndex)
{
    UINT32 nOctet = nBitIndex / 8;
    UINT8  un8Mask   = 1 << (7 - (nBitIndex % 8));

    if( un8Bytes[nOctet] & un8Mask ){
        return TRUE;
    }

    return FALSE;
}

static void SwapPixel1(uint32_t  x1,
                       uint32_t  y1,
                       uint32_t  x2,
                       uint32_t  y2,
                       uint8_t*  bytes,
                       uint8_t   bitsPerSample,
                       uint32_t  samplesPerPixel,
                       uint32_t  bytesPerRow)
{
    uint32_t pixelBits =  bitsPerSample * samplesPerPixel;
    uint32_t widthBits =  bytesPerRow * 8;

    BOOL temp = GetBit(bytes, widthBits * y1 + x1 * pixelBits);
    {
        BOOL right =  GetBit(bytes, widthBits * y2 + x2 * pixelBits);
        SetBit(bytes, widthBits * y1 + x1 * pixelBits, right);
    }
    SetBit(bytes, widthBits * y2 + x2 * pixelBits, temp);
}

static void SwapPixel8(uint32_t  x1,
                       uint32_t  y1,
                       uint32_t  x2,
                       uint32_t  y2,
                       uint8_t*  bytes,
                       uint8_t   bitsPerSample,
                       uint32_t  samplesPerPixel,
                       uint32_t  bytesPerRow)
{
    uint32_t pixelBytes =  samplesPerPixel * bitsPerSample / 8;

    for (uint8_t i = 0; i < pixelBytes; i++) {
        uint8_t temp = bytes[y1 * bytesPerRow +  (pixelBytes *  x1 + i)];
        bytes[y1 * bytesPerRow + (pixelBytes * x1 + i)] =  bytes[y2 * bytesPerRow +  (pixelBytes * x2 + i)];
        bytes[y2 * bytesPerRow + (pixelBytes * x2 + i)] =  temp;
    }
}


static void SwapPixel(uint32_t  x1,
                      uint32_t  y1,
                      uint32_t  x2,
                      uint32_t  y2,
                      uint8_t*  bytes,
                      uint8_t   bitsPerSample,
                      uint32_t  samplesPerPixel,
                      uint32_t  bytesPerRow)
{
    if (bitsPerSample == 1) {
        SwapPixel1(x1, y1, x2, y2, bytes, bitsPerSample, samplesPerPixel, bytesPerRow);
    }else if(bitsPerSample == 8 || bitsPerSample == 16){
        SwapPixel8(x1, y1, x2, y2, bytes, bitsPerSample, samplesPerPixel, bytesPerRow);
    }else{
        assert(false);
    }
}


static void CopyPixel1(UINT32  nX1,
                       UINT32  nY1,
                       UINT32  nX2,
                       UINT32  nY2,
                       UINT8*  pun8InBytes,
                       UINT8*  pun8OutBytes,
                       UINT8   un8BitsPerSample,
                       UINT32  nSamplesPerPixel,
                       UINT32  nInBytesPerRow,
                       UINT32  nOutBytesPerRow)
{
    UINT32 nPixelBits    =  un8BitsPerSample * nSamplesPerPixel;
    UINT32 nInWidthBits  =  nInBytesPerRow * 8;
    UINT32 nOutWidthBits =  nOutBytesPerRow * 8;

    BOOL bTemp = GetBit(pun8InBytes, nInWidthBits * nY2 + nX2 * nPixelBits);
    SetBit(pun8OutBytes, nOutWidthBits * nY1 + nX1 * nPixelBits, bTemp);
}

static void CopyPixel8(UINT32  nX1,
                       UINT32  nY1,
                       UINT32  nX2,
                       UINT32  nY2,
                       UINT8*  pun8InBytes,
                       UINT8*  pun8OutBytes,
                       UINT8   un8BitsPerSample,
                       UINT32  nSamplesPerPixel,
                       UINT32  nInBytesPerRow,
                       UINT32  nOutBytesPerRow)
{
    uint32_t nPixelBytes =  nSamplesPerPixel * un8BitsPerSample / 8;

    for (uint8_t i = 0; i < nPixelBytes; i++) {
       pun8OutBytes[nY1 * nOutBytesPerRow + (nPixelBytes * nX1 + i)] =  pun8InBytes[nY2 * nInBytesPerRow +  (nPixelBytes * nX2 + i)];
    }
}


static void CopyPixel(UINT32  nX1,
                      UINT32  nY1,
                      UINT32  nX2,
                      UINT32  nY2,
                      UINT8*  pun8InBytes,
                      UINT8*  pun8OutBytes,
                      UINT8   un8BitsPerSample,
                      UINT32  nSamplesPerPixel,
                      UINT32  nInBytesPerRow,
                      UINT32  nOutBytesPerRow)
{
    if (un8BitsPerSample == 1) {
        CopyPixel1(nX1, nY1, nX2, nY2, pun8InBytes, pun8OutBytes, un8BitsPerSample, nSamplesPerPixel, nInBytesPerRow, nOutBytesPerRow);
    }else if(un8BitsPerSample == 8 || un8BitsPerSample == 16){
        CopyPixel8(nX1, nY1, nX2, nY2, pun8InBytes, pun8OutBytes, un8BitsPerSample, nSamplesPerPixel, nInBytesPerRow, nOutBytesPerRow);
    }else{
        assert(FALSE);
    }
}


//static
void  ProcOrientation::RotateImageBy90OnMem(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData,
											  				 const ESImageInfo& dictImageInfo,
										     ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cInData,
									  							  	         BOOL bClockwise,
													 	               SDIError& eOutError)
{

    SDIError eError = kSDIErrorNone;
	ES_ErrorBailWithAction((cInData.GetBufferPtr() != NULL ), BAIL, NULL, eError = kSDIErrorNoMemory, ES_STRING("param error"));
  {
  	ESNumber nHeight		  = ES_IMAGE_INFO::GetESImageHeight(dictImageInfo);
  	ESNumber nWidth			  = ES_IMAGE_INFO::GetESImageWidth(dictImageInfo);
  	ESNumber nBitsPerSample   = ES_IMAGE_INFO::GetESImageBitsPerSample(dictImageInfo);
  	ESNumber nSamplesPerPixel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(dictImageInfo);
  	ESNumber nInBytesPerRow   = ES_IMAGE_INFO::GetESImageBytesPerRow(dictImageInfo);
  	ESNumber nOutBytesPerRow = (ES_IMAGE_INFO::GetESImageHeight(dictImageInfo) * ES_IMAGE_INFO::GetESImageBitsPerPixel(dictImageInfo) + 7) / 8;

  	ES_ErrorBailWithAction(cDestData.AllocBuffer(nOutBytesPerRow * nWidth),BAIL,ProcOrientation,eError = kSDIErrorNoMemory,ES_STRING("allocation fail"));

  	UInt8*  pcInBytes        = cInData.GetBufferPtr();
  	UInt8*  pcOutBytes       = cDestData.GetBufferPtr();

      for (ESNumber i = 0 ; i < nHeight; i++) {
          for (ESNumber j = 0 ; j < nWidth; j++) {

              if (bClockwise) {
                  CopyPixel(nHeight - i - 1, j, j, i,  pcInBytes, pcOutBytes, nBitsPerSample, nSamplesPerPixel, nInBytesPerRow, nOutBytesPerRow);
              }else{
                  CopyPixel(i, nWidth - j - 1, j, i, pcInBytes, pcOutBytes, nBitsPerSample, nSamplesPerPixel, nInBytesPerRow, nOutBytesPerRow);
              }
          }
      }
  }

BAIL:;

}

//static
void   ProcOrientation::RotateImageB1800OnMem(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData,
											   const ESImageInfo& dictImageInfo,
										       ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cInData,
									  	       SDIError& eOutError)
{
	SDIError eError = kSDIErrorNone;

	ES_ErrorBailWithAction((cInData.GetBufferPtr() != NULL ), BAIL, NULL, eError = kSDIErrorNoMemory, ES_STRING("param error"));
  {
  	cDestData.Attach(cInData);

  	UInt8*	  inBytes		 = cDestData.GetBufferPtr();
    uint32_t  height         = ES_IMAGE_INFO::GetESImageHeight(dictImageInfo);
    uint32_t  width          = ES_IMAGE_INFO::GetESImageWidth(dictImageInfo);
    uint32_t  bitsPerSample  = ES_IMAGE_INFO::GetESImageBitsPerSample(dictImageInfo);
    uint32_t  samplesPerPxel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(dictImageInfo);
    uint32_t  bytesPerRow    = ES_IMAGE_INFO::GetESImageBytesPerRow(dictImageInfo);
    ESNumber  half           = (height / 2) - 1;

  	if(half <0){
  		half =0;
  	}

      /* 長さが2で割り切れない場合は真ん中部分を先行して反転 */
      if((height % 2) == 1) {
          uint32_t ymid = ( (height - 1 ) / 2 );
          for(uint32_t x = 0;x < (width / 2); x++) {
              SwapPixel(x, ymid, width - x - 1, ymid, inBytes, bitsPerSample, samplesPerPxel, bytesPerRow);
          }
      }

  	if(height !=0){
  		/* 残りの部分を回転 */
  		for(uint32_t x = 0; x < width; x++) {
  			for(ESNumber y = 0;y <= half; y++) {
  				SwapPixel(x, y, width - x - 1, height - y -1, inBytes, bitsPerSample, samplesPerPxel, bytesPerRow);
  			}
  		}
  	}
  }

BAIL:;


}


//static
void  ProcOrientation::RotateImageOnMem(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData,
			  											   	    EIPRotateType eRotateType,
				   						  			      const ESImageInfo& dictImageInfo,
											    ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cInData,
										  					    SDIError& eOutError)
{
	SDIError eError = kSDIErrorNone;

	ES_ErrorBailWithAction((cInData.GetBufferPtr() != NULL ), BAIL, NULL, eError = kSDIErrorNoMemory, ES_STRING("param error"));


	switch(eRotateType) {
		case kEIPRotateType90:
			RotateImageBy90OnMem(cDestData, dictImageInfo, cInData, TRUE , eError);
		break;

		case kEIPRotateType270:
			RotateImageBy90OnMem(cDestData, dictImageInfo, cInData, FALSE , eError);
		break;

		case kEIPRotateType180:
			RotateImageB1800OnMem(cDestData, dictImageInfo, cInData, eError);
		break;
	}

	ES_ErrorBail((cDestData.GetLength() != 0), BAIL, NULL, ES_STRING("destination is null"));

BAIL:;

	if (eError != kSDIErrorNone)
	{
		cDestData.FreeBuffer();
	}


}

}//epsonscan