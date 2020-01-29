#include <SDL.h>
//#include <UI.h>

class TextUI : public UIWindow
{
	private:
	//vars
	vector <string> LinesVec;
	TTF_Font *LineFont;
	TTF_Font *SaveOptionFont;
	TTF_Font *SaveHeaderFont;
	unsigned int LineToRenderFrom = 0;
	unsigned int SelectedLine = 0;
	bool ScrollUp = false;
	bool ScrollDown = false;
	int SelectedSaveOption = 0;
	//Functions
	void DrawTextLines();
	public:
	//vars
	string *ChosenFile;
	//funcrions
	TextUI();
	void GetInput();
	void DrawUI();
	void DrawSaveOptions();
	void GetSaveInput();
	void LoadFile();
	void SaveFile();
};