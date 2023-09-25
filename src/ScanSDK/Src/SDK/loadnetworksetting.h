#include "PathUtils.h"
#include <string>
#include <fstream>

static bool GetNetworkDevcicesFromIni(std::list<SDIDeviceInfo>& devices)
{
    if(strlen(getenv("HOME")) > 1023){
        return false;
    }
    std::string strDst = getenv("HOME");

    //std::string strDst(qPrintable(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)));
#ifdef AKBMODE
    std::string dir_name = ".";
    dir_name = dir_name + DRIVER_NAME;
    dir_name = dir_name + "/Network/";
    dir_name = dir_name + DRIVER_NAME;
    dir_name = dir_name + ".conf";
    ES_CMN_FUNCS::PATH::ES_CombinePath(strDst, strDst, dir_name.c_str());
#else
    ES_CMN_FUNCS::PATH::ES_CombinePath(strDst, strDst, ".epsonscan2/Network/epsonscan2.conf");
#endif
    std::ifstream netSettingFile(strDst.c_str());
    SDIDeviceInfo devInfo;
    memset(&devInfo, 0, sizeof(SDIDeviceInfo));

    char ip_row[256];
    std::string ip;

    if (netSettingFile.fail())
    {
        return false;
    }
    //Networkセクションまでスキップ
    while (netSettingFile.getline(ip_row, 256 - 1))
    {
        if(strcmp(ip_row, "[Network]") == 0){
            break;
        }
    }

    devices.clear();

    while (netSettingFile.getline(ip_row, 256 - 1))
    {

        ip = ip_row;
        ip.erase(std::remove(ip.begin(), ip.end(), ' '), ip.end());  //行に含まれる空白文字を削除する

        //コメントはスキップする
        if(ip[0] == '#' || ip[0] == ';'){
            continue;
        }
        //空文字はスキップする
        if ( !ip.size() ) continue;

        //文字数制限オーバー時設定ファイルエラーに
        if (ip.size() > MAX_IP_ADDR) return false;

        //同じIPアドレスがある場合ははじく
        auto device = devices.begin();
        int i = 0;
        while(i < devices.size()){
            if(device->ipAddress == ip){
                break;
            }
            ++device;
            i++;
        }
        if(device == devices.end()){

            memset(devInfo.ipAddress, 0, MAX_IP_ADDR);
            memset(devInfo.displayName, 0, MAX_DISPLAYNAME);
            devInfo.productID = 0;

            memcpy(devInfo.ipAddress, ip.c_str(), std::min(sizeof(devInfo.ipAddress), ip.size()));
            devices.push_back(devInfo);
        }
    }

    return true;
}
