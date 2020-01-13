#include <string>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <switch.h>
#include <fstream>

std::vector <dirent> LoadDirs(std::string Path)
{
	std::vector <dirent> Files(0);
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

std::string GoUpDir(std::string Path)
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

bool CheckIsDir(std::string Path)
{
	struct stat statbuf;
	stat(Path.c_str(), &statbuf);
	return S_ISDIR(statbuf.st_mode);
}

//Stolen from https://stackoverflow.com/a/12774387
bool CheckFileExists(std::string Path)
{
	struct stat buffer;   
	return (stat (Path.c_str(), &buffer) == 0); 
}

std::string GetKeyboardInput(std::string OkButtonText, std::string GuideText, std::string InitialText)
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

//Stolen from https://stackoverflow.com/a/6039648
std::string GetFileSize(std::string Path)
{
	if(CheckIsDir(Path)) return "DIR";
	struct stat stat_buf;
    stat(Path.c_str(), &stat_buf);
	if(stat_buf.st_size > 1073741824) return std::to_string(stat_buf.st_size / 1073741824) + " GB";
	else if(stat_buf.st_size > 1048576) return std::to_string(stat_buf.st_size / 1048576) + " MB";
	else if(stat_buf.st_size > 1024) return std::to_string(stat_buf.st_size / 1024) + " KB";
	else return std::to_string(stat_buf.st_size) + " B";
}

//This should really be threaded
//Mostly taken from before the rewrite
void RecursiveFileCopy(std::string SourcePath, std::string DestPath, std::string FileName)
{
	//If dir
	if(CheckIsDir(SourcePath))
	{
		//Don't copy a dir in to it's self
		if(DestPath.find(SourcePath) == 0) return;
		//Make the new dir
		std::string NewDirPath = DestPath + "/" + FileName;
		mkdir(NewDirPath.c_str(), 0);
		//For each file in the source path call this function
		DIR* dir;
		struct dirent* ent;
		dir = opendir(SourcePath.c_str());
		while ((ent = readdir(dir)))
		{
			//Get path of file we want to copy
			std::string PathToCopy = SourcePath + "/" +ent->d_name;
			RecursiveFileCopy(PathToCopy.c_str(), NewDirPath.c_str(), ent->d_name);
		}
		closedir(dir);
	}
	//If file just copy it
	else
	{
		//Stolen from https://stackoverflow.com/a/10195497 because <filesystem> is broken with the toolchain
		std::string PathToCopyTo = DestPath + "/" + FileName;
		std::ifstream src(SourcePath.c_str(), std::ios::binary);
		std::ofstream dst(PathToCopyTo.c_str(), std::ios::binary);
		dst << src.rdbuf();
	}
}