#pragma once

#include "IInterface.h"

class InterfaceFactory
{
public:
    static ESErrorCode Create(const ES_CHAR* json, IInterface** ppInterface);
};
