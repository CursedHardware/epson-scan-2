#pragma once

#include <mutex>

namespace epsonscan2 {
namespace es2command {

class Environment
{
private:
    Environment();
public:
    static bool IsCommandRetry();
    static bool IsAllowStatusAuthError();
};

} // namespace es2command
} // namespace epsonscan2