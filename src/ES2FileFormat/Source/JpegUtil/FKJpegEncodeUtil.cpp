//
//  FKJpegEncodeUtil.m
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 14/12/19.
//  Copyright (c) 2014 EPSON. All rights reserved.
//
#include "stdafx.h"

#include "FileFormatKitErrors.h"
#include "FKJpegEncodeUtil.h"
#include "jpegUserDefines.h"
#include "ESImageInfo.h"
#include "ESFile.h"

static const INT64 kBufferSize         = 4096;

/* Expanded data destination object for memory output */
typedef struct {
    struct jpeg_destination_mgr pub; /* public fields */

    ESImageInfo*    imageInfo;
    CFKDestination*  destination;
    UINT8*        buffer;
} my_mem_destination_mgr;


static void init_destination_callback(j_compress_ptr cinfo)
{
}

static void notifyDidReceiveImageData(my_mem_destination_mgr *dst,INT64 size)
{
    if (size > 0) {
		ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBuffer;
		cBuffer.CopyBuffer((UINT8*)dst->buffer, (ESNumber)size );

		if ( dst->destination->GetFKDestinationType() == kFKDestinationTypeData)
		{


			(dst->destination->GetSource()).AppendBuffer(cBuffer.GetBufferPtr(),cBuffer.GetLength());

		}else if(dst->destination->GetFKDestinationType() == kFKDestinationTypeEmpty) {

			ESString strImageInfoAsJson;

			ES_IMAGE_INFO::GetImageInfoAsJson(strImageInfoAsJson,*(dst->imageInfo));

			dst->destination->NotifyReceiveImageData( cBuffer, (ES_CHAR_CPTR)strImageInfoAsJson.c_str() );

        }
    }
}

static boolean empty_output_buffer_callback(j_compress_ptr cinfo)
{
    my_mem_destination_mgr *dst = (my_mem_destination_mgr*)cinfo->dest;

    dst->pub.next_output_byte   = (UINT8 *)dst->buffer;
    dst->pub.free_in_buffer     = kBufferSize;

	if ( dst->destination->GetFKDestinationType() == kFKDestinationTypeData ||
		 dst->destination->GetFKDestinationType() == kFKDestinationTypeEmpty ) {
        notifyDidReceiveImageData(dst, kBufferSize);
    }

    return TRUE;
}

static void term_mem_destination_callback(j_compress_ptr cinfo)
{
	ESString strImageInfoAsJson;

  my_mem_destination_mgr *dst = (my_mem_destination_mgr *)cinfo->dest;
  INT64 size = kBufferSize - dst->pub.free_in_buffer;
  notifyDidReceiveImageData(dst, size);

	ES_IMAGE_INFO::GetImageInfoAsJson(strImageInfoAsJson,*(dst->imageInfo));

	dst->destination->NotifyDidEndReceivingForImageInfo( (ES_CHAR_CPTR)strImageInfoAsJson.c_str() );


}


static void jpeg_mem_dest (j_compress_ptr cinfo, CFKDestination* destination, ESImageInfo* imageInfo)
{
    my_mem_destination_mgr *dst = (my_mem_destination_mgr *)cinfo->dest;
	ESString strImageInfoAsJson;

    if (dst == NULL) {
        dst                       = (my_mem_destination_mgr*)(*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(my_mem_destination_mgr));
        dst->buffer               = (UINT8*)cinfo->mem->alloc_small((j_common_ptr) cinfo, JPOOL_IMAGE, kBufferSize);
        dst->pub.next_output_byte = (UINT8 *)dst->buffer;
        dst->pub.free_in_buffer   = kBufferSize;
        dst->destination          = destination;
        dst->imageInfo            = imageInfo;
        cinfo->dest               = (struct jpeg_destination_mgr *)dst;
    }

    dst->pub.init_destination    = init_destination_callback;
    dst->pub.empty_output_buffer = empty_output_buffer_callback;
    dst->pub.term_destination    = term_mem_destination_callback;

	ES_IMAGE_INFO::GetImageInfoAsJson(strImageInfoAsJson,*imageInfo);

	destination->NotifyDidBeginReceivingForImageInfo( (ES_CHAR_CPTR)strImageInfoAsJson.c_str() );
}

