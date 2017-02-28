#include "UI_Fixed_Button.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Gui.h"

//Constructors ============================================
UI_Fixed_Button::UI_Fixed_Button(const SDL_Rect& box, const UI_Image& tex_up, const UI_Image& tex_down, const UI_Image& tex_over, const UI_Image& tex_over_down) :UI_Element(box, BUTTON), tex_up(tex_up), tex_down(tex_down), tex_over(tex_over), tex_over_down(tex_over_down), button_state(UP) {}

UI_Fixed_Button::UI_Fixed_Button(const UI_Fixed_Button* copy) : UI_Element(copy->box, copy->ui_type), tex_up(copy->tex_up), tex_down(copy->tex_down), tex_over(copy->tex_over), tex_over_down(copy->tex_over_down), button_state(UP) {};

UI_Fixed_Button::UI_Fixed_Button() : UI_Element({ 0,0,0,0 }, FIXED_BUTTON), button_state(UP) {}


//Destructor ==============================================
UI_Fixed_Button::~UI_Fixed_Button(){}


//Game Loop ===============================================
void UI_Fixed_Button::Draw(bool debug) const
{

	//This Draw
	int x = this->box.x;
	int y = this->box.y;

	if (debug)App->render->DrawQuad({ x - App->render->camera.x, y - App->render->camera.y, box.w, box.h }, 150, 250, 0);

	//This Draw
	switch (button_state)
	{
	case FIX_BUTTON_STATE::DOWN:
	{
		tex_down.DrawAt(x, y);
		break;
	}
	case FIX_BUTTON_STATE::UP:
	{
		tex_up.DrawAt(x, y);
		break;
	}
	case FIX_BUTTON_STATE::OVER_UP:
	{
		tex_over.DrawAt(x, y);
		break;
	}
	case FIX_BUTTON_STATE::OVER_DOWN:
	{
		tex_over_down.DrawAt(x, y);
		break;
	}
	}

	//Childs Draw
	DrawChilds(debug);
}

void UI_Fixed_Button::HandleInput()
{
	if (button_state == DOWN) return;
	//Mouse In/Out ------------------------------
	if (this->MouseIsIn() && App->gui->upper_element == this->layer)
	{
		button_state = OVER_UP;
		input_target->GUI_Input(this, MOUSE_IN);
	}
	else if (this->MouseIsIn()==false && App->gui->upper_element == this->layer && button_state != DOWN)
	{
		button_state = UP;
		input_target->GUI_Input(this, MOUSE_OUT);
	}

	//Mouse Left Button -------------------------
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (this->MouseIsIn() && App->gui->upper_element == this->layer)
		{
			App->gui->ItemSelected = this;
			button_state = OVER_DOWN;
			input_target->GUI_Input(this, MOUSE_LEFT_BUTTON_DOWN);
		}
	}
	//Mouse Left Button UP -----------------------
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		if (this->MouseIsIn() && App->gui->upper_element == this->layer)
		{
			button_state = DOWN;
			App->gui->ItemSelected = nullptr;
			input_target->GUI_Input(this, MOUSE_LEFT_BUTTON_UP);
		}
	}

	//Mouse Left Button DOWN REPEAT---------------
	if (App->gui->ItemSelected != this)return;
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		button_state = OVER_DOWN;
		input_target->GUI_Input(this, MOUSE_LEFT_BUTTON_REPEAT);
	}

	//Mouse Right Button ------------------------
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		input_target->GUI_Input(this, MOUSE_RIGHT_BUTTON);
	}
}


//Functionality ===========================================
FIX_BUTTON_STATE UI_Fixed_Button::Change_State(FIX_BUTTON_STATE new_button_state)
{
	FIX_BUTTON_STATE last = button_state;
	button_state = new_button_state;
	return last;
}

FIX_BUTTON_STATE UI_Fixed_Button::Get_State() const
{
	return button_state;
}

void UI_Fixed_Button::SetTexUP(SDL_Rect img_rect, iPoint pivot, int text_id)
{
	tex_up = UI_Image({ 0,0,0,0 }, pivot, img_rect, text_id);
}

void UI_Fixed_Button::SetTexDOWN(SDL_Rect img_rect, iPoint pivot, int text_id)
{
	tex_down = UI_Image({ 0,0,0,0 }, pivot, img_rect, text_id);
}

void UI_Fixed_Button::SetTexOVERUP(SDL_Rect img_rect, iPoint pivot, int text_id)
{
	tex_over = UI_Image({ 0,0,0,0 }, pivot, img_rect, text_id);
}

void UI_Fixed_Button::SetTexOVERDOWN(SDL_Rect img_rect, iPoint pivot, int text_id)
{
	tex_over_down = UI_Image({ 0,0,0,0 }, pivot, img_rect, text_id);
}

void UI_Fixed_Button::SetTexFromXML(const pugi::xml_node & conf, int texture_id)
{
	int x = 0;
	pugi::xml_node img = conf.first_child();

	while (img != NULL)
	{
		std::string name = img.attribute("n").as_string("");
		SDL_Rect rect = { img.attribute("x").as_int(0),img.attribute("y").as_int(0),img.attribute("w").as_int(0),img.attribute("h").as_int(0) };

		iPoint pivot;
		float pivx = img.attribute("pX").as_float(0.0f)*rect.w;
		pivx = (pivx > (floor(pivx) + 0.5f)) ? ceil(pivx) : floor(pivx);
		pivot.x = (int)pivx;
		float pivy = img.attribute("pY").as_float(0.0f)*rect.h;
		pivy = (pivy > (floor(pivy) + 0.5f)) ? ceil(pivy) : floor(pivy);
		pivot.y = (int)pivy;

		if (strcmp("up", name.c_str())==0) this->SetTexUP(rect, pivot, texture_id);
		if (strcmp("down", name.c_str())==0) this->SetTexDOWN(rect, pivot, texture_id);
		if (strcmp("hover-down", name.c_str())==0) this->SetTexOVERDOWN(rect, pivot, texture_id);
		if (strcmp("hover-up", name.c_str())==0) this->SetTexOVERUP(rect, pivot, texture_id);

		box.h = rect.h;
		box.w = rect.w;

		img = img.next_sibling();
	
	}

	
}
