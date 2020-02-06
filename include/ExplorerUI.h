#include <SDL.h>
#include <UI.h>
#include <dirent.h>
#include <SDL2/SDL_ttf.h>
#include <thread>
#include <mutex>
class ExplorerUI : public UIWindow
{
	private:
	//vars
	int HeaderHeight = 55;
	int FooterHeight = 50;
	TTF_Font *HeaderFooterFont;
	std::mutex ListAccesMutex;
	public:
	//vars
	ScrollList *FileNameList;
	ScrollList *FileSizeList;
	vector <dirent> Files = vector <dirent>(0);
	std::string HighlightedPath = "";
	std::string *ChosenFile;
	std::string DirPath = "mount:/";
	int FileSortMode = 0;
	//functions
	ExplorerUI();
	void GetInput();
	void DrawUI();
	void OpenFile(std::string);
	void LoadListDirs(std::string);
	void DrawHeader();
	void DrawFooter();
	void GoToIndexOfFile(std::string);
	void ChangeFileSortMode();
};

class MenuUI : public UIWindow
{
	private:
	//vars
	std::string ClipboardPath = "";
	std::string ClipboardFileName = "";
	std::string LongOpMessage = "";
	//functions
	void RecFileCopy();
	public:
	//vars
	ScrollList *MenuList;
	ExplorerUI *Explorer;
	//Functions
	MenuUI();
	void GetInput();
	void DrawUI();
	void DrawLongOpMessage();
};