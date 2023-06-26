#include <NXPState.h>
#include <Utils.h>
#include <NXPUI.h>

namespace NXPState {
    void changePath(std::string path) {
        browserPath = path;
        filesInCurrentPath = loadFiles(path);
        NXP::UI::pathChanged();
    }
}