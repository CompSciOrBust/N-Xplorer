#include <SDL.h>
#include <UI.h>
#include <string>
#include <switch.h>
#include <Utils.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <mutex>
#include <unordered_map>
using namespace std;

class ExplorerUI : public UIWindow
{
	private:
	//vars
	int HeaderHeight = 55;
	int FooterHeight = 50;
	TTF_Font *HeaderFooterFont;
	std::mutex ListAccesMutex;
	std::vector <std::string> SaveMounts = std::vector <std::string>(0);
	std::unordered_map <std::string, u64> SaveMountMap = std::unordered_map <std::string, u64>(0);
	AccountUid CurrentUuid;
	//functions
	std::vector <std::string> GetSaveDataMounts();
	public:
	//vars
	ScrollList *FileNameList;
	ScrollList *FileSizeList;
	std::vector <dirent> Files = std::vector <dirent>(0);
	std::string HighlightedPath = "";
	std::string *ChosenFile;
	std::string DirPath = "mount:/";
	std::string CurrentMount;
	std::string ClipBoardMount;
	std::string *LongOpMessagePtr;
	int FileSortMode = 0;
	int HeaderColour_R = 94;
	int HeaderColour_G = 94;
	int HeaderColour_B = 94;
	int HeaderTextColour_R = 255;
	int HeaderTextColour_G = 255;
	int HeaderTextColour_B = 255;
	int FooterColour_R = 94;
	int FooterColour_G = 94;
	int FooterColour_B = 94;
	int FooterTextColour_R = 255;
	int FooterTextColour_G = 255;
	int FooterTextColour_B = 255;
	int BGColour_R = 44;
	int BGColour_G = 44;
	int BGColour_B = 44;
	//functions
	ExplorerUI();
	void GetInput();
	void DrawUI();
	void OpenFile(std::string);
	void LoadListDirs(std::string);
	void DrawHeader();
	void DrawFooter();
	void GoToIndexOfFile(std::string);
	void ChangeFileSortMode();
};

ExplorerUI::ExplorerUI()
{
	//Set up shared font
	PlFontData standardFontData;
	plGetSharedFontByType(&standardFontData, PlSharedFontType_Standard);
	
	//Set up the file lists
	//File name
	FileNameList = new ScrollList();
	//FileNameList->TouchListX = &TouchX;
	//FileNameList->TouchListY = &TouchY;
	FileNameList->ListFont = GetSharedFont(30); //Load the list font
	FileNameList->ListingsOnScreen = 15;
	FileNameList->ListHeight = Height - HeaderHeight - FooterHeight;
	FileNameList->ListWidth = Width * 0.85;
	FileNameList->ListYOffset = HeaderHeight;
	FileNameList->IsActive = true;
	//File size
	FileSizeList = new ScrollList();
	//FileSizeList->TouchListX = &TouchX;
	//FileSizeList->TouchListY = &TouchY;
	FileSizeList->Enslave(FileNameList);
	FileSizeList->ListFont = FileNameList->ListFont;
	FileSizeList->ListingsOnScreen = 15;
	FileSizeList->ListHeight = Height - HeaderHeight - FooterHeight;
	FileSizeList->ListWidth = Width * 0.15;
	FileSizeList->ListYOffset = HeaderHeight;
	FileSizeList->ListXOffset = FileNameList->ListWidth;
	FileSizeList->CenterText = true;
	FileSizeList->IsActive = true;
	//Populate listsand set up save data mounting
	accountGetPreselectedUser(&CurrentUuid);
	SaveMounts = GetSaveDataMounts();
	LoadListDirs(DirPath);
	HeaderFooterFont = GetSharedFont(40);
}

