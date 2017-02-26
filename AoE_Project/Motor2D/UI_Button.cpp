#include "UI_Button.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Gui.h"

//Constructors ============================================
UI_Button::UI_Button(const SDL_Rect & box, const UI_Image& tex_on, const UI_Image& tex_off, const UI_Image& tex_over) :UI_Element(box, BUTTON), tex_on(tex_on), tex_off(tex_off), tex_over(tex_over), button_state(OFF) {}

UI_Button::UI_Button(const UI_Button* copy) : UI_Element(copy->box, copy->ui_type), tex_on(copy->tex_on), tex_off(copy->tex_off), tex_over(copy->tex_over), button_state(OFF) {}

UI_Button::UI_Button() : UI_Element({ 0,0,0,0 }, BUTTON), button_state(OFF) {}

//Destructor ==============================================
UI_Button::~UI_Button()
{

}


//Game Loop ===============================================
void UI_Button::Draw(bool debug) const
{

	//This Draw
	int x = this->box.x;
	int y = this->box.y;

	if(debug)App->render->DrawQuad({ x - App->render->camera.x, y - App->render->camera.y, box.w, box.h }, 150, 250, 0);

	//This Draw
	switch (button_state) {

		case BUTTON_STATE::ON:		tex_on.DrawAt(x,y);			break;
		case BUTTON_STATE::OFF:		tex_off.DrawAt(x, y);		break;
		case BUTTON_STATE::OVER:	tex_over.DrawAt(x, y);		break;

	}

	//Childs Draw
	DrawChilds(debug);
}

void UI_Button::HandleInput()
{
	//Mouse In/Out ------------------------------
	if (this->MouseIsIn() && App->gui->upper_element == this->layer)
	{
		button_state = OVER;
		input_target->GUI_Input(this, MOUSE_IN);
	}
	else
	{
		button_state = OFF;
		input_target->GUI_Input(this, MOUSE_OUT);
	}

	//Mouse Left Button -------------------------
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (this->MouseIsIn() && App->gui->upper_element == this->layer)
		{
			App->gui->ItemSelected = this;
			button_state = ON;
			input_target->GUI_Input(this, MOUSE_LEFT_BUTTON_DOWN);
		}
		else if (App->gui->ItemSelected == this)
		{
			App->gui->ItemSelected = nullptr;
			return;
		}
	}
	if (App->gui->ItemSelected != this)return;
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		button_state = ON;
		input_target->GUI_Input(this, MOUSE_LEFT_BUTTON_REPEAT);
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		button_state = OVER;
		input_target->GUI_Input(this, MOUSE_LEFT_BUTTON_UP);
	}

	//Mouse Right Button ------------------------
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		input_target->GUI_Input(this, MOUSE_RIGHT_BUTTON);
	}

	//Arrows ------------------------------------
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		input_target->GUI_Input(this, LEFT_ARROW);
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		input_target->GUI_Input(this, RIGHT_ARROW);
	}
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
	{
		input_target->GUI_Input(this, UP_ARROW);
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
	{
		input_target->GUI_Input(this, DOWN_ARROW);
	}

	//Backspace/Delete --------------------------
	else if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		input_target->GUI_Input(this, BACKSPACE);
	}
	else if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		input_target->GUI_Input(this, SUPR);
	}
}


//Functionality ===========================================
BUTTON_STATE UI_Button::Change_State(BUTTON_STATE new_button_state)
{
	BUTTON_STATE last = button_state;
	button_state = new_button_state;
	return last;
}

BUTTON_STATE UI_Button::Get_State() const
{
	return button_state;
}

void UI_Button::SetTexON(SDL_Rect img_rect, int text_id)
{
	tex_on = UI_Image({0,0,0,0}, { 0,0 }, img_rect, text_id);
}

void UI_Button::SetTexOFF(SDL_Rect img_rect, int text_id)
{
	tex_off = UI_Image({ 0,0,0,0 }, { 0,0 }, img_rect, text_id);
}

void UI_Button::SetTexOVER(SDL_Rect img_rect, int text_id)
{
	tex_over = UI_Image({ 0,0,0,0 }, { 0,0 }, img_rect, text_id);
}
