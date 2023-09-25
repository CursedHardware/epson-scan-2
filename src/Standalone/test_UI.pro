#-------------------------------------------------
#
# Project created by QtCreator 2016-10-07T00:23:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET =  epsonscan2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    capitem.cpp \
    documentsource.cpp \
    scanningside.cpp \
    documentsize.cpp \
    imagetype.cpp \
    resolution.cpp \
    rotate.cpp \
    deskewcheckbox.cpp \
    addpagescheckbox.cpp \
    skipblankpagescomb.cpp \
    detectdoublefeedcomb.cpp \
    imageformatcomb.cpp \
    foldercomb.cpp \
    prefixlineedit.cpp \
    noneradiobutton.cpp \
    removebackradiobutton.cpp \
    removebackcombobox.cpp \
    dropoutcombobox.cpp \
    brightnessslider.cpp \
    contrastslider.cpp \
    gammaslider.cpp \
    gammadoublespinbox.cpp \
    thresholdslider.cpp \
    documentsizedialog.cpp \
    widthdoublespinbox.cpp \
    lengthdoublespinbox.cpp \
    detectpapercheckbox.cpp \
    skipblankpagessettingsdialog.cpp \
    skipblankpagesdetlvslider.cpp \
    detectdoublefeedsettingsdialog.cpp \
    paperthicknesscombobox.cpp \
    configurationdialog.cpp \
    configuration.cpp \
    unitscombobox.cpp \
    lastusedcheckbox.cpp \
    transferdataaftercompcheckbox.cpp \
    openimgfolderafterscancheckbox.cpp \
    sleeptimerspinbox.cpp \
    acpowercombobox.cpp \
    batterypowercombobox.cpp \
    supervisor.cpp \
    scanbutton.cpp \
    deviceselectorcombobox.cpp \
    imageformatsettingsdialog.cpp \
    imageformatsettingscombobox.cpp \
    tiffcompressionmonocombobox.cpp \
    networkdeviceselectdialog.cpp \
    waitingdialog.cpp \
    devicenamestackedwidget.cpp \
    deviceselectdialog.cpp \
    initdeviceselectcombobox.cpp \
    devicelist.cpp \
    devicefinder.cpp \
    ../Platform.cpp \
    detectionareamindoublespinbox.cpp \
    detectionareamaxdoublespinbox.cpp \
    scanningdialog.cpp \
    selectabledetectdoublefeedsettingsdialog.cpp \
    aboutepsonscan2dialog.cpp \
    addpagedialog.cpp \
    horizondoctoolbutton.cpp \
    verticaldoctoolbutton.cpp \
    lastusedsettings.cpp \
    commandlinemode.cpp \
    translationstring.cpp \
    saveimagepathlist.cpp \
    deviceconnectiondialog.cpp \
    savedestfolder.cpp \
    custommessagebox.cpp \
    backgroundcolorcombobox.cpp \
    continuousscandialog.cpp \
    automaticfeedingmodedialog.cpp \
    automaticfeedingmodecombobox.cpp \
    documentsourcesettingsdialog.cpp \
    afmtimeoutcombobox.cpp \
    previewbutton.cpp \
    previewwindow.cpp \
    drawingarea.cpp \
    myrubberband.cpp \
    qualitypreviewcheckbox.cpp \
    filenamesettingdialog.cpp \
    adfpaperprotectioncombobox.cpp \
    sensorglassdirtsensitivitycombobox.cpp \
    behaviorwhendoublefeedcombobox.cpp \
    nonconnectpoweroffcombobox.cpp \
    directpoweroncombobox.cpp \
    administratorpasswordinputdialog.cpp \
    imagetypesettingsdialog.cpp \
    textenhancecombobox.cpp \
    textenhancesettingdialog.cpp \
    imageeditdialog.cpp \
    imageeditscene.cpp \
    imageeditview.cpp \
    addeditpagedialog.cpp \
    rotateimage.cpp \
    administratorpasswordinputforuksecuritydialog.cpp \
    defaultsettings.cpp \
    removepunchholes.cpp \
    documenttype.cpp \
    focuspositionsettingdialog.cpp \
    doubleslider.cpp \
    edgecorrection.cpp \
    edgecorrectionsettingsdialog.cpp \
    documentsizesettingdialog.cpp \
    blackedgeinscribecheckbox.cpp \
    autocroppinginscribedcheckbox.cpp

