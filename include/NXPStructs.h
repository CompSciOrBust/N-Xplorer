#pragma once
#include <string>
#include <sys/stat.h>
#include <vector>

enum FSEntryType {Folder, Device, UnknownFile};

class FSEntry {
    public:
        FSEntry(const std::string& locationPath, const std::string& fileName);
        FSEntry(const std::string& deviceName);

        std::string name = "";
        std::string path = "";
        std::string absolutePath = "";
        FSEntryType entryType = UnknownFile;
        uint size = 0;
        uint dateModified = 0;
};

typedef std::vector<FSEntry> FSFileList;