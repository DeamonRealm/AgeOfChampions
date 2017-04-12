#include "j1Render.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Console.h"
#include <math.h>

///Class Blit_Call ------------------------------
//Constructors ========================
Blit_Call::Blit_Call()
{

}

Blit_Call::Blit_Call(const iPoint & position, const iPoint& pivot, SDL_Texture* texture, SDL_Rect* rect, bool flip, int priority, uint opacity) :position(position),pivot(pivot), texture(texture), rect(rect), flip(flip), priority(priority), opacity(opacity)
{

}

//Destructors =========================
Blit_Call::~Blit_Call()
{

}

int Blit_Call::GetX() const
{
	return position.x;
}

int Blit_Call::GetY() const
{
	return position.y;
}

int Blit_Call::GetXPivot() const
{
	return pivot.x;
}

int Blit_Call::GetYPivot() const
{
	return pivot.y;
}

SDL_Texture * Blit_Call::GetTex() const
{
	return texture;
}

const SDL_Rect* Blit_Call::GetRect() const
{
	return rect;
}

bool Blit_Call::GetFlip() const
{
	return flip;
}

uint Blit_Call::GetOpacity() const
{
	return opacity;
}

bool Blit_Call::operator<(const Blit_Call & target) const
{
	if (priority < target.priority)return true;
	return false;
}

//Operator to compare blit calls in the priority queue

/// ---------------------------------------------

j1Render::j1Render() : j1Module()
{
	name = "renderer";
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	uint size = blit_queue.size();
	for (uint k = 0; k < size; k++)
	{
		const Blit_Call* blit = &blit_queue.top();
		Blit(blit->GetTex(), blit->GetX(), blit->GetY(), blit->GetRect(), blit->GetFlip(),blit->GetOpacity(), blit->GetXPivot(), blit->GetYPivot());
		blit_queue.pop();
	}

	SDL_Texture* fog_texture = SDL_CreateTextureFromSurface(App->render->renderer, App->win->screen_surface);

	if (SDL_RenderCopy(App->render->renderer, fog_texture, NULL, NULL))
	{
		LOG("Blit error: %s", SDL_GetError());
	}


	SDL_DestroyTexture(fog_texture);
	fog_texture = nullptr;

	return true;
}

bool j1Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	//LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::ChangeVSYNCstate(bool state)
{
	//Choose renderer vsync related flag
	Uint32 renderer_flag = SDL_RENDERER_PRESENTVSYNC;
	if (!state)renderer_flag |= SDL_RENDERER_ACCELERATED;
	
	//Generate renderer whit the new vsync state
	SDL_DestroyRenderer(renderer);
	renderer = SDL_CreateRenderer(App->win->window, -1, renderer_flag);
}

bool j1Render::CallBlit(SDL_Texture * texture, int x, int y, const SDL_Rect* section, bool horizontal_flip, int priority, uint opacity, int pivot_x, int pivot_y, float speed, double angle)
{
	bool ret = false;
	if (texture != nullptr)ret = true;
	blit_queue.emplace(iPoint(x, y), iPoint(pivot_x, pivot_y) , texture, (SDL_Rect*)section, horizontal_flip, priority, opacity);
	return true;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

iPoint j1Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, bool horizontal_flip, uint opacity, int pivot_x, int pivot_y, float speed, double angle) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	if (!horizontal_flip)
	{
		rect.x -= pivot_x;
		rect.y -= pivot_y;
	}
	else
	{
		rect.x -= (rect.w-pivot_x);
		rect.y -= pivot_y;
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != 0 && pivot_y != 0)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}
	int op_check = 2;
	if (opacity != 255)
	{
		op_check = SDL_SetTextureAlphaMod(texture, opacity);
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, (SDL_RendererFlip)horizontal_flip))
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	if (op_check != 2)
	{
		op_check = SDL_SetTextureAlphaMod(texture,255);
	}
	return ret;
}

