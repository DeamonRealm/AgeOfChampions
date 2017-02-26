#ifndef _UI_FIXED_BUTTON_
#define _UI_FIXED_BUTTON_

#include "UI_Element.h"
#include "UI_Image.h"
#include "p2Point.h"

enum FIX_BUTTON_STATE {
	DOWN,
	UP,
	OVER_UP,
	OVER_DOWN

};

class UI_Fixed_Button : public UI_Element
{
public:
	UI_Fixed_Button(const SDL_Rect& box, const UI_Image& tex_up, const UI_Image& tex_down, const UI_Image& tex_over, const UI_Image& tex_over_down);
	UI_Fixed_Button(const UI_Fixed_Button* copy);
	UI_Fixed_Button();

	~UI_Fixed_Button();

public:

	UI_Image			tex_up;
	UI_Image			tex_down;
	UI_Image			tex_over;
	UI_Image			tex_over_down;
	FIX_BUTTON_STATE	button_state;

public:
	//Game Loop
	void				Draw(bool debug) const;
	void				HandleInput();

	//Functionality ===========================================
	FIX_BUTTON_STATE	Change_State(FIX_BUTTON_STATE new_button_state);
	FIX_BUTTON_STATE	Get_State()const;
	void				SetTexUP(SDL_Rect img_rect, iPoint pivot = { 0,0 }, int text_id = -1);
	void				SetTexDOWN(SDL_Rect img_rect, iPoint pivot = { 0,0 }, int text_id = -1);
	void				SetTexOVERUP(SDL_Rect img_rect, iPoint pivot = { 0,0 }, int text_id = -1);
	void				SetTexOVERDOWN(SDL_Rect img_rect, iPoint pivot = { 0,0 }, int text_id = -1);
	void				SetTexFromXML(const pugi::xml_node&	conf, int text_id = -1);

};

#endif //_UI_FIXED_BUTTON_