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