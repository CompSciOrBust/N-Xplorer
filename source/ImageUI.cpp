#include <SDL.h>
#include <SDL2/sdl_image.h>
#include <iostream>
#include <UI.h>
#include <unistd.h>
using namespace std;

class ImageUI : public UIWindow
{
	private:
	//vars
	float Scalar = 1;
	float PosOffsetX = 0;
	float PosOffsetY = 0;
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

//A lot of this is taken from before the rewrite
void ImageUI::DrawUI()
{
	PhotoTexture = SDL_CreateTextureFromSurface(Renderer, PhotoSurface);
	//Check if too big to fit on screen
	if(PhotoSurface->w > Width || PhotoSurface->h > Height)
	{
		if(PhotoSurface->h - Height> PhotoSurface->w - Width)
		{
			Scalar = Height / (float)PhotoSurface->h; 
		}
		else
		{
			Scalar = Width / (float)PhotoSurface->w;
		}
	}
	else
	{
		Scalar = 1;
	}
	SDL_Rect PhotoRect = {0, 0, PhotoSurface->w * Scalar, PhotoSurface->h * Scalar};
	PhotoRect.x = ((Width - PhotoRect.w) / 2) + PosOffsetX;
	PhotoRect.y = ((Height - PhotoRect.h) / 2) + PosOffsetY;
	SDL_RenderCopyEx(Renderer, PhotoTexture, NULL, &PhotoRect, 0, nullptr, SDL_FLIP_NONE);
	SDL_DestroyTexture(PhotoTexture);
	usleep(1000000 / 120); //Quick and dirty way to run at about 120 fps
}

//TODO: Implement rotating, zooming, and moving
void ImageUI::GetInput()
{
	//Scan input
	while (SDL_PollEvent(Event))
	{
		switch(Event->type)
		{
			//Button down
			case SDL_JOYBUTTONDOWN:
			//Joycon 1 button pressed
			if (Event->jbutton.which == 0)
			{
				//B pressed
				if(Event->jbutton.button == 1)
				{
					*WindowState = 0;
				}
			}
		}
	}
}

void ImageUI::LoadFile()
{
	PhotoSurface = IMG_Load(ChosenFile->c_str());
	PosOffsetX = 0;
	PosOffsetY = 0;
}