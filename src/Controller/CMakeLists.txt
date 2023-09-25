#  Copyright (C) 2018-2022  SEIKO EPSON CORPORATION
#
#  License: LGPL-2.1+
#  Author : SEIKO EPSON CORPORATION
#
#  This file is part of the 'Epson Scan 2' package.
#	 This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.

#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#  
#  CMakeLists.txt -- template and derived ESC/I ESC/I-2 protocol commands

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)  
endif(COMMAND cmake_policy)

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../CommonUtility/Linux)

PROJECT(epsonscan2)

add_definitions(-DBOOST_NO_CXX11_RVALUE_REFERENCES=1)

INCLUDE_DIRECTORIES(
../
./Include
./Src
./Src/Finder
./Src/Scanner
./Src/ModelInfo 
./Src/Transfer 
./Src/KeysValues
./Src/KeysValues/All
./Src/Filter
./Src/FileFormat
../../thirdparty/HaruPDF/include
../../thirdparty/HaruPDF/zlib
../../thirdparty/HaruPDF/zlib/
../CommonUtility/utils/
../CommonUtility
../Include
../Include/Controller
../../thirdparty/sha1
./Src/KeysValues/All/TextEnhance/
)
ADD_LIBRARY(epsonscan2 SHARED
./Src/EntryPoints.cpp
./Src/Controller.cpp 
./Src/Finder/NetFinder.cpp
./Src/Finder/UsbFinder.cpp
./Src/Finder/Finder.cpp
./Src/Finder/DeviceResolver.cpp
./Src/Scanner/Scanner.cpp 
./Src/Scanner/Engine.cpp
./Src/Filter/DTR.cpp
./Src/Filter/Resize.cpp
./Src/Filter/Filter.cpp
./Src/Filter/Lut.cpp
./Src/Filter/DDEFilter.cpp
./Src/Filter/ProcOrientation.cpp
./Src/Filter/GetOrientation.cpp
./Src/Filter/ESIMP2.cpp
./Src/ModelInfo/ModelInfo.cpp
./Src/Transfer/TransferMgr.cpp
./Src/Transfer/Image.cpp
./Src/Transfer/TransferEvent.cpp
./Src/KeysValues/KeyMgr.cpp
./Src/KeysValues/Key.cpp
./Src/KeysValues/All/TextEnhance/NoiseReductionLevel.cpp
./Src/KeysValues/All/TextEnhance/PaperCreaseReduction.cpp
./Src/KeysValues/All/TextEnhance/TextEnhance.cpp
./Src/KeysValues/All/TextEnhance/TextEnhanceSensitivity.cpp
./Src/KeysValues/All/TextEnhance/ThresholdAdjustment.cpp
./Src/KeysValues/All/FunctionalUnit.cpp
./Src/KeysValues/All/ColorType.cpp
./Src/KeysValues/All/Resolution.cpp
./Src/KeysValues/All/ImageFormat.cpp
./Src/KeysValues/All/ScanAreaHeight.cpp 
./Src/KeysValues/All/ScanAreaWidth.cpp
./Src/KeysValues/All/ScanAreaOffsetX.cpp
./Src/KeysValues/All/ScanAreaOffsetY.cpp
./Src/KeysValues/All/ScanWidthMax.cpp
./Src/KeysValues/All/ScanHeightMax.cpp
./Src/KeysValues/All/ScanHeightMaxInLongPaper.cpp
./Src/KeysValues/All/Dropout.cpp
./Src/KeysValues/All/Orientation.cpp
./Src/KeysValues/All/MultiTiff.cpp
./Src/KeysValues/All/TiffCompression.cpp
./Src/KeysValues/All/Threshold.cpp
./Src/KeysValues/All/DocumentType.cpp
./Src/KeysValues/All/Contrast.cpp
./Src/KeysValues/All/Brightness.cpp
./Src/KeysValues/All/JpegQuality.cpp
./Src/KeysValues/All/DuplexType.cpp
./Src/KeysValues/All/PaperLoaded.cpp
./Src/KeysValues/All/PagesTobeScanned.cpp
./Src/KeysValues/All/DoubleFeedDetection.cpp
./Src/KeysValues/All/DoubleFeedDetectionLevel.cpp
./Src/KeysValues/All/SkipBlankPage.cpp
./Src/KeysValues/All/PaperDeskew.cpp
./Src/KeysValues/All/BackgroundRemoval.cpp
./Src/KeysValues/All/TransferCompression.cpp
./Src/KeysValues/All/AutoSize.cpp
./Src/KeysValues/All/UnscannableScanParameter.cpp
./Src/KeysValues/All/UnscannableScanParameterForAFMC.cpp
./Src/KeysValues/All/ScanHeightMaxInLongPaperResolutionTable.cpp
./Src/KeysValues/All/AutoCropInscribed.cpp
./Src/KeysValues/All/DeficiencyCorrection.cpp
./Src/FileFormat/ESManageFileFormatKit.cpp 
./Src/FileFormat/FFManager.cpp
../../thirdparty/HaruPDF/hpdf_annotation.c
../../thirdparty/HaruPDF/hpdf_array.c
../../thirdparty/HaruPDF/hpdf_binary.c
../../thirdparty/HaruPDF/hpdf_boolean.c
../../thirdparty/HaruPDF/hpdf_catalog.c
../../thirdparty/HaruPDF/hpdf_destination.c
../../thirdparty/HaruPDF/hpdf_dict.c
../../thirdparty/HaruPDF/hpdf_doc_png.c
../../thirdparty/HaruPDF/hpdf_doc.c
../../thirdparty/HaruPDF/hpdf_encoder_cns.c
../../thirdparty/HaruPDF/hpdf_encoder_cnt.c
../../thirdparty/HaruPDF/hpdf_encoder_jp.c
../../thirdparty/HaruPDF/hpdf_encoder_kr.c
../../thirdparty/HaruPDF/hpdf_encoder.c
../../thirdparty/HaruPDF/hpdf_encrypt.c
../../thirdparty/HaruPDF/hpdf_encryptdict.c
../../thirdparty/HaruPDF/hpdf_error.c
../../thirdparty/HaruPDF/hpdf_ext_gstate.c
../../thirdparty/HaruPDF/hpdf_font_cid.c
../../thirdparty/HaruPDF/hpdf_font_tt.c
../../thirdparty/HaruPDF/hpdf_font_type1.c
../../thirdparty/HaruPDF/hpdf_font.c
../../thirdparty/HaruPDF/hpdf_fontdef_base14.c
../../thirdparty/HaruPDF/hpdf_fontdef_cid.c
../../thirdparty/HaruPDF/hpdf_fontdef_cns.c
../../thirdparty/HaruPDF/hpdf_fontdef_cnt.c
../../thirdparty/HaruPDF/hpdf_fontdef_jp.c
../../thirdparty/HaruPDF/hpdf_fontdef_kr.c
../../thirdparty/HaruPDF/hpdf_fontdef_tt.c
../../thirdparty/HaruPDF/hpdf_fontdef_type1.c
../../thirdparty/HaruPDF/hpdf_fontdef.c
../../thirdparty/HaruPDF/hpdf_gstate.c
../../thirdparty/HaruPDF/hpdf_image_ccitt.c
../../thirdparty/HaruPDF/hpdf_image_png.c
../../thirdparty/HaruPDF/hpdf_image.c
../../thirdparty/HaruPDF/hpdf_info.c
../../thirdparty/HaruPDF/hpdf_list.c
../../thirdparty/HaruPDF/hpdf_mmgr.c
../../thirdparty/HaruPDF/hpdf_name.c
../../thirdparty/HaruPDF/hpdf_namedict.c
../../thirdparty/HaruPDF/hpdf_null.c
../../thirdparty/HaruPDF/hpdf_number.c
../../thirdparty/HaruPDF/hpdf_objects.c
../../thirdparty/HaruPDF/hpdf_outline.c
../../thirdparty/HaruPDF/hpdf_page_label.c
../../thirdparty/HaruPDF/hpdf_page_operator.c
../../thirdparty/HaruPDF/hpdf_pages.c
../../thirdparty/HaruPDF/hpdf_real.c
../../thirdparty/HaruPDF/hpdf_streams.c
../../thirdparty/HaruPDF/hpdf_string.c
../../thirdparty/HaruPDF/hpdf_u3d.c
../../thirdparty/HaruPDF/hpdf_utils.c
../../thirdparty/HaruPDF/hpdf_xref.c
../../thirdparty/HaruPDF/hpdf_pdfa.c
../../thirdparty/HaruPDF/hpdf_3dmeasure.c
../../thirdparty/HaruPDF/hpdf_exdata.c
../../thirdparty/HaruPDF/hpdf_encoder_utf.c
	
../../thirdparty/zlib/adler32.c 
../../thirdparty/zlib/crc32.c
../../thirdparty/zlib/gzclose.c
../../thirdparty/zlib/gzread.c
../../thirdparty/zlib/infback.c
../../thirdparty/zlib/inflate.c
../../thirdparty/zlib/trees.c
../../thirdparty/zlib/zutil.c
../../thirdparty/zlib/compress.c
../../thirdparty/zlib/deflate.c
../../thirdparty/zlib/gzlib.c 
../../thirdparty/zlib/gzwrite.c
../../thirdparty/zlib/inffast.c
../../thirdparty/zlib/inftrees.c 
../../thirdparty/zlib/uncompr.c
../../thirdparty/sha1/sha1.c
)

target_link_libraries( epsonscan2
	pthread
	dl
	commonutility
	${Boost_LIBRARIES}
	usb-1.0
)

install(TARGETS epsonscan2 DESTINATION ${EPSON_INSTALL_ROOT}${EPSON_INSTALL_PATH})

