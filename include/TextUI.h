#include <SDL.h>
//#include <UI.h>

class TextUI : public UIWindow
{
	private:
	//vars
	vector <string> LinesVec;
	TTF_Font *LineFont;
	int LineToRenderFrom = 0;
	int SelectedLine = 0;
	bool ScrollUp = false;
	bool ScrollDown = false;
	//Functions
	void DrawTextLines();
	public:
	//vars
	string *ChosenFile;
	//funcrions
	TextUI();
	void GetInput();
	void DrawUI();
	void LoadFile();
	void SaveFile();
};