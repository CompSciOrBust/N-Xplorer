#include <SDL.h>
#include <UI.h>
#include <fstream>
#include <iostream>
using namespace std;

class TextUI : public UIWindow
{
	private:
	//vars
	vector <string> LinesVec;
	TTF_Font *LineFont;
	int LineToRenderFrom = 0;
	int SelectedLine = 0;
	bool ScrollUp = false;
	bool ScrollDown = false;
	//Functions
	void DrawTextLines();
	public:
	//vars
	string *ChosenFile;
	//funcrions
	TextUI();
	void GetInput();
	void DrawUI();
	void LoadFile();
};

TextUI::TextUI()
{
	LineFont = GetSharedFont(24); //Load the font
}

void TextUI::DrawUI()
{
	//Draw the BG
	SDL_SetRenderDrawColor(Renderer, 94, 94, 94, 255);
	SDL_Rect BGRect = {0,0, Width, Height};
	SDL_RenderFillRect(Renderer, &BGRect);
	DrawTextLines();
}

void TextUI::GetInput()
{
	//Reset the scroll vars
	ScrollUp = false;
	ScrollDown = false;
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
				//Up pressed
				if (Event->jbutton.button == 13)
				{
					ScrollUp = true;
				}
				//Down pressed
				else if(Event->jbutton.button == 15)
				{
					ScrollDown = true;
				}
				//B pressed
				else if(Event->jbutton.button == 1)
				{
					*WindowState = 0;
				}
			}
		}
	}
}

//This function was largely taken unaltered from before the rewrite
void TextUI::DrawTextLines()
{
	//vars
	int LineWidth = (Width / 100) * 96;
	int LineYOffset = 0;
	
	//Pre rewrite input scan was here
	
	//If the lines vector is empty then we need to add a line
	if(LinesVec.empty()) LinesVec.insert(LinesVec.begin(), "");
	
	//Indirectly edit the selected line otherwise sdl break while using the Analogue stick
	if(ScrollUp) SelectedLine--;
	if(ScrollDown) SelectedLine++;
	
	//Check the lines to draw from is valid
	if(SelectedLine < 0) SelectedLine = 0;
	if(SelectedLine > LinesVec.size()-1) SelectedLine = LinesVec.size()-1;
	if(SelectedLine < LineToRenderFrom) LineToRenderFrom = SelectedLine;
	//Vars for checking if the selected line was drawn
	bool HasRenderedSelectedLine = false;
	bool NeedsToReDraw = false;
	Redraw:
	//Draw the lines
	for(int i = LineToRenderFrom; i < LinesVec.size(); i++)
	{
		//vars
		bool ThisIsTheSeletcedLine = false;
		string EmptyLine = " ";
		//Line reference
		string LineToDraw = "";
		//If line is emptry make it a space to avoid crashing SDL
		if(LinesVec.at(i).empty())
		{
			LineToDraw = EmptyLine;
		}
		else
		{
			LineToDraw = LinesVec.at(i);
		}
		//Alternate colours
		if(i % 2 == 1)
		{
			SDL_SetRenderDrawColor(Renderer, 66, 66, 66, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(Renderer, 33, 33, 33, 255);
		}
		if(i == SelectedLine)
		{
			SDL_SetRenderDrawColor(Renderer, 161, 161, 161, 255);
			HasRenderedSelectedLine = true;
			ThisIsTheSeletcedLine = true;
		}
		//Draw line
		SDL_Color TextColour = {255, 255, 255};
		SDL_Surface* LineSurface = TTF_RenderUTF8_Blended_Wrapped(LineFont, LineToDraw.c_str(), TextColour, LineWidth);
		SDL_Texture* LineTexture = SDL_CreateTextureFromSurface(Renderer, LineSurface);
		//Calculate bg rectangle height based on LineSurface
		SDL_Rect LineRect = {(Width / 100) * 4, LineYOffset, Width, LineSurface->h};
		SDL_RenderFillRect(Renderer, &LineRect);
		LineRect.w = LineSurface->w;
		//Draw the line number
		SDL_Surface* LineNumSurface = TTF_RenderText_Blended_Wrapped(LineFont, to_string(i+1).c_str(), TextColour, (Width / 100) * 4);
		SDL_Texture* LineNumTexture = SDL_CreateTextureFromSurface(Renderer, LineNumSurface);
		SDL_Rect LineNumRect = {0, LineYOffset, LineNumSurface->w, LineNumSurface->h};
		SDL_RenderCopy(Renderer, LineNumTexture, NULL, &LineNumRect);
		//Check if the selected line is cut off by the bottom of the screem
		if(ThisIsTheSeletcedLine && LineYOffset + LineSurface->h > Height)
		{
			LineToRenderFrom++;
			NeedsToReDraw = true;
		}
		//Increase LineYOffset
		LineYOffset += LineSurface->h;
		SDL_RenderCopy(Renderer, LineTexture, NULL, &LineRect);
		//Clean up
		SDL_DestroyTexture(LineTexture);
		SDL_FreeSurface(LineSurface);
		SDL_DestroyTexture(LineNumTexture);
		SDL_FreeSurface(LineNumSurface);
		//No need to draw what we can't see
		if(LineYOffset > Height)
		{
			//Ok maybe we do need to draw what we can't see...
			if(!HasRenderedSelectedLine)
			{
				//Selected line is below the rendered lines
				if(SelectedLine > i)
				{
					LineToRenderFrom++;
				}
				NeedsToReDraw = true;
			}
			break;
		}
	}
	//Check if we need to redraw
	if(NeedsToReDraw)
	{
		//Make sure we don't redraw forever
		NeedsToReDraw = false;
		//Use an evil goto statement for a quick and dirty redraw
		goto Redraw;
	}
}

void TextUI::LoadFile()
{
	LineToRenderFrom = 0;
	SelectedLine = 0;
	LinesVec.clear();
	ifstream FileReader(ChosenFile->c_str());
	//Can not read file
	if(!FileReader)
	{
		LinesVec.push_back("Error opening file");
		return;
	}
	//Check if file is empty
	if(FileReader.peek() == std::ifstream::traits_type::eof())
	{
		LinesVec.push_back("");
		return;
	}
	//Read each line
	string FileLine;
	for(int i = 0; !FileReader.eof(); i++)
	{
		getline(FileReader, FileLine);
		LinesVec.push_back(FileLine);
	}
}