/*
 * This routine writes the given ICC profile data into a JPEG file.
 * It *must* be called AFTER calling jpeg_start_compress() and BEFORE
 * the first call to jpeg_write_scanlines().
 * (This ordering ensures that the APP2 marker(s) will appear after the
 * SOI and JFIF or Adobe markers, but before all else.)
 */
#define ICC_MARKER					(JPEG_APP0 + 2)                             /* JPEG marker code for ICC			 */
#define ICC_OVERHEAD_LEN			14											/* size of non-profile data in APP2  */
#define MAX_BYTES_IN_MARKER			65533										/* maximum data len of a JPEG marker */
#define MAX_DATA_BYTES_IN_MARKER	(MAX_BYTES_IN_MARKER - ICC_OVERHEAD_LEN)

static BOOL writeICCProfile (j_compress_ptr cinfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer* iccData)
{
	unsigned int	num_markers = 0;		/* total number of markers we'll write */
	int				cur_marker = 1;		/* per spec, counting starts at 1 */
	unsigned int	length = 0;				/* number of bytes to write in this marker */


    //const char  *icc_data_ptr = [iccData bytes];
	const char  *icc_data_ptr = (char*)iccData->GetBufferPtr();
	unsigned int icc_data_len = (unsigned int)iccData->GetLength();

	/* Calculate the number of markers we'll need, rounding up of course */
	num_markers		= icc_data_len /  MAX_DATA_BYTES_IN_MARKER;

	if (num_markers * MAX_DATA_BYTES_IN_MARKER != icc_data_len)
	{
		num_markers ++;
	}

	while (icc_data_len > 0)
	{
		/* length of profile to put in this marker */
		length = icc_data_len;
		if (length > MAX_DATA_BYTES_IN_MARKER)
		{
			length = MAX_DATA_BYTES_IN_MARKER;
		}
		icc_data_len -= length;

		/* Write the JPEG marker header (APP2 code and marker length) */
		jpeg_write_m_header(cinfo,
							ICC_MARKER,
							(unsigned int) (length + ICC_OVERHEAD_LEN));

		/* Write the marker identifying string "ICC_PROFILE" (null-terminated).
		 * We code it in this less-than-transparent way so that the code works
		 * even if the local character set is not ASCII.
		 */
		jpeg_write_m_byte(cinfo, 0x49);
		jpeg_write_m_byte(cinfo, 0x43);
		jpeg_write_m_byte(cinfo, 0x43);
		jpeg_write_m_byte(cinfo, 0x5F);
		jpeg_write_m_byte(cinfo, 0x50);
		jpeg_write_m_byte(cinfo, 0x52);
		jpeg_write_m_byte(cinfo, 0x4F);
		jpeg_write_m_byte(cinfo, 0x46);
		jpeg_write_m_byte(cinfo, 0x49);
		jpeg_write_m_byte(cinfo, 0x4C);
		jpeg_write_m_byte(cinfo, 0x45);
		jpeg_write_m_byte(cinfo, 0x0);

		/* Add the sequencing info */
		jpeg_write_m_byte(cinfo, cur_marker);
		jpeg_write_m_byte(cinfo, (int) num_markers);

		/* Add the profile data */
		while (length--)
		{
            jpeg_write_m_byte(cinfo, *icc_data_ptr);
            icc_data_ptr++;
		}
		cur_marker++;

	} //End	while (icc_data_len > 0)

    return TRUE;
}

static BOOL setUpICCProfile(j_compress_ptr cinfo,ESString* iccPath)
{
	CESFile* pFile = NULL;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer iccData;

	pFile = CESFile::CreateFileInstanceWithPath( *iccPath );

	if( pFile ) {
		pFile->ReadAvailableData( iccData );
		delete pFile;
	}

	if (iccData.IsEmpty()) {
        return FALSE;
    }

    return writeICCProfile(cinfo, &iccData);
}

