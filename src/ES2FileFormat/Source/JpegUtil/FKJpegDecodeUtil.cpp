//
//  FKJpegDecodeUtil.cpp
//  FileFormatKit
//
//  Created by Nakazawa Yuichi on 13/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#include "ESFile.h"

#include "stdafx.h"

#include "FKJpegDecodeUtil.h"
#include "jpegUserDefines.h"

typedef struct {
	struct jpeg_source_mgr pub;	/* public fields */


	void*	pData;

	ENUM_FK_ERROR_CODE m_eError;


} my_source_mgr;
typedef my_source_mgr * my_src_ptr;


METHODDEF(void) init_source (j_decompress_ptr cinfo)
{
}

/*
 入力対象とするデータを取得し、バッファーにコピーする。
 読み込んでない部の長さをデータにセットする。
*/
METHODDEF(boolean) fill_input_buffer (j_decompress_ptr cinfo)
{
    my_src_ptr src = (my_src_ptr)cinfo->src;

	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cJpegData;
	cJpegData.Attach(((CFKJpegDecodeUtil*)src->pData)->m_cJpegData);

	if (cJpegData.GetLength() == 0) {
		ES_Info_Log(fill_input_buffer, ES_STRING("fill_input_buffer cJpegDataLen  %08d,%08d,%08X,%08d"),0,cJpegData.GetLength(),src->pub.next_input_byte,src->pub.bytes_in_buffer);
        return FALSE;
    }

	ESNumber length = cJpegData.GetLength();

	JOCTET * buffer =(JOCTET *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
									length * sizeof(JOCTET));


	if(buffer == NULL){
		ES_Error_Log(fill_input_buffer, ES_STRING("Alloc small fail"));
		src->m_eError = kFKMemError;
		my_error_exit((j_common_ptr)cinfo);
	}

	memcpy( buffer, cJpegData.GetBufferPtr(), length );

	ES_Info_Log(fill_input_buffer, ES_STRING("fill_input_buffer cJpegDataLen  %08d,%08d,%08X,%08d"),length,cJpegData.GetLength(),src->pub.next_input_byte,src->pub.bytes_in_buffer);

    src->pub.next_input_byte = buffer;
    src->pub.bytes_in_buffer = length;

	((CFKJpegDecodeUtil*)src->pData)->m_cJpegData.FreeBuffer();

	return TRUE;
}