HEADERS  += mainwindow.h \
    capitem.h \
    documentsource.h \
    scanningside.h \
    documentsize.h \
    imagetype.h \
    resolution.h \
    rotate.h \
    deskewcheckbox.h \
    addpagescheckbox.h \
    skipblankpagescomb.h \
    detectdoublefeedcomb.h \
    imageformatcomb.h \
    foldercomb.h \
    prefixlineedit.h \
    noneradiobutton.h \
    removebackradiobutton.h \
    removebackcombobox.h \
    dropoutcombobox.h \
    brightnessslider.h \
    contrastslider.h \
    gammaslider.h \
    gammadoublespinbox.h \
    thresholdslider.h \
    documentsizedialog.h \
    widthdoublespinbox.h \
    lengthdoublespinbox.h \
    detectpapercheckbox.h \
    skipblankpagessettingsdialog.h \
    skipblankpagesdetlvslider.h \
    detectdoublefeedsettingsdialog.h \
    paperthicknesscombobox.h \
    configurationdialog.h \
    configuration.h \
    unitscombobox.h \
    lastusedcheckbox.h \
    transferdataaftercompcheckbox.h \
    openimgfolderafterscancheckbox.h \
    sleeptimerspinbox.h \
    acpowercombobox.h \
    batterypowercombobox.h \
    scanbutton.h \
    supervisor.h \
    deviceselectorcombobox.h \
    imageformatsettingsdialog.h \
    imageformatsettingscombobox.h \
    tiffcompressionmonocombobox.h \
    networkdeviceselectdialog.h \
    waitingdialog.h \
    devicenamestackedwidget.h \
    deviceselectdialog.h \
    initdeviceselectcombobox.h \
    devicelist.h \
    devicefinder.h \
    detectionareamindoublespinbox.h \
    detectionareamaxdoublespinbox.h \
    scanningdialog.h \
    selectabledetectdoublefeedsettingsdialog.h \
    aboutepsonscan2dialog.h \
    addpagedialog.h \
    horizondoctoolbutton.h \
    verticaldoctoolbutton.h \
    lastusedsettings.h \
    commandlinemode.h \
    common_utility.h \
    translationstring.h \
    saveimagepathlist.h \
    deviceconnectiondialog.h \
    savedestfolder.h \
    custommessagebox.h \
    backgroundcolorcombobox.h \
    continuousscandialog.h \
    automaticfeedingmodedialog.h \
    automaticfeedingmodecombobox.h \
    documentsourcesettingsdialog.h \
    afmtimeoutcombobox.h \
    previewbutton.h \
    previewwindow.h \
    drawingarea.h \
    myrubberband.h \
    qualitypreviewcheckbox.h \
    filenamesettingdialog.h \
    adfpaperprotectioncombobox.h \
    sensorglassdirtsensitivitycombobox.h \
    behaviorwhendoublefeedcombobox.h \
    nonconnectpoweroffcombobox.h \
    directpoweroncombobox.h \
    defaultsettings.h \
    administratorpasswordinputdialog.h \
    imagetypesettingsdialog.h \
    textenhancecombobox.h \
    textenhancesettingdialog.h \
    imageeditdialog.h \
    imageeditscene.h \
    imageeditview.h \
    addeditpagedialog.h \
    rotateimage.h \
    administratorpasswordinputforuksecuritydialog.h \
    removepunchholes.h \
    documenttype.h \
    focuspositionsettingdialog.h \
    doubleslider.h \
    edgecorrection.h \
    edgecorrectionsettingsdialog.h \
    doubleslider.h \
    removepunchholes.h \
    documentsizesettingdialog.h \
    blackedgeinscribecheckbox.h \
    autocroppinginscribedcheckbox.h

FORMS    += mainwindow.ui \
    scanningdialog.ui \
    documentsizedialog.ui \
    skipblankpagessettingsdialog.ui \
    detectdoublefeedsettingsdialog.ui \
    configurationdialog.ui \
    imageformatsettingsdialog.ui \
    networkdeviceselectdialog.ui \
    waitingdialog.ui \
    deviceselectdialog.ui \
    selectabledetectdoublefeedsettingsdialog.ui \
    aboutepsonscan2dialog.ui \
    addpagedialog.ui \
    deviceconnectiondialog.ui \
    continuousscandialog.ui \
    automaticfeedingmodedialog.ui \
    documentsourcesettingsdialog.ui \
    filenamesettingdialog.ui \
    administratorpasswordinputdialog.ui \
    imagetypesettingsdialog.ui \
    textenhancesettingdialog.ui \
    imageeditdialog.ui \
    addeditpagedialog.ui \
    administratorpasswordinputforuksecuritydialog.ui \
    focuspositionsettingdialog.ui \
    edgecorrectionsettingsdialog.ui \
    documentsizesettingdialog.ui

DISTFILES += \
    CMakeLists.txt

LIBS += -ldl \
        -lboost_system \
        -lboost_filesystem \
        -L/usr/lib/x86_64-linux-gnu/epsonscan2 -lcommonutility

INCLUDEPATH += ../include \
        ../CommonUtility/utils \
        ../CommonUtility \
        ../

RESOURCES += \
    icons/icon.qrc