CFKJpegEncodeUtil::CFKJpegEncodeUtil()
{
	memset( &m_stCInfo, 0, sizeof(m_stCInfo) );
	memset( &m_stJerr, 0, sizeof(m_stJerr) );

	m_pCFile = NULL;

	m_bStarted = FALSE;
}

CFKJpegEncodeUtil::~CFKJpegEncodeUtil()
{
	if( m_pCFile ) {
		fclose(m_pCFile);
		m_pCFile = NULL;
	}

	if(m_bStarted){
		jpeg_destroy_compress(&m_stCInfo);
		m_bStarted =FALSE;
	}

}

BOOL CFKJpegEncodeUtil::OpenOutFileWithPath( ESString* outFilePath )
{
   if(outFilePath){
#ifdef WIN32
      if (_wfopen_s(&m_pCFile, outFilePath->data(), ES_STRING("wb")) != 0) {
#else
      if (fopen_s(&m_pCFile, outFilePath->data(), ES_STRING("wb")) != 0) {
#endif
           return FALSE;
       }
   }
   return TRUE;
}

void CFKJpegEncodeUtil::CloseOutFile()
{
	if( m_pCFile ) {
		fclose(m_pCFile);
		m_pCFile = NULL;
	}
}

ENUM_FK_ERROR_CODE CFKJpegEncodeUtil::UpdateJpegStructWithDest(	CFKDestination* pDestination,
																ESImageInfo* pImageInfo,
																INT32 nQuality,
																BOOL bProgressiveMode)
{
	ENUM_FK_ERROR_CODE errorCode = kFKNoError;

	m_stCInfo.err = jpeg_custom_error(&m_stJerr);
	jpeg_create_compress(&m_stCInfo);

  m_stCInfo.image_height     = (JDIMENSION)ES_IMAGE_INFO::GetESImageHeight(*pImageInfo);
  m_stCInfo.image_width      = (JDIMENSION)ES_IMAGE_INFO::GetESImageWidth(*pImageInfo);
  m_stCInfo.input_components = (JDIMENSION)ES_IMAGE_INFO::GetESImageSamplesPerPixel(*pImageInfo);
  m_stCInfo.progressive_mode = bProgressiveMode;

  m_stCInfo.in_color_space = JCS_RGB;
  if (m_stCInfo.input_components == 1) {
    m_stCInfo.in_color_space = JCS_GRAYSCALE;
  }

  jpeg_set_defaults( &m_stCInfo );
  jpeg_set_quality( &m_stCInfo , nQuality , TRUE);
  jpeg_default_colorspace( &m_stCInfo );

  m_stCInfo.density_unit     = 1;
	m_stCInfo.X_density        = ES_IMAGE_INFO::GetESImageOutputResolution(*pImageInfo);
	m_stCInfo.Y_density        = ES_IMAGE_INFO::GetESImageOutputResolution(*pImageInfo);

	if ( pDestination->GetFKDestinationType() == kFKDestinationTypePath ) {
		ESString strPath;

		strPath = (LPTSTR)pDestination->GetSource().GetBufferPtr();

		BOOL bSuccess = OpenOutFileWithPath( &strPath );
        ES_ErrorBailWithAction(bSuccess, BAIL, this, errorCode = kFKFileCreateError  , ES_STRING("file create error"));

        jpeg_stdio_dest( &m_stCInfo , m_pCFile );
    }else if(pDestination->GetFKDestinationType() == kFKDestinationTypeData ||
		pDestination->GetFKDestinationType() == kFKDestinationTypeEmpty) {
        jpeg_mem_dest(&m_stCInfo, pDestination, pImageInfo);
    }else{
        assert(false);
    }

BAIL:;

    return errorCode;
}

BOOL CFKJpegEncodeUtil::StartEncodingWithDest( CFKDestination* pDestination,
							ESImageInfo* pImageInfo,
							INT32 nQuality,
							BOOL bProgressiveMode,
							ESString strIccProfilePath,
							ENUM_FK_ERROR_CODE& eError )
{
    ENUM_FK_ERROR_CODE errorCode = kFKNoError;

    ES_ErrorBailWithAction(m_bStarted == FALSE, BAIL, this, errorCode = kFKInconsistentError, ES_STRING("m_bStarted should be FALSE"));
    ES_ErrorBailWithAction((bool)(ES_IMAGE_INFO::GetESImageBitsPerSample(*pImageInfo) == 8), BAIL, this, errorCode = kFKParameterError, ES_STRING("Image BitsPerSample should be 8"));

	errorCode = UpdateJpegStructWithDest( pDestination, pImageInfo, nQuality, bProgressiveMode);
    ES_ErrorBail(errorCode == kFKNoError, BAIL, this, ES_STRING("updateJpegStructWithDest fails"));

	if(setjmp(m_stJerr.setjmp_buffer)){
		jpeg_destroy_compress(&m_stCInfo);
		ES_ErrorBailWithAction(false,BAIL,this,errorCode = kFKJpegReadHaderError,ES_STRING("jpeg_start_compress jpegException"));
	}

    {
        jpeg_start_compress( &m_stCInfo , 1 );
    }

	if (strIccProfilePath.empty() == false) {
        setUpICCProfile(&m_stCInfo,&strIccProfilePath);
    }

    m_bStarted = TRUE;

BAIL:;

    eError = errorCode;

    return errorCode == kFKNoError;
}


BOOL CFKJpegEncodeUtil::WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer& cData, ENUM_FK_ERROR_CODE& eError )
{
  ENUM_FK_ERROR_CODE errorCode = kFKNoError;

  UINT32   bytesPerRow    = m_stCInfo.image_width * m_stCInfo.input_components;
	JDIMENSION writeScanLines = (JDIMENSION) cData.GetLength() / bytesPerRow ;

  ES_ErrorBailWithAction( m_bStarted, BAIL, this, errorCode = kFKInconsistentError, ES_STRING("m_bStarted should be TRUE"));
  ES_ErrorBailWithAction( m_stCInfo.mem, BAIL, this, errorCode = kFKInconsistentError, ES_STRING("m_stCInfo.mem should not be null"));
  {
	  JSAMPARRAY img = (m_stCInfo.mem->alloc_sarray)((j_common_ptr)&m_stCInfo, JPOOL_IMAGE, sizeof(JSAMPROW), writeScanLines);
	  ES_ErrorBailWithAction(img, BAIL, this, errorCode = kFKMemError, ES_STRING("alloc_sarray fails"));

    for(UINT32 i = 0; i < writeScanLines; ++i){
	    UINT8* bytes = (UINT8*)cData.GetConstBufferPtr();
      img[i] = &bytes[i * bytesPerRow];
    }

	  if(setjmp(m_stJerr.setjmp_buffer)){
		  jpeg_destroy_compress(&m_stCInfo);
		  ES_ErrorBailWithAction(false,BAIL,this,errorCode = kFKJpegWriteScanlineError,ES_STRING("WriteScanlinesWithData jpegException"));
	  }
	  {
      jpeg_write_scanlines( &m_stCInfo , img , writeScanLines );
    }
  }

BAIL:;

  eError = errorCode;

  return errorCode == kFKNoError;
}


BOOL CFKJpegEncodeUtil::FinalizeEncodingAndReturnError( ENUM_FK_ERROR_CODE& eError )
{
  ENUM_FK_ERROR_CODE errorCode = kFKNoError;

  ES_ErrorBailWithAction(m_bStarted, BAIL, this, errorCode = kFKInconsistentError, ES_STRING("m_bStarted should be TRUE"));

	if(setjmp(m_stJerr.setjmp_buffer)){
		jpeg_destroy_compress(&m_stCInfo);
		ES_ErrorBailWithAction(false,BAIL,this,errorCode = kFKJpegFinishCompressError,ES_STRING("kFKJpegFinishCompressError jpegException"));
	}


	{
    jpeg_finish_compress(&m_stCInfo);
    jpeg_destroy_compress(&m_stCInfo);
  }

  m_bStarted = FALSE;

BAIL:;

    CloseOutFile();

    eError = errorCode;

    return errorCode == kFKNoError;
}
