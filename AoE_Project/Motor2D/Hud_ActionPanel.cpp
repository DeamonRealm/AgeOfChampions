#include "Hud_ActionPanel.h"

#include "j1App.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1EntitiesManager.h"
#include "j1Render.h"
#include "j1SoundManager.h"

#include "BaseEntities.h"

#include "j1ActionManager.h"
#include "Actions_Building.h"
#include "Actions_Unit.h"

#include "Hud_GamePanel.h"

#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Fixed_Button.h"

// BASE ACTION ELEMENTS --------------------------------------------------------------------------------------
Action_Panel_Elements::Action_Panel_Elements() 
{
	
};

Action_Panel_Elements::~Action_Panel_Elements()
{
	panel_icons.clear();
};

void Action_Panel_Elements::AddIcon(SDL_Rect icon_rect, uint position)
{
	if (position < MAX_PANEL_CELLS)	panel_icons[position] = icon_rect;
}

void Action_Panel_Elements::ChangePanelIcons(std::vector<UI_Image*> & actual_panel) const
{
	for (uint i = 0; i < MAX_PANEL_CELLS; i++)
	{
		actual_panel[i]->ChangeTextureRect(panel_icons[i]);
	}
}

Entity * Action_Panel_Elements::GetActualEntity()
{
	return entitis_panel;
}

void Action_Panel_Elements::ChangePlayerGamePanel(Game_Panel * game_panel)
{
	player_game_panel_resources = game_panel;
}

TownCenterPanel::TownCenterPanel() : Action_Panel_Elements()
{
	panel_icons.reserve(MAX_PANEL_CELLS);
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		panel_icons.push_back({ 0,0,1,1 });
	}
	panel_icons[0] = { 576,585,36,36 };
	panel_icons[1] = { 288,585,36,36 };
};

// TOWNCENTER ------------------------------------------------------------------------------------------------------------
bool TownCenterPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 0: {
		if(player_game_panel_resources->UseResource(0,50,0,0,1)) entitis_panel->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)entitis_panel, VILLAGER, ALLY));
		}
		break;
	case 1: {
		if (player_game_panel_resources->UseResource(0, 0, 100, 0, 1)) {
			entitis_panel->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)entitis_panel, WARRIOR_CHMP, ALLY));
		}
		}
		break;
	default:
		break;
	}
	return true;
}

// Barrack Panel ---------------------------------------------------------------------------------------------------------

BarrackPanel::BarrackPanel() : Action_Panel_Elements()
{
	panel_icons.reserve(MAX_PANEL_CELLS);
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		panel_icons.push_back({ 0,0,1,1 });	
	}
	panel_icons[0] = { 504,585,36,36 };
}

bool BarrackPanel::ActivateCell(int i)
{
	switch (i)
	{
	case 0:	{
		if (player_game_panel_resources->UseResource(0, 25, 45, 0, 1)) entitis_panel->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)entitis_panel, MILITIA, ALLY));
		}
		break;
	default:
		break;
	}
	return false;
}


// UNIT PANEL -------------------------------------------------------------------------------------------------------

UnitPanel::UnitPanel() : Action_Panel_Elements() 
{
	panel_icons.reserve(MAX_PANEL_CELLS);
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		panel_icons.push_back({ 0,0,1,1 });
	}
	panel_icons[3] = { 0,76,36,36 };
};

bool UnitPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 3: {
		entitis_panel->SetLife(0);
		((Unit*)entitis_panel)->AddPriorizedAction(App->action_manager->DieAction((Unit*)entitis_panel));
		entitis_panel = nullptr;
	}
		break;
	default:
		break;
	}
	return true;
}

// VILLAGER PANEL ---------------------------------------------------------------------------------------------------

VillagerPanel::VillagerPanel() : Action_Panel_Elements()
{
	panel_icons.reserve(MAX_PANEL_CELLS);
	v_normal_panel.reserve(MAX_PANEL_CELLS);
	v_militari_panel.reserve(MAX_PANEL_CELLS);
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		panel_icons.push_back({ 0,0,1,1 });
		v_normal_panel.push_back({ 0,0,1,1 });
		v_militari_panel.push_back({ 0,0,1,1 });
	}
	panel_icons[0] = { 76,153,36,36 };
	panel_icons[1] = { 593,36,36,36 };
	panel_icons[2] = { 405,36,36,36 };
	panel_icons[3] = { 0,76,36,36 };

	v_normal_panel[13] = {332,36,36,36};
	v_normal_panel[14] = { 477,36,36,36 };
	v_militari_panel[0] = { 132,0,36,36 };
	v_militari_panel[13] = { 332,36,36,36 };
	v_militari_panel[14] = { 477,36,36,36 };
}

