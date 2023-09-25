//  translationstring.cpp -- template and derived ESC/I ESC/I-2 protocol commands
//  Copyright (C) 2018  SEIKO EPSON CORPORATION
//
//  License: GPL-3.0+
//  Author : SEIKO EPSON CORPORATION
//
//  This file is part of the 'Epson Scan 2' package.
//  This package is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License or, at
//  your option, any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//  You ought to have received a copy of the GNU General Public License
//  along with this package.  If not, see <http://www.gnu.org/licenses/>.

#include "translationstring.h"

static TranslationString translation;

TranslationString::TranslationString()
{
}

TranslationString& TranslationString::GetString()
{
    return translation;
}

void TranslationString::SetString()
{
    TRANSLATIONSTR temp_translation;

    temp_translation.MAINSETTING_003 = QString::fromLocal8Bit((QObject::tr("MAINSETTING_003").toLocal8Bit()).constData());
    temp_translation.MAINSETTING_004 = QObject::tr("MAINSETTING_004");
    temp_translation.MAINSETTING_005 = QObject::tr("MAINSETTING_005");
    temp_translation.MAINSETTING_006 = QObject::tr("MAINSETTING_006");
    temp_translation.MAINSETTING_009 = QObject::tr("MAINSETTING_009");
    temp_translation.MAINSETTING_010 = QObject::tr("MAINSETTING_010");
    temp_translation.MAINSETTING_014 = QObject::tr("MAINSETTING_014");
    temp_translation.MAINSETTING_015 = QObject::tr("MAINSETTING_015");
    temp_translation.MAINSETTING_016 = QObject::tr("MAINSETTING_016");
    temp_translation.MAINSETTING_017 = QObject::tr("MAINSETTING_017");
    temp_translation.MAINSETTING_021 = QObject::tr("MAINSETTING_021");
    temp_translation.MAINSETTING_022 = QObject::tr("MAINSETTING_022");
    temp_translation.MAINSETTING_023 = QObject::tr("MAINSETTING_023");
    temp_translation.MAINSETTING_024 = QObject::tr("MAINSETTING_024");
    temp_translation.MAINSETTING_025 = QObject::tr("MAINSETTING_025");
    temp_translation.MAINSETTING_031 = QObject::tr("MAINSETTING_031");
    temp_translation.MAINSETTING_047 = QObject::tr("MAINSETTING_047");
    temp_translation.MAINSETTING_075 = QObject::tr("MAINSETTING_075");
    temp_translation.MAINSETTING_076 = QObject::tr("MAINSETTING_076");
    temp_translation.MAINSETTING_077 = QObject::tr("MAINSETTING_077");
    temp_translation.MAINSETTING_078 = QObject::tr("MAINSETTING_078");
    temp_translation.MAINSETTING_081 = QObject::tr("MAINSETTING_081");
    temp_translation.ADVANCEDSETTING_008 = QObject::tr("ADVANCEDSETTING_008");
    temp_translation.ADVANCEDSETTING_009 = QObject::tr("ADVANCEDSETTING_009");
    temp_translation.ADVANCEDSETTING_011 = QObject::tr("ADVANCEDSETTING_011");
    temp_translation.ADVANCEDSETTING_012 = QObject::tr("ADVANCEDSETTING_012");
    temp_translation.ADVANCEDSETTING_013 = QObject::tr("ADVANCEDSETTING_013");
    temp_translation.ADVANCEDSETTING_050 = QObject::tr("ADVANCEDSETTING_050");
    temp_translation.ADVANCEDSETTING_051 = QObject::tr("ADVANCEDSETTING_051");
    temp_translation.ADVANCEDSETTING_052 = QObject::tr("ADVANCEDSETTING_052");
    temp_translation.ADVANCEDSETTING_053 = QObject::tr("ADVANCEDSETTING_053");
    temp_translation.ADVANCEDSETTING_054 = QObject::tr("ADVANCEDSETTING_054");
    temp_translation.ADVANCEDSETTING_055 = QObject::tr("ADVANCEDSETTING_055");
    temp_translation.ADVANCEDSETTING_059 = QObject::tr("ADVANCEDSETTING_059");
    temp_translation.ADVANCEDSETTING_060 = QObject::tr("ADVANCEDSETTING_060");
    temp_translation.ADVANCEDSETTING_061 = QObject::tr("ADVANCEDSETTING_061");
    temp_translation.ADVANCEDSETTING_063 = QObject::tr("ADVANCEDSETTING_063");
    temp_translation.ADVANCEDSETTING_064 = QObject::tr("ADVANCEDSETTING_064");
    temp_translation.ADVANCEDSETTING_070 = QObject::tr("ADVANCEDSETTING_070");
    temp_translation.ADVANCEDSETTING_072 = QObject::tr("ADVANCEDSETTING_072");
    temp_translation.ADVANCEDSETTING_114 = QObject::tr("ADVANCEDSETTING_114");
    temp_translation.AUTOFEEDMODE_005 = QObject::tr("AUTOFEEDMODE_005");
    temp_translation.AUTOFEEDMODE_006 = QObject::tr("AUTOFEEDMODE_006");
    temp_translation.AUTOFEEDMODE_007 = QObject::tr("AUTOFEEDMODE_007");
    temp_translation.IMAGEFORMATOPTION_TIF_011 = QObject::tr("IMAGEFORMATOPTION_TIF_011");
    temp_translation.IMAGEFORMATOPTION_TIF_012 = QObject::tr("IMAGEFORMATOPTION_TIF_012");
    temp_translation.E_SCAN_001 = QObject::tr("E_SCAN_001");
    temp_translation.E_SCAN_003 = QObject::tr("E_SCAN_003");
    temp_translation.E_SCAN_101 = QObject::tr("E_SCAN_101");
    temp_translation.E_SCAN_102 = QObject::tr("E_SCAN_102");
    temp_translation.E_SCAN_103 = QObject::tr("E_SCAN_103");
    temp_translation.E_SCAN_104 = QObject::tr("E_SCAN_104");
    temp_translation.E_SCAN_105 = QObject::tr("E_SCAN_105");
    temp_translation.E_SCAN_107 = QObject::tr("E_SCAN_107");
    temp_translation.E_SCAN_109 = QObject::tr("E_SCAN_109");
    temp_translation.E_SCAN_109 = QObject::tr("E_SCAN_110");
    temp_translation.E_SCAN_111 = QObject::tr("E_SCAN_111");
    temp_translation.E_SCAN_113 = QObject::tr("E_SCAN_113");
    temp_translation.E_SCAN_114 = QObject::tr("E_SCAN_114");
    temp_translation.E_SCAN_117 = QObject::tr("E_SCAN_117");
    temp_translation.E_SCAN_118 = QObject::tr("E_SCAN_118");
    temp_translation.ES2U_MAIN_008 = QObject::tr("ES2U_MAIN_008");
    temp_translation.ES2U_MAIN_025 = QObject::tr("ES2U_MAIN_025");
    temp_translation.ES2U_MAIN_052 = QObject::tr("ES2U_MAIN_052");
    temp_translation.ES2U_MAIN_053 = QObject::tr("ES2U_MAIN_053");
    temp_translation.ES2U_SCANNER_003 = QObject::tr("ES2U_SCANNER_003");
    temp_translation.ES2U_SCANNER_004 = QObject::tr("ES2U_SCANNER_004");
    temp_translation.ES2U_SCANNER_005 = QObject::tr("ES2U_SCANNER_005");
    temp_translation.ES2U_SCANNER_008 = QObject::tr("ES2U_SCANNER_008");
    temp_translation.ES2U_SCANNER_010 = QObject::tr("ES2U_SCANNER_010");
    temp_translation.ES2U_SCANNER_011 = QObject::tr("ES2U_SCANNER_011");
    temp_translation.SIZELIST_NOSIZE_028 = QObject::tr("SIZELIST_NOSIZE_028");
    temp_translation.SIZELIST_NOSIZE_030 = QObject::tr("SIZELIST_NOSIZE_030");
    temp_translation.SIZELIST_NOSIZE_015 = QObject::tr("SIZELIST_NOSIZE_015");
    temp_translation.SIZELIST_NOSIZE_016 = QObject::tr("SIZELIST_NOSIZE_016");
    temp_translation.SIZELIST_NOSIZE_019 = QObject::tr("SIZELIST_NOSIZE_019");
    temp_translation.SIZELIST_NOSIZE_025 = QObject::tr("SIZELIST_NOSIZE_025");
    temp_translation.SIZELIST_NOSIZE_017 = QObject::tr("SIZELIST_NOSIZE_017");
    temp_translation.SIZELIST_NOSIZE_018 = QObject::tr("SIZELIST_NOSIZE_018");
    temp_translation.SIZELIST_NOSIZE_011 = QObject::tr("SIZELIST_NOSIZE_011");
    temp_translation.SIZELIST_NOSIZE_012 = QObject::tr("SIZELIST_NOSIZE_012");
    temp_translation.SIZELIST_NOSIZE_009 = QObject::tr("SIZELIST_NOSIZE_009");
    temp_translation.SIZELIST_NOSIZE_010 = QObject::tr("SIZELIST_NOSIZE_010");
    temp_translation.SIZELIST_NOSIZE_007 = QObject::tr("SIZELIST_NOSIZE_007");
    temp_translation.SIZELIST_NOSIZE_008 = QObject::tr("SIZELIST_NOSIZE_008");
    temp_translation.SIZELIST_NOSIZE_023 = QObject::tr("SIZELIST_NOSIZE_023");
    temp_translation.SIZELIST_NOSIZE_024 = QObject::tr("SIZELIST_NOSIZE_024");
    temp_translation.SIZELIST_NOSIZE_013 = QObject::tr("SIZELIST_NOSIZE_013");
    temp_translation.SIZELIST_NOSIZE_014 = QObject::tr("SIZELIST_NOSIZE_014");
    temp_translation.SIZELIST_NOSIZE_021 = QObject::tr("SIZELIST_NOSIZE_021");
    temp_translation.SIZELIST_NOSIZE_001 = QObject::tr("SIZELIST_NOSIZE_001");
    temp_translation.SIZELIST_NOSIZE_002 = QObject::tr("SIZELIST_NOSIZE_002");
    temp_translation.SIZELIST_NOSIZE_003 = QObject::tr("SIZELIST_NOSIZE_003");
    temp_translation.SIZELIST_NOSIZE_004 = QObject::tr("SIZELIST_NOSIZE_004");
    temp_translation.SIZELIST_NOSIZE_027 = QObject::tr("SIZELIST_NOSIZE_027");
    temp_translation.SIZELIST_NOSIZE_029 = QObject::tr("SIZELIST_NOSIZE_029");
    temp_translation.SIZELIST_NOSIZE_031 = QObject::tr("SIZELIST_NOSIZE_031");
    temp_translation.SIZELIST_NOSIZE_032 = QObject::tr("SIZELIST_NOSIZE_032");
    temp_translation.SIZELIST_NOSIZE_038 = QObject::tr("SIZELIST_NOSIZE_038");
    temp_translation.SIZELIST_NOSIZE_039 = QObject::tr("SIZELIST_NOSIZE_039");
    temp_translation.SIZELIST_NOSIZE_040 = QObject::tr("SIZELIST_NOSIZE_040");
    temp_translation.SIZELIST_NOSIZE_041 = QObject::tr("SIZELIST_NOSIZE_041");
    temp_translation.SIZELIST_NOSIZE_042 = QObject::tr("SIZELIST_NOSIZE_042");
    temp_translation.SIZELIST_NOSIZE_043 = QObject::tr("SIZELIST_NOSIZE_043");
    temp_translation.A_UI_003 = QObject::tr("A_UI_003");
    temp_translation.A_UI_005 = QObject::tr("A_UI_005");
    temp_translation.A_UI_006 = QObject::tr("A_UI_006");
    temp_translation.A_SCAN_001 = QObject::tr("A_SCAN_001");
    temp_translation.A_UNIT_001 = QObject::tr("A_UNIT_001");
    temp_translation.A_UNIT_007 = QObject::tr("A_UNIT_007");
    temp_translation.A_UNIT_008 = QObject::tr("A_UNIT_008");
    temp_translation.A_UNIT_009 = QObject::tr("A_UNIT_009");
    temp_translation.C_UI_002 = QObject::tr("C_UI_002");
    temp_translation.C_UI_004 = QObject::tr("C_UI_004");
    temp_translation.C_UI_005 = QObject::tr("C_UI_005");
    temp_translation.C_UI_006 = QObject::tr("C_UI_006");
    temp_translation.C_UI_101 = QObject::tr("C_UI_101");
    temp_translation.C_UI_102 = QObject::tr("C_UI_102");
    temp_translation.C_UNIT_001 = QObject::tr("C_UNIT_001");
    temp_translation.C_UNIT_002 = QObject::tr("C_UNIT_002");
    temp_translation.C_UNIT_003 = QObject::tr("C_UNIT_003");
    temp_translation.E_UI_103 = QObject::tr("E_UI_103");
    temp_translation.E_UI_109 = QObject::tr("E_UI_109");
    temp_translation.E_SAVE_001 = QObject::tr("E_SAVE_001");
    temp_translation.E_SAVE_004 = QObject::tr("E_SAVE_004");
    temp_translation.E_COMM_001 = QObject::tr("E_COMM_001");
    temp_translation.E_COMM_102 = QObject::tr("E_COMM_102");
    temp_translation.E_COMM_103 = QObject::tr("E_COMM_103");
    temp_translation.E_UNIT_008 = QObject::tr("E_UNIT_008");
    temp_translation.E_ENV_001 = QObject::tr("E_ENV_001");
    temp_translation.E_PARA_001 = QObject::tr("E_PARA_001");
    temp_translation.E_PARA_005 = QObject::tr("E_PARA_005");
    temp_translation.E_PARA_003 = QObject::tr("E_PARA_003");
    temp_translation.E_PARA_011 = QObject::tr("E_PARA_011");
    temp_translation.E_PARA_013 = QObject::tr("E_PARA_013");
    temp_translation.CUSTOMSIZE_008 = QObject::tr("CUSTOMSIZE_008");
    temp_translation.CUSTOMSIZE_009 = QObject::tr("CUSTOMSIZE_009");
    temp_translation.CUSTOMSIZE_010 = QObject::tr("CUSTOMSIZE_010");
    temp_translation.CONFIG_GENERAL_003 = QObject::tr("CONFIG_GENERAL_003");
    temp_translation.CONFIG_GENERAL_004 = QObject::tr("CONFIG_GENERAL_004");
    temp_translation.CONFIG_DOC_004 = QObject::tr("CONFIG_DOC_004");
    temp_translation.CONFIG_DOC_005 = QObject::tr("CONFIG_DOC_005");
    temp_translation.CONFIG_DOC_010 = QObject::tr("CONFIG_DOC_010");
    temp_translation.CONFIG_DOC_013 = QObject::tr("CONFIG_DOC_013");
    temp_translation.CONFIG_DOC_014 = QObject::tr("CONFIG_DOC_014");
    temp_translation.CONFIG_DOC_019 = QObject::tr("CONFIG_DOC_019");
    temp_translation.CONFIG_DOC_020 = QObject::tr("CONFIG_DOC_020");
    temp_translation.CONFIG_DOC_021 = QObject::tr("CONFIG_DOC_021");
    temp_translation.CONFIG_DOC_022 = QObject::tr("CONFIG_DOC_022");
    temp_translation.CONFIG_DOC_023 = QObject::tr("CONFIG_DOC_023");
    temp_translation.CONFIG_DOC_024 = QObject::tr("CONFIG_DOC_024");
    temp_translation.CONFIG_SCAN_008 = QObject::tr("CONFIG_SCAN_008");
    temp_translation.CONFIG_SCAN_009 = QObject::tr("CONFIG_SCAN_009");
    temp_translation.PROGRESS_003 = QObject::tr("PROGRESS_003");
    temp_translation.PROGRESS_005 = QObject::tr("PROGRESS_005");
    temp_translation.PROGRESS_007 = QObject::tr("PROGRESS_007");
    temp_translation.PROGRESS_008 = QObject::tr("PROGRESS_008");
    temp_translation.FILESAVESETTING_003 = QObject::tr("FILESAVESETTING_003");
    temp_translation.FILESAVESETTING_004 = QObject::tr("FILESAVESETTING_004");
    temp_translation.FILESAVESETTING_005 = QObject::tr("FILESAVESETTING_005");
    temp_translation.FILESAVESETTING_006 = QObject::tr("FILESAVESETTING_006");
    temp_translation.FILESAVESETTING_007 = QObject::tr("FILESAVESETTING_007");
    temp_translation.FILESAVESETTING_009 = QObject::tr("FILESAVESETTING_009");
    temp_translation.FILESAVESETTING_016 = QObject::tr("FILESAVESETTING_016");
    temp_translation.FILESAVESETTING_012 = QObject::tr("FILESAVESETTING_012");
    temp_translation.MENU_001 = QObject::tr("MENU_001");
    temp_translation.SCANNERSETTING_005 = QObject::tr("SCANNERSETTING_005");
    temp_translation.SCANNERSETTING_006 = QObject::tr("SCANNERSETTING_006");
    temp_translation.VERSION_003 = QObject::tr("VERSION_003");
    temp_translation.CONFIRMCONTINUE_002 = QObject::tr("CONFIRMCONTINUE_002");
    temp_translation.CONFIRMCONTINUE_003 = QObject::tr("CONFIRMCONTINUE_003");
    temp_translation.CONFIRMCONTINUE_004 = QObject::tr("CONFIRMCONTINUE_004");
    temp_translation.CONFIRMCONTINUE_005 = QObject::tr("CONFIRMCONTINUE_005");
    temp_translation.CONFIRMCONTINUE_008 = QObject::tr("CONFIRMCONTINUE_008");
    temp_translation.CONFIRMCONTINUE_011 = QObject::tr("CONFIRMCONTINUE_011");
    temp_translation.CONFIRMCONTINUE_013 = QObject::tr("CONFIRMCONTINUE_013");
    temp_translation.CONFIRMCONTINUE_014 = QObject::tr("CONFIRMCONTINUE_014");
    temp_translation.CONFIRMCONTINUE_015 = QObject::tr("CONFIRMCONTINUE_015");
    temp_translation.LINUX_E_COMM_001 = QObject::tr("LINUX_E_COMM_001");
    temp_translation.LINUX_C_UI_001 = QObject::tr("LINUX_C_UI_001");
    temp_translation.LINUX_C_UI_002 = QObject::tr("LINUX_C_UI_002");
    temp_translation.LINUX_DEVICESELECT_003 = QObject::tr("LINUX_DEVICESELECT_003");
    temp_translation.LINUX_SIZELIST_NOSIZE_001 = QObject::tr("LINUX_SIZELIST_NOSIZE_001");
    temp_translation.LINUX_FILESAVESETTING_001 = QObject::tr("LINUX_FILESAVESETTING_001");
    temp_translation.LINUX_FILESAVESETTING_002 = QObject::tr("LINUX_FILESAVESETTING_002");
    temp_translation.LINUX_FILESAVESETTING_003 = QObject::tr("LINUX_FILESAVESETTING_003");
    temp_translation.LINUX_FILESAVESETTING_004 = QObject::tr("LINUX_FILESAVESETTING_004");
    temp_translation.LINUX_FILESAVESETTING_005 = QObject::tr("LINUX_FILESAVESETTING_005");
    temp_translation.LINUX_FILESAVESETTING_006 = QObject::tr("LINUX_FILESAVESETTING_006");
    temp_translation.LINUX_FILESAVESETTING_007 = QObject::tr("LINUX_FILESAVESETTING_007");
    temp_translation.LINUX_FILESAVESETTING_008 = QObject::tr("LINUX_FILESAVESETTING_008");
    temp_translation.LINUX_FILESAVESETTING_009 = QObject::tr("LINUX_FILESAVESETTING_009");
    temp_translation.LINUX_MAINSETTING_001 = QObject::tr("LINUX_MAINSETTING_001");
    temp_translation.PHOTO_MAIN_008 = QObject::tr("PHOTO_MAIN_008");
    temp_translation.PHOTO_MAIN_009 = QObject::tr("PHOTO_MAIN_009");
    temp_translation.PHOTO_MAIN_010 = QObject::tr("PHOTO_MAIN_010");
    temp_translation.PHOTO_MAIN_013 = QObject::tr("PHOTO_MAIN_013");
    temp_translation.PHOTO_MAIN_019 = QObject::tr("PHOTO_MAIN_019");
    temp_translation.PHOTO_MAIN_020 = QObject::tr("PHOTO_MAIN_020");
    this->translation_str = temp_translation;
}