METHODDEF(void) skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    struct jpeg_source_mgr * src = cinfo->src;

    /* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
     * any trouble anyway --- large skips are infrequent.
     */
    if (num_bytes > 0) {
        while (num_bytes > (long) src->bytes_in_buffer) {
            num_bytes -= (long) src->bytes_in_buffer;
            (void) (*src->fill_input_buffer) (cinfo);
            /* note we assume that fill_input_buffer will never return FALSE,
             * so suspension need not be handled.
             */
        }
        src->next_input_byte += (size_t) num_bytes;
        src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

METHODDEF(void) term_source (j_decompress_ptr cinfo)
{
}

LOCAL(void) jpeg_memory_src (j_decompress_ptr cinfo, void* pData)
{
    my_src_ptr src;

    if (cinfo->src == NULL) {	/* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                    sizeof(my_source_mgr));


    }

    src                        = (my_src_ptr)cinfo->src;
    src->pData                 = pData;
    src->pub.init_source       = init_source;
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data   = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;/* use default method */
    src->pub.term_source       = term_source;
    src->pub.bytes_in_buffer   = 0;
    src->pub.next_input_byte   = NULL;
	src->m_eError			 = kFKNoError;

}

CFKJpegDecodeUtil::CFKJpegDecodeUtil()
{
	memset( &m_stCinfo, 0, sizeof(m_stCinfo));
	memset( &m_stJerr, 0, sizeof(m_stJerr));
	m_bHeaderRead = FALSE;
	m_bStart	  = FALSE;

	m_stCinfo.err = jpeg_custom_error( &m_stJerr );
	jpeg_create_decompress( &m_stCinfo );
	jpeg_memory_src( &m_stCinfo, (void*)this );
}

CFKJpegDecodeUtil::~CFKJpegDecodeUtil()
{
	if(m_bStart){
		jpeg_destroy_decompress(&m_stCinfo);
	}
}


BOOL CFKJpegDecodeUtil::StartDecodingAndReturnError( ENUM_FK_ERROR_CODE& eError )
{
    ENUM_FK_ERROR_CODE errorCode = kFKNoError;

    ES_ErrorBailWithAction(m_bStart == FALSE, BAIL, this, errorCode = kFKInconsistentError, ES_STRING("m_bStart should be FALSE"));


    m_bStart = TRUE;

BAIL:

    eError = errorCode;

    return errorCode == kFKNoError;
}

void CFKJpegDecodeUtil::AppendData( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cData )
{
	m_cJpegData.AppendBuffer(cData.GetBufferPtr(),cData.GetLength());
}


BOOL CFKJpegDecodeUtil::DecodedDataAndReturnError( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData, ENUM_FK_ERROR_CODE& eError ,ESNumber nValidDataLength,BOOL bReadOnlyValidData)
{

	BOOL bComplete = TRUE;

    ES_ErrorBailWithAction(m_bStart, BAIL, this, eError = kFKInconsistentError, ES_STRING("m_bStart should be TRUE"));

    eError = jpegStartDecompress();
    ES_ErrorBail(eError == kFKNoError, BAIL, this,  ES_STRING("jpegStartDecompress fails"));

    if (m_bHeaderRead) {
		bComplete = scanJpegLines(cDestData,nValidDataLength,bReadOnlyValidData,eError);
		ES_ErrorBail(eError == kFKNoError, BAIL, this, ES_STRING("scanJpegLines fails"));
    }

BAIL:

	return bComplete;

}

ENUM_FK_ERROR_CODE CFKJpegDecodeUtil::jpegStartDecompress()
{
    ENUM_FK_ERROR_CODE errorCode = kFKNoError;

    if (!m_bHeaderRead) {

		if(setjmp(m_stJerr.setjmp_buffer)){
			errorCode = ((my_src_ptr)m_stCinfo.src)->m_eError;

			if(errorCode == kFKNoError){
				errorCode = kFKJpegReadHaderError;
			}
			jpeg_destroy_decompress(&m_stCinfo);

			ES_ErrorBail(false, BAIL, this, ES_STRING("scanJpegLines jpegException"));
		}


		{
            if (jpeg_read_header(&m_stCinfo, TRUE) == JPEG_SUSPENDED) {
                goto BAIL;
            }
			ES_Info_Log(this, ES_STRING("Jpeg decode header size w[%d] h[%d]"), m_stCinfo.image_width, m_stCinfo.image_height);


            m_bHeaderRead = TRUE;

            jpeg_start_decompress(&m_stCinfo);
        }
    }

BAIL:

    return errorCode;
}

const UInt32 kMaxScanLines = 50;
static UInt8 kPaddingColor = 255;
BOOL CFKJpegDecodeUtil::scanJpegLines( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData, ESNumber nValidHeight, BOOL bReadOnlyValidData, ENUM_FK_ERROR_CODE& errorCode)
{
  errorCode = kFKNoError;
	bool bComplete = true;

    UINT32   bufsize   = sizeof(char) * m_stCinfo.output_width * m_stCinfo.output_components;
	JSAMPARRAY scanlines = (m_stCinfo.mem->alloc_sarray)((j_common_ptr)&m_stCinfo, JPOOL_IMAGE, bufsize, 1);

	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cPadding(bufsize);
	cPadding.FillBuffer(kPaddingColor);

  ES_ErrorBailWithAction(scanlines, BAIL, this, errorCode = kFKMemError, ES_STRING("jpeg scanLine allocation fails"));
	ES_ErrorBailWithAction(cPadding.GetLength(), BAIL, this, errorCode = kFKMemError, ES_STRING("cPadding allocation fails"));
	{
		cDestData.GetStreamBuffer(bufsize *kMaxScanLines ,bufsize *kMaxScanLines);

		ESNumber nReadedLines = 0;							//今回ReadしたLine

		ESNumber nStartLineNum =m_stCinfo.output_scanline;	//前回までにOutputしたLineをStartとして記憶

		ESNumber nTarget = nValidHeight;					//nTarget：有効なデータがどこまで含まれるかを示す

		ESNumber nNeedScanLines =m_stCinfo.output_height;	//nNeedScanLines：出力すべきLine数を示す

		if(nTarget == 0){
			nTarget = m_stCinfo.output_height;
		}
		if(bReadOnlyValidData){
			//読み捨てが必要な場合は、有効データだけをDecodeし、それ以外のバイナリは読み捨てる
			//PEDT等で、ページ単位でデコードをする場合は、JpegDecodeのOutimageInfoで高さが正しく次のフィルタに伝搬するため
			//Paddingを行う必要がない。
			nNeedScanLines = nTarget;
		}

		if(setjmp(m_stJerr.setjmp_buffer)){
			errorCode = ((my_src_ptr)m_stCinfo.src)->m_eError;

			if(errorCode == kFKNoError){
				errorCode = kFKJpegReadHaderError;
			}
			jpeg_destroy_decompress(&m_stCinfo);

			ES_ErrorBail(false, BAIL, this, ES_STRING("scanJpegLines jpegException"));
		}


		while (m_stCinfo.output_scanline < nNeedScanLines) {


			{

				if( jpeg_read_scanlines(&m_stCinfo, scanlines, 1) == 0) {

						break;
				}
				else{
					nReadedLines = m_stCinfo.output_scanline - nStartLineNum;
				}
			}

			if(m_stCinfo.output_scanline > nTarget){
				memset(scanlines[0],kPaddingColor,bufsize);
			}

			cDestData.AppendBuffer(scanlines[0],bufsize);


			if(nReadedLines == kMaxScanLines){
				bComplete = false;
				break;
			}
		}

 	}

BAIL:;

	cDestData.ReleaseStreamBuffer();

  if (errorCode != kFKNoError) {
		cDestData.FreeBuffer();
  }

	return bComplete;
}


BOOL CFKJpegDecodeUtil::FinalizeDecodingAndReturnError( ENUM_FK_ERROR_CODE& eError )
{
    ENUM_FK_ERROR_CODE errorCode = kFKNoError;

    ES_ErrorBailWithAction(m_bStart, BAIL, this, errorCode = kFKInconsistentError, L"m_bStart should be TRUE");

	if(setjmp(m_stJerr.setjmp_buffer)){
		errorCode = ((my_src_ptr)m_stCinfo.src)->m_eError;

		if(errorCode == kFKNoError){
			errorCode = kFKJpegReadHaderError;
		}
		jpeg_destroy_decompress(&m_stCinfo);

		ES_ErrorBail(false,BAIL,this,ES_STRING("scanJpegLines jpegException"));

	}

    {
        jpeg_abort_decompress(&m_stCinfo);
        jpeg_destroy_decompress(&m_stCinfo);
    }

    m_bStart = FALSE;

BAIL:

    eError = errorCode;

    return errorCode == kFKNoError;
}
// リサイズの比率を取得
// target_size : 目標のサイズ
// width , height : 縦横のサイズ
double get_scale_factor(int target_width,int target_height, int width, int height) {
    double dScaleW = (double)target_width / width;
    double dScaleH = (double)target_height / height;


	if (dScaleH > dScaleW) {
		return dScaleW;
    } else {
		return dScaleH;
    }
}

/*///////////////////////////////////
JpegDataからサムネイルを生成する。
	cSrcJpegData		入力となるJpegデータ
	cDestResizedRawData	リサイズされたRawデータが返る
	stRequestSize		ほしいイメージのサイズ
	outImageInfoAsJsonn	出力画像の情報

	NOTE:LibJpegの仕様として整数分の１にしか圧縮できないため、希望サイズに近い希望サイズより大きな画像が返る。
		 希望サイズ通りに返すには、出力画像をさらに縮小する必要がある。

////////////////////////////////////*/
BOOL CFKJpegDecodeUtil::DecodeJpegDataWithResize(const ES_CMN_FUNCS::BUFFER::IESBuffer& cSrcJpegData,ES_CMN_FUNCS::BUFFER::IESBuffer& cDestResizedRawData, ST_ES_SIZE_UN32& stRequestSize,ESString& outImageInfoAsJsonn){


    struct jpeg_error_mgr jpeg_error;
    m_stCinfo.err = jpeg_std_error(&jpeg_error);

	m_cJpegData.CopyBuffer(cSrcJpegData);

    jpeg_read_header(&m_stCinfo, TRUE);

    // 縮小サイズで高速に展開するための準備
    jpeg_calc_output_dimensions(&m_stCinfo);
	m_stCinfo.scale_denom = (unsigned int)(1 / get_scale_factor(stRequestSize.cx,stRequestSize.cy, m_stCinfo.output_width, m_stCinfo.output_height));
    m_stCinfo.two_pass_quantize = FALSE;
	m_stCinfo.dither_mode = JDITHER_FS;
    if (! m_stCinfo.quantize_colors) {
        m_stCinfo.desired_number_of_colors = 216;
    }
    m_stCinfo.dct_method = JDCT_FASTEST;
    m_stCinfo.do_fancy_upsampling = FALSE;

    int components = m_stCinfo.output_components;
    J_COLOR_SPACE color_space = components == 3 ? JCS_RGB : JCS_GRAYSCALE;

    //// サムネイルのサイズを計算
    //int image_width = 0;
    //int image_height = 0;
    //double scale_factor = get_scale_factor(stRequestSize.cx,stRequestSize.cy, m_stCinfo.output_width, m_stCinfo.output_height);
    //if (m_stCinfo.output_width > m_stCinfo.output_height) {
    //    image_width = stRequestSize.cx;
    //    image_height = (int)(m_stCinfo.output_height * scale_factor);
    //} else {
    //    image_height = stRequestSize.cy;
    //    image_width = (int)(m_stCinfo.output_width * scale_factor);
    //}

    jpeg_start_decompress(&m_stCinfo);

    // 展開用メモリの確保
		UINT32   Linebufsize   = sizeof(char) * m_stCinfo.output_width * m_stCinfo.output_components;
		JSAMPARRAY scanlines = (m_stCinfo.mem->alloc_sarray)((j_common_ptr)&m_stCinfo, JPOOL_IMAGE, Linebufsize, 1);

		cDestResizedRawData.GetStreamBuffer(m_stCinfo.output_width* m_stCinfo.output_components* m_stCinfo.output_height,Linebufsize);

    // 画像のスキャン
    while(m_stCinfo.output_scanline < m_stCinfo.output_height){
		if(  jpeg_read_scanlines(&m_stCinfo, scanlines , 1) == 1){
			cDestResizedRawData.AppendBuffer(scanlines[0],m_stCinfo.output_width * m_stCinfo.output_components);
		}
    }
	cDestResizedRawData.ReleaseStreamBuffer();

	AfxGetLog()->InitImage(ES_STRING("ResizedRawData"),ENUM_IMAGE_TYPE::LogImageRaw,8*m_stCinfo.num_components,m_stCinfo.output_width,m_stCinfo.output_height);
	AfxGetLog()->DumpImage(ES_STRING("ResizedRawData"),ENUM_IMAGE_TYPE::LogImageRaw,8*m_stCinfo.num_components,cDestResizedRawData);

	ESImageInfo outImageInfo;
	outImageInfo[ES_IMAGE_INFO::kESImageInfoWidthKey]			= (ESNumber)m_stCinfo.output_width;
	outImageInfo[ES_IMAGE_INFO::kESImageInfoHeightKey]		  	= (ESNumber)m_stCinfo.output_height;
	outImageInfo[ES_IMAGE_INFO::kESImageInfoSamplesPerPixelKey] = (ESNumber)m_stCinfo.num_components;
	outImageInfo[ES_IMAGE_INFO::kESImageInfoBitsPerSampleKey]	= (ESNumber)8;

	ES_IMAGE_INFO::GetImageInfoAsJson(outImageInfoAsJsonn,outImageInfo);

    jpeg_finish_decompress(&m_stCinfo);
    jpeg_destroy_decompress(&m_stCinfo);


	return cDestResizedRawData.GetLength() != 0;

}
