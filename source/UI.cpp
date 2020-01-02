#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <switch.h>
using namespace std;

//Global vars
int BorderSize = 3;

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
};

ScrollList::ScrollList()
{
	PSelectedIndex = &SelectedIndex;
	PCursorIndex = &CursorIndex;
	PListRenderOffset = &ListRenderOffset;
}

void ScrollList::Enslave(ScrollList* Master)
{
	PSelectedIndex = Master->PSelectedIndex;
	PCursorIndex = Master->PCursorIndex;
	PListRenderOffset = Master->PListRenderOffset;
}

bool CheckButtonPressed(SDL_Rect* ButtonRect, int TouchX, int TouchY)
{
	if(TouchX > ButtonRect->x && TouchX < ButtonRect->x + ButtonRect->w && TouchY > ButtonRect->y && TouchY < ButtonRect->y + ButtonRect->h) return true;
	else return false;
}

void ScrollList::DrawList()
{
	ItemSelected = false;
	int ListingHeight = ListHeight / ListingsOnScreen;
	int ListLength = ListingsOnScreen;
	if(ListLength > ListingTextVec.size()) ListLength = ListingTextVec.size();
	
	//Check cursor is with in bounds
	if(*PCursorIndex > ListLength-1)
	{
		CursorIndex = ListLength-1;
		ListRenderOffset++;
	}
	if(*PCursorIndex < 0)
	{
		CursorIndex = 0;
		ListRenderOffset--;
	}
	
	//Check selected index is with in the bounds
	if(*PSelectedIndex < 0)
	{
		*PSelectedIndex = ListingTextVec.size()-1;
		*PCursorIndex = ListLength;
		ListRenderOffset = ListingTextVec.size() - ListLength-1;
		//Don't crash if we scroll up and don't have enough items in the list to show
		if(ListRenderOffset < 0)
		{
			*PSelectedIndex = ListingTextVec.size()-1;
			*PCursorIndex = ListingTextVec.size()-1;
			ListRenderOffset = 0;
		}
	}
	else if(*PSelectedIndex > ListingTextVec.size()-1)
	{
		*PSelectedIndex = 0;
		*PCursorIndex = 0;
		ListRenderOffset = 0;
	}
	
	//Draw the list
	for(int i = 0; i < ListLength; i++)
	{
		//Set the background color
		if(IsActive) SDL_SetRenderDrawColor(Renderer, ListColour_R, ListColour_G, ListColour_B, 255);
		else SDL_SetRenderDrawColor(Renderer, 96, 204, 204, 255);
		//Check if this is the highlighted file
		if(i == *PCursorIndex && IsActive)
		{
			SDL_SetRenderDrawColor(Renderer, ListColourSelected_R, ListColourSelected_G, ListColourSelected_B, 255);
			//Cyan 50
			//if(IsActive) SDL_SetRenderDrawColor(Renderer, 224, 247, 250, 255);
			//else SDL_SetRenderDrawColor(Renderer, 232, 234, 246, 255); //Indigo
		}
		
		SDL_Rect MenuItem = {ListXOffset, ListYOffset + (i * ListingHeight), ListWidth, ListingHeight};
		SDL_RenderFillRect(Renderer, &MenuItem);
		
		//Draw file names
		SDL_Color TextColour = {255, 255, 255};
		SDL_Surface* FileNameSurface = TTF_RenderUTF8_Blended_Wrapped(ListFont, ListingTextVec.at(i + *PListRenderOffset).c_str(), TextColour, ListWidth);
		SDL_Texture* FileNameTexture = SDL_CreateTextureFromSurface(Renderer, FileNameSurface);
		//Calculate text X and Y Coords
		int TextY = MenuItem.y + ((MenuItem.h - FileNameSurface->h) / 2);
		int TextX = 0;
		if(CenterText)
		{
			TextX = MenuItem.x + ((MenuItem.w - FileNameSurface->w) / 2);
		}
		else
		{
			TextX = MenuItem.x;
		}
		SDL_Rect AmiiboNameRect = {TextX, TextY, FileNameSurface->w, FileNameSurface->h};
		SDL_RenderCopy(Renderer, FileNameTexture, NULL, &AmiiboNameRect);
		
		//Draw borders
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_Rect BorderRect = {ListXOffset, ListYOffset + (i * ListingHeight) - 1, ListWidth, BorderSize};
		SDL_RenderFillRect(Renderer, &BorderRect);
		//Check if we need to draw one more border
		if(i == ListLength - 1)
		{
			BorderRect = {ListXOffset, ListYOffset + (++i * ListingHeight) - 1, ListWidth, BorderSize};
			SDL_RenderFillRect(Renderer, &BorderRect);
		}
		
		/*
		//Check if option is pressed
		if(CheckButtonPressed(&MenuItem, *TouchListX, *TouchListY))
		{
			int SelectedDifference = i - *PCursorIndex;
			SelectedIndex += SelectedDifference;
			*PCursorIndex = i;
			ItemSelected = true;
		}*/
	
		//Clean up
		SDL_DestroyTexture(FileNameTexture);
		SDL_FreeSurface(FileNameSurface);
	}
}

//Thank you to Nichole Mattera for telling me how to do this
TTF_Font *GetSharedFont(int FontSize)
{
	PlFontData standardFontData;
	plGetSharedFontByType(&standardFontData, PlSharedFontType_Standard);
	return TTF_OpenFontRW(SDL_RWFromMem(standardFontData.address, standardFontData.size), 1, FontSize);
}

TTF_Font *GetSharedFontExt(int FontSize)
{
	PlFontData standardFontData;
	plGetSharedFontByType(&standardFontData, PlSharedFontType_NintendoExt );
	return TTF_OpenFontRW(SDL_RWFromMem(standardFontData.address, standardFontData.size), 1, FontSize);
}