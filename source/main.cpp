#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <switch.h>
#include <ExplorerUI.h>
#include <TextUI.h>
#include <ImageUI.h>
#include <Settings.h>
#include <Updater.h>
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
	nsInitialize(); //Init ns service for game save mounting
	accountInitialize(AccountServiceType_Application); //Init the account service. Needed for getting the user ID.
	
	//Mount the file systems
	//user
	FsFileSystem UserFS;
	fsOpenBisFileSystem(&UserFS, FsBisPartitionId_User, "");
	fsdevMountDevice("user", UserFS);
	//System
	FsFileSystem SysFS;
	fsOpenBisFileSystem(&SysFS, FsBisPartitionId_System, "");
	fsdevMountDevice("sys", SysFS);
	
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
	Explorer->LongOpMessagePtr = &Menu->LongOpMessage;
	
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
	
	//Init the settings menu
	SettingsUI *SettingsMenu = new SettingsUI();
	SettingsMenu->Renderer = Renderer;
	SettingsMenu->Event = &Event;
	SettingsMenu->WindowState = WindowStatePtr;
	SettingsMenu->Explorer = Explorer;
	SettingsMenu->ContextMenu = Menu;
	
	//Init the updater
	UpdaterUI *Updater = new UpdaterUI();
	Updater->Renderer = Renderer;
	Updater->IsDone = DonePtr;
	Updater->Event = &Event;
	Updater->WindowState = WindowStatePtr;
	Updater->Menu = Menu;
	Updater->NroPath = argv[0];
	
	//Load the settings and create ini on first start up
	SettingsMenu->CreateNewIni();
	
	//Input loop thread
	std::thread InputLoopThread([DonePtr, WindowStatePtr, Explorer, Menu, TextEditor, ImageViewer, SettingsMenu, ExplorerAccessPtr, TextEditorAccessPtr, ImageViewerAccessPtr, Updater]()
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
					else if(*WindowStatePtr == 6)
					{
						SettingsMenu->UpdateSettingsText();
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
				//Text editor save options input
				case 5:
				{
					TextEditorAccessPtr->lock();
					TextEditor->GetSaveInput();
					//Save file if user selected option
					if(*WindowStatePtr == 0)
					{
						TextEditor->SaveFile();
						Explorer->LoadListDirs(Explorer->DirPath);
					}
					TextEditorAccessPtr->unlock();
				}
				break;
				//N-Xplorer settings input
				case 6:
				{
					ExplorerAccessPtr->lock();
					SettingsMenu->GetInput();
					ExplorerAccessPtr->unlock();
				}
				break;
				//Updater input
				case 7:
				{
					Updater->GetInput();
				}
				break;
				//Unzip logic
				case 8:
				{
					std::string ExtractionDirName = GetKeyboardInput("Extract", "Folder name", GetFileNameFromPath(*Explorer->ChosenFile));
					std::string ExtractionPath = Explorer->DirPath + ExtractionDirName + "/";
					if(ExtractionDirName != "")
					{
						mkdir(ExtractionPath.c_str(), 0);
					}
					*WindowStatePtr = 4;
					UnzipFile(*Explorer->ChosenFile, ExtractionPath);
					*WindowStatePtr = 0;
					Explorer->LoadListDirs(Explorer->DirPath);
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
			//Draw text editor save options
			case 5:
			{
				TextEditorAccess.lock();
				TextEditor->DrawUI();
				TextEditor->DrawSaveOptions();
				TextEditorAccess.unlock();
			}
			break;
			//Draw N-Xplorer settings
			case 6:
			{
				ExplorerAccess.lock();
				Explorer->DrawUI();
				SettingsMenu->DrawUI();
				ExplorerAccess.unlock();
			}
			break;
			//Draw the updater UI
			case 7:
			{
				Explorer->DrawUI();
				Updater->DrawUI();
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
	nsExit();
	accountExit();
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
    return 0;
}