void ExplorerUI::GetInput()
{
	//ScanInput
	while (SDL_PollEvent(Event))
	{
		 switch (Event->type)
		 {
			 //Joycon button down
			  case SDL_JOYBUTTONDOWN:
			  {
				  if (Event->jbutton.which == 0)
				  {
					  //Y pressed
					  if(Event->jbutton.button == 3 && DirPath != "mount:/")
					  {
						  //Update highlighted file if one exists
						  if(!FileNameList->ListingTextVec.empty())
						  {
							  HighlightedPath = DirPath + Files.at(FileNameList->SelectedIndex).d_name;
						  }
						  else
						  {
							  HighlightedPath = "";
						  }
						  *WindowState = 1;
					  }
					  //Plus pressed
					  else if(Event->jbutton.button == 10)
					  {
						  *IsDone = 1;
					  }
					  //Minus pressed
					  else if(Event->jbutton.button == 11)
					  {
						  *WindowState = 6;
					  }
					  //Up pressed
					  else if(Event->jbutton.button == 13)
					  {
						  FileNameList->MoveUp();
					  }
					  //Down pressed
					  else if(Event->jbutton.button == 15)
					  {
						  FileNameList->MoveDown();
					  }
					  //Left pressed
					  else if(Event->jbutton.button == 12)
					  {
						  FileNameList->JumpUp();
					  }
					  //Right pressed
					  else if(Event->jbutton.button == 14)
					  {
						  FileNameList->JumpDown();
					  }
					  //A pressed
					  else if(Event->jbutton.button == 0)
					  {
						  //If not in a partition show partition list
						  if(DirPath == "mount:/")
						  {
							DirPath = FileNameList->ListingTextVec.at(FileNameList->SelectedIndex);
							if(DirPath == "sdmc:/")
							{
								LoadListDirs(DirPath);
								//Reset the cursor
								FileNameList->ResetPos();
							}
							else if(FileSizeList->ListingTextVec.at(FileNameList->SelectedIndex) == "SAVE")
							{
								printf("%s\n", DirPath.c_str());
								u64 AppID = SaveMountMap[DirPath];
								char UniqueSaveMount[7];
								sprintf(UniqueSaveMount, "Save_%03i", FileNameList->SelectedIndex - 3);
								DirPath = UniqueSaveMount;
								printf("%lu\n", AppID);
								fsdevMountSaveData(DirPath.c_str(), AppID, CurrentUuid);
								DirPath += ":/";
								LoadListDirs(DirPath);
								CurrentMount = DirPath;
							}
							else if(GetParentalControl())
							{
								LoadListDirs(DirPath);
								//Reset the cursor
								FileNameList->ResetPos();
							}
							else
							{
								DirPath = "mount:/";
							}
							break;
						  }
						  //If there are no files don't do anything
						  if(Files.empty()) break;
						  string FilePath = DirPath + Files.at(FileNameList->SelectedIndex).d_name;
						  //Check if directory. If not open file.
						  if(CheckIsDir(FilePath))
						  {
								DirPath = FilePath + "/";
								LoadListDirs(DirPath);
								//Reset the cursor
								FileNameList->ResetPos();
						  }
						  else
						  {
								OpenFile(DirPath + Files.at(FileNameList->SelectedIndex).d_name);
						  }
					  }
					  //B pressed
					  else if(Event->jbutton.button == 1)
					  {
						  std::string FileToGoTo = DirPath;
						  FileToGoTo.erase(0, GoUpDir(DirPath).size()+1);
						  DirPath = GoUpDir(DirPath) + "/";
						  LoadListDirs(DirPath);
						  FileNameList->ResetPos();
						  if(DirPath != "mount:/") FileToGoTo.erase(FileToGoTo.size()-1, FileToGoTo.size());
						  GoToIndexOfFile(FileToGoTo);
						  //Unmount save file
						  if(DirPath == "mount:/")
						  {
							  if(CurrentMount != "" && CurrentMount != ClipBoardMount)
							  {
								  fsdevCommitDevice(CurrentMount.c_str());
								  fsdevUnmountDevice(CurrentMount.c_str());
								  CurrentMount = "";
							  }
						  }
					  }
				  }
			  }
			  break;
		 }
	}
}

