#include "./GeneralFunctionLibrary.h"


FHardwareCursorData UGeneralFunctionLibrary::MakeHardwareCursorData(FName cursorPath, FVector2D hotSpot) {
    FHardwareCursorData data;
    data.CursorPath = cursorPath;
    data.HotSpot = hotSpot;
    return data;
}