bool VillagerPanel::ActivateCell(int i)
{
	switch (i)
	{
	case 0:{
		if (villagerisbuilding == VP_NOT_BUILDING)
		{
			villagerisbuilding = VP_NORMAL;
			return false;
		}
		else if (villagerisbuilding == VP_NORMAL)
		{

		}
		else if (villagerisbuilding == VP_MILITARY && player_game_panel_resources->UseResource(100,100,0,0,0))
		{
			//incress population left
			buildingthis = App->entities_manager->GenerateBuilding(BARRACK,ALLY,true);
			int x = 0, y = 0;
			App->input->GetMousePosition(x, y);
			buildingthis->SetPosition(x - App->render->camera.x, y - App->render->camera.y, false);

			isbuilding = true;
		}
		}
		break;
	case 1: {
		if (villagerisbuilding == VP_NOT_BUILDING){
			villagerisbuilding = VP_MILITARY;
			return false;
		}
		}
		break;
	case 2:
		break;
	case 3: {
		entitis_panel->SetLife(0);
		((Unit*)entitis_panel)->AddPriorizedAction(App->action_manager->DieAction((Unit*)entitis_panel));
		entitis_panel = nullptr;
		}
		break;
	case 4:
		break;
	case 13: {
		if (villagerisbuilding == VP_NORMAL) {
			villagerisbuilding = VP_MILITARY;
			return false;
		}
		if (villagerisbuilding == VP_MILITARY)
		{
			villagerisbuilding = VP_NORMAL;
			return false;
		}
		}
		break;
	case 14: if (villagerisbuilding != VP_NOT_BUILDING) {
		villagerisbuilding = VP_NOT_BUILDING;
		return false;
		}
		break;
	default:
		break;
	}
	return true;
}

void VillagerPanel::ChangePanelIcons(std::vector<UI_Image*>& actual_panel) const
{
	switch (villagerisbuilding)
	{
	case VP_NOT_BUILDING: 
		for (uint i = 0; i < MAX_PANEL_CELLS; i++)
		{
			actual_panel[i]->ChangeTextureRect(panel_icons[i]);
		}
		
		break;
	case VP_NORMAL:
		for (uint i = 0; i < MAX_PANEL_CELLS; i++)
		{
			actual_panel[i]->ChangeTextureRect(v_normal_panel[i]);
		}
		break;
	case VP_MILITARY:
		for (uint i = 0; i < MAX_PANEL_CELLS; i++)
		{
			actual_panel[i]->ChangeTextureRect(v_militari_panel[i]);
		}
		break;
	default:
		break;
	}
}

bool VillagerPanel::Villager_Handle_input(GUI_INPUT input)
{
	
	switch (input)
	{
	case MOUSE_LEFT_BUTTON_DOWN:
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		if (isbuilding)
		{
			int x = 0, y = 0;
			App->input->GetMousePosition(x, y);
			buildingthis->SetPosition(x - App->render->camera.x, y - App->render->camera.y, false);
		}
		break;
	case MOUSE_LEFT_BUTTON_UP:
		if (isbuilding)
		{
			int x = 0, y = 0;
			App->input->GetMousePosition(x, y);
			buildingthis->SetPosition(x - App->render->camera.x, y - App->render->camera.y, true);
			isbuilding = false;
			buildingthis = nullptr;
		}
		break;

	default:
		break;
	}
	return false;
}

bool VillagerPanel::GetIsBuilding() const
{
	return isbuilding;
}


// HERO PANEL -------------------------------------------------------------------------------------------------------

