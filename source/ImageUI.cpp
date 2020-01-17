#include <SDL.h>
#include <SDL2/sdl_image.h>
#include <iostream>
#include <UI.h>
#include <unistd.h>
#include <cmath>
using namespace std;

class ImageUI : public UIWindow
{
	private:
	//vars
	float Scalar = 1;
	float PosOffsetX = 0;
	float PosOffsetY = 0;
	float ZoomModifier = 0;
	float RotationModifier = 0;
	float PosXModifier = 0;
	float PosYModifier = 0;
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

//A lot of this is taken from before the rewrite
void ImageUI::DrawUI()
{
	PhotoTexture = SDL_CreateTextureFromSurface(Renderer, PhotoSurface);
	
	//Change zoom modifer
	if(Zooming > 0)
	{
		ZoomModifier += 0.025;
	}
	else if(Zooming < 0)
	{
		ZoomModifier -= 0.025;
	}
	//Change the rotation modifier
	if(Rotating > 0)
	{
		RotationModifier += 1;
	}
	else if(Rotating < 0)
	{
		RotationModifier -= 1;
	}
	//Change PosX modifier
	if(PosXModifier != 0)
	{
		PosOffsetX += PosXModifier / 1000;
	}
	if(PosYModifier != 0)
	{
		PosOffsetY += PosYModifier / 1000;
	}
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
	//Add or subtract the modifier
	Scalar += ZoomModifier;
	//Actually draw the image
	SDL_Rect PhotoRect = {0, 0, PhotoSurface->w * Scalar, PhotoSurface->h * Scalar};
	PhotoRect.x = ((Width - PhotoRect.w) / 2) + PosOffsetX;
	PhotoRect.y = ((Height - PhotoRect.h) / 2) + PosOffsetY;
	SDL_RenderCopyEx(Renderer, PhotoTexture, NULL, &PhotoRect, RotationModifier, nullptr, SDL_FLIP_NONE);
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
				//Up pressed
				else if(Event->jbutton.button == 13)
				{
					Zooming += 1;
				}
				//Down pressed
				else if(Event->jbutton.button == 15)
				{
					Zooming -= 1;
				}
				//Left pressed
				else if(Event->jbutton.button == 12)
				{
					Rotating -= 1;
				}
				//Right pressed
				else if(Event->jbutton.button == 14)
				{
					Rotating += 1;
				}
				//Y pressed
				else if(Event->jbutton.button == 10)
				{
					//Rottate image
					//Not Implemented
				}
			}
			break;
			
			//button up
			case SDL_JOYBUTTONUP:
			//Joycon 1 button released
			if (Event->jbutton.which == 0)
			{
				//Up released
				if(Event->jbutton.button == 13)
				{
					Zooming -= 1;
				}
				//Down released
				else if(Event->jbutton.button == 15)
				{
					Zooming += 1;
				}
				//Left released
				else if(Event->jbutton.button == 12)
				{
					Rotating += 1;
				}
				//Right released
				else if(Event->jbutton.button == 14)
				{
					Rotating -= 1;
				}
			}
			break;
		}
		//Broken code or broken joycons?
		/*
		//Reset the pos modifiers
		PosXModifier = 0;
		PosYModifier = 0;
		//Get the position of the joystick
		if(Event->jaxis.axis == 0)
		{
			PosXModifier = Event->jaxis.value;
		}
		else if(Event->jaxis.axis == 1)
		{
			PosYModifier = Event->jaxis.value;
		}
		//Reset the pos modifiers if the modifier is small to combat joycon drift
		if(abs(PosXModifier) < 10)
		{
			PosXModifier = 0;
		}
		if(abs(PosYModifier) < 10)
		{
			PosYModifier = 0;
		}*/
	}
}

void ImageUI::LoadFile()
{
	PhotoSurface = IMG_Load(ChosenFile->c_str());
	Scalar = 1;
	PosOffsetX = 0;
	PosOffsetY = 0;
	ZoomModifier = 0;
	RotationModifier = 0;
	Zooming = 0;
	Rotating = 0;
	PosXModifier = 0;
	PosYModifier = 0;
}