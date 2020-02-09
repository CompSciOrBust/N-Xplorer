#include <SDL.h>
#include <UI.h>
#include <dirent.h>
#include <SDL2/SDL_ttf.h>
#include <thread>
#include <mutex>
#include <unordered_map>

class ExplorerUI : public UIWindow
{
	private:
	//vars
	int HeaderHeight = 55;
	int FooterHeight = 50;
	TTF_Font *HeaderFooterFont;
	std::mutex ListAccesMutex;
	std::vector <std::string> SaveMounts = std::vector <std::string>(0);
	std::unordered_map <std::string, u64> SaveMountMap = std::unordered_map <std::string, u64>(0);
	AccountUid CurrentUuid;
	//functions
	std::vector <std::string> GetSaveDataMounts();
	public:
	//vars
	ScrollList *FileNameList;
	ScrollList *FileSizeList;
	std::vector <dirent> Files = std::vector <dirent>(0);
	std::string HighlightedPath = "";
	std::string *ChosenFile;
	std::string DirPath = "mount:/";
	std::string CurrentMount;
	std::string ClipBoardMount;
	std::string *LongOpMessagePtr;
	int FileSortMode = 0;
	int HeaderColour_R = 94;
	int HeaderColour_G = 94;
	int HeaderColour_B = 94;
	int HeaderTextColour_R = 255;
	int HeaderTextColour_G = 255;
	int HeaderTextColour_B = 255;
	int FooterColour_R = 94;
	int FooterColour_G = 94;
	int FooterColour_B = 94;
	int FooterTextColour_R = 255;
	int FooterTextColour_G = 255;
	int FooterTextColour_B = 255;
	int BGColour_R = 44;
	int BGColour_G = 44;
	int BGColour_B = 44;
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
	//functions
	void RecFileCopy();
	public:
	//vars
	ScrollList *MenuList;
	ExplorerUI *Explorer;
	int LongOpMessageBorder_R = 0;
	int LongOpMessageBorder_G = 0;
	int LongOpMessageBorder_B = 0;
	int LongOpMessageBG_R = 94;
	int LongOpMessageBG_G = 94;
	int LongOpMessageBG_B = 94;
	int LongOpMessageTextColour_R = 255;
	int LongOpMessageTextColour_G = 255;
	int LongOpMessageTextColour_B = 255;
	std::string LongOpMessage = "Unset";
	//Functions
	MenuUI();
	void GetInput();
	void DrawUI();
	void DrawLongOpMessage();
};