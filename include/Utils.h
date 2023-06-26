#pragma once
#include <string>
#include <vector>
#include <dirent.h>
#include <NXPStructs.h>

bool pathIsDir(std::string path);
FSFileList loadFiles(std::string Path);