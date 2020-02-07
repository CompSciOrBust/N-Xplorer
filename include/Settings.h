class SettingsUI : public UIWindow
{
	private:
	//functions
	void SaveINI();
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
	void CreateNewIni();
};