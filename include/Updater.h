#include <Utils.h>
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