#ifndef COMMON_UTILITY_H
#define COMMON_UTILITY_H

#endif // COMMON_UTILITY_H

#include <string>

class StandAloneCommonUtility
{
public:

    char *StrShift( char *String, size_t nShift )
    {
        char *start = String;
        char *stop  = String + strlen( String );
        memmove( start + nShift, start, stop-start+1 );

        return String + nShift;
    }//StrShift


    std::string ReplaceString( char* String, const char *From, const char* To)
    {
           int StrLen = strlen( To ) + strlen( String );
           char destStr[StrLen + 1];
           memset(destStr, 0, StrLen + 1);
           memcpy(destStr, String, strlen( String ));

           StrReplace(destStr, From , To);

           return std::string(destStr);
    }

    void StrReplace( char* String, const char *From, const char* To )
    {
        int   nToLen;   // 置換する文字列の長さ
        int   nFromLen; // 検索する文字列の長さ
        int   nShift;
        char *start;    // 検索を開始する位置
        char *stop;     // 文字列 String の終端
        char *p;

        nToLen   = strlen( To );
        nFromLen = strlen( From );
        nShift   = nToLen - nFromLen;
        start    = String;
        stop     = String + strlen( String );

        // 文字列 String の先頭から文字列 From を検索
        while( NULL != ( p = strstr( start, From ) ) )
        {
            // 文字列 To が複写できるようにする
            start = StrShift( p + nFromLen, nShift );
            stop  = stop + nShift;

            // 文字列 To を複写
            memmove( p, To, nToLen );
        }
    }//StrReplace

    double PixelToMm(double input_num, int resolution)
    {
        int temp;
        input_num = input_num / (double)resolution * 25.4;
        temp = input_num * 10 + 5;
        return temp / 10;
    }//PixelToMm

    double PixelToInchi(double input_num, int resolution)
    {
        return input_num / (double)resolution;
    }//PixelToInchi

    double MmToInchi(double input_num, int resolution)
    {
        int temp;
        double mpp = 25.4 / (double)resolution;
        temp = (input_num / mpp / (double)resolution * 1000 + 5) / 10;
        return temp;
    }

    double MmToPixel(double value, int res)
    {
        double mpp = 25.4 / (double)res;
        return value / mpp;
    }

    double InchiToPixel(double value, int res)
    {
        return value * (double)res;
    }

    bool search_item_list(int item, int32_t item_list[], int32_t list_size)
    {
        int i = 0;
        while(i < list_size){
            if(item_list[i] == item){
                return true;
            }
            i++;
        }
        return false;
    }
};

static StandAloneCommonUtility common_utility;
