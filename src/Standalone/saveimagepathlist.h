#ifndef SAVEIMAGEPATHLIST_H
#define SAVEIMAGEPATHLIST_H

#include "supervisor.h"

class SaveImagePathList
{
public:
    SaveImagePathList();

    static std::list<imageEditInfo> save_image_path_list;
};

#endif // SAVEIMAGEPATHLIST_H
