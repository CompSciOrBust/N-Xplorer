#include <string>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <switch.h>
#include <fstream>
#include <algorithm>
#include <minizip/unzip.h>

std::vector <dirent> SortFiles(std::string Path, std::vector <dirent> FilesVec, int SortMode)
{
	switch(SortMode)
	{
		//Sort by file size (biggest to smallest)
		case 0:
		std::sort(FilesVec.begin(), FilesVec.end(), [Path](dirent A, dirent B) -> bool{
			struct stat ABuf;
			struct stat BBuf;
			std::string APath = Path + A.d_name;
			std::string BPath = Path + B.d_name;
			stat(APath.c_str(), &ABuf);
			stat(BPath.c_str(), &BBuf);
			if(S_ISDIR(ABuf.st_mode)) return false;
			return ABuf.st_size > BBuf.st_size;
		});
		break;
		//Sort by file size (smallest to biggest)
		case 1:
		std::sort(FilesVec.begin(), FilesVec.end(), [Path](dirent A, dirent B) -> bool{
			struct stat ABuf;
			struct stat BBuf;
			std::string APath = Path + A.d_name;
			std::string BPath = Path + B.d_name;
			stat(APath.c_str(), &ABuf);
			stat(BPath.c_str(), &BBuf);
			unsigned int ASize = 4294967295;
			unsigned int BSize = 4294967295;
			if(!S_ISDIR(ABuf.st_mode)) ASize = ABuf.st_size;
			if(!S_ISDIR(BBuf.st_mode)) BSize = BBuf.st_size;
			return ASize < BSize;
		});
		break;
		//Sort by file name A-Z
		case 2:
		std::sort(FilesVec.begin(), FilesVec.end(), [](dirent A, dirent B) -> bool{
			int MaxLength = 0;
			if(sizeof(A.d_name) > sizeof(B.d_name)) MaxLength = sizeof(A.d_name);
			else MaxLength = sizeof(B.d_name);
			int Itterate = 0;
			while(Itterate < MaxLength)
			{
				if(tolower(A.d_name[Itterate]) != tolower(B.d_name[Itterate])) break;
				else Itterate++;
			}
			return tolower(A.d_name[Itterate]) < tolower(B.d_name[Itterate]);
		});
		break;
		//Sort by file name Z-A
		case 3:
		std::sort(FilesVec.begin(), FilesVec.end(), [](dirent A, dirent B) -> bool{
			int MaxLength = 0;
			if(sizeof(A.d_name) > sizeof(B.d_name)) MaxLength = sizeof(A.d_name);
			else MaxLength = sizeof(B.d_name);
			int Itterate = 0;
			while(Itterate < MaxLength)
			{
				if(tolower(A.d_name[Itterate]) != tolower(B.d_name[Itterate])) break;
				else Itterate++;
			}
			return tolower(A.d_name[Itterate]) > tolower(B.d_name[Itterate]);
		});
		break;
	}
	return FilesVec;
}

std::vector <dirent> LoadDirs(std::string Path)
{
	std::vector <dirent> Files(0);
	DIR* dir;
	struct dirent* ent;
	dir = opendir(Path.c_str());
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
	if(LastSlash == -1)
	{
		return "mount:";
	}
	CurrentPath[LastSlash] = 0x00;
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
	else if(CheckFileExists(SourcePath.c_str()))
	{
		//get the full path of the destination file
		std::string PathToCopyTo = DestPath + "/" + FileName;
		//Get files as streams
		std::ifstream SourceFile (SourcePath, std::ifstream::binary);
		std::ofstream DestFile (PathToCopyTo, std::ofstream::binary);
		//Get file size
		SourceFile.seekg (0,SourceFile.end);
		long size = SourceFile.tellg();
		SourceFile.seekg (0);
		//Create a 0.25 gb buffer
		int Chunksize = 1024 * 1024 * 256;
		char* Buffer = new char[Chunksize];
		//If the file is smaller than the buffer change the chunk size
		if(size < Chunksize) Chunksize = size;
		//Calculate how much of the file will be left over when split in to chunks
		int LeftOvers = size % Chunksize;
		while(SourceFile.tellg() != size - LeftOvers)
		{
			//Copy the source in to the buffer
			SourceFile.read(Buffer,Chunksize);
			//Writet he buffer in to the destination file
			DestFile.write(Buffer,Chunksize);
		}
		//Copy the left over bytes that didn't fit evenly in to a buffer
		SourceFile.read(Buffer,LeftOvers);
		DestFile.write(Buffer,LeftOvers);
		//Clean up
		delete[] Buffer;
		DestFile.close();
		SourceFile.close();
	}
}

std::string GetFileExtension(std::string Path)
{
	int ExtensionStart = Path.find_last_of(".")+1;
	std::string FileSuffix;
	FileSuffix.assign(Path, ExtensionStart, Path.size() - ExtensionStart);
	return FileSuffix;
}

std::string GetFileNameFromPath(std::string Path)
{
	int NameStart = Path.find_last_of("/")+1;
	int ExtensionStart = Path.find_last_of(".");
	std::string FileName;
	FileName.assign(Path, NameStart, ExtensionStart - NameStart);
	return FileName;
}

bool GetParentalControl()
{
	bool ParentalControlIsEnabled = false;
	pctlInitialize();
	pctlIsRestrictionEnabled(&ParentalControlIsEnabled);
	pctlExit();
	if(ParentalControlIsEnabled)
	{
		Result rc = pctlauthShow(true);
		if(R_SUCCEEDED(rc))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

//Based on https://github.com/ITotalJustice/atmosphere-updater/blob/master/source/unzip.c
void UnzipFile(std::string FileLocation, std::string LocationToUnzipTo)
{
	unzFile ZipFile = unzOpen(FileLocation.c_str());
	//Get info about the zip
	unz_global_info ZipInfo;
	unzGetGlobalInfo(ZipFile, &ZipInfo);
	//Loop through every file
	for(int i = 0; i < ZipInfo.number_entry; i++)
	{
		char FileNameInZip[256];
		unz_file_info FileInfo;
		unzOpenCurrentFile(ZipFile);
		unzGetCurrentFileInfo(ZipFile, &FileInfo, FileNameInZip, sizeof(FileNameInZip), NULL, 0, NULL, 0);
		//Check if a dir
		if ((FileNameInZip[strlen(FileNameInZip) - 1]) == '/')
		{
			std::string DirPa = LocationToUnzipTo.c_str();
			DirPa += FileNameInZip;
			mkdir(DirPa.c_str(), 0);
		}
		else
		{
			const char *write_filename = FileNameInZip;
			void *buf = malloc(500000);
			FILE *outfile;
			std::string FilePathOfFile = LocationToUnzipTo.c_str();
			FilePathOfFile += write_filename;
			outfile = fopen(FilePathOfFile.c_str(), "wb");
			for (int j = unzReadCurrentFile(ZipFile, buf, 500000); j > 0; j = unzReadCurrentFile(ZipFile, buf, 500000))
			{
				fwrite(buf, 1, j, outfile);
			}
			fclose(outfile);
			free(buf);
		}
		unzCloseCurrentFile(ZipFile);
		unzGoToNextFile(ZipFile);
	}
	unzClose(ZipFile);
}