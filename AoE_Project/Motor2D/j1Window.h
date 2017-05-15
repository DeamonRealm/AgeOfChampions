#ifndef __j1WINDOW_H__
#define __j1WINDOW_H__

#include "j1Module.h"

struct SDL_Window;
struct SDL_Surface;

class j1Window : public j1Module
{
public:

	j1Window();

	// Destructor
	virtual ~j1Window();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();

	// Called before quitting
	bool CleanUp();

	// Change title
	void SetTitle(const char* new_title);

	// Retrieve window size
	void GetWindowSize(uint& width, uint& height) const;

	// Retrieve window scale
	uint GetScale() const;

public:

	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

private:

	std::string	title;
	uint		width = 0;
	uint		height = 0;
	uint		scale = 0;

public:

	bool		fullscreen = false;

};

#endif // __j1WINDOW_H__