void ExplorerUI::ChangeFileSortMode()
{
	++FileSortMode %= 8;
	if(Files.size() == 0) return;
	std::string CurrentFileName = Files.at(FileNameList->SelectedIndex).d_name;
	LoadListDirs(DirPath);
	GoToIndexOfFile(CurrentFileName);
}

void ExplorerUI::GoToIndexOfFile(string FileName)
{
	if(DirPath != "mount:/")
	{
		//Find the cursor pos
		for(unsigned int i = 0; i < Files.size(); i++)
		{
			if(Files.at(i).d_name == FileName)
			{
				FileNameList->JumpToIndex(i);
				break;
			}
		}
	}
	else
	{
		FileNameList->ResetPos();
	}
}

void ExplorerUI::DrawUI()
{
	//Draw the BG
	SDL_SetRenderDrawColor(Renderer, BGColour_R, BGColour_G, BGColour_B, 255);
	SDL_Rect BGRect = {0,0, Width, Height};
	SDL_RenderFillRect(Renderer, &BGRect);
	//Draw the header
	DrawHeader();
	//Draw the footer
	DrawFooter();
	//Draw the lists
	ListAccesMutex.lock();
	FileNameList->DrawList();
	FileSizeList->DrawList();
	ListAccesMutex.unlock();
}

void ExplorerUI::DrawHeader()
{
	//Draw rect
	SDL_SetRenderDrawColor(Renderer, HeaderColour_R, HeaderColour_G, HeaderColour_B, 255);
	SDL_Rect HeaderRect = {0,0, Width, HeaderHeight};
	SDL_RenderFillRect(Renderer, &HeaderRect);
	//Vars for the text
	SDL_Color TextColour = {HeaderTextColour_R, HeaderTextColour_G, HeaderTextColour_B};
	//Draw the title
	SDL_Surface* TitleTextSurface = TTF_RenderText_Blended_Wrapped(HeaderFooterFont, " N-Xplorer", TextColour, Width);
	SDL_Texture* TitleTextTexture = SDL_CreateTextureFromSurface(Renderer, TitleTextSurface);
	SDL_Rect TitleRect = {0, (HeaderHeight - TitleTextSurface->h) / 2, TitleTextSurface->w, TitleTextSurface->h};
	SDL_RenderCopy(Renderer, TitleTextTexture, NULL, &TitleRect);
	SDL_DestroyTexture(TitleTextTexture);
	SDL_FreeSurface(TitleTextSurface);
	//Draw the time (thanks Lucy from AtlasNX for the simple code)
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char date[87];
	sprintf(date, "%02d/%02d/%d %02d:%02d:%02d ", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
	SDL_Surface* TimeTextSurface = TTF_RenderText_Blended_Wrapped(HeaderFooterFont, date, TextColour, Width);
	SDL_Texture* TimeTextTexture = SDL_CreateTextureFromSurface(Renderer, TimeTextSurface);
	SDL_Rect TimeRect = {Width - TimeTextSurface->w, (HeaderHeight - TimeTextSurface->h) / 2, TimeTextSurface->w, TimeTextSurface->h};
	SDL_RenderCopy(Renderer, TimeTextTexture, NULL, &TimeRect);
	SDL_DestroyTexture(TimeTextTexture);
	SDL_FreeSurface(TimeTextSurface);
	//Draw separator
	SDL_Rect BorderRect = {0, HeaderHeight, Width, 3};
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &BorderRect);
}

