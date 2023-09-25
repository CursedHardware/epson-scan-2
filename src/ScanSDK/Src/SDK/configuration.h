#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "supervisor.h"

class Configuration
{
public:
    Configuration();
    void SetConfiguration(CONFIGURATION _config);


    static Configuration& GetConfiguration();

    CONFIGURATION config_para;
};

#endif // CONFIGURATION_H

/*
exp{

   Configuration::GetConfiguration().SetInt(10)

}
*/
