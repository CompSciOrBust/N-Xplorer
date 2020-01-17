#include <SDL.h>
//#include <UI.h>

class ImageUI : public UIWindow
{
	private:
	//vars
	float Scalar = 1;
	float PosOffsetX = 0;
	float PosOffsetY = 0;
	float ZoomModifier = 0;
	float RotationModifier = 0;
	int Zooming = 0;
	int Rotating = 0;
	SDL_Surface* PhotoSurface;
	SDL_Texture* PhotoTexture;
	public:
	//vars
	string *ChosenFile;
	//funcrions
	void GetInput();
	void DrawUI();
	void LoadFile();
};