#include <string>
#include <vector>
#include <stdio.h>
#include <dirent.h>

std::vector <dirent> SortFiles(std::string, std::vector <dirent>, int);
std::vector <dirent> LoadDirs(std::string Path);
std::string GoUpDir(std::string);
bool CheckIsDir(std::string Path);
bool CheckFileExists(std::string);
std::string GetKeyboardInput(std::string, std::string, std::string);
std::string GetFileSize(std::string);
void RecursiveFileCopy(std::string, std::string, std::string);
std::string GetFileExtension(std::string);
bool GetParentalControl(void);
void UnzipFile(std::string, std::string);