void ExplorerUI::DrawFooter()
{
	//Draw rect
	SDL_SetRenderDrawColor(Renderer, FooterColour_R, FooterColour_G, FooterColour_B, 255);
	SDL_Rect FooterRect = {0, Height - FooterHeight, Width, FooterHeight};
	SDL_RenderFillRect(Renderer, &FooterRect);
	//Vars for the text
	SDL_Color TextColour = {FooterTextColour_R, FooterTextColour_G, FooterTextColour_B};
	//Draw the path
	//char dbg[256];
	//sprintf(dbg, "Render offset: %d CursorIndex: %d SelectedIndex: %d", *FileSizeList->PListRenderOffset, *FileSizeList->PCursorIndex, *FileSizeList->PSelectedIndex);
	//SDL_Surface* PathTextSurface = TTF_RenderText_Blended_Wrapped(HeaderFooterFont, dbg, TextColour, Width);
	SDL_Surface* PathTextSurface = TTF_RenderText_Blended_Wrapped(HeaderFooterFont, DirPath.c_str(), TextColour, Width);
	SDL_Texture* PathTextTexture = SDL_CreateTextureFromSurface(Renderer, PathTextSurface);
	SDL_Rect PathRect = {0, (Height - FooterHeight) + (FooterHeight - PathTextSurface->h) / 2, PathTextSurface->w, PathTextSurface->h};
	SDL_RenderCopy(Renderer, PathTextTexture, NULL, &PathRect);
	SDL_DestroyTexture(PathTextTexture);
	SDL_FreeSurface(PathTextSurface);
	//Draw the control info
	//Needs to use ext font for glyths
	//SDL_Surface* ButtonTextSurface = TTF_RenderUTF8_Blended_Wrapped(FileNameList->ListFont, "\uE000 Open | \uE001 Back | \uE0EB Up | \uE0EC Down", TextColour, Width);
	SDL_Surface* ButtonTextSurface = TTF_RenderUTF8_Blended_Wrapped(FileNameList->ListFont, "A Open | B Back | ▲ Up | ▼ Down ", TextColour, Width);
	SDL_Texture* ButtonTextTexture = SDL_CreateTextureFromSurface(Renderer, ButtonTextSurface);
	SDL_Rect ButtonTextRect = {Width - ButtonTextSurface->w, (Height - FooterHeight) + (FooterHeight - ButtonTextSurface->h) / 2, ButtonTextSurface->w, ButtonTextSurface->h};
	if(PathRect.x + PathRect.w < Width - ButtonTextSurface->w) SDL_RenderCopy(Renderer, ButtonTextTexture, NULL, &ButtonTextRect);
	SDL_DestroyTexture(ButtonTextTexture);
	SDL_FreeSurface(ButtonTextSurface);
	//Draw separator
	SDL_Rect BorderRect = {0, Height - FooterHeight - 3, Width, 3};
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer, &BorderRect);
}

void ExplorerUI::OpenFile(string Path)
{
	string FileSuffix = GetFileExtension(Path);
	if(FileSuffix == "png" || FileSuffix == "jpg" || FileSuffix == "jpeg" || FileSuffix == "bmp")
	{
		*WindowState = 3;
		*ChosenFile = Path;
	}
	else if(FileSuffix == "txt" || FileSuffix == "ini" || FileSuffix == "json" || FileSuffix == "plist" || FileSuffix == "cfg" || FileSuffix == "log")
	{
		*WindowState = 2;
		*ChosenFile = Path;
	}
	//This should be done as a seperate sysmodule
	else if(FileSuffix == "mp3" || FileSuffix == "wav")
	{
		//*WindowState = 4;
		*ChosenFile = Path;
	}
	else if(FileSuffix == "zip")
	{
		*ChosenFile = Path;
		*LongOpMessagePtr = "Unzipping " + FileNameList->ListingTextVec.at(FileNameList->SelectedIndex) + ". please wait!";
		*WindowState = 8;
	}
}

