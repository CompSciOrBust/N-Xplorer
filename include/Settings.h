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