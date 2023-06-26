#pragma once
#include <string>
#include "Utils.h"
#include "NXPStructs.h"

// General FS entry
FSEntry::FSEntry(const std::string& locationPath, const std::string& fileName) {
    name = fileName;
    path = locationPath;
    absolutePath = path + "/" + name;
    if (pathIsDir(absolutePath)) entryType = Folder;
    // If file get properties
    else {
        struct stat fileInfo;
        stat(absolutePath.c_str(), &fileInfo);
        dateModified = fileInfo.st_mtime;
    }
}

// FS entry for devices
FSEntry::FSEntry(const std::string& deviceName) {
    name = deviceName + "/";
    absolutePath = deviceName;
    entryType = Device;
}