HeroPanel::HeroPanel() : Action_Panel_Elements()
{
	App->gui->SetDefaultInputTarget((j1Module*)App->player);

	panel_icons.reserve(MAX_PANEL_CELLS);
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		panel_icons.push_back({ 0,0,1,1 });
	}
	panel_icons[3] = { 504,441,36,36 };
	panel_icons[4] = { 0,76,36,36 };


	skill_tree = (UI_Image*)App->gui->GenerateUI_Element(IMG);
	skill_tree->SetBox({ 30, 100, 243, 345 });
	skill_tree->ChangeTextureId(CHAMPION_SKILL);
	skill_tree->ChangeTextureRect({ 0,0, 243, 345 });
	skill_tree->SetLayer(15);
	skill_tree->AdjustBox();
	skill_tree->Desactivate();

	mele_champion.reserve(MAX_SKILLS_LEARNED);
/*	mele_champion.push_back({ 243,80,36,36 });
	mele_champion.push_back({ 243,118,36,36 });
	mele_champion.push_back({ 243,156,36,36 });
	mele_champion.push_back({ 243,194,36,36 });
	mele_champion.push_back({ 243,232,36,36 });
	mele_champion.push_back({ 243,270,36,36 });
	*/
	mele_champion.push_back({ 540,441,36,36 });
	mele_champion.push_back({ 576,441, 36, 36 });
	mele_champion.push_back({ 612,441,36,36 });
	mele_champion.push_back({ 540,477,36,36 });
	mele_champion.push_back({ 576,477, 36, 36 });
	mele_champion.push_back({ 612,477,36,36 });

	skills.reserve(MAX_SKILLS_LEARNED);
	skills_buttons.reserve(MAX_SKILLS_LEARNED);
	for (int i = 0; i < MAX_SKILLS_LEARNED; i++)
	{
		skills.push_back((UI_Image*)App->gui->GenerateUI_Element(IMG));
		skills[i]->SetBox({ 0, 0, 67, 61 });
		skills[i]->ChangeTextureId(CHAMPION_SKILL);
		skills[i]->ChangeTextureRect(mele_champion[i]);
		skills[i]->SetBoxPosition(97 + 67 * (i % 2), 177 + 55 * (i / 2));
		skills[i]->Activate();

		skills_buttons.push_back((UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON));
		skills_buttons[i]->SetBox({ 77 + 67 * (i % 2), 188 + 55 * (i / 2),16,15 });
		skills_buttons[i]->SetTexDOWN({ 265,62,16,15 }, { 0,0 }, CHAMPION_SKILL);
		skills_buttons[i]->SetTexUP({ 244,62,16,15 }, { 0,0 }, CHAMPION_SKILL);
		skills_buttons[i]->SetTexOVERUP({ 244,62,16,15 }, { 0,0 }, CHAMPION_SKILL);
		skills_buttons[i]->SetTexOVERDOWN({ 266,62,16,15 }, { 0,0 }, CHAMPION_SKILL);
		skills_buttons[i]->Activate();

		skill_tree->AddChild(skills_buttons[i]);
		skill_tree->AddChild(skills[i]);
		skills_buttons[i]->SetLayer(8);
		skills[i]->SetLayer(8);
	}

	//champion
	mele_learned[0] = (-1);
	mele_learned[1] = (-1);
	mele_learned[2] = (-1);
	
}

HeroPanel::~HeroPanel()
{
	skills.clear();
	skills_buttons.clear();
	
	mele_champion.clear();
}

bool HeroPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 0: {
		if (champion_selected == WARRIOR_CHMP && mele_learned[0] != -1)((Warrior*)entitis_panel)->Hability_A();
		}
		break;
	case 1: 
		{
		if (champion_selected == WARRIOR_CHMP && mele_learned[1] != -1)((Warrior*)entitis_panel)->Hability_B();
		}
		break;
	case 2:
		break;
	case 3: {
		if (skill_tree->GetActiveState() == true) skill_tree->Desactivate();
		else skill_tree->Activate();
		}
		break;
	case 4:
		{
		entitis_panel->SetLife(0);
		((Unit*)entitis_panel)->AddPriorizedAction(App->action_manager->DieAction((Unit*)entitis_panel));
		entitis_panel = nullptr;
		}
	default:	break;
	}
	return true;
}

