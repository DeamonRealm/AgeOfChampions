#include "UI_Element.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "SDL/include/SDL.h"

//Constructors
UI_Element::UI_Element(const SDL_Rect& box, UI_TYPE ui_type, bool IsActive) :box(box), ui_type(ui_type), IsActive(IsActive), input_target(App->gui->GetDefaultInputTarget()) {}

UI_Element::UI_Element(const UI_Element* copy) : box(copy->box), ui_type(copy->ui_type), IsActive(copy->IsActive), tab_num(copy->tab_num), input_target(copy->input_target) {}

UI_Element::UI_Element() : box({0,0,0,0}), ui_type(UNDEFINED), IsActive(false), input_target(nullptr) {}

//Destructor
UI_Element::~UI_Element()
{

}


// Game Loop ==============================================
bool UI_Element::Update()
{
	HandleInput();
	/*
			This Update
	*/

	//Childs Update
	UpdateChilds();

	return true;
}

bool UI_Element::UpdateChilds()
{

	bool ret = true;

	for (std::list<UI_Element*>::iterator item = childs.begin(); item != childs.end(); item++)
	{
		if (item._Ptr->_Myval->IsActive)
			ret = item._Ptr->_Myval->Update();
	}

	return ret;
}

bool UI_Element::CleanUp()
{
	/*
			This CleanUp
	*/
	input_target = nullptr;
	//Childs CleanUp
	CleanUpChilds();
	return true;

}

bool UI_Element::CleanUpChilds()
{
	bool ret = true;

	while (childs.size() > 0) {

		ret = childs.front()->CleanUp();
		childs.pop_front();

	}

	return ret;
}

void UI_Element::Draw(bool debug) const
{
	/*
			This Draw
	*/

	//Childs Draw
	DrawChilds(debug);
}

void UI_Element::DrawAt(int x, int y) const
{
	/*
			This Draw
	*/
}

void UI_Element::DrawChilds(bool debug) const 
{
	for (std::list<UI_Element*>::const_iterator item = childs.begin(); item != childs.end(); item++) 
	{
		if (item._Ptr->_Myval->IsActive)
			item._Ptr->_Myval->Draw(debug);
	}
}

void UI_Element::HandleInput()
{
	//Mouse In/Out ------------------------------
	if (this->MouseIsIn())
	{
		input_target->GUI_Input(this, MOUSE_IN);
	}
	else
	{
		input_target->GUI_Input(this, MOUSE_OUT);
	}

	//Mouse Left Button -------------------------
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (this->MouseIsIn() && App->gui->upper_element == this->layer)
		{
			App->gui->ItemSelected = this;
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
		input_target->GUI_Input(this, MOUSE_LEFT_BUTTON_REPEAT);
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
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

// Functionality ==========================================
void UI_Element::SetBoxPosition(int new_pos_x, int new_pos_y)
{
	box.x = new_pos_x;
	box.y = new_pos_y;


	if (childs.size() == 0)return;

	for (std::list<UI_Element*>::iterator item = childs.begin(); item != childs.end(); item++)
	{
		item._Ptr->_Myval->SetBoxPosition(box.x + item._Ptr->_Myval->box.x, box.y + item._Ptr->_Myval->box.y);
	}
}

void UI_Element::MoveBox(int x_vel, int y_vel)
{
	box.x += x_vel;
	box.y += y_vel;

	for (std::list<UI_Element*>::iterator item = childs.begin(); item != childs.end(); item++)
	{
		item._Ptr->_Myval->MoveBox(x_vel, y_vel);
	}
}

bool UI_Element::MouseIsIn(int x, int y) const
{
	int x_pos, ypos;

	App->input->GetMousePosition(x_pos, ypos);

	return ((box.x + x < x_pos && (box.x + x + box.w) > x_pos) && (box.y + y < ypos && (box.y + y + box.h) > ypos));
}

void UI_Element::ResizeBox(const iPoint & new_size)
{
	box.w = new_size.x;
	box.h = new_size.y;
}

void UI_Element::SetBox(SDL_Rect new_box)
{
	box = new_box;
}

SDL_Rect UI_Element::AdjustBox()
{
	box = { 0,0,0,0 };
	return SDL_Rect(box);
}

SDL_Rect* UI_Element::GetBox() const
{
	return (SDL_Rect*)&box;
}

bool UI_Element::RectIsIn(const SDL_Rect* target, int x_vel, int y_vel, bool x_axis )const
{
	bool ret = false;
	
	if (x_axis == false)
	{

		ret = (target->y <= box.y + y_vel && (target->y + target->h) >= (box.y + box.h + y_vel));

	}
	else
	{
		ret = (target->x <= box.x + x_vel && (target->x + target->w) >= (box.x + box.w + x_vel));
	}
	return ret;
}

void UI_Element::SetTabable()
{
	tab_num = App->gui->GetTabNumber() + 1;
	App->gui->SetTabNumber(tab_num);
}

uint UI_Element::GetTabNum() const
{
	return tab_num;
}

void UI_Element::SetLayer(uint new_layer)
{
	layer = new_layer;
}

uint UI_Element::GetLayer() const
{
	return layer;
}

UI_TYPE UI_Element::GetUItype() const
{
	return ui_type;
}

void UI_Element::Activate()
{
	IsActive = true;
}

void UI_Element::Desactivate()
{
	IsActive = false;
}

bool UI_Element::GetActiveState() const
{
	return IsActive;
}



// ==========================
void UI_Element::AddChild(UI_Element* child, uint start_layer)
{
	//Set the new element parent
	child->SetParent(this);
	//Set child layer
	child->layer = this->layer + 1 + start_layer;
	//Add the new element to the list of this childs
	childs.push_back(child);
}

void UI_Element::Delete_Child(UI_Element* child)
{
	childs.remove(child);
}

void UI_Element::Delete_Child(uint index)
{
	std::list<UI_Element*>::iterator item = childs.begin();

	for (uint k = 0; k < index; k++)
			item++;

	childs.remove(item._Ptr->_Myval);
}

UI_Element * UI_Element::SetParent(const UI_Element* parent)
{
	this->box.x += parent->box.x;
	this->box.y += parent->box.y;
	return this->parent = ((UI_Element*)parent);
}

UI_Element * UI_Element::GetParent() const
{
	return parent;
}

void UI_Element::SetInputTarget(j1Module * target)
{
	this->input_target = target;
}

j1Module * UI_Element::GetInputTarget() const
{
	return this->input_target;
}
