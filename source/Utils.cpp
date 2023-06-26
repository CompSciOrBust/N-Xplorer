#include "Utils.h"
#include <string>
#include "NXPStructs.h"
#include <vector>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>

bool pathIsDir(std::string path){
    struct stat buffer;
    stat(path.c_str(), &buffer);
    return S_ISDIR(buffer.st_mode);
}

FSFileList loadFiles(std::string Path){
    // If no path given, add in the base devices
    FSFileList fileList;
    if (Path == "") {
        fileList.push_back(FSEntry("sdmc:"));
        return fileList;
    }
    // Init vars
    DIR* dirStream = opendir(Path.c_str());
    struct dirent* dirEntry;
    // Loop over every dirent in the path
    while (dirEntry = readdir(dirStream)) {
        FSEntry newEntry = FSEntry(Path, dirEntry->d_name);
        fileList.push_back(newEntry);
    }
    // Return
    return fileList;
}