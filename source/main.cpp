#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <switch.h>
#include <ExplorerUI.h>
#include <TextUI.h>
#include <ImageUI.h>
using namespace std;

// Main program entrypoint
int main(int argc, char* argv[])
{
	//vars
	int Done = 0;
	int Width = 1280;
	int Height = 720;
	int WindowState = 0;
	SDL_Window *Window;
	SDL_Renderer *Renderer;
	SDL_Event Event;
	string ChosenFile = "";
	
	//init
    // mandatory at least on switch, else gfx is not properly closed
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
	
    // create an SDL window (OpenGL ES2 always enabled)
    // when SDL_FULLSCREEN flag is not set, viewport is automatically handled by SDL (use SDL_SetWindowSize to "change resolution")
    // available switch SDL2 video modes :
    // 1920 x 1080 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    // 1280 x 720 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    Window = SDL_CreateWindow("sdl2_gles2", 0, 0, Width, Height, 0);
    if (!Window) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
	
    // create a renderer (OpenGL ES2)
    Renderer = SDL_CreateRenderer(Window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!Renderer) {
        SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
	
    // open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
    for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }
	
	TTF_Init(); //Init the font
	plInitialize(); //Init needed for shared font
	
	//Init explorer UI
	ExplorerUI *Explorer = new ExplorerUI();
	Explorer->Renderer = Renderer;
	Explorer->IsDone = &Done;
	Explorer->Event = &Event;
	Explorer->WindowState = &WindowState;
	Explorer->FileNameList->Renderer = Renderer;
	Explorer->FileSizeList->Renderer = Renderer;
	Explorer->ChosenFile = &ChosenFile;
	
	//Init context menu
	MenuUI *Menu = new MenuUI();
	Menu->Renderer = Renderer;
	Menu->IsDone = &Done;
	Menu->Event = &Event;
	Menu->WindowState = &WindowState;
	Menu->MenuList->Renderer = Renderer;
	Menu->Explorer = Explorer;
	
	//Init text editor
	TextUI *TextEditor = new TextUI();
	TextEditor->Renderer = Renderer;
	TextEditor->IsDone = &Done;
	TextEditor->Event = &Event;
	TextEditor->WindowState = &WindowState;
	TextEditor->ChosenFile = &ChosenFile;
	
	//Init the image viewer
	ImageUI *ImageViewer = new ImageUI();
	ImageViewer->Renderer = Renderer;
	ImageViewer->Event = &Event;
	ImageViewer->WindowState = &WindowState;
	ImageViewer->ChosenFile = &ChosenFile;
	
	//Main loop
	while(!Done)
	{
		//Clear the frame
		SDL_RenderClear(Renderer);
		switch(WindowState)
		{
			//Draw the file explorer
			case 0:
			{
				Explorer->GetInput();
				Explorer->DrawUI();
				//If switched to the text editor we need to load the file
				if(WindowState == 2) TextEditor->LoadFile();
				else if (WindowState == 3) ImageViewer->LoadFile();
			}
			break;
			//Draw the menu
			case 1:
			{
				Explorer->DrawUI();
				Menu->GetInput();
				Menu->DrawUI();
			}
			break;
			//Draw the text editor
			case 2:
			{
				TextEditor->GetInput();
				TextEditor->DrawUI();
			}
			break;
			//Draw the image viewer
			case 3:
			{
				ImageViewer->GetInput();
				ImageViewer->DrawUI();
			}
			break;
		}
		//Draw the frame
        SDL_RenderPresent(Renderer);
	}
	
	//Clean up
	plExit();
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
    return 0;
}
