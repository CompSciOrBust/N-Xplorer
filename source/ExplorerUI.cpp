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
using namespace std;

class ExplorerUI : public UIWindow
{
	private:
	//vars
	int HeaderHeight = 55;
	int FooterHeight = 50;
	TTF_Font *HeaderFooterFont;
	public:
	//vars
	ScrollList *FileList;
	vector <dirent> Files = vector <dirent>(0);
	string HighlightedPath = "";
	string *ChosenFile;
	string DirPath = "sdmc:/";
	//functions
	ExplorerUI();
	void GetInput();
	void DrawUI();
	void OpenFile(string);
	void LoadListDirs(string);
	void DrawHeader();
	void DrawFooter();
};

ExplorerUI::ExplorerUI()
{
	//Set up shared font
	PlFontData standardFontData;
	plGetSharedFontByType(&standardFontData, PlSharedFontType_Standard);
	//Get a list of the files
	FileList = new ScrollList();
	//Set up the file list
	//FileList->TouchListX = &TouchX;
	//FileList->TouchListY = &TouchY;
	FileList->ListFont = GetSharedFont(30); //Load the list font
	FileList->ListingsOnScreen = 15;
	FileList->ListHeight = Height - HeaderHeight - FooterHeight;
	FileList->ListWidth = Width;
	FileList->ListYOffset = HeaderHeight;
	FileList->IsActive = true;
	//Populate list
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
					  //Plus or Y pressed
					  if(Event->jbutton.button == 10 || Event->jbutton.button == 3)
					  {
						  //Update highlighted file if one exists
						  if(!FileList->ListingTextVec.empty())
						  {
							  HighlightedPath = DirPath + Files.at(FileList->SelectedIndex).d_name;
						  }
						  else
						  {
							  HighlightedPath = "";
						  }
						  *WindowState = 1;
					  }
					  //Up pressed
					  else if(Event->jbutton.button == 13)
					  {
						  FileList->CursorIndex--;
						  FileList->SelectedIndex--;
					  }
					  //Down pressed
					  else if(Event->jbutton.button == 15)
					  {
						  FileList->CursorIndex++;
						  FileList->SelectedIndex++;
					  }
					  //A pressed
					  else if(Event->jbutton.button == 0)
					  {
						  //Check if directory. If not open file.
						  string FilePath = DirPath + Files.at(FileList->SelectedIndex).d_name;
						  if(CheckIsDir(FilePath))
						  {
								DirPath = FilePath + "/";
								LoadListDirs(DirPath);
						  }
						  else
						  {
								OpenFile(DirPath + Files.at(FileList->SelectedIndex).d_name);
						  }
					  }
					  //B pressed
					  else if(Event->jbutton.button == 1)
					  {
						  DirPath = GoUpDir(DirPath) + "/";
						  LoadListDirs(DirPath);
						  //Reset the cursor
						  FileList->SelectedIndex = 0;
						  FileList->CursorIndex = 0;
					  }
				  }
			  }
			  break;
		 }
	}
}

void ExplorerUI::DrawUI()
{
	//Draw the BG
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	SDL_Rect BGRect = {0,0, Width, Height};
	SDL_RenderFillRect(Renderer, &BGRect);
	//Draw the header
	DrawHeader();
	//Draw the footer
	DrawFooter();
	//Draw the list
	FileList->DrawList();
}

