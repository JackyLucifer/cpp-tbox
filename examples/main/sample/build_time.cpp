#include <main/main.h>

namespace tbox {
namespace main {

std::string GetAppBuildTime()
{
    return __DATE__ " " __TIME__;
}

}
}