std::vector <std::string> ExplorerUI::GetSaveDataMounts()
{
	//Modified code provided by WerWolv
	FsSaveDataInfoReader reader;
	FsSaveDataInfo saveDataInfo;
	s64 totalSaveEntries = 0;
	std::vector <std::string> AppMounts;
	fsOpenSaveDataInfoReader(&reader, FsSaveDataSpaceId_User);
	do
	{
		fsSaveDataInfoReaderRead(&reader, &saveDataInfo, 1, &totalSaveEntries);
		if (saveDataInfo.save_data_type == FsSaveDataType_Account)
		{
			//Get the app name. This is unsafe but probably won't fail.
			NacpLanguageEntry *langentry = nullptr;
			std::unique_ptr<NsApplicationControlData> buf = std::make_unique<NsApplicationControlData>();
			memset(buf.get(), 0, sizeof(NsApplicationControlData));
			nsGetApplicationControlData(NsApplicationControlSource_Storage, saveDataInfo.application_id, buf.get(), sizeof(NsApplicationControlData), nullptr);
			nacpGetLanguageEntry(&buf->nacp, &langentry);
			std::string AppName = std::string(langentry->name);
			//Add the app ID and the name to vectors for use later
			AppMounts.push_back(AppName);
			SaveMountMap.insert({AppName, saveDataInfo.application_id});
		}
	} while(totalSaveEntries > 0);
	fsSaveDataInfoReaderClose(&reader);
	return AppMounts;
}

void ExplorerUI::LoadListDirs(string DirPath)
{
	ListAccesMutex.lock();
	//clear the lists
	FileNameList->ListingTextVec.clear();
	FileSizeList->ListingTextVec.clear();
	//If no drive selected show drives
	if(DirPath == "mount:/")
	{
		FileNameList->ListingTextVec.push_back("sdmc:/");
		FileSizeList->ListingTextVec.push_back("SD card");
		FileNameList->ListingTextVec.push_back("user:/");
		FileSizeList->ListingTextVec.push_back("NAND");
		FileNameList->ListingTextVec.push_back("sys:/");
		FileSizeList->ListingTextVec.push_back("NAND");
		for(int i = 0; i < SaveMounts.size(); i++)
		{
			FileNameList->ListingTextVec.push_back(SaveMounts.at(i));
			FileSizeList->ListingTextVec.push_back("SAVE");
		}
	}
	else
	{
		Files = LoadDirs(DirPath);
		Files = SortFiles(DirPath, Files, FileSortMode);
		for(int i = 0; i < Files.size(); i++)
		{
			FileNameList->ListingTextVec.push_back(Files.at(i).d_name);
			FileSizeList->ListingTextVec.push_back(GetFileSize(DirPath + "/" + Files.at(i).d_name));
		}
	}
	ListAccesMutex.unlock();
}

class MenuUI : public UIWindow
{
	private:
	//vars
	std::string ClipboardPath = "";
	std::string ClipboardFileName = "";
	//functions
	void RecFileCopy();
	public:
	//vars
	ScrollList *MenuList;
	ExplorerUI *Explorer;
	int LongOpMessageBorder_R = 0;
	int LongOpMessageBorder_G = 0;
	int LongOpMessageBorder_B = 0;
	int LongOpMessageBG_R = 94;
	int LongOpMessageBG_G = 94;
	int LongOpMessageBG_B = 94;
	int LongOpMessageTextColour_R = 255;
	int LongOpMessageTextColour_G = 255;
	int LongOpMessageTextColour_B = 255;
	std::string LongOpMessage = "Unset";
	//Functions
	MenuUI();
	void GetInput();
	void DrawUI();
	void DrawLongOpMessage();
};

