//
//  jpegUserDefines.h
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 13/12/14.
//  Copyright (c) 2013 EPSON. All rights reserved.
//
#pragma once

#define HAVE_BOOLEAN
typedef int boolean;
#include "jpeglib.h"

#include <setjmp.h>

//JPEG拡張エラー構造体
struct my_error_mgr {
  struct jpeg_error_mgr pub;	//エラーメッセージ
  jmp_buf setjmp_buffer;        //ジャンプバッファ
};

typedef struct my_error_mgr * my_error_ptr;


METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{

	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	char buffer[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message) (cinfo, buffer);

#ifdef WIN32
	ES_Error_Log(NULL,ES_STRING("Jpeg encode error [%s]"),ES_CMN_FUNCS::STRING::MBStringToUnicode(buffer).c_str());
#else
	ES_Error_Log(NULL,ES_STRING("Jpeg encode error [%s]"),buffer);
#endif

	longjmp(myerr->setjmp_buffer,1);
}

LOCAL(struct jpeg_error_mgr *) jpeg_custom_error (struct my_error_mgr * err)
{

	jpeg_error_mgr* pRet  = jpeg_std_error(&(err->pub));
	err->pub.error_exit = my_error_exit;

	return pRet;
}
