#include "savedestfolder.h"
#include "PathUtils.h"

SaveDestFolder::SaveDestFolder()
{

}

std::string SaveDestFolder::ReplaceString
(
      std::string String1   // 置き換え対象
    , std::string String2   // 検索対象
    , std::string String3   // 置き換える内容
)
{
    std::string::size_type  Pos( String1.find( String2 ) );

    while( Pos != std::string::npos )
    {
        String1.replace( Pos, String2.length(), String3 );
        Pos = String1.find( String2, Pos + String3.length() );
    }

    return String1;
}

bool SaveDestFolder::open_folder(Supervisor* sv)
{
    //メジャーなファイラーがインストールされている場合、それを使用してフォルダを表示する
    char *failer = NULL;
    char *cmd = NULL;
    int cmd_size = 0;


    //保存先のディレクトリが存在しているか確認する
    const bool result = ES_CMN_FUNCS::PATH::ES_IsExistFile(sv->device_data.SelectPath);
    if (result) {
        //同じディレクトリを保存後に何度も開いてしまうためコマンドで開く仕様に
        //上に記載されているほど使用優先度が高い
        if(system("which nautilus > /dev/null") == 0){
            cmd_size = strlen("nautilus ");
            failer = (char *)malloc(cmd_size+1);
            if(failer){
               memset(failer, 0, cmd_size+1);
               memcpy(failer, "nautilus", cmd_size);
            }
        }else if(system("which dolphin > /dev/null") == 0){
            cmd_size = strlen("dolphin ");
            failer = (char *)malloc(cmd_size+1);
            if(failer){
               memset(failer, 0, cmd_size+1);
               memcpy(failer, "dolphin", cmd_size);
            }
        }else if(system("which caja > /dev/null") == 0){
            cmd_size = strlen("caja ");
            failer = (char *)malloc(cmd_size+1);
            if(failer){
               memset(failer, 0, cmd_size+1);
               memcpy(failer, "caja", cmd_size);
            }
        }else if(system("which dde-file-manager > /dev/null") == 0){
            cmd_size = strlen("dde-file-manager ");
            failer = (char *)malloc(cmd_size+1);
            if(failer){
               memset(failer, 0, cmd_size+1);
               memcpy(failer, "dde-file-manager", cmd_size);
            }
        }else if(system("which peony > /dev/null") == 0){
            cmd_size = strlen("peony ");
            failer = (char *)malloc(cmd_size+1);
            if(failer){
               memset(failer, 0, cmd_size+1);
               memcpy(failer, "peony", cmd_size);
            }
        }else {
            //
            return true;
        }
        cmd_size +=1;// " " space
        cmd_size += strlen(sv->device_data.SelectPath.c_str());

        cmd_size += 2;//" &"
        cmd_size += 2;//"\"\""

        cmd = (char *)malloc(cmd_size + 1);
        if(cmd){
           memset(cmd, 0, cmd_size);
        }else {
            if(failer){
                free(failer);
                failer = NULL;
            }
            return false;
        }
        //エスケープ対応
        std::string SelectPath = sv->device_data.SelectPath;
        SelectPath = ReplaceString(SelectPath.c_str(), "\"", "\\\"");
        //SelectPath = ReplaceString(SelectPath.c_str(), "\$", "\\\$");

        if(failer && cmd){
            sprintf(cmd , "%s \"%s\" &" , failer, SelectPath.c_str()) ;
            system(cmd);

            free(failer);
            failer = NULL;

            free(cmd);
            cmd = NULL;
        }else {
            if(cmd){
                free(cmd);
                cmd = NULL;
            }
            return false;
        }

        return true;
    }else {
        return false;
    }
}