MenuUI::MenuUI()
{
	//Init the list
	MenuList = new ScrollList();
	MenuList->ListFont = GetSharedFont(32); //Load the list font
	MenuList->ListHeight = Height / 2;
	MenuList->ListWidth = Width / 6;
	MenuList->ListXOffset = Width - MenuList->ListWidth;
	MenuList->ListYOffset = (Height - MenuList->ListHeight) / 2;
	MenuList->IsActive = true;
	MenuList->CenterText = true;
	//populate list
	MenuList->ListingTextVec.push_back("Select");
	MenuList->ListingTextVec.push_back("Paste");
	MenuList->ListingTextVec.push_back("Move");
	MenuList->ListingTextVec.push_back("Rename");
	MenuList->ListingTextVec.push_back("New folder");
	MenuList->ListingTextVec.push_back("New file");
	MenuList->ListingTextVec.push_back("Delete");
	MenuList->ListingTextVec.push_back("Exit");
	MenuList->ListingsOnScreen = MenuList->ListingTextVec.size();
}

void MenuUI::DrawUI()
{
	//Draw the bg rectangle
	SDL_SetRenderDrawColor(Renderer, MenuList->BorderColour_R, MenuList->BorderColour_G, MenuList->BorderColour_B, 255);
	SDL_Rect BGRect = {MenuList->ListXOffset - 3, MenuList->ListYOffset , MenuList->ListWidth , MenuList->ListHeight + 3};
	SDL_RenderFillRect(Renderer, &BGRect);
	//Draw the list
	MenuList->DrawList();
}

//Draw the long op message
void MenuUI::DrawLongOpMessage()
{
	//Draw the outline
	SDL_SetRenderDrawColor(Renderer, LongOpMessageBorder_R, LongOpMessageBorder_G, LongOpMessageBorder_B, 255);
	int BGWidth = Width * 0.7;
	int BGHeight = Height * 0.5;
	SDL_Rect BGRect = {(Width - BGWidth) / 2, (Height - BGHeight) / 2, BGWidth, BGHeight};
	SDL_RenderFillRect(Renderer, &BGRect);
	//Draw the BG foreground (yes that is ironic)
	BGWidth -= 4;
	BGHeight -= 4;
	BGRect = {(Width - BGWidth) / 2, (Height - BGHeight) / 2, BGWidth, BGHeight};
	SDL_SetRenderDrawColor(Renderer, LongOpMessageBG_R, LongOpMessageBG_G, LongOpMessageBG_B, 255);
	SDL_RenderFillRect(Renderer, &BGRect);
	//Draw the message text
	SDL_Color TextColour = {LongOpMessageTextColour_R, LongOpMessageTextColour_G, LongOpMessageTextColour_B};
	SDL_Surface* MessageTextSurface = TTF_RenderUTF8_Blended_Wrapped(MenuList->ListFont, LongOpMessage.c_str(), TextColour, BGWidth);
	SDL_Texture* MessageTextTexture = SDL_CreateTextureFromSurface(Renderer, MessageTextSurface);
	SDL_Rect MessageTextRect = {BGRect.x + (BGWidth - MessageTextSurface->w) / 2, BGRect.y + (BGHeight - MessageTextSurface->h) / 2, MessageTextSurface->w, MessageTextSurface->h};
	SDL_RenderCopy(Renderer, MessageTextTexture, NULL, &MessageTextRect);
	SDL_DestroyTexture(MessageTextTexture);
	SDL_FreeSurface(MessageTextSurface);
}

//Seperate function for recursive file copy needed for threading
//Should we do this as a lambda?
void MenuUI::RecFileCopy()
{
	RecursiveFileCopy(ClipboardPath.c_str(), Explorer->DirPath.c_str(), ClipboardFileName.c_str());
	*WindowState = 0;
}

