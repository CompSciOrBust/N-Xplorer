#include <SDL.h>
#include <ExplorerUI.h>
#include <SimpleIniParser.hpp>
#include <Utils.h>
using namespace simpleIniParser;

class SettingsUI : public UIWindow
{
	private:
	//functions
	void SaveINI();
	void LoadTheme();
	//vars
	SimpleList SettingsList;
	public:
	//functions
	SettingsUI();
	void DrawUI();
	void GetInput();
	//vars
	ExplorerUI *Explorer;
	MenuUI *ContextMenu;
	void UpdateSortSettingText();
	void CreateNewIni();
};

SettingsUI::SettingsUI()
{
	SettingsList = SimpleList();
	SettingsList.HeaderText = "Settings";
	SettingsList.OptionsTextVec = {"Sort mode:", "Check for updates"};
}

void SettingsUI::DrawUI()
{
	SettingsList.Renderer = Renderer;
	SettingsList.DrawList();
}

void SettingsUI::GetInput()
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
					//Minus, Plus, or B pressed exit settings
					if(Event->jbutton.button == 11 || Event->jbutton.button == 10 || Event->jbutton.button == 1)
					{
						*WindowState = 0;
					}
					//A pressed activate function
					else if(Event->jbutton.button == 0)
					{
						switch(SettingsList.SelectedOption)
						{
							//Change sort mode
							case 0:
							{
								Explorer->ChangeFileSortMode();
								UpdateSortSettingText();
							}
							break;
							//Update append
							case 1:
							{
								*WindowState = 7;
							}
							break;
						}
						SaveINI();
					}
					//Up pressed
					else if(Event->jbutton.button == 13)
					{
						SettingsList.MoveUp();
					}
					//Down pressed
					else if(Event->jbutton.button == 15)
					{
						SettingsList.MoveDown();
					}
				}
			}
			break;
		}
	}	
}

void SettingsUI::UpdateSortSettingText()
{
	std::string BaseText = "Sort mode: ";
	std::string ModeName;
	switch(Explorer->FileSortMode)
	{
		case 0:
		{
			ModeName = "Size descending";
		}
		break;
		case 1:
		{
			ModeName = "Size ascending";
		}
		break;
		case 2:
		{
			ModeName = "Name A-Z";
		}
		break;
		case 3:
		{
			ModeName = "Name Z-A";
		}
		break;
	}
	SettingsList.OptionsTextVec.at(0) = BaseText + ModeName;
}

void SettingsUI::CreateNewIni()
{
	//Create
	mkdir("sdmc:/config/N-Xplorer/", 0);
	Ini * SettingsIni;
	if(CheckFileExists("sdmc:/config/N-Xplorer/settings.ini")) SettingsIni = Ini::parseFile("sdmc:/config/N-Xplorer/settings.ini");
	else SettingsIni = new Ini();
	IniSection * SettingsSection = SettingsIni->findOrCreateSection("Settings", false);
	IniOption * SortOption = SettingsSection->findOrCreateFirstOption("SortMode", "2", false);
	SettingsIni->writeToFile("sdmc:/config/N-Xplorer/settings.ini");
	//Load values in to their places
	Explorer->FileSortMode = std::stoi(SortOption->value, nullptr, 10);
	delete SettingsIni;
	//Chain
	LoadTheme();
}

void SettingsUI::SaveINI()
{
	Ini * SettingsIni = Ini::parseFile("sdmc:/config/N-Xplorer/settings.ini");
	IniSection * SettingsSection = SettingsIni->findSection("Settings", false);
	SettingsSection->findFirstOption("SortMode", false)->value = std::to_string(Explorer->FileSortMode);
	SettingsIni->writeToFile("sdmc:/config/N-Xplorer/settings.ini");
	delete SettingsIni;
}

