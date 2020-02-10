//Based on Amiigo's updater
#include <SDL.h>
#include <Networking.h>
#include <Utils.h>
#include <ExplorerUI.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class UpdaterUI : public UIWindow
{
	private:
	//vars
	int State = 0;
	bool NewVersion = false;
	std::string LatestID;
	json GitAPIData;
	//functions
	bool CheckForNewVersion();
	public:
	//vars
	std::string NroPath = "sdmc:/switch/N-Xplorer.nro";
	MenuUI *Menu;
	//functions
	void DrawUI();
	void GetInput();
	UpdaterUI();
};

UpdaterUI::UpdaterUI()
{
	nifmInitialize(NifmServiceType_User); //Init nifm for connection stuff
}

void UpdaterUI::DrawUI()
{
	Menu->DrawLongOpMessage();
}

void UpdaterUI::GetInput()
{
	//scan input
	bool BPressed = false;
	while (SDL_PollEvent(Event))
	{
		switch (Event->type)
		{
			//Joycon button down
			case SDL_JOYBUTTONDOWN:
			{
				if (Event->jbutton.which == 0)
				{
					if(Event->jbutton.button == 1)
					{
						BPressed = true;
					}
				}
			}
		}
	}
	
	//Update the message and do logic
	switch(State)
	{
		//Check for connection and update
		case 0:
		{
			if(!HasConnection())
			{
				Menu->LongOpMessage = "Waiting for connection.";
				if(BPressed)
				{
					*WindowState = 6;
				}
			}
			else
			{
				Menu->LongOpMessage = "Checking for new version...";
				NewVersion = CheckForNewVersion();
				State++;
			}
		}
		break;
		//Download new zip
		case 1:
		{
			if(NewVersion)
			{
				Menu->LongOpMessage = "Downloading " + LatestID + ".";
				string UpdateFileURL = "https://github.com/CompSciOrBust/N-Xplorer/releases/download/" + LatestID + "/N-Xplorer.zip";
				RetrieveToFile(UpdateFileURL, "sdmc:/config/N-Xplorer/update.zip");
				UnzipFile("sdmc:/config/N-Xplorer/update.zip", "sdmc:/");
				if(NroPath != "sdmc:/switch/N-Xplorer.nro")
				{
					remove(NroPath.c_str());
					rename("sdmc:/switch/N-Xplorer.nro", NroPath.c_str());
				}
				*IsDone = 1;
			}
			else
			{
				Menu->LongOpMessage = "Already on the latest version. Press B to exit.";
				if(BPressed)
				{
					*WindowState = 6;
					NewVersion = false;
					State = 0;
				}
			}
		}
		break;
		//Error
		case 999:
		{
			Menu->LongOpMessage = "Error! Is GitHub rate limiting you? Press B to exit";
			if(BPressed)
			{
				*WindowState = 6;
			}
		}
		break;
		
	}
}

bool UpdaterUI::CheckForNewVersion()
{
	//Get data from GitHub API
	std::string Data = RetrieveContent("https://api.github.com/repos/CompSciOrBust/N-Xplorer/releases", "application/json");
	//Get the release tag string from the data
	GitAPIData = json::parse(Data);
	//Check if GitAPI gave us a release tag otherwise we'll crash
	if(Data.length() < 300)
	{
		State = 999;
		return false;
	}
	LatestID = GitAPIData[0]["tag_name"].get<std::string>();
	//Check if we're running the latest version
	return (LatestID != VERSION);
}