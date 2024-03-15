#include "./Typing.h"
#include <string>
#include <regex>

DEFINE_LOG_CATEGORY(LgPlayer);
DEFINE_LOG_CATEGORY(LgService);
DEFINE_LOG_CATEGORY(LgComponent);
DEFINE_LOG_CATEGORY(LgObject);

FString GetLevelName(ULevel* level) {
    FString path = level->GetPathName();
    std::string pathStr;

#ifndef UNICODE
    pathStr = *path;
#else
    std::wstring wStr = *path;
    pathStr = std::string(wStr.begin(), wStr.end());
#endif

    const std::regex r(R"(\.(\w+):)");
    std::smatch m;
    if (std::regex_search(pathStr, m, r)) {
        return FString(m[1].str().c_str());
    }
    UE_LOG(LgService, Error, TEXT("Failde get name of Level!"));

    path = path.Replace(TEXT("/"), TEXT("_")).Replace(TEXT(":"), TEXT("-")).Replace(TEXT(" "), TEXT("--"));
    return path;
}
