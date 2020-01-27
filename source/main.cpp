#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <switch.h>
#include <ExplorerUI.h>
#include <TextUI.h>
#include <ImageUI.h>
#include <thread>
#include <mutex>

// Main program entrypoint
int main(int argc, char* argv[])
{
	//vars
	int Done = 0;
	int Width = 1280;
	int Height = 720;
	int WindowState = 0;
	int *WindowStatePtr = &WindowState;
	int *DonePtr = &Done;
	SDL_Window *Window;
	SDL_Renderer *Renderer;
	SDL_Event Event;
	string ChosenFile = "";
	
	//Init nx link for printf
	socketInitializeDefault();
	nxlinkStdio();
	
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
	
	//Mount the file systems
	//user
	FsFileSystem UserFS;
	fsOpenBisFileSystem(&UserFS, FsBisPartitionId_User, "");
	fsdevMountDevice("user", UserFS);
	//System
	FsFileSystem SysFS;
	fsOpenBisFileSystem(&SysFS, FsBisPartitionId_System, "");
	fsdevMountDevice("sys", UserFS);
	
	//Init explorer UI
	ExplorerUI *Explorer = new ExplorerUI();
	Explorer->Renderer = Renderer;
	Explorer->IsDone = DonePtr;
	Explorer->Event = &Event;
	Explorer->WindowState = WindowStatePtr;
	Explorer->FileNameList->Renderer = Renderer;
	Explorer->FileSizeList->Renderer = Renderer;
	Explorer->ChosenFile = &ChosenFile;
	std::mutex ExplorerAccess;
	std::mutex *ExplorerAccessPtr = &ExplorerAccess;
	
	//Init context menu
	MenuUI *Menu = new MenuUI();
	Menu->Renderer = Renderer;
	Menu->IsDone = DonePtr;
	Menu->Event = &Event;
	Menu->WindowState = WindowStatePtr;
	Menu->MenuList->Renderer = Renderer;
	Menu->Explorer = Explorer;
	
	//Init text editor
	TextUI *TextEditor = new TextUI();
	TextEditor->Renderer = Renderer;
	TextEditor->IsDone = DonePtr;
	TextEditor->Event = &Event;
	TextEditor->WindowState = WindowStatePtr;
	TextEditor->ChosenFile = &ChosenFile;
	std::mutex TextEditorAccess;
	std::mutex *TextEditorAccessPtr = &TextEditorAccess;
	
	//Init the image viewer
	ImageUI *ImageViewer = new ImageUI();
	ImageViewer->Renderer = Renderer;
	ImageViewer->Event = &Event;
	ImageViewer->WindowState = WindowStatePtr;
	ImageViewer->ChosenFile = &ChosenFile;
	std::mutex ImageViewerAccess;
	std::mutex *ImageViewerAccessPtr = &ImageViewerAccess;
	
	//Input loop thread
	std::thread InputLoopThread([DonePtr, WindowStatePtr, Explorer, Menu, TextEditor, ImageViewer, ExplorerAccessPtr, TextEditorAccessPtr, ImageViewerAccessPtr]()
	{
		while(!*DonePtr)
		{
			switch(*WindowStatePtr)
			{
				//Get explorer input.
				case 0:
				{
					ExplorerAccessPtr->lock();
					Explorer->GetInput();
					//If switched to the text editor we need to load the file
					if(*WindowStatePtr == 2)
					{
						TextEditorAccessPtr->lock();
						TextEditor->LoadFile();
						TextEditorAccessPtr->unlock();
					}
					else if(*WindowStatePtr == 3)
					{
						ImageViewerAccessPtr->lock();
						ImageViewer->LoadFile();
						ImageViewerAccessPtr->unlock();
					}
					ExplorerAccessPtr->unlock();
				}
				break;
				//Get the sub menu input
				case 1:
				{
					ExplorerAccessPtr->lock();
					Menu->GetInput();
					ExplorerAccessPtr->unlock();
				}
				break;
				//Get the text editor input
				case 2:
				{
					TextEditorAccessPtr->lock();
					TextEditor->GetInput();
					TextEditorAccessPtr->unlock();
				}
				break;
				//Get the image viewer input
				case 3:
				{
					ImageViewerAccessPtr->lock();
					ImageViewer->GetInput();
					ImageViewerAccessPtr->unlock();
				}
				break;
			}
		}
	});
	
	//Main gui loop
	while(!Done)
	{
		//Clear the frame
		SDL_RenderClear(Renderer);
		switch(WindowState)
		{
			//Draw the file explorer
			case 0:
			{
				ExplorerAccess.lock();
				//Draw the UI
				Explorer->DrawUI();
				ExplorerAccess.unlock();
			}
			break;
			//Draw the menu
			case 1:
			{
				ExplorerAccess.lock();
				Explorer->DrawUI();
				Menu->DrawUI();
				ExplorerAccess.unlock();
			}
			break;
			//Draw the text editor
			case 2:
			{
				TextEditorAccess.lock();
				TextEditor->DrawUI();
				TextEditorAccess.unlock();
			}
			break;
			//Draw the image viewer
			case 3:
			{
				ImageViewerAccess.lock();
				ImageViewer->DrawUI();
				ImageViewerAccess.unlock();
			}
			break;
			//Long opperation display window
			case 4:
			{
				Explorer->DrawUI();
				Menu->DrawLongOpMessage();
			}
			break;
		}
		//Draw the frame
        SDL_RenderPresent(Renderer);
	}
	
	InputLoopThread.join();
	
	//Clean up
	socketExit();
	plExit();
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
    return 0;
}
