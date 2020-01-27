#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <switch.h>
using namespace std;
class UIWindow
{
	protected:
	//vars
	int Width = 1280;
	int Height = 720;
	public:
	//vars
	SDL_Renderer *Renderer;
	SDL_Event *Event;
	int *IsDone;
	int *WindowState;
};

class ScrollList
{
	public:
	void DrawList();
	int ListHeight = 0;
	int ListWidth = 0;
	int SelectedIndex = 0;
	int CursorIndex = 0;
	int ListRenderOffset = 0;
	int *PSelectedIndex = NULL;
	int *PCursorIndex = NULL;
	int *PListRenderOffset = NULL;
	int ListColour_R = 66;
	int ListColour_G = 66;
	int ListColour_B = 66;
	int ListColourSelected_R = 161;
	int ListColourSelected_G = 161;
	int ListColourSelected_B = 161;
	int ListingsOnScreen = 0;
	int *TouchListX = NULL;
	int *TouchListY = NULL;
	vector <string> ListingTextVec = vector <string>(0);
	SDL_Renderer *Renderer;
	TTF_Font *ListFont;
	int ListYOffset = 0;
	int ListXOffset = 0;
	bool ItemSelected = false;
	bool IsActive = false;
	bool CenterText = false;
	ScrollList();
	void Enslave(ScrollList*);
	void MoveUp();
	void MoveDown();
	void JumpUp();
	void JumpDown();
	void ResetPos();
};

bool CheckButtonPressed(SDL_Rect*, int, int);
TTF_Font *GetSharedFont(int FontSize);