void ExplorerUI::DrawHeader()
{
	//Draw rect
	SDL_SetRenderDrawColor(Renderer, 94, 94, 94, 255);
	SDL_Rect HeaderRect = {0,0, Width, HeaderHeight};
	SDL_RenderFillRect(Renderer, &HeaderRect);
	//Vars for the text
	SDL_Color TextColour = {255, 255, 255};
	//Draw the title
	SDL_Surface* TitleTextSurface = TTF_RenderText_Blended_Wrapped(HeaderFooterFont, "N-Xplorer", TextColour, Width);
	SDL_Texture* TitleTextTexture = SDL_CreateTextureFromSurface(Renderer, TitleTextSurface);
	SDL_Rect TitleRect = {0, (HeaderHeight - TitleTextSurface->h) / 2, TitleTextSurface->w, TitleTextSurface->h};
	SDL_RenderCopy(Renderer, TitleTextTexture, NULL, &TitleRect);
	SDL_DestroyTexture(TitleTextTexture);
	SDL_FreeSurface(TitleTextSurface);
	//Draw the time (thanks Lucy from AtlasNX for the simple code)
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char date[87];
	sprintf(date, "%02d/%02d/%d %02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min);
	SDL_Surface* TimeTextSurface = TTF_RenderText_Blended_Wrapped(HeaderFooterFont, date, TextColour, Width);
	SDL_Texture* TimeTextTexture = SDL_CreateTextureFromSurface(Renderer, TimeTextSurface);
	SDL_Rect TimeRect = {Width - TimeTextSurface->w, (HeaderHeight - TimeTextSurface->h) / 2, TimeTextSurface->w, TimeTextSurface->h};
	SDL_RenderCopy(Renderer, TimeTextTexture, NULL, &TimeRect);
	SDL_DestroyTexture(TimeTextTexture);
	SDL_FreeSurface(TimeTextSurface);
}

void ExplorerUI::DrawFooter()
{
	//Draw rect
	SDL_SetRenderDrawColor(Renderer, 94, 94, 94, 255);
	SDL_Rect FooterRect = {0, Height - FooterHeight, Width, FooterHeight};
	SDL_RenderFillRect(Renderer, &FooterRect);
	//Vars for the text
	SDL_Color TextColour = {255, 255, 255};
	//Draw the path
	SDL_Surface* PathTextSurface = TTF_RenderText_Blended_Wrapped(HeaderFooterFont, DirPath.c_str(), TextColour, Width);
	SDL_Texture* PathTextTexture = SDL_CreateTextureFromSurface(Renderer, PathTextSurface);
	SDL_Rect PathRect = {0, (Height - FooterHeight) + (FooterHeight - PathTextSurface->h) / 2, PathTextSurface->w, PathTextSurface->h};
	SDL_RenderCopy(Renderer, PathTextTexture, NULL, &PathRect);
	SDL_DestroyTexture(PathTextTexture);
	SDL_FreeSurface(PathTextSurface);
	//Draw the control info
	//Needs to use ext font
	/*
	SDL_Surface* ButtonTextSurface = TTF_RenderUTF8_Blended_Wrapped(FileList->ListFont, "\uE000 Open | \uE001 Back | \uE0EB Up | \uE0EC Down", TextColour, Width);
	SDL_Texture* ButtonTextTexture = SDL_CreateTextureFromSurface(Renderer, ButtonTextSurface);
	SDL_Rect ButtonTextRect = {Width - ButtonTextSurface->w, (Height - FooterHeight) + (FooterHeight - ButtonTextSurface->h) / 2, ButtonTextSurface->w, ButtonTextSurface->h};
	if(PathRect.x + PathRect.w < Width - ButtonTextSurface->w) SDL_RenderCopy(Renderer, ButtonTextTexture, NULL, &ButtonTextRect);
	SDL_DestroyTexture(ButtonTextTexture);
	SDL_FreeSurface(ButtonTextSurface);
	*/
}

void ExplorerUI::OpenFile(string Path)
{
	int ExtensionStart = Path.find(".")+1;
	string FileSuffix;
	FileSuffix.assign(Path, ExtensionStart, Path.size() - ExtensionStart);
	if(FileSuffix == "png" || FileSuffix == "jpg" || FileSuffix == "jpeg" || FileSuffix == "bmp")
	{
		*WindowState = 3;
		*ChosenFile = Path;
	}
	else if(FileSuffix == "mp3" || FileSuffix == "wav")
	{
		*WindowState = 4;
		*ChosenFile = Path;
	}
	else if(FileSuffix == "txt" || FileSuffix == "ini" || FileSuffix == "json" || FileSuffix == "plist" || FileSuffix == "cfg" || FileSuffix == "log")
	{
		*WindowState = 2;
		*ChosenFile = Path;
	}
}

void ExplorerUI::LoadListDirs(string DirPath)
{
	FileList->ListingTextVec.clear();
	Files = LoadDirs(DirPath);
	for(int i = 0; i < Files.size(); i++)
	{
		FileList->ListingTextVec.push_back(Files.at(i).d_name);
	}
}

class MenuUI : public UIWindow
{
	private:
	//vars
	string ClipboardPath = "";
	string ClipboardFileName = "";
	public:
	//vars
	ScrollList *MenuList;
	ExplorerUI *Explorer;
	//Functions
	MenuUI();
	void GetInput();
	void DrawUI();
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
	MenuList->ListingTextVec.push_back("Copy");
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
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	SDL_Rect BGRect = {MenuList->ListXOffset - 3, MenuList->ListYOffset , MenuList->ListWidth , MenuList->ListHeight + 3};
	SDL_RenderFillRect(Renderer, &BGRect);
	//Draw the list
	MenuList->DrawList();
}

void MenuUI::GetInput()
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
					//Plus or B pressed
					if(Event->jbutton.button == 10 || Event->jbutton.button == 1)
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
								if(!Explorer->FileList->ListingTextVec.empty())
								{
									ClipboardPath = Explorer->HighlightedPath.c_str();
									ClipboardFileName = Explorer->FileList->ListingTextVec.at(Explorer->FileList->SelectedIndex);
								}
							}
							break;
							//paste
							case 1:
							{
							  //Not implemented
							}
							break;
							//Move
							case 2:
							{
								string PathToMoveTo = Explorer->DirPath + ClipboardFileName;
								rename(ClipboardPath.c_str(), PathToMoveTo.c_str());
							}
							break;
							//Rename
							case 3:
							{
								//Make sure the dir isn't empty
								if(!Explorer->FileList->ListingTextVec.empty())
								{
								  	string ExistingFileName = Explorer->FileList->ListingTextVec.at(Explorer->FileList->SelectedIndex);
									string NewFileName = Explorer->DirPath + GetKeyboardInput("Rename", "Rename " + ExistingFileName, ExistingFileName);
									rename(Explorer->HighlightedPath.c_str(), NewFileName.c_str());
								}
							}
							break;
							//New dir
							case 4:
							{
								string NewDirName = Explorer->DirPath + GetKeyboardInput("New folder", "Enter folder name", "New Folder");
								mkdir(NewDirName.c_str(), 0);
							}
							break;
							//New file
							case 5:
							{
								string NewFileName = Explorer->DirPath + GetKeyboardInput("New file", "Enter file name", "New File");
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
						*WindowState = 0;
						Explorer->LoadListDirs(Explorer->DirPath);
					  }
				  }
			  }
			  break;
		}
	}	
}