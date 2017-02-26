#include "UI_Popup_Menu.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Gui.h"

//Constructors ===============================================
UI_Popup_menu::UI_Popup_menu(const SDL_Rect & box, const UI_Image & tex_menu, const UI_Image & tex_selection): UI_Element(box, POPUP_MENU), tex_menu(tex_menu), tex_selection(tex_selection){}

UI_Popup_menu::UI_Popup_menu(const UI_Popup_menu * copy): UI_Element(copy->box, copy->GetUItype()), tex_menu(copy->tex_menu), tex_selection(copy->tex_selection){}

UI_Popup_menu::UI_Popup_menu() : UI_Element({ 0,0,0,0 }, POPUP_MENU), tex_selection(UI_Image({ 0,0,0,0 }, { 0,0 }, { 0, 0, 0, 0 }, -1)), tex_menu(UI_Image({ 0,0,0,0 }, {0,0}, { 0, 0, 0, 0 }, -1)) {}

//Destructor =================================================
UI_Popup_menu::~UI_Popup_menu()
{

}

//Game Loop ==================================================
void UI_Popup_menu::Draw(bool debug) const
{
	//This Draw
	if (debug)
	{
		App->render->DrawQuad({ box.x - App->render->camera.x, box.y - App->render->camera.y, box.w, box.h }, 150, 250, 0);
		App->render->DrawQuad({ tex_selection.GetBox()->x - App->render->camera.x, tex_selection.GetBox()->y - App->render->camera.y, box.w, box.h }, 250, 250, 0);
	}
	
	App->render->DrawQuad({ box.x - App->render->camera.x, box.y - App->render->camera.y, box.w, box.h }, 150, 150, 150);
	App->render->DrawQuad({ tex_selection.GetBox()->x - App->render->camera.x, tex_selection.GetBox()->y - App->render->camera.y, 150, 30}, 250, 250, 0);
	//tex_menu.DrawAt(box.x, box.y);
	//tex_selection.DrawAt(tex_selection.GetBox()->x, tex_selection.GetBox()->y);

	
	//Childs Draw
	DrawChilds(debug);

}

void UI_Popup_menu::HandleInput()
{
	//Mouse Left Button -------------------------
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (this->MouseIsIn() && App->gui->upper_element == this->layer)
		{
			SetOptionSelection();
			input_target->GUI_Input(this, MOUSE_LEFT_BUTTON_DOWN);
		}
		else if (App->gui->ItemSelected == this)
		{
			App->gui->ItemSelected = nullptr;
			return;
		}
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		if (this->MouseIsIn())
		{
			App->gui->ItemSelected = nullptr;
			this->Desactivate();
		}
	}

	//Enter -------------------------------------
	if (App->gui->ItemSelected != this)return;
	else if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		input_target->GUI_Input(this, ENTER);
		this->Desactivate();
	}
	//Arrows ------------------------------------
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
	{
		SetOptionSelectionArrows(false);
		input_target->GUI_Input(this, UP_ARROW);
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
	{
		SetOptionSelectionArrows(true);
		input_target->GUI_Input(this, DOWN_ARROW);
	}

}

void UI_Popup_menu::AddOption(UI_String * new_item)
{
	if (new_item == nullptr) return;
	
	//Add new item to items list
	Items.push_back(new_item);

	//Recalculate items position
	UpdateContentPosition(new_item);

	//Set item layer
	new_item->SetLayer(layer + 1);

	//Set first item as Selected item
	if (option_selected == nullptr) SetOptionSelection(new_item);
}

void UI_Popup_menu::UpdateContentPosition(UI_String * new_item)
{
	//Calculate cells again
	ResizeCells();
	

	text_size = new_item->GetPixelHeightLenght();
	//new_item->AdjustBox();

	//position y of list items
	int count = 0;

	for (std::list<UI_String*>::const_iterator item = Items.begin(); item != Items.end(); item++)
	{
		item._Ptr->_Myval->SetBoxPosition(item._Ptr->_Myval->GetBox()->x, box.y + cell_height*count + cell_height / 2 - text_size / 2);
		count++;
	}
	//new_item x pos
	new_item->SetBoxPosition(box.x + (box.w - new_item->GetPixelLenght(new_item->GetLenght())) / 2, new_item->GetBox()->y);
}

void UI_Popup_menu::ResizeCells()
{
	cell_height = box.h / Items.size();

	int count = 0;
	for (std::list<iPoint>::const_iterator item = Items_location.begin(); item != Items_location.end(); item++)
	{
		item._Ptr->_Myval.x = box.x;
		item._Ptr->_Myval.y = box.y + count*cell_height;
		count++;
	}
	Items_location.push_back(iPoint(box.x, box.y + count*cell_height));

}

void UI_Popup_menu::SetOptionSelection()
{
	//check option selected and move selection img
	int x_pos, ypos;
	App->input->GetMousePosition(x_pos, ypos);

	std::list<iPoint>::const_iterator position = Items_location.begin();
	for (std::list<UI_String*>::const_iterator item = Items.begin(); item != Items.end(); item++)
	{
		if (ypos >= position._Ptr->_Myval.y && ypos < position._Ptr->_Myval.y + cell_height)
		{
			tex_selection.SetBoxPosition(box.x, item._Ptr->_Myval->GetBox()->y + text_size / 2 - cell_height/ 2);
			option_selected = item._Ptr->_Myval;
			return;
		}
		position++;
	}

}

void UI_Popup_menu::SetOptionSelection(UI_String * new_item)
{
	//select new_item as first option
	if (new_item == nullptr)return;
	option_selected = new_item;
	tex_selection.SetBoxPosition(box.x, option_selected->GetBox()->y +text_size / 2 - cell_height/2);
}

void UI_Popup_menu::SetOptionSelectionArrows(bool go_down)
{
	std::list<UI_String*>::iterator a = std::find(Items.begin(), Items.end(), option_selected);

	if (go_down == true && a._Ptr->_Next->_Myval != NULL)	option_selected = a._Ptr->_Next->_Myval;
	if (go_down == false && a != Items.begin())	option_selected = a._Ptr->_Prev->_Myval;
	
	tex_selection.SetBoxPosition(box.x, option_selected->GetBox()->y + text_size / 2 - cell_height / 2);
	
}

char * UI_Popup_menu::GetOptionSelected() const
{
	return option_selected->GetString();
}

void UI_Popup_menu::SetTexSelection(SDL_Rect img_rect, int text_id)
{
	tex_selection = UI_Image({ 0,0,0,0 }, {0,0}, img_rect, text_id);
}

void UI_Popup_menu::SetTexMenu(SDL_Rect img_rect, int text_id)
{
	tex_menu = UI_Image({ 0,0,0,0 }, {0,0}, img_rect, text_id);
}
