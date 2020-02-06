#include <SDL.h>
#include <ExplorerUI.h>

class SettingsUI : public UIWindow
{
	private:
	//vars
	SimpleList SettingsList;
	public:
	//functions
	SettingsUI();
	void DrawUI();
	void GetInput();
	//vars
	ExplorerUI *Explorer;
	void UpdateSortSettingText();
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