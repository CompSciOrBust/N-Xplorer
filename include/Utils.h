#include <string>
#include <vector>
#include <stdio.h>
#include <dirent.h>
using namespace std;

vector <dirent> LoadDirs(string Path);
string GoUpDir(string);
bool CheckIsDir(string Path);
bool CheckFileExists(string);
string GetKeyboardInput(string, string, string);
string GetFileSize(string);