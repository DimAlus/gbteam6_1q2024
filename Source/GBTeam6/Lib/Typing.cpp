#include "./Typing.h"
#include <string>
#include <regex>

DEFINE_LOG_CATEGORY(LgGame);
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


FString* UStringCycledIterator::Next() {
    if (this->Iterable.Num() == 0) {
        return nullptr;
    }
    if (this->iter >= this->Iterable.Num()) {
        this->iter = 0;
    }
    return &this->Iterable[this->iter++];
}

FString* UStringCycledIterator::Prev() {
    if (this->Iterable.Num() == 0) {
        return nullptr;
    }
    this->iter = (this->iter - 1 + this->Iterable.Num()) % this->Iterable.Num();

    return &this->Iterable[this->iter]; 
}
