#ifndef _UI_POPUP_MENU_
#define _UI_POPUP_MENU_

#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_String.h"


class UI_Popup_menu : public UI_Element {
public:

	UI_Popup_menu(const SDL_Rect& box, const UI_Image& tex_menu, const UI_Image& tex_selection);
	UI_Popup_menu(const UI_Popup_menu* copy);
	UI_Popup_menu();

	~UI_Popup_menu();

public:

	UI_Image		tex_selection;
	UI_Image		tex_menu;
	SDL_Color		menu_color;
	SDL_Color		Selection_color;

private:
	//List of menu options and its locations
	std::list<UI_String*>		Items;
	std::list<iPoint>			Items_location;

	UI_String*		option_selected = nullptr;
	uint			cell_height = 0;
	uint			text_size = 0;

public:

	//Game Loop
	void			Draw(bool debug) const;
	void			HandleInput();

	//Functionality
	void			AddOption(UI_String* new_item);
	void			UpdateContentPosition(UI_String* new_item);
	void			ResizeCells();
	void			SetOptionSelection();
	void			SetOptionSelection(UI_String* new_item);
	void			SetOptionSelectionArrows(bool go_down);
	char*			GetOptionSelected() const;
	void			SetTexSelection(SDL_Rect img_rect, int text_id = -1);
	void			SetTexMenu(SDL_Rect img_rect, int text_id = -1);

};

#endif // __UI_Popup_Menu__