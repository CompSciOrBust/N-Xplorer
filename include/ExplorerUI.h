#include <SDL.h>
#include <UI.h>
#include <dirent.h>
#include <SDL2/SDL_ttf.h>

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