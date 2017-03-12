#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"

#include "p2Point.h"
#include "SDL\include\SDL_rect.h"


struct UI_Element;
struct SDL_Texture;
enum UI_TYPE;


enum GUI_INPUT {

	UP_ARROW,
	DOWN_ARROW,
	LEFT_ARROW,
	RIGHT_ARROW,
	MOUSE_LEFT_BUTTON_DOWN,
	MOUSE_LEFT_BUTTON_REPEAT,
	MOUSE_LEFT_BUTTON_UP,
	MOUSE_RIGHT_BUTTON,
	MOUSE_IN,
	MOUSE_OUT,
	SUPR,
	BACKSPACE,
	ENTER,
	TAB

};

enum CURSOR_TARGET
{
	DEFAULT,
	SELECT,
	ATTACK,
	REPAIR,
	GUARD,
	CAPTURE,
	MARK,
	FIRE
};

enum TEXTURE_ID
{
	BACKGROUND,
	STANDARD,
	MAINMENU,
	ICONS,
	HUD
};

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:

	//UI debug mode
	mutable bool			debug = false;

	//UI elements
	std::list<UI_Element*>	screens;
	uint					tabable_elements = 0;

	//UI Textures
	SDL_Texture*			atlas;
	SDL_Texture*			background;
	SDL_Texture*			mainmenu;
	SDL_Texture*			standardmenu;
	SDL_Texture*			icons;
	SDL_Texture*			hud;
	
	std::list<SDL_Texture*>	ui_textures;

	std::string				atlas_file_name;
	std::string				background_file_name;
	std::string				mainmenu_file_name;
	std::string				standardmenu_file_name;
	std::string				icons_file_name;
	std::string				hud_file_name;

	//Target Module of gui input
	j1Module*				default_input_target;

	//Cursor type;
	CURSOR_TARGET			cursor_type;
	SDL_Rect				cursor_rect;
	iPoint					cursor_pivot;

	std::vector<SDL_Rect>	cursors_rects;
	std::vector<iPoint>		cursors_pivots;

public:

	mutable UI_Element*			ItemSelected = nullptr;
	mutable uint				upper_element = 0;

	//UI textures functions
	const SDL_Texture*	GetAtlas() const;
	SDL_Texture*		Get_UI_Texture(uint tex_id);

	//UI tab functions
	uint				GetTabNumber()const;
	void				SetTabNumber(uint new_tab_num);
	UI_Element*			GetTabElement(UI_Element* screen, uint index)const;

	//UI elements functions
	uint				PushScreen(const UI_Element* new_screen);
	UI_Element*			GenerateUI_Element(UI_TYPE element_type);

	UI_Element*			GetActiveScreen();
	uint				CalculateUpperElement(UI_Element* parent, uint layer = 0);

	//mouse function
	void				ChangeMouseTexture(CURSOR_TARGET type);
	void				LoadCursorTypes();

	//Draw mouse
	void				DrawMouseTexture();

	//Gui taget funtions
	void				SetDefaultInputTarget(j1Module* target);
	j1Module*			GetDefaultInputTarget()const;
};

#endif // __GUI_H__