void MenuUI::GetInput()
{
	//Scan input
	while (SDL_PollEvent(Event))
	{
		switch (Event->type)
		{
			//Joycon button down
			case SDL_JOYBUTTONDOWN:
			{
				if (Event->jbutton.which == 0)
				{
					//Plus, B, or Y pressed
					if(Event->jbutton.button == 10 || Event->jbutton.button == 1 || Event->jbutton.button == 3)
					{
					  *WindowState = 0;
					}
					//Up pressed
					else if(Event->jbutton.button == 13)
					{
					  MenuList->CursorIndex--;
					  MenuList->SelectedIndex--;
					}
					//Down pressed
					else if(Event->jbutton.button == 15)
					{
					  MenuList->CursorIndex++;
					  MenuList->SelectedIndex++;
					}
					//A pressed
					else if(Event->jbutton.button == 0)
					{
						switch(MenuList->SelectedIndex)
						{
							//Copy
							case 0:
							{
								//Only copy the file if the file exists
								if(!Explorer->FileNameList->ListingTextVec.empty())
								{
									ClipboardPath = Explorer->HighlightedPath.c_str();
									ClipboardFileName = Explorer->FileNameList->ListingTextVec.at(Explorer->FileNameList->SelectedIndex);
									if(Explorer->CurrentMount != Explorer->ClipBoardMount)
									{
										fsdevCommitDevice(Explorer->ClipBoardMount.c_str());
										fsdevUnmountDevice(Explorer->ClipBoardMount.c_str());
									}
									Explorer->ClipBoardMount = Explorer->CurrentMount;
								}
							}
							break;
							//paste
							case 1:
							{
								if(!ClipboardPath.empty())
								{
									LongOpMessage = "Copying " + ClipboardFileName + ". This may take some time.";
									*WindowState = 4;
									RecFileCopy();
								}
							}
							break;
							//Move
							case 2:
							{
								string PathToMoveTo = Explorer->DirPath + ClipboardFileName;
								rename(ClipboardPath.c_str(), PathToMoveTo.c_str());
								ClipboardPath = PathToMoveTo;
							}
							break;
							//Rename
							case 3:
							{
								//Make sure the dir isn't empty
								if(!Explorer->FileNameList->ListingTextVec.empty())
								{
								  	string ExistingFileName = Explorer->FileNameList->ListingTextVec.at(Explorer->FileNameList->SelectedIndex);
									string NewFileName = Explorer->DirPath + GetKeyboardInput("Rename", "Rename " + ExistingFileName, ExistingFileName);
									rename(Explorer->HighlightedPath.c_str(), NewFileName.c_str());
								}
							}
							break;
							//New dir
							case 4:
							{
								string DirNameInput = Explorer->DirPath + GetKeyboardInput("Done", "Enter folder name", "New Folder");
								string NewDirName = DirNameInput;
								int NumDirsAlreadyExist = 1;
								while(CheckFileExists(NewDirName))
								{
									NewDirName = DirNameInput + " (" + to_string(++NumDirsAlreadyExist) + ")";
								}
								mkdir(NewDirName.c_str(), 0);
							}
							break;
							//New file
							case 5:
							{
								string NewFileName = Explorer->DirPath + GetKeyboardInput("Done", "Enter file name", "New File");
								if(!CheckFileExists(NewFileName))
								{
									ofstream outfile (NewFileName.c_str());
									outfile.close();
								}
							}
							break;
							//Delete
							case 6:
							{
								//Don't nuke the /switch/ directory if the user presses delete in an empty dir
								if(Explorer->HighlightedPath.empty()) break;
								LongOpMessage = "Deleting " + Explorer->FileNameList->ListingTextVec.at(Explorer->FileNameList->SelectedIndex) + ". This may take some time.";
								*WindowState = 4;
								//Check if directory
								if(CheckIsDir(Explorer->HighlightedPath.c_str()))
								{
									//Recursive delete function provided by libnx
									fsdevDeleteDirectoryRecursively(Explorer->HighlightedPath.c_str());
								}
								else
								{
									//Remove individual file with standard C++ function
									remove(Explorer->HighlightedPath.c_str());
								}
							}
							break;
							//Exit
							case 7:
							{
								*IsDone = 1;
							}
						  break;
						}
						//Return to explorer and reload the dir list
						Explorer->LoadListDirs(Explorer->DirPath);
						*WindowState = 0;
					  }
				  }
			  }
			  break;
		}
	}	
}