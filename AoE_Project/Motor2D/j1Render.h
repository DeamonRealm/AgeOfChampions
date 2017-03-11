#ifndef __j1RENDER_H__
#define __j1RENDER_H__

#include "SDL/include/SDL.h"
#include "p2Point.h"
#include "j1Module.h"
#include <queue>

class Cvar;

///Class Blit_Call ------------------------------
//Little class to save all blit calls and organize them in a priority queue
class Blit_Call
{
public:

	Blit_Call();
	Blit_Call(const iPoint& position, const iPoint& pivot, SDL_Texture* texture, SDL_Rect* rect, bool flip, int priority, uint opacity);
	~Blit_Call();

private:

	iPoint			position = { 0,0 };
	iPoint			pivot = { 0,0 };
	SDL_Texture*	texture = nullptr;
	SDL_Rect*		rect = nullptr;
	bool			flip = false;
	uint			opacity = 255;
	int				priority = 0;


public:

	//Get Methods -----------
	int				GetX()const;
	int				GetY()const;
	int				GetXPivot()const;
	int				GetYPivot()const;
	SDL_Texture*	GetTex()const;
	const SDL_Rect*	GetRect()const;
	bool			GetFlip()const;
	uint			GetOpacity()const;

	//Operator to compare blit calls in the priority queue
	bool operator<(const Blit_Call& target) const;

};
/// ---------------------------------------------

class j1Render : public j1Module
{
public:

	j1Render();

	// Destructor
	virtual ~j1Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	//Console Input method
	void Console_Cvar_Input(Cvar* cvar, Command* command_type, std::string* input);

private:

	//Priority queue to organize all the blits
	std::priority_queue<Blit_Call> blit_queue;

public:

	SDL_Renderer*	renderer;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;

public:

	//Add blit call ---------
	bool	CallBlit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, bool horizontal_flip = false, int priority = 0, uint opacity = 255, int pivot_x = 0, int pivot_y = 0, float speed = 1.0f, double angle = 0);
	
	//View port Methods ------
	void	SetViewPort(const SDL_Rect& rect);
	void	ResetViewPort();

	//View frame change -----
	iPoint	ScreenToWorld(int x, int y) const;

	// Draw & Blit ----------
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, bool horizontal_flip = false, uint opacity = 255, int pivot_x = 0, int pivot_y = 0, float speed = 1.0f, double angle = 0) const;
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, float x_angle = 0, bool use_camera = true) const;

	// Set background color -
	void SetBackgroundColor(SDL_Color color);

	//Active/Deactivate vsync
	void ChangeVSYNCstate(bool state);

};

#endif // __j1RENDER_H__