void SettingsUI::LoadTheme()
{
	//Vars
	//File list
	int FileListColour_R = 66;
	int FileListColour_G = 66;
	int FileListColour_B = 66;
	int FileListColourSelected_R = 161;
	int FileListColourSelected_G = 161;
	int FileListColourSelected_B = 161;
	int FileListBorderColour_R = 0;
	int FileListBorderColour_G = 0;
	int FileListBorderColour_B = 0;
	int FileListTextColour_R = 255;
	int FileListTextColour_G = 255;
	int FileListTextColour_B = 255;
	//Context menu
	int ContextColour_R = 66;
	int ContextColour_G = 66;
	int ContextColour_B = 66;
	int ContextColourSelected_R = 161;
	int ContextColourSelected_G = 161;
	int ContextColourSelected_B = 161;
	int ContextColourTextColour_R = 255;
	int ContextColourTextColour_G = 255;
	int ContextColourTextColour_B = 255;
	//Header and footer
	int HeaderColour_R = 94;
	int HeaderColour_G = 94;
	int HeaderColour_B = 94;
	int FooterColour_R = 94;
	int FooterColour_G = 94;
	int FooterColour_B = 94;
	int HeaderTextColour_R = 255;
	int HeaderTextColour_G = 255;
	int HeaderTextColour_B = 255;
	int FooterTextColour_R = 255;
	int FooterTextColour_G = 255;
	int FooterTextColour_B = 255;
	//Background
	int BGColour_R = 44;
	int BGColour_G = 44;
	int BGColour_B = 44;
	//TODO: Support images
	
	//Read the ini
	Ini * ThemeIni;
	if(CheckFileExists("sdmc:/config/N-Xplorer/Theme.ini")) ThemeIni = Ini::parseFile("sdmc:/config/N-Xplorer/Theme.ini");
	else ThemeIni = new Ini;
	IniSection * ThemeColourSection = ThemeIni->findOrCreateSection("MainUI Colours", false);
	//File names
	FileListColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListColour_R", std::to_string(FileListColour_R), false)->value);
	FileListColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListColour_G", std::to_string(FileListColour_G), false)->value);
	FileListColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListColour_B", std::to_string(FileListColour_B), false)->value);
	FileListColourSelected_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListColourSelected_R", std::to_string(FileListColourSelected_R), false)->value);
	FileListColourSelected_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListColourSelected_G", std::to_string(FileListColourSelected_G), false)->value);
	FileListColourSelected_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListColourSelected_B", std::to_string(FileListColourSelected_B), false)->value);
	FileListBorderColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListBorderColour_R", std::to_string(FileListBorderColour_R), false)->value);
	FileListBorderColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListBorderColour_G", std::to_string(FileListBorderColour_G), false)->value);
	FileListBorderColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListBorderColour_B", std::to_string(FileListBorderColour_B), false)->value);
	FileListTextColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListTextColour_R", std::to_string(FileListTextColour_R), false)->value);
	FileListTextColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListTextColour_G", std::to_string(FileListTextColour_G), false)->value);
	FileListTextColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("FileListTextColour_B", std::to_string(FileListTextColour_B), false)->value);
	//Context menu
	ContextColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("ContextColour_R", std::to_string(ContextColour_R), false)->value);
	ContextColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("ContextColour_G", std::to_string(ContextColour_G), false)->value);
	ContextColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("ContextColour_B", std::to_string(ContextColour_B), false)->value);
	ContextColourSelected_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("ContextColourSelected_R", std::to_string(ContextColourSelected_R), false)->value);
	ContextColourSelected_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("ContextColourSelected_G", std::to_string(ContextColourSelected_G), false)->value);
	ContextColourSelected_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("ContextColourSelected_B", std::to_string(ContextColourSelected_B), false)->value);
	ContextColourTextColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("ContextColourTextColour_R", std::to_string(ContextColourTextColour_R), false)->value);
	ContextColourTextColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("ContextColourTextColour_G", std::to_string(ContextColourTextColour_G), false)->value);
	ContextColourTextColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("ContextColourTextColour_B", std::to_string(ContextColourTextColour_B), false)->value);
	//Header and footer
	HeaderColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("HeaderColour_R", std::to_string(HeaderColour_R), false)->value);
	HeaderColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("HeaderColour_G", std::to_string(HeaderColour_G), false)->value);
	HeaderColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("HeaderColour_B", std::to_string(HeaderColour_B), false)->value);
	FooterColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("FooterColour_R", std::to_string(FooterColour_R), false)->value);
	FooterColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("FooterColour_G", std::to_string(FooterColour_G), false)->value);
	FooterColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("FooterColour_B", std::to_string(FooterColour_B), false)->value);
	HeaderTextColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("HeaderTextColour_R", std::to_string(HeaderTextColour_R), false)->value);
	HeaderTextColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("HeaderTextColour_G", std::to_string(HeaderTextColour_G), false)->value);
	HeaderTextColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("HeaderTextColour_B", std::to_string(HeaderTextColour_B), false)->value);
	FooterTextColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("FooterTextColour_R", std::to_string(FooterTextColour_R), false)->value);
	FooterTextColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("FooterTextColour_G", std::to_string(FooterTextColour_G), false)->value);
	FooterTextColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("FooterTextColour_B", std::to_string(FooterTextColour_B), false)->value);
	//Background
	BGColour_R = std::stoi(ThemeColourSection->findOrCreateFirstOption("BGColour_R", std::to_string(BGColour_R), false)->value);
	BGColour_G = std::stoi(ThemeColourSection->findOrCreateFirstOption("BGColour_G", std::to_string(BGColour_G), false)->value);
	BGColour_B = std::stoi(ThemeColourSection->findOrCreateFirstOption("BGColour_B", std::to_string(BGColour_B), false)->value);
	ThemeIni->writeToFile("sdmc:/config/N-Xplorer/Theme.ini");
	delete ThemeIni;
	//update the vars
	//File names
	Explorer->FileNameList->ListColour_R = FileListColour_R;
	Explorer->FileNameList->ListColour_G = FileListColour_G;
	Explorer->FileNameList->ListColour_B = FileListColour_B;
	Explorer->FileNameList->ListColourSelected_R = FileListColourSelected_R;
	Explorer->FileNameList->ListColourSelected_G = FileListColourSelected_G;
	Explorer->FileNameList->ListColourSelected_B = FileListColourSelected_B;
	Explorer->FileNameList->BorderColour_R = FileListBorderColour_R;
	Explorer->FileNameList->BorderColour_G = FileListBorderColour_G;
	Explorer->FileNameList->BorderColour_B = FileListBorderColour_B;
	Explorer->FileNameList->TextColour_R = FileListTextColour_R;
	Explorer->FileNameList->TextColour_G = FileListTextColour_G;
	Explorer->FileNameList->TextColour_B = FileListTextColour_B;
	//File size
	Explorer->FileSizeList->ListColour_R = FileListColour_R;
	Explorer->FileSizeList->ListColour_G = FileListColour_G;
	Explorer->FileSizeList->ListColour_B = FileListColour_B;
	Explorer->FileSizeList->ListColourSelected_R = FileListColourSelected_R;
	Explorer->FileSizeList->ListColourSelected_G = FileListColourSelected_G;
	Explorer->FileSizeList->ListColourSelected_B = FileListColourSelected_B;
	Explorer->FileSizeList->BorderColour_R = FileListBorderColour_R;
	Explorer->FileSizeList->BorderColour_G = FileListBorderColour_G;
	Explorer->FileSizeList->BorderColour_B = FileListBorderColour_B;
	Explorer->FileSizeList->TextColour_R = FileListTextColour_R;
	Explorer->FileSizeList->TextColour_G = FileListTextColour_G;
	Explorer->FileSizeList->TextColour_B = FileListTextColour_B;
	//Context menu
	ContextMenu->MenuList->ListColour_R = ContextColour_R;
	ContextMenu->MenuList->ListColour_G = ContextColour_G;
	ContextMenu->MenuList->ListColour_B = ContextColour_B;
	ContextMenu->MenuList->ListColourSelected_R = ContextColourSelected_R;
	ContextMenu->MenuList->ListColourSelected_G = ContextColourSelected_G;
	ContextMenu->MenuList->ListColourSelected_B = ContextColourSelected_B;
	ContextMenu->MenuList->TextColour_R = ContextColourTextColour_R;
	ContextMenu->MenuList->TextColour_G = ContextColourTextColour_G;
	ContextMenu->MenuList->TextColour_B = ContextColourTextColour_B;
	//Header and footer
	Explorer->HeaderColour_R = HeaderColour_R;
	Explorer->HeaderColour_G = HeaderColour_G;
	Explorer->HeaderColour_B = HeaderColour_B;
	Explorer->FooterColour_R = FooterColour_R;
	Explorer->FooterColour_G = FooterColour_G;
	Explorer->FooterColour_B = FooterColour_B;
	Explorer->HeaderTextColour_R = HeaderTextColour_R;
	Explorer->HeaderTextColour_G = HeaderTextColour_G;
	Explorer->HeaderTextColour_B = HeaderTextColour_B;
	Explorer->FooterTextColour_R = FooterTextColour_R;
	Explorer->FooterTextColour_G = FooterTextColour_G;
	Explorer->FooterTextColour_B = FooterTextColour_B;
	//Background
	Explorer->BGColour_R = BGColour_R;
	Explorer->BGColour_G = BGColour_G;
	Explorer->BGColour_B = BGColour_B;
}