bool j1Render::TileBlit(SDL_Texture * texture, int x, int y, const SDL_Rect* section)
{
	uint scale = App->win->GetScale();

	SDL_Rect rect = { camera.x + x * scale ,camera.y + y * scale ,section->w * scale ,section->h * scale };

	SDL_Point p = { 0,0 };

	if (SDL_RenderCopyEx(renderer, texture, section, &rect,0, &p,SDL_RendererFlip::SDL_FLIP_NONE))
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, float x_angle, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	//In case that the circle is fixed at camera coordinates
	if (!use_camera)
	{
		for (uint i = 0; i < 360; ++i)
		{
			points[i].x = (int)(x + radius * cos(i * factor));
			points[i].y = (int)((y + radius * sin(i * factor) * sin(x_angle)));
		}
	}
	//Else if the circle is not fixed
	else
	{
		for (uint i = 0; i < 360; ++i)
		{
			points[i].x = (int)(x + radius * cos(i * factor)) + camera.x;
			points[i].y = (int)((y + radius * sin(i * factor) * sin(x_angle))) + camera.y;
		}
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawTriangle(int x1, int y1, int x2, int y2, uint length, float angle, float x_angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	//Prepare render
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	/*
	A-----M-----B
	 -		   -
	  -       -
	   -     -
	    -   -
	     - -
		  0
	*/
	//Calculate 0 -> M vector
	fPoint mid_vector(x2 - x1, y2 - y1);
	mid_vector.Norm();
	mid_vector *= length * 0.5;

	//Calculate 0 -> A vector
	fPoint A_vector;
	A_vector.x = mid_vector.x * cos((angle * 0.5)) - mid_vector.y * sin((angle * 0.5));
	A_vector.y = mid_vector.x * sin((angle * 0.5)) + mid_vector.y * cos((angle * 0.5));
	A_vector.Norm();
	A_vector *= length;
	A_vector.y *= -sin(x_angle);

	//Calculate 0 -> B vector
	fPoint B_vector;
	B_vector.x = mid_vector.x * cos((-angle * 0.5)) - mid_vector.y * sin((-angle * 0.5));
	B_vector.y = mid_vector.x * sin((-angle * 0.5)) + mid_vector.y * cos((-angle * 0.5));
	B_vector.Norm();
	B_vector *= length;
	B_vector.y *= -sin(x_angle);

	iPoint v_A, v_B;
	iPoint position(x1, y1);

	//Calculate v_A vertex with vector 0 -> A
	v_A.x = position.x + A_vector.x;
	v_A.y = position.y + A_vector.y;

	//Calculate v_B vertex with vector 0 -> B
	v_B.x = position.x + B_vector.x;
	v_B.y = position.y + B_vector.y;

	DrawTriangle(&position, &v_A, &v_B, 0,0,0, 255);

	return true;
}

bool j1Render::DrawTriangle(const iPoint* pick, const iPoint* v_A, const iPoint* v_B, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	//Draw all the calculated vectors
	App->render->DrawLine(pick->x, pick->y, v_A->x, v_A->y, r, g, b, a);
	App->render->DrawLine(pick->x, pick->y, v_B->x, v_B->y, r, g, b, a);
	App->render->DrawLine(v_A->x, v_A->y, v_B->x, v_B->y, r, g, b, a);

	return true;
}

void j1Render::Console_Cvar_Input(Cvar* cvar, Command* command_type, std::string * input)
{
	//Set command
	if (*command_type->GetCommandStr() == "set")
	{
		if (*cvar->GetCvarName() == "vsync")
		{
			if (*input == "1")ChangeVSYNCstate(true);
			else if (*input == "0")ChangeVSYNCstate(false);
			else App->console->GenerateConsoleLabel("Value error vsync Cvar[0/1]");
		}

		//Unknown cvar
		else
		{
			App->console->GenerateConsoleLabel("Cvar id Error at module Render");
			return;
		}
	}
}

