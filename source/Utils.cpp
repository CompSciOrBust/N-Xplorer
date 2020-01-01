#include <string>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <switch.h>
using namespace std;

vector <dirent> LoadDirs(string Path)
{
	vector <dirent> Files(0);
	DIR* dir;
	struct dirent* ent;
	dir = opendir(Path.c_str());
	Files.clear();
	while ((ent = readdir(dir)))
	{
		Files.push_back(*ent);
	}
	closedir(dir);
	return Files;
}

string GoUpDir(string Path)
{
	char CurrentPath[Path.length()] = "";
	strcat(CurrentPath, Path.c_str());
	CurrentPath[Path.length()-1] = 0x00;
	Path = CurrentPath;
	int LastSlash = Path.find_last_of('/');
	CurrentPath[LastSlash] = 0x00;
	if(strlen(CurrentPath) < 8)
	{
		return "sdmc:/";
	}
	return CurrentPath;
}

bool CheckIsDir(string Path)
{
	struct stat statbuf;
	stat(Path.c_str(), &statbuf);
	return S_ISDIR(statbuf.st_mode);
}

//Stolen from https://stackoverflow.com/a/12774387
bool CheckFileExists(string Path)
{
	struct stat buffer;   
	return (stat (Path.c_str(), &buffer) == 0); 
}

string GetKeyboardInput(string OkButtonText, string GuideText, string InitialText)
{
	SwkbdConfig kbd;
	swkbdCreate(&kbd, 0);
	swkbdConfigMakePresetDefault(&kbd);
	
	char tmpoutstr[255] = {0};
	swkbdConfigSetOkButtonText(&kbd, OkButtonText.c_str());
	swkbdConfigSetGuideText(&kbd, GuideText.c_str());
	swkbdConfigSetInitialText(&kbd, InitialText.c_str());
	swkbdShow(&kbd, tmpoutstr, sizeof(tmpoutstr));
	return tmpoutstr;
}

//tolen from https://stackoverflow.com/a/6039648
string GetFileSize(string Path)
{
	if(CheckIsDir(Path)) return "DIR";
	struct stat stat_buf;
    stat(Path.c_str(), &stat_buf);
	if(stat_buf.st_size > 1073741824) return to_string(stat_buf.st_size / 1073741824) + " GB";
	else if(stat_buf.st_size > 1048576) return to_string(stat_buf.st_size / 1048576) + " MB";
	else if(stat_buf.st_size > 1024) return to_string(stat_buf.st_size / 1024) + " KB";
	else return to_string(stat_buf.st_size) + " B";
}