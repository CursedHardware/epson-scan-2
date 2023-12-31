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

PROJECT(es2standalone)
cmake_minimum_required (VERSION 2.8.10.2)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

add_definitions(-DBOOST_NO_CXX11_RVALUE_REFERENCES=1)

INCLUDE_DIRECTORIES(
../
../../
../../../
../Include
../CommonUtility
../CommonUtility/utils
)

file(GLOB_RECURSE UI_FILES *.ui)
file(GLOB_RECURSE QRC_FILES icons/*.qrc)

add_executable(es2standalone 
	main.cpp
    mainwindow.cpp 
    capitem.cpp 
    documentsource.cpp 
    scanningside.cpp
    documentsize.cpp 
    imagetype.cpp 
    resolution.cpp 
    rotate.cpp 
    deskewcheckbox.cpp 
    addpagescheckbox.cpp 
    skipblankpagescomb.cpp 
    detectdoublefeedcomb.cpp 
    imageformatcomb.cpp 
    foldercomb.cpp 
    prefixlineedit.cpp 
    noneradiobutton.cpp 
    removebackradiobutton.cpp 
    removebackcombobox.cpp 
    dropoutcombobox.cpp 
    brightnessslider.cpp 
    contrastslider.cpp 
    gammaslider.cpp 
    gammadoublespinbox.cpp 
    thresholdslider.cpp 
    documentsizedialog.cpp 
    widthdoublespinbox.cpp 
    lengthdoublespinbox.cpp
    detectpapercheckbox.cpp 
    skipblankpagessettingsdialog.cpp 
    skipblankpagesdetlvslider.cpp 
    detectdoublefeedsettingsdialog.cpp 
    paperthicknesscombobox.cpp 
    configurationdialog.cpp 
    configuration.cpp 
    unitscombobox.cpp 
    lastusedcheckbox.cpp 
    transferdataaftercompcheckbox.cpp 
    openimgfolderafterscancheckbox.cpp 
    sleeptimerspinbox.cpp 
    acpowercombobox.cpp 
    batterypowercombobox.cpp 
    supervisor.cpp 
    scanbutton.cpp 
    deviceselectorcombobox.cpp 
    imageformatsettingsdialog.cpp 
    imageformatsettingscombobox.cpp 
    tiffcompressionmonocombobox.cpp 
    networkdeviceselectdialog.cpp 
    waitingdialog.cpp 
    devicenamestackedwidget.cpp 
    deviceselectdialog.cpp 
    initdeviceselectcombobox.cpp 
    devicelist.cpp 
    devicefinder.cpp 
    ../Platform.cpp 
    detectionareamindoublespinbox.cpp 
    detectionareamaxdoublespinbox.cpp 
    scanningdialog.cpp 
    selectabledetectdoublefeedsettingsdialog.cpp 
    aboutepsonscan2dialog.cpp 
    addpagedialog.cpp 
    horizondoctoolbutton.cpp 
    verticaldoctoolbutton.cpp 
    lastusedsettings.cpp 
    commandlinemode.cpp 
    documenttype.cpp
    translationstring.cpp 
    saveimagepathlist.cpp
	deviceconnectiondialog.cpp
	savedestfolder.cpp
	custommessagebox.cpp
	continuousscandialog.cpp
	automaticfeedingmodedialog.cpp
	automaticfeedingmodecombobox.cpp
	documentsourcesettingsdialog.cpp
	afmtimeoutcombobox.cpp
	backgroundcolorcombobox.cpp
	previewwindow.cpp
	previewbutton.cpp
	drawingarea.cpp
	myrubberband.cpp
	qualitypreviewcheckbox.cpp
	filenamesettingdialog.cpp
	adfpaperprotectioncombobox.cpp
	behaviorwhendoublefeedcombobox.cpp
	directpoweroncombobox.cpp
	nonconnectpoweroffcombobox.cpp
	sensorglassdirtsensitivitycombobox.cpp
	defaultsettings.cpp
	imagetypesettingsdialog.cpp
	administratorpasswordinputdialog.cpp
        administratorpasswordinputforuksecuritydialog.cpp
	textenhancecombobox.cpp
	textenhancesettingdialog.cpp
	addeditpagedialog.cpp
	imageeditdialog.cpp
	imageeditscene.cpp
	imageeditview.cpp
	rotateimage.cpp
        removepunchholes.cpp
    documentsizesettingdialog.cpp
    blackedgeinscribecheckbox.cpp
    focuspositionsettingdialog.cpp
    edgecorrection.cpp
    autocroppinginscribedcheckbox.cpp
	${UI_FILES}
   	${QRC_FILES}
)

if (${CMAKE_VERSION} LESS 3.0)
    qt5_wrap_ui(FORMS ${UI_FILES})
    add_custom_target(uic_cmake28_compat ALL DEPENDS ${FORMS})
endif()

set_target_properties(es2standalone  PROPERTIES OUTPUT_NAME "epsonscan2")

target_link_libraries(es2standalone ${QT_LIBRARIES}
	pthread
	dl	
	commonutility
)

QT5_USE_MODULES(es2standalone Widgets)
install(TARGETS es2standalone DESTINATION "${EPSON_INSTALL_ROOT}/usr/bin")

