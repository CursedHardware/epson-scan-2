#ifndef SAVEDESTFOLDER_H
#define SAVEDESTFOLDER_H

#include "supervisor.h"

class SaveDestFolder
{
public:
    SaveDestFolder();
    bool open_folder(Supervisor* sv);
    std::string ReplaceString(std::string String1, std::string String2, std::string String3);
};

#endif // SAVEDESTFOLDER_H