bool HeroPanel::Hero_Handle_input(UI_Element * ui_element, GUI_INPUT ui_input)
{
	switch (ui_input)
	{
	case UP_ARROW:						break;
	case DOWN_ARROW:					break;
	case LEFT_ARROW:					break;
	case RIGHT_ARROW:					break;
	case MOUSE_LEFT_BUTTON_DOWN:
		{
			if (ui_element->GetUItype() == FIXED_BUTTON) {
				for (int i = 0; i < MAX_SKILLS_LEARNED; i++)
				{
					if (ui_element == skills_buttons[i])
					{
						LearnSkill(i);
						if (i / 2 == 0)((Champion*)entitis_panel)->SetAbility_A((i % 2));
						if (i / 2 == 1)((Champion*)entitis_panel)->SetAbility_B((i % 2));
						return true;
					}
				}
			}
		}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:		break;
	case MOUSE_LEFT_BUTTON_UP:			break;
	case MOUSE_RIGHT_BUTTON:			break;
	case MOUSE_IN:						break;
	case MOUSE_OUT:						break;
	case SUPR:							break;
	case BACKSPACE:						break;
	case ENTER:							break;
	case TAB:							break;
	default:break;
	}
	return false;
}

void HeroPanel::LearnSkill(int i)
{
	if (champion_selected == WARRIOR_CHMP && mele_learned[i / 2] == -1)
	{
		mele_learned[i / 2] = i; 
	}
}

void HeroPanel::ChangePanelIcons(std::vector<UI_Image*>& actual_panel) const
{
	for (uint i = 0; i < MAX_PANEL_CELLS; i++)
	{
		if(champion_selected == WARRIOR_CHMP)	actual_panel[i]->ChangeTextureRect(panel_icons[i]);
	}

	if (champion_selected == WARRIOR_CHMP) {
		for (int i = 0; i < 3; i++)
		{
			if (mele_learned[i] != -1) actual_panel[i]->ChangeTextureRect(mele_champion[mele_learned[i]]);
		}
	}
}

void HeroPanel::ChangePanelTarget(Entity * new_target)
{
	entitis_panel = new_target;
	champion_selected = ((Unit*)entitis_panel)->GetUnitType();
	
}


// ACTION PANEL  ================================================================================================

Action_Panel::Action_Panel() : action_rect({37, 624, 200, 123}), isin(false)
{
	panel_pos.x = 37;
	panel_pos.y = 624;

	panel_cells.reserve(MAX_PANEL_CELLS);

	towncenterpanel = new TownCenterPanel();
	unitpanel = new UnitPanel();
	heropanel = new HeroPanel();
	villagerpanel = new VillagerPanel();
	barrackpanel = new BarrackPanel();

	UI_Image* cell = nullptr;
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		cell = (UI_Image*)App->gui->GenerateUI_Element(IMG);
		cell->ChangeTextureRect({ 0, 0, 1, 1 });
		cell->SetBoxPosition( panel_pos.x + (i% PANEL_COLUMNS)*CELL_WIDTH, panel_pos.y + ((int)i / PANEL_COLUMNS)*CELL_HEIGHT);
		cell->ChangeTextureId(ICONS);
		panel_cells.push_back(cell);
	}
}

Action_Panel::~Action_Panel()
{
	CleanUp();
}

bool Action_Panel::CleanUp()
{


	panel_cells.clear();

	delete towncenterpanel;
	delete barrackpanel;
	delete villagerpanel;
	delete unitpanel;
	delete heropanel;

	return false;
}

bool Action_Panel::PreUpdate()
{
	App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
	SDL_Point p = { mouse_pos.x,mouse_pos.y };
	if(SDL_PointInRect(&p, &action_rect)) isin = true;
	else isin = false;

	return true;
}

bool Action_Panel::PostUpdate()
{
	return false;
}

void Action_Panel::Handle_Input(GUI_INPUT newevent)
{
	if (isin == false && villagerpanel->GetIsBuilding() == false) return;

	switch (newevent)
	{
	case MOUSE_LEFT_BUTTON_DOWN:
		{
		if (actualpanel != nullptr) {
			cell = GetCell();
			if (actualpanel->ActivateCell(cell) == false)
			{
				actualpanel->ChangePanelIcons(panel_cells);
			}
		}
		}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT: {
			if (actualpanel == villagerpanel && villagerpanel->GetIsBuilding())
			{
				villagerpanel->Villager_Handle_input(MOUSE_LEFT_BUTTON_REPEAT);
			}
		}
		break;
	case MOUSE_LEFT_BUTTON_UP: {
			if (actualpanel == villagerpanel && villagerpanel->GetIsBuilding())
			{
				villagerpanel->Villager_Handle_input(MOUSE_LEFT_BUTTON_UP);
			}
		}
		break;
	case MOUSE_IN:
		break;
	case MOUSE_OUT:
		break;
	default:
		break;
	}
}

