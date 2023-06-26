#pragma once
#include <string.h>
#include <NXPStructs.h>
#include <NXPUI.h>

namespace NXPState {
    inline std::string browserPath = "";
    inline FSFileList filesInCurrentPath;

    void changePath(std::string path);
}