bool Action_Panel::Draw()
{
	if (actualpanel == nullptr) return false;
	for (int count = 0; count < MAX_PANEL_CELLS; count++)
	{
		panel_cells[count]->Draw(false);
	}
	return true;
}

int Action_Panel::GetCell() const
{
	SDL_Rect cell_clicked = {0,0,0,0};
	int ret = -1;

	for (int count = 0; count < MAX_PANEL_CELLS; count++)
	{
		cell_clicked =  { panel_pos.x + (count% PANEL_COLUMNS)*CELL_WIDTH, panel_pos.y + ((int)count / PANEL_COLUMNS)*CELL_HEIGHT,CELL_WIDTH, CELL_HEIGHT } ;
		if (SDL_PointInRect(&mouse_pos, &cell_clicked))
		{
			return count;
		}
	}

	return ret;
}

void Action_Panel::ActivateCell(int i)
{
	if(actualpanel != nullptr) actualpanel->ActivateCell(i);
}

bool Action_Panel::GetIsIn() const
{
	return isin;
}

void Action_Panel::Handle_Input(UI_Element * ui_element, GUI_INPUT ui_input)
{
	if (actualpanel == heropanel)
	{
		if(heropanel->Hero_Handle_input(ui_element, ui_input)) heropanel->ChangePanelIcons(panel_cells);
	}
}

UI_Element * Action_Panel::GetHeroSkillTree() const
{
	return heropanel->skill_tree;
}

void Action_Panel::SetSelectionPanelPointer(Selection_Panel * selection_panel)
{
	player_selection_panel = selection_panel;
}

void Action_Panel::GetEntitySelected()
{
	actual_entity = player_selection_panel->GetSelected();
	if (actual_entity == nullptr)
	{
		actualpanel = nullptr;
		return;
	}
}

void Action_Panel::SetGamePanelPointer(Game_Panel * game_panel)
{
	player_game_panel = game_panel;
}

void Action_Panel::SetPanelType()
{
	GetEntitySelected();

	if (actual_entity == nullptr) return;

	DIPLOMACY			d_type = NEUTRAL;
	ENTITY_TYPE			e_type = NO_ENTITY; 
	UNIT_TYPE			u_type = NO_UNIT; 
	BUILDING_TYPE		b_type = NO_BUILDING;

	player_selection_panel->GetSelectedType(d_type, e_type, u_type, b_type);

	if (d_type != ALLY)
	{
		actualpanel = nullptr;
		actual_entity = nullptr;
		return;
	}

	switch (e_type)
	{
	case NO_ENTITY: {
		actualpanel = nullptr;
		}
		break;
	case UNIT:
		{
			if (u_type == VILLAGER) {	
				villagerpanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = villagerpanel;
			}
			else if (u_type == WARRIOR_CHMP)
			{
				actualpanel = heropanel;
			}
			else
			{
				actualpanel = unitpanel;
			}
		}
		break;
	case RESOURCE:
		actualpanel = nullptr;
		break;
	case BUILDING:
		{
			if (b_type == TOWN_CENTER)
			{
				App->sound->PlayFXAudio(TOWN_CENTER_SELECTED_SOUND);
				towncenterpanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = towncenterpanel;
			}
			else if (b_type == BARRACK)
			{
				App->sound->PlayFXAudio(BARRACK_SOUND);
				barrackpanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = barrackpanel;
			}
			else actualpanel = nullptr;		
		}
		break;
	default:	actualpanel = nullptr;
		break;
	}

	if (actualpanel != nullptr)
	{
		actualpanel->ChangePanelTarget(actual_entity);
		actualpanel->ChangePanelIcons(panel_cells);
	}
}

void Action_Panel::CheckSelected(int selected)
{	
	if (selected == 0)
	{
		actual_entity = nullptr;
		actualpanel = nullptr;
		heropanel->skill_tree->Desactivate();
		return;
	}
	if (actualpanel != nullptr)
	{
		if (actualpanel->GetActualEntity() == nullptr)
		{
			actualpanel->ChangePanelTarget(player_selection_panel->GetSelected());
		}
	}
}
