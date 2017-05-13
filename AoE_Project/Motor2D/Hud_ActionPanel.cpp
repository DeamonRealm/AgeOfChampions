#include "Hud_ActionPanel.h"

#include "j1App.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1EntitiesManager.h"
#include "j1Render.h"
#include "j1SoundManager.h"
#include "j1FileSystem.h"

#include "BaseEntities.h"

#include "j1ActionManager.h"
#include "Actions_Building.h"
#include "Actions_Unit.h"

#include "Hud_GamePanel.h"

#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_String.h"
#include "UI_Button.h"
#include "UI_Fixed_Button.h"

// Cells Data Methods ------------------------------------------------------------------------------
void Cells_Data::SetInfo()
{
	if (wood_cost > 0 || food_cost > 0 || gold_cost > 0 || stone_cost > 0)
	{
		cell_info += " (";
		if (wood_cost > 0) cell_info = cell_info + " Wood: " + App->gui->SetStringFromInt(wood_cost);
		if (food_cost > 0) cell_info = cell_info + " Food: " + App->gui->SetStringFromInt(food_cost);
		if (gold_cost > 0) cell_info = cell_info + " Gold: " + App->gui->SetStringFromInt(gold_cost);
		if (stone_cost > 0) cell_info = cell_info + " Stone: " + App->gui->SetStringFromInt(stone_cost);
		cell_info += ")";
	}
}

const char * Cells_Data::GetInfo() const
{
	return cell_info.c_str();
}

// BASE ACTION ELEMENTS --------------------------------------------------------------------------------------
Action_Panel_Elements::Action_Panel_Elements() 
{
	Cells_Data celldata;
	panel_icons.reserve(MAX_PANEL_CELLS);
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		panel_icons.push_back(celldata);
	}
	entitis_panel = nullptr;
};

Action_Panel_Elements::~Action_Panel_Elements()
{
	panel_icons.clear();
	cell_data.clear();
}

bool Action_Panel_Elements::Load(pugi::xml_node & data)
{
	// Load cell levels
	if (data != NULL)
	{
		pugi::xml_node curr_cell = data.first_child();
		int i = 0;
		while (curr_cell != NULL)
		{
			cell_lvl[i] = curr_cell.attribute("level").as_int(0);
			curr_cell = curr_cell.next_sibling();
			i++;
		}
		UpdateCells();
	}
	return true;
}

bool Action_Panel_Elements::Save(pugi::xml_node & data) const
{
	// Save Panel Cell Levels
	pugi::xml_node curr_cell;
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		curr_cell = data.append_child("cell");
		curr_cell.append_attribute("level") = cell_lvl[i];
	}
	return true;
}

void Action_Panel_Elements::SetDataFromXML()
{
	current_age = 2;
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		cell_lvl[i] = 0;
	}
	UpdateCells();

	entitis_panel = nullptr;
}

void Action_Panel_Elements::UpdateCells()
{
	// Reset Cells
	Cells_Data celldata;
	panel_icons.clear();
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		panel_icons.push_back(celldata);
	}

	// Set Current Cells
	int size = cell_data.size();
	for (int i = 0; i < size; i++)
	{
		if (cell_data[i].cell_at_level == cell_lvl[cell_data[i].cell_position] && cell_data[i].cell_at_age <= current_age)
		{
			panel_icons[cell_data[i].cell_position] = cell_data[i];
		}
	}
}

void Action_Panel_Elements::ResetPanel()
{
	SetDataFromXML();
}

void Action_Panel_Elements::SetDefault()
{
	UpdateCells();
}

bool Action_Panel_Elements::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	
	if (panel_icons[i].u_type != NO_UNIT)
	{
		if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost,
			panel_icons[i].stone_cost, panel_icons[i].population_cost))
		{
			entitis_panel->AddAction(App->action_manager->SpawnAction(entitis_panel->GetWorker(), (ProductiveBuilding*)entitis_panel, panel_icons[i].u_type, ALLY));
			App->sound->PlayGUIAudio(CLICK_INGAME);
		}
		else App->sound->PlayGUIAudio(ERROR_SOUND);
	}
	else if (panel_icons[i].r_type != NO_TECH)
	{
		if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost,
			panel_icons[i].stone_cost, panel_icons[i].population_cost))
		{
			entitis_panel->AddAction(App->action_manager->ResearchAction(entitis_panel->GetWorker(), panel_icons[i].r_type, 3000, ALLY));
			App->sound->PlayGUIAudio(CLICK_INGAME);
			cell_lvl[i] -= RESEARCH_MARGIN;
			UpdateCells();
		}
		else App->sound->PlayGUIAudio(ERROR_SOUND);
	}
	return false;
}

bool Action_Panel_Elements::CancelAction()
{
	return false;
}

void Action_Panel_Elements::ChangePanelIcons(std::vector<UI_Image*> & actual_panel) 
{
	for (uint i = 0; i < MAX_PANEL_CELLS; i++)
	{
		actual_panel[i]->ChangeTextureRect(panel_icons[i].cell_icon);
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

void Action_Panel_Elements::LoadPanelFromXML(const pugi::xml_node & conf)
{
	Cells_Data data;
	pugi::xml_node item = conf.first_child();
	while (item != NULL)
	{
		data.cell_position = item.attribute("cp").as_int(0);
		data.cell_at_level = item.attribute("cl").as_int(0);
		data.cell_at_age = item.attribute("al").as_int(0);
		data.cell_icon.x = item.attribute("x").as_int(0);
		data.cell_icon.y = item.attribute("y").as_int(0);
		data.cell_icon.w = item.attribute("w").as_int(0);
		data.cell_icon.h = item.attribute("h").as_int(0);
		data.cell_info	 = item.attribute("ci").as_string("");
		data.wood_cost = item.attribute("wc").as_int(0);
		data.food_cost = item.attribute("fc").as_int(0);
		data.gold_cost = item.attribute("gc").as_int(0);
		data.stone_cost = item.attribute("sc").as_int(0);
		data.population_cost = item.attribute("pc").as_int(0);

		data.SetInfo();

		data.u_type = (UNIT_TYPE)item.attribute("ut").as_int(0);
		data.b_type = (BUILDING_TYPE)item.attribute("bt").as_int(0);
		data.r_type = (RESEARCH_TECH)item.attribute("rt").as_int(0);

		cell_data.push_back(data);
		
		item = item.next_sibling();
	}

	UpdateCells();
}

void Action_Panel_Elements::UpgradeCurrentAge(uint curr_age)
{
	current_age = curr_age;
	UpdateCells();
}

bool Action_Panel_Elements::UpgradeResearch(RESEARCH_TECH type)
{
	bool ret = false;
	int size = cell_data.size();
	for (int i = 0; i < size; i++)
	{
		if (cell_data[i].r_type == type)
		{ 
			if (cell_data[i].u_type != NO_UNIT)
			{
				cell_lvl[cell_data[i].cell_position]++;
				ret = true;
			}
			else 
			{
				cell_lvl[cell_data[i].cell_position] = cell_data[i].cell_at_level + 1;
				ret = true;
			}
		}
	}
	if (ret)
	{
		UpdateCells();
	}
	return ret;
}

const char* Action_Panel_Elements::GetCellInfo(int i) const
{
	return panel_icons[i].GetInfo();
}

// TOWNCENTER ------------------------------------------------------------------------------------------------------------

TownCenterPanel::TownCenterPanel() : Action_Panel_Elements(), got_melechmp(false){};

void TownCenterPanel::ResetPanel()
{
	SetDataFromXML();
	got_melechmp = false;
	got_wizchmp = false;
	got_archchmp = false;
}

bool TownCenterPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 1: {
		if (got_melechmp == false)
		{
			if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost,
				panel_icons[i].stone_cost, panel_icons[i].population_cost))
			{
				App->sound->PlayGUIAudio(CLICK_INGAME);
				entitis_panel->AddAction(App->action_manager->SpawnAction(entitis_panel->GetWorker(), (ProductiveBuilding*)entitis_panel, panel_icons[i].u_type, ALLY));
				got_melechmp = true;
			}
			else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		break;
	case 2: {
		if (got_wizchmp == false)
		{
			if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost,
				panel_icons[i].stone_cost, panel_icons[i].population_cost))
			{
				App->sound->PlayGUIAudio(CLICK_INGAME);
				entitis_panel->AddAction(App->action_manager->SpawnAction(entitis_panel->GetWorker(), (ProductiveBuilding*)entitis_panel, panel_icons[i].u_type, ALLY));
				got_wizchmp = true;
			}
			else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		else App->sound->PlayGUIAudio(ERROR_SOUND);
	}
			break;
	case 3: {
		if (got_archchmp == false)
		{
			if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost,
				panel_icons[i].stone_cost, panel_icons[i].population_cost))
			{
				App->sound->PlayGUIAudio(CLICK_INGAME);
				entitis_panel->AddAction(App->action_manager->SpawnAction(entitis_panel->GetWorker(), (ProductiveBuilding*)entitis_panel, panel_icons[i].u_type, ALLY));
				got_archchmp = true;
			}
			else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		else App->sound->PlayGUIAudio(ERROR_SOUND);
	}
		break;
	default:
	{
		if (panel_icons[i].u_type != NO_UNIT)
		{
			if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost,
				panel_icons[i].stone_cost, panel_icons[i].population_cost))
			{
				entitis_panel->AddAction(App->action_manager->SpawnAction(entitis_panel->GetWorker(), (ProductiveBuilding*)entitis_panel, panel_icons[i].u_type, ALLY));
				App->sound->PlayGUIAudio(CLICK_INGAME);
			}
			else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		else if (panel_icons[i].r_type != NO_TECH)
		{
			if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost,
				panel_icons[i].stone_cost, panel_icons[i].population_cost))
			{
				entitis_panel->AddAction(App->action_manager->ResearchAction(entitis_panel->GetWorker(), panel_icons[i].r_type, 3000, ALLY));
				App->sound->PlayGUIAudio(CLICK_INGAME);
				cell_lvl[i] -= RESEARCH_MARGIN;
				UpdateCells();
			}
			else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
	}
		break;
	}
	return false;
}

void TownCenterPanel::ChampionIsDead(UNIT_TYPE type)
{
	switch (type)
	{
	case WARRIOR_CHMP: {
		got_melechmp = false;
	}
		break;
	case WIZARD_CHMP: {
		got_wizchmp = false;
	}
		break;
	case ARCHER_CHMP: {
		got_archchmp = false;
	}
		break;
	default:
		break;
	}
}

// UNIT PANEL -------------------------------------------------------------------------------------------------------

UnitPanel::UnitPanel() : Action_Panel_Elements() {};

void UnitPanel::ResetPanel(){}

bool UnitPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 3: {
		App->sound->PlayGUIAudio(CLICK_INGAME);
		entitis_panel->SetLife(0);
		((Unit*)entitis_panel)->AddPriorizedAction(App->action_manager->DieAction((Unit*)entitis_panel));
		entitis_panel = nullptr;
		return false;
	}
		break;
	default:
		break;
	}
	return false;
}

// VILLAGER PANEL ---------------------------------------------------------------------------------------------------

VillagerPanel::VillagerPanel() : Action_Panel_Elements(){}

void VillagerPanel::PreUpdate()
{
	if (isbuilding)
	{
		int x = 0, y = 0;
		App->input->GetMousePosition(x, y);
		buildingthis->OnlySetPosition((float)x - App->render->camera.x,(float) y - App->render->camera.y);
		if (!((Building*)buildingthis)->CheckZone(x - App->render->camera.x, y - App->render->camera.y))
		{
			buildingthis->SetBlitColor({ 255,100,100,255 });
		}
		buildingthis->Draw(false);
	}
}

void VillagerPanel::ResetPanel()
{
	villagerisbuilding = VP_NOT_BUILDING;
	SetDataFromXML();
	isbuilding = false;
	buildingthis = nullptr;
}

void VillagerPanel::SetDefault()
{
	villagerisbuilding = VP_NOT_BUILDING;
	ChangePanelLvl(villagerisbuilding);
	isbuilding = false;
	buildingthis = nullptr;
}

bool VillagerPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;

	if (panel_icons[i].b_type != NO_BUILDING)
	{
		if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost, 
			panel_icons[i].stone_cost, 0, false))
		{
			building_type = i;
			App->sound->PlayGUIAudio(CLICK_INGAME);
			buildingthis = App->entities_manager->GenerateBuilding(panel_icons[i].b_type, ALLY, false);
			isbuilding = true;
			return true;
		}
		else App->sound->PlayGUIAudio(ERROR_SOUND);
	}
	else switch (i)
	{
	case 0: {
		if (villagerisbuilding == VP_NOT_BUILDING)
		{
			villagerisbuilding = VP_NORMAL;
			App->sound->PlayGUIAudio(CLICK_INGAME);
			ChangePanelLvl(villagerisbuilding);
			return false;
		}
		else if (villagerisbuilding == VP_NORMAL)
		{
			return false;
		}
	}
			break;
	case 1: {
		if (villagerisbuilding == VP_NOT_BUILDING) {
			villagerisbuilding = VP_MILITARY;
			ChangePanelLvl(villagerisbuilding);
			App->sound->PlayGUIAudio(CLICK_INGAME);
		}
	}
			break;
	case 2:
		break;
	case 3: {
		App->sound->PlayGUIAudio(CLICK_INGAME);
		entitis_panel->SetLife(0);
		((Unit*)entitis_panel)->AddPriorizedAction(App->action_manager->DieAction((Unit*)entitis_panel));
		entitis_panel = nullptr;
		return false;
	}
			break;
	case 4:
		break;
	case 13: {
		if (villagerisbuilding == VP_NORMAL) {
			villagerisbuilding = VP_MILITARY;
			App->sound->PlayGUIAudio(CLICK_INGAME);
			ChangePanelLvl(villagerisbuilding);
		}
		else if (villagerisbuilding == VP_MILITARY)
		{
			villagerisbuilding = VP_NORMAL;
			App->sound->PlayGUIAudio(CLICK_INGAME);
			ChangePanelLvl(villagerisbuilding);
		}
	}
			 break;
	case 14: if (villagerisbuilding != VP_NOT_BUILDING) {
		villagerisbuilding = VP_NOT_BUILDING;
		App->sound->PlayGUIAudio(CLICK_INGAME);
		ChangePanelLvl(villagerisbuilding);
	}
			 break;
	default:
		break;
	}
	return false;
}

bool VillagerPanel::CancelAction()
{
	if (isbuilding)
	{
		if (buildingthis != nullptr) delete buildingthis;
		buildingthis = nullptr;
		isbuilding = false;
	}
	return false;
}

bool VillagerPanel::Villager_Handle_input(GUI_INPUT input)
{
	switch (input)
	{
	case MOUSE_LEFT_BUTTON_DOWN:
		if (isbuilding)
		{
			int x = 0, y = 0;
			App->input->GetMousePosition(x, y);
			if (((Building*)buildingthis)->CheckZone(x - App->render->camera.x, y - App->render->camera.y))
			{
				player_game_panel_resources->UseResource(panel_icons[building_type].wood_cost, panel_icons[building_type].food_cost, panel_icons[building_type].gold_cost,
					panel_icons[building_type].stone_cost, panel_icons[building_type].population_cost);
				App->entities_manager->buildings.push_back((Building*)buildingthis);
				buildingthis->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y, true);
			}
			else {
				App->entities_manager->DeleteEntity(buildingthis);
			}
			isbuilding = false;
			buildingthis = nullptr;
			if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
			{
				ActivateCell(building_type);
				return true;
			}
			else return false;
		}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:
		if (isbuilding)
		{
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

void VillagerPanel::ChangePanelLvl(int lvl)
{
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		cell_lvl[i] = lvl;
	}
	UpdateCells();
}

bool VillagerPanel::GetIsBuilding() const
{
	return isbuilding;
}

// HERO PANEL -------------------------------------------------------------------------------------------------------

HeroPanel::HeroPanel() : Action_Panel_Elements()
{
	App->gui->SetDefaultInputTarget((j1Module*)App->player);

	skill_tree = (UI_Image*)App->gui->GenerateUI_Element(IMG);
	skill_tree->SetBox({ 30, 100, 243, 345 });
	skill_tree->ChangeTextureId(CHAMPION_SKILL);
	skill_tree->ChangeTextureRect({ 0,0, 243, 345 });
	skill_tree->SetLayer(15);
	skill_tree->AdjustBox();

	warrior_skills_rect.reserve(MAX_SKILLS_LEARNED);
	warrior_skills_rect.push_back({ 651,38, 36, 36 });
	warrior_skills_rect.push_back({ 651,0,36,36 });
	warrior_skills_rect.push_back({ 651,114,36,36 });
	warrior_skills_rect.push_back({ 651,76,36,36 });
	warrior_skills_rect.push_back({ 651,190, 36, 36 });
	warrior_skills_rect.push_back({ 651,152,36,36 });

	wizard_skills_rect.reserve(MAX_SKILLS_LEARNED);
	wizard_skills_rect.push_back({ 689,454, 36, 36 });
	wizard_skills_rect.push_back({ 689,491,36,36 });
	wizard_skills_rect.push_back({ 689,565,36,36 });
	wizard_skills_rect.push_back({ 689,528,36,36 });
	wizard_skills_rect.push_back({ 689,639,36,36 });
	wizard_skills_rect.push_back({ 689,602, 36, 36 });

	archer_skills_rect.reserve(MAX_SKILLS_LEARNED);
	archer_skills_rect.push_back({ 651,454, 36, 36 });
	archer_skills_rect.push_back({ 651,491,36,36 });
	archer_skills_rect.push_back({ 651,528,36,36 });
	archer_skills_rect.push_back({ 651,565,36,36 });
	archer_skills_rect.push_back({ 651,602, 36, 36 });
	archer_skills_rect.push_back({ 651,639,36,36 });

	skills.reserve(MAX_SKILLS_LEARNED);
	skills_buttons.reserve(MAX_SKILLS_LEARNED);
	for (int i = 0; i < MAX_SKILLS_LEARNED; i++)
	{
		skills.push_back((UI_Image*)App->gui->GenerateUI_Element(IMG));
		skills[i]->SetBox({ 0, 0, 36,36 });
		skills[i]->ChangeTextureId(ICONS);
		skills[i]->ChangeTextureRect(warrior_skills_rect[i]);
		skills[i]->SetBoxPosition(97 + 67 * (i % 2), 177 + 55 * (i / 2));
		skills[i]->Desactivate();

		skills_buttons.push_back((UI_Fixed_Button*)App->gui->GenerateUI_Element(FIXED_BUTTON));
		skills_buttons[i]->SetBox({ 77 + 67 * (i % 2), 188 + 55 * (i / 2),16,15 });
		skills_buttons[i]->SetTexDOWN({ 265,62,16,15 }, { 0,0 }, CHAMPION_SKILL);
		skills_buttons[i]->SetTexUP({ 244,62,16,15 }, { 0,0 }, CHAMPION_SKILL);
		skills_buttons[i]->SetTexOVERUP({ 244,62,16,15 }, { 0,0 }, CHAMPION_SKILL);
		skills_buttons[i]->SetTexOVERDOWN({ 266,62,16,15 }, { 0,0 }, CHAMPION_SKILL);
		skills_buttons[i]->Desactivate();

		skill_tree->AddChild(skills_buttons[i]);
		skill_tree->AddChild(skills[i]);
		skills_buttons[i]->SetLayer(8);
		skills[i]->SetLayer(8);
	}
	skill_tree->Desactivate();
	skill_tree->DesactivateChids();

	champion_mele = nullptr;
	champion_wizard = nullptr;
	champion_archer = nullptr;

	champion_row.push_back(champion_mele);
	champion_row.push_back(champion_wizard);
	champion_row.push_back(champion_archer);


}

HeroPanel::~HeroPanel()
{
	skills.clear();
	skills_buttons.clear();

	warrior_skills_rect.clear();
	wizard_skills_rect.clear();
	archer_skills_rect.clear();
}

bool HeroPanel::Load(pugi::xml_node & data)
{
	// Load Panel Cell Levels
	pugi::xml_node curr_cell = data.child("warrior_levels").first_child();
	for (int i = 0; i < 5; i++)
	{
		mele_learned[i] = curr_cell.attribute("level").as_int(0);
		curr_cell = curr_cell.next_sibling();
	}

	curr_cell = data.child("wizard_levels").first_child();
	for (int i = 0; i < 5; i++)
	{
		wizard_learned[i] = curr_cell.attribute("level").as_int(0);
		curr_cell = curr_cell.next_sibling();
	}

	curr_cell = data.child("archer_levels").first_child();
	for (int i = 0; i < 5; i++)
	{
		archer_learned[i] = curr_cell.attribute("level").as_int(0);
		curr_cell = curr_cell.next_sibling();
	}
	return true;
}

bool HeroPanel::Save(pugi::xml_node & data) const
{
	// Save Panel Cell Levels
	pugi::xml_node curr_cell = data.append_child("warrior_levels");
	for (int i = 0; i < 5; i++)
	{
		curr_cell = data.child("warrior_levels").append_child("cell");
		curr_cell.append_attribute("level") = mele_learned[i];
	}

	curr_cell = data.append_child("wizard_levels");
	for (int i = 0; i < 5; i++)
	{
		curr_cell = data.child("wizard_levels").append_child("cell");
		curr_cell.append_attribute("level") = wizard_learned[i];
	}

	curr_cell = data.append_child("archer_levels");
	for (int i = 0; i < 5; i++)
	{
		curr_cell = data.child("archer_levels").append_child("cell");
		curr_cell.append_attribute("level") = archer_learned[i];
	}

	return true;
}

void HeroPanel::ResetPanel()
{
	entitis_panel = nullptr;
	champion_selected = NO_UNIT;

	champion_mele = nullptr;
	champion_wizard = nullptr;
	champion_archer = nullptr;

	champion_row.clear();
	champion_row.push_back(champion_mele);
	champion_row.push_back(champion_wizard);
	champion_row.push_back(champion_archer);

	skill_tree->Desactivate();
	skill_tree->DesactivateChids();

	for (int i = 0; i < 5; i++)
	{
		mele_learned[i] = 0;
		archer_learned[i] = 0;
		wizard_learned[i] = 0;
	}
	
	for (int i = 0; i < MAX_SKILLS_LEARNED; i++)
	{
		skills_buttons[i]->button_state = UP;
	}
	SetDataFromXML();

	activate_skill = -1;
	show_skill = -1;
}

void HeroPanel::HeroisDead(UNIT_TYPE u_type)
{

	if (u_type == WARRIOR_CHMP) {
		for (int i = 0; i < 5; i++)
			mele_learned[i] = 0;
		if(champion_mele != nullptr)
			champion_mele = nullptr;
		else return;
	}
	else if (u_type == WIZARD_CHMP) {
		for (int i = 0; i < 5; i++)
			wizard_learned[i] = 0;
		if (champion_wizard != nullptr)
			champion_wizard = nullptr;
		else return;
	}
	else {
		for (int i = 0; i < 5; i++)
			archer_learned[i] = 0;
		if (champion_archer != nullptr)
			champion_archer = nullptr;
		else return;
	}
	champion_row.clear();
	if (champion_mele != nullptr) champion_row.push_back(champion_mele);
	if (champion_wizard != nullptr) champion_row.push_back(champion_wizard);
	if (champion_archer != nullptr) champion_row.push_back(champion_archer);

	while (champion_row.size() < 3)
	{
		champion_row.push_back(nullptr);
	}
	if (champion_row[0] != nullptr)
	{
		entitis_panel = champion_row[0];
		champion_selected = ((Unit*)entitis_panel)->GetUnitType();
		activate_skill = -1;
		if (skill_tree->GetActiveState() == true)
		{
			skill_tree->Desactivate();
			skill_tree->DesactivateChids();
		}
		UpdateCells();
	}
	else
	{
		entitis_panel = nullptr;
		SetDefault();
	}
}


void HeroPanel::UpdateCells()
{
	SetNewOrder();
}

void HeroPanel::SetDefault()
{
	entitis_panel = nullptr;
	champion_mele = nullptr;
	champion_wizard = nullptr;
	champion_archer = nullptr;
	champion_selected = NO_UNIT;
	skill_tree->Desactivate();
	skill_tree->DesactivateChids();
	activate_skill = -1;
}

bool HeroPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	
	// Set Champion selected 
	if (champion_row[i / 5] != entitis_panel)
	{
		if (skill_tree->GetActiveState() == true)
		{
			skill_tree->Desactivate();
			skill_tree->DesactivateChids();
		}
		entitis_panel = champion_row[i / 5];
		activate_skill = -1;
		champion_selected = ((Unit*)entitis_panel)->GetUnitType();
	}

	int cellpos = i % 5;
	switch (cellpos)
	{
	case 0: {
		App->sound->PlayGUIAudio(CLICK_INGAME);
		((Champion*)entitis_panel)->Hability_lvl_1();
		return false;
		}
		break;
	case 1: {
			if (((Champion*)entitis_panel)->PrepareAbility_lvl_2()) {
				activate_skill = 1;
				App->sound->PlayGUIAudio(CLICK_INGAME);
				return true;
			}
			else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		break;
	case 2:
		if (champion_selected == WARRIOR_CHMP)
		{
			App->sound->PlayGUIAudio(CLICK_INGAME);
			((Champion*)entitis_panel)->Hability_lvl_3();
			return false;
		}
		else
		{
			if (((Champion*)entitis_panel)->PrepareAbility_lvl_3())
			{
				activate_skill = 2;
				App->sound->PlayGUIAudio(CLICK_INGAME);
				return true;
			}
			else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		break;
	case 3:
		if (skill_tree->GetActiveState() == true)
		{
			App->sound->PlayGUIAudio(CLICK_INGAME);
			SetSkillTree();
			skill_tree->Desactivate();
			skill_tree->DesactivateChids();
		}
		else {
			App->sound->PlayGUIAudio(CLICK_INGAME);
			SetSkillTree();
			skill_tree->Activate();
			skill_tree->ActivateChilds();
		}
		break;
	case 4: {
		App->sound->PlayGUIAudio(CLICK_INGAME);
		UNIT_TYPE u_type = ((Unit*)entitis_panel)->GetUnitType();
		entitis_panel->SetLife(0);
		((Unit*)entitis_panel)->AddPriorizedAction(App->action_manager->DieAction((Unit*)entitis_panel));
		App->player->selection_panel->RemoveDeadSelectedUnits((Unit*)entitis_panel);
		HeroisDead(u_type);
		champion_selected = NO_UNIT;
		activate_skill = -1;
		show_skill = -1;
		}
		break;
	default:
		break;
	}
	return false;
}

bool HeroPanel::ActivateSkill(int i)
{
	int n = i;
	if (entitis_panel == champion_row[1]) n += 5;
	else if (entitis_panel == champion_row[2]) n += 10;
	return ActivateCell(n);
}

bool HeroPanel::CancelAction()
{
	if (skill_tree->GetActiveState())
	{
		skill_tree->Desactivate();
		skill_tree->DesactivateChids();
	}
	// Cancel Skill;
	if (activate_skill == 1) ((Champion*)entitis_panel)->Cancel_Ability_lvl_2();
	else ((Champion*)entitis_panel)->Cancel_Ability_lvl_3();
	activate_skill = -1;

	return false;
}

bool HeroPanel::Hero_Handle_input(UI_Element * ui_element, GUI_INPUT ui_input)
{
	switch (ui_input)
	{
	case UP_ARROW:						break;
	case DOWN_ARROW:					break;
	case LEFT_ARROW:					break;
	case RIGHT_ARROW:					break;
	case MOUSE_LEFT_BUTTON_DOWN:		break;
	case MOUSE_LEFT_BUTTON_REPEAT:		break;
	case MOUSE_LEFT_BUTTON_UP:			
		{
			for (int i = 0; i < MAX_SKILLS_LEARNED; i++)
			{
				if (ui_element == skills_buttons[i])
				{
					App->sound->PlayGUIAudio(CLICK_INGAME);
					int r = i;
					if (entitis_panel == champion_row[1]) r += 10;
					else if (entitis_panel == champion_row[2]) r += 20;

					if (cell_lvl[r / 2] == 0 && ((Champion*)entitis_panel)->GetLevel() >= (i / 2))
					{
						LearnSkill(i);
						return true;
					}
					else
					{
						skills_buttons[i]->button_state = UP;
						return false;
					}
				}
			}
		}
		break;
	case MOUSE_RIGHT_BUTTON:			break;
	case MOUSE_IN: {
		if (!skill_tree->GetActiveState()) return false;
		for (int count = 0; count < MAX_SKILLS_LEARNED; count++)
		{
			if (ui_element == skills[count])
			{
				show_skill = count;
				return true;
			}
		}
		}
		break;
	case MOUSE_OUT: {
		if (show_skill == -1 || !skill_tree->GetActiveState()) return true;
		for (int count = 0; count < MAX_SKILLS_LEARNED; count++)
		{
			if (ui_element == skills[count] && count == show_skill)
			{
				show_skill = -1;
				return false;
			}
		}
		return true;
		}				
		break;
	case SUPR:							break;
	case BACKSPACE:						break;
	case ENTER:							break;
	case TAB:							break;
	default:break;
	}
	return false;
}

bool HeroPanel::Handle_input(GUI_INPUT input)
{
	switch (input)
	{
	case MOUSE_LEFT_BUTTON_DOWN: if (activate_skill != -1)
	{
		int x = 0, y = 0;
		App->input->GetMousePosition(x, y);
		x -= App->render->camera.x;
		y -= App->render->camera.y;
		entitis_panel->GetWorker()->ResetChannel(TASK_CHANNELS::PRIMARY);
		
		if (activate_skill == 1)
			((Champion*)entitis_panel)->Hability_lvl_2(x, y);
		else if(activate_skill == 2)
			((Champion*)entitis_panel)->Hability_lvl_3(x, y);
		activate_skill = -1;
		return false;
	}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:
		break;
	case MOUSE_RIGHT_BUTTON:
		break;
	default:
		break;
	}
	return false;
}

void HeroPanel::LearnSkill(int i)
{
	int skill_cell = i / 2;
	if (champion_selected == WIZARD_CHMP && wizard_learned[skill_cell] == 0) wizard_learned[skill_cell] = (i % 2) + 1;
	else if (champion_selected == ARCHER_CHMP && archer_learned[skill_cell] == 0) archer_learned[skill_cell] = (i % 2) + 1;
	else if(champion_selected == WARRIOR_CHMP && mele_learned[skill_cell] == 0) mele_learned[skill_cell] = (i % 2) + 1;
	
	bool skill = false;
	if (i % 2 == 1) skill = true;
	if (i / 2 == 0)((Champion*)entitis_panel)->SetAbility_lvl_1(skill);
	else if (i / 2 == 1)((Champion*)entitis_panel)->SetAbility_lvl_2(skill);
	else if (i / 2 == 2)((Champion*)entitis_panel)->SetAbility_lvl_3(skill);

	SetNewOrder();
}

void HeroPanel::SetSkillTree()
{
	for (int i = 0; i < MAX_SKILLS_LEARNED; i++)
	{
		skills_buttons[i]->button_state = UP;
	}
	if (curr_skills_tree != champion_selected)
	{
		curr_skills_tree = champion_selected;
		ChangeSkillIcons();
	}
}

void HeroPanel::ChangeSkillIcons()
{
	show_skill = -1;
	std::vector<SDL_Rect>* curr;
	if (curr_skills_tree == WARRIOR_CHMP) curr = &warrior_skills_rect;
	else if (curr_skills_tree == WIZARD_CHMP) curr = &wizard_skills_rect;
	else curr = &archer_skills_rect;

	for (int i = 0; i < MAX_SKILLS_LEARNED; i++)
	{
		skills[i]->ChangeTextureRect((*curr)[i]);
	}
}

void HeroPanel::SetNewOrder()
{
	// Reset Cells
	Cells_Data celldata;
	panel_icons.clear();
	int count = -1;
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		if (i % 5 == 0) count++;
		if (champion_row[count] == nullptr);
		else if (champion_row[count] == champion_mele) cell_lvl[i] = mele_learned[i];
		else if (champion_row[count] == champion_wizard)
			cell_lvl[i] = wizard_learned[i - (count * 5)];
		else if (champion_row[count] == champion_archer) cell_lvl[i] = archer_learned[i - (count * 5)];
		panel_icons.push_back(celldata);
	}
	
	// Set Current Cells
	int pos = 0;
	int size = cell_data.size();
	if (champion_mele != nullptr)
	{		
		for (int i = 0; i < size; i++)
		{
			pos = cell_data[i].cell_position;
			if (pos == 5)break;
			else if (cell_data[i].cell_at_level == cell_lvl[pos] && cell_data[i].cell_at_age <= current_age)
			{
				panel_icons[pos] = cell_data[i];
			}
		}
	}	
	if (champion_wizard != nullptr)
	{
		int place = 0;
		if (champion_mele == nullptr) place = -5;
		for (int i = 0; i < size; i++)
		{
			pos = cell_data[i].cell_position;
			if (pos < 5 || pos >= 10) continue;
			else if (cell_data[i].cell_at_level == cell_lvl[pos + place] && cell_data[i].cell_at_age <= current_age)
			{
				panel_icons[pos + place] = cell_data[i];
			}
		}
	}
	if (champion_archer != nullptr)
	{
		int place = 0;
		if (champion_row[0] == champion_archer) place = -10;
		else if (champion_row[1] == champion_archer) place = -5;
		for (int i = 0; i < size; i++)
		{
			pos = cell_data[i].cell_position;
			if (pos < 10)continue;
			else if (cell_data[i].cell_at_level == cell_lvl[pos + place] && cell_data[i].cell_at_age <= current_age)
			{
				panel_icons[pos + place] = cell_data[i];
			}
		}
	}
}

void HeroPanel::ChangePanelTarget(Entity * new_target)
{
	activate_skill = -1;
	App->player->selection_panel->GetChampionsSelected(champion_mele, champion_wizard, champion_archer);
	champion_row.clear();
	if (champion_mele != nullptr) champion_row.push_back(champion_mele);
	if (champion_wizard != nullptr) champion_row.push_back(champion_wizard);
	if (champion_archer != nullptr) champion_row.push_back(champion_archer);
	while (champion_row.size() < 3)
	{
		champion_row.push_back(nullptr);
	}
	entitis_panel = champion_row[0];
	if (entitis_panel != nullptr) champion_selected = ((Unit*)entitis_panel)->GetUnitType();
	SetNewOrder();
}

const char * HeroPanel::GetSkillOvering() const
{
	int i = show_skill%2;
	int r = show_skill;
	if (entitis_panel == champion_wizard) r += 10;
	else if (entitis_panel == champion_archer) r += 20;
	r = r / 2;
	int size = cell_data.size();
	for (int j = 0; j < size; j++)
	{
		if (cell_data[j].cell_position == r && cell_data[j].cell_at_level == i+1)
		{
			return cell_data[j].GetInfo();
		}
	}
	return ("");
}

// ACTION PANEL  ================================================================================================

Action_Panel::Action_Panel() : action_rect({ 37, 624, 200, 123 }), panel_pos({ 37,624 }), isin(false)
{
	// Create Panels
	towncenterpanel = new TownCenterPanel();
	barrackpanel = new Action_Panel_Elements();
	archerypanel = new Action_Panel_Elements();
	stablepanel = new Action_Panel_Elements();
	blacksmithpanel = new Action_Panel_Elements();
	lumbercamppanel = new Action_Panel_Elements();
	miningcamppanel = new Action_Panel_Elements();
	monasterypanel = new Action_Panel_Elements();
	universitypanel = new Action_Panel_Elements();
	castlepanel = new Action_Panel_Elements();
	unitpanel = new UnitPanel();
	heropanel = new HeroPanel();
	villagerpanel = new VillagerPanel();


	//Load Panels Data from loaded folder
	char* buffer = nullptr;
	int size = App->fs->Load("gui/Hud_Action_Panel_Data.xml", &buffer);
	pugi::xml_document panel_data;
	pugi::xml_parse_result result = panel_data.load_buffer(buffer, size);
	RELEASE_ARRAY(buffer);

	pugi::xml_node panel_node = panel_data.first_child();
	towncenterpanel->LoadPanelFromXML(panel_node.child("Buildings").child("TownCenter"));
	barrackpanel->LoadPanelFromXML(panel_node.child("Buildings").child("Barrack"));
	archerypanel->LoadPanelFromXML(panel_node.child("Buildings").child("Archery_Range"));
	stablepanel->LoadPanelFromXML(panel_node.child("Buildings").child("Stable"));
	blacksmithpanel->LoadPanelFromXML(panel_node.child("Buildings").child("BlackSmith"));
	lumbercamppanel->LoadPanelFromXML(panel_node.child("Buildings").child("Lumber_Camp"));
	miningcamppanel->LoadPanelFromXML(panel_node.child("Buildings").child("Mining_Camp"));
	monasterypanel->LoadPanelFromXML(panel_node.child("Buildings").child("Monastery"));
	universitypanel->LoadPanelFromXML(panel_node.child("Buildings").child("University"));
	castlepanel->LoadPanelFromXML(panel_node.child("Buildings").child("Castle"));

	villagerpanel->LoadPanelFromXML(panel_node.child("Units").child("Villager"));
	unitpanel->LoadPanelFromXML(panel_node.child("Units").child("Unit"));
	heropanel->LoadPanelFromXML(panel_node.child("Units").child("Champion"));

	// Set GUI Elements
	App->gui->SetDefaultInputTarget((j1Module*)App->player);

	action_screen = App->gui->GenerateUI_Element(UNDEFINED);
	action_screen->SetBox(action_rect);
	action_screen->SetLayer(20);
	action_screen->Activate();
	action_screen->SetInputTarget((j1Module*)App->player);

	cell_information = (UI_String*)App->gui->GenerateUI_Element(STRING);
	cell_information->SetBoxPosition(10, 570);
	cell_information->Activate();
	cell_information->SetColor({255, 255, 255, 255});

	panel_cells.reserve(MAX_PANEL_CELLS);
	panel_buttons.reserve(MAX_PANEL_CELLS);

	UI_Image* cell = nullptr;
	UI_Button* button = nullptr;
	for (int i = 0; i < MAX_PANEL_CELLS; i++)
	{
		cell = (UI_Image*)App->gui->GenerateUI_Element(IMG);
		cell->ChangeTextureRect({ 0, 0, 1, 1 });
		cell->SetBoxPosition(panel_pos.x +2 + (i% PANEL_COLUMNS)*(CELL_WIDTH+1), panel_pos.y +2 + ((int)i / PANEL_COLUMNS)*CELL_HEIGHT);
		cell->ChangeTextureId(ICONS);
		panel_cells.push_back(cell);

		button = (UI_Button*)App->gui->GenerateUI_Element(BUTTON);
		button->SetBox({(i% PANEL_COLUMNS)*(CELL_WIDTH+1), ((int)i / PANEL_COLUMNS)*CELL_HEIGHT, 40, 40 });
		button->SetTexON({ 650,330,40,40 },ICONS);
		button->SetTexOVER({ 650,412,40,40 },ICONS);
		button->SetTexOFF({ 650,412,40,40 },ICONS);
		panel_buttons.push_back(button);
		action_screen->AddChild(button);
	}
	action_screen->DesactivateChids();

}

Action_Panel::~Action_Panel()
{
	CleanUp();
}

bool Action_Panel::CleanUp()
{
	panel_cells.clear();
	panel_buttons.clear();

	delete towncenterpanel;
	delete barrackpanel;
	delete archerypanel;
	delete stablepanel;
	delete blacksmithpanel;
	delete lumbercamppanel;
	delete miningcamppanel;
	delete monasterypanel;
	delete universitypanel;
	delete castlepanel;
	delete villagerpanel;
	delete unitpanel;
	delete heropanel;

	return false;
}

void Action_Panel::Enable()
{
	on_action = false;
	show_cell_info = false;
	show_hero_skills = false;
	
	action_screen->Activate();
	action_screen->DesactivateChids();

	towncenterpanel->ResetPanel();
	barrackpanel->ResetPanel();
	archerypanel->ResetPanel();
	stablepanel->ResetPanel();
	blacksmithpanel->ResetPanel();
	lumbercamppanel->ResetPanel();
	miningcamppanel->ResetPanel();
	monasterypanel->ResetPanel();
	universitypanel->ResetPanel();
	castlepanel->ResetPanel();
	villagerpanel->ResetPanel();
	unitpanel->ResetPanel();
	heropanel->ResetPanel();
}

void Action_Panel::Disable()
{
	action_screen->Desactivate();
	action_screen->DesactivateChids();
}

bool Action_Panel::PreUpdate()
{
	// Action Panel Check Selected
	CheckSelected(player_selection_panel->GetSelectedSize());

	if (on_action)
	{
		if (actualpanel == villagerpanel)
		{
			villagerpanel->PreUpdate();
		}
	}
	return true;
}

bool Action_Panel::PostUpdate()
{
	return false;
}

void Action_Panel::Handle_Input(GUI_INPUT newevent)
{
	if (!on_action) return;

	switch (newevent)
	{
	case MOUSE_LEFT_BUTTON_DOWN:
		{
			if (actualpanel == heropanel)
			{
				on_action = heropanel->Handle_input(newevent);
			}
			else if (actualpanel == villagerpanel)
			{
				on_action = villagerpanel->Villager_Handle_input(newevent);
			}
		}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT: 
		break;
	case MOUSE_LEFT_BUTTON_UP: {
		if (actualpanel == villagerpanel)
		{
			on_action = villagerpanel->Villager_Handle_input(newevent);
		}
		}
		break;
	case MOUSE_RIGHT_BUTTON:
		{
		if (on_action) {
			on_action = actualpanel->CancelAction();;
		}
		}
		break;
	default:
		break;
	}
	return;
}

void Action_Panel::Handle_Input(UI_Element * ui_element, GUI_INPUT ui_input)
{
	switch (ui_input)
	{
	case MOUSE_LEFT_BUTTON_DOWN:
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		if (isin && ui_element->GetUItype() == BUTTON)
		{
			if (actualpanel == unitpanel && !on_action)
			{
				for (int count = 0; count < MAX_PANEL_CELLS; count++)
				{
					if (ui_element == panel_buttons[count])
					{
						on_action = actualpanel->ActivateCell(count);
					}
				}
			}
		}
		break;
	case MOUSE_LEFT_BUTTON_UP:
		if (isin && ui_element->GetUItype() == BUTTON)
		{
			for (int count = 0; count < MAX_PANEL_CELLS; count++)
			{
				if (ui_element == panel_buttons[count]) {
					on_action = actualpanel->ActivateCell(count);
					if (!on_action)
					{
						actualpanel->ChangePanelIcons(panel_cells);
						SetButtons();
						return;
					}
				}
			}
		}
		else if (actualpanel == heropanel && ui_element->GetUItype() == FIXED_BUTTON)
		{
			if (heropanel->Hero_Handle_input(ui_element, ui_input))
			{
				action_screen->DesactivateChids();
				heropanel->ChangePanelIcons(panel_cells);
				SetButtons();
			}
		}
		break;
	case MOUSE_RIGHT_BUTTON:
		break;
	case MOUSE_IN:
	{
		if (ui_element == action_screen) {
			isin = true;
		}
		if (ui_element == panel_buttons[cell_shown] && show_cell_info == true) break;
		if (ui_element->GetUItype() == BUTTON && show_cell_info == false)
		{
			for (int count = 0; count < MAX_PANEL_CELLS; count++)
			{
				if (ui_element == panel_buttons[count])
				{
					cell_information->SetString((char*)actualpanel->GetCellInfo(count));
					cell_shown = count;
					show_cell_info = true;
				}
			}
		}
		else if (ui_element->GetUItype() == IMG && show_hero_skills == false && actualpanel == heropanel)
		{
			if (heropanel->Hero_Handle_input(ui_element, ui_input))
			{
				cell_information->SetString((char*)heropanel->GetSkillOvering());
				show_hero_skills = true;
			}
		}
	}
		break;
	case MOUSE_OUT:
		if (ui_element == action_screen)
		{
			isin = false;
			show_cell_info = false;
		}
		else if (ui_element == panel_buttons[cell_shown] && isin)
		{
			show_cell_info = false;
		}
		else if (actualpanel == heropanel && show_hero_skills == true)
		{
			if ((ui_element->GetUItype() == IMG && !heropanel->Hero_Handle_input(ui_element, ui_input)) || !heropanel->skill_tree->GetActiveState())
			{
				show_hero_skills = false;
			}
		}
		break;
	default:
		break;
	}
}

void Action_Panel::ActivateSkill(int i)
{
	if (actualpanel == heropanel) {
		on_action = actualpanel->ActivateCell(i);
	}
}

bool Action_Panel::Draw()
{
	if (actualpanel == nullptr) return false;
	for (int count = 0; count < MAX_PANEL_CELLS; count++)
	{
		if(panel_cells[count]->GetTextureBox().w > 1) panel_cells[count]->Draw(false);
	}
	if (show_cell_info || show_hero_skills)
	{
		cell_information->Draw(false);
	}
	return true;
}

bool Action_Panel::Load(pugi::xml_node & data)
{
	// Action Panel Load ---------------------------
	// Action Panel node
	pugi::xml_node game_panel_data = data.child("hud_action_panel");

	// Load Panel Data
	pugi::xml_node curr_data = game_panel_data.child("action_panel");

	current_age = curr_data.attribute("current_age").as_int(2);

	// Load Player Action Panels
	towncenterpanel->Load(game_panel_data.child("town_center"));
	barrackpanel->Load(game_panel_data.child("barrack"));
	archerypanel->Load(game_panel_data.child("archery"));
	stablepanel->Load(game_panel_data.child("stable"));
	blacksmithpanel->Load(game_panel_data.child("blacksmmith"));
	lumbercamppanel->Load(game_panel_data.child("lumber_camp"));
	miningcamppanel->Load(game_panel_data.child("mining_camp"));
	universitypanel->Load(game_panel_data.child("university"));
	heropanel->Load(game_panel_data.child("heroes"));

	villagerpanel->UpgradeCurrentAge(current_age);

	SetPanelType();

	return true;
}

bool Action_Panel::Save(pugi::xml_node & data) const
{
	// Action Panel Save ---------------------------
	// Action Panel node
	pugi::xml_node game_panel_data = data.append_child("hud_action_panel");

	// Save Panel Data
	pugi::xml_node curr_data = game_panel_data.append_child("action_panel");
	curr_data.append_attribute("current_age") = current_age;

	// Save Player Action Panels
	towncenterpanel->Save(game_panel_data.append_child("town_center"));
	barrackpanel->Save(game_panel_data.append_child("barrack"));
	archerypanel->Save(game_panel_data.append_child("archery"));
	stablepanel->Save(game_panel_data.append_child("stable"));
	blacksmithpanel->Save(game_panel_data.append_child("blacksmmith"));
	lumbercamppanel->Load(game_panel_data.append_child("lumber_camp"));
	miningcamppanel->Load(game_panel_data.append_child("mining_camp"));
	universitypanel->Load(game_panel_data.append_child("university"));
	heropanel->Save(game_panel_data.append_child("heroes"));

	return true;
}

void Action_Panel::Reset()
{
	on_action = false;
	show_cell_info = false;
	show_hero_skills = false;

	action_screen->Activate();
	action_screen->DesactivateChids();

	towncenterpanel->ResetPanel();
	barrackpanel->ResetPanel();
	archerypanel->ResetPanel();
	stablepanel->ResetPanel();
	blacksmithpanel->ResetPanel();
	lumbercamppanel->ResetPanel();
	miningcamppanel->ResetPanel();
	universitypanel->ResetPanel();
	villagerpanel->ResetPanel();
	heropanel->ResetPanel();
}

void Action_Panel::SetSelectionPanelPointer(Selection_Panel * selection_panel)
{
	player_selection_panel = selection_panel;
}

void Action_Panel::SetGamePanelPointer(Game_Panel * game_panel)
{
	player_game_panel = game_panel;
}

void Action_Panel::SetPanelType(bool force_setup)
{
	if (isin && !force_setup) return;
	SetEntitySelected();

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

	if (actualpanel == heropanel && u_type != WARRIOR_CHMP && u_type != WIZARD_CHMP && u_type != ARCHER_CHMP)
	{
		heropanel->skill_tree->Desactivate();
		heropanel->skill_tree->DesactivateChids();
	}

	switch (e_type)
	{
	case NO_ENTITY: {
		actual_entity = nullptr;
		actualpanel = nullptr;
		}
		break;
	case UNIT:
		{
			if (u_type == VILLAGER) {	
				if (villagerpanel->GetActualEntity() != actual_entity) villagerpanel->SetDefault();
				villagerpanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = villagerpanel;
			}
			else if (u_type == WARRIOR_CHMP || u_type == WIZARD_CHMP || u_type == ARCHER_CHMP)
			{
				heropanel->SetDefault();
				actualpanel = heropanel;
			}
			else
			{
				actualpanel = unitpanel;
			}
		}
		break;
	case RESOURCE: {
		actual_entity = nullptr;
		actualpanel = nullptr;
		}
		break;
	case BUILDING:
		{
			if (b_type == TOWN_CENTER || b_type == TOWN_CENTER_C)
			{
				if(towncenterpanel->GetActualEntity() != actual_entity || actualpanel != towncenterpanel) App->sound->PlayFXAudio(TOWN_CENTER_SELECTED_SOUND);
				towncenterpanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = towncenterpanel;
			}
			else if (b_type == BARRACK || b_type == BARRACK_C)
			{
				if (barrackpanel->GetActualEntity() != actual_entity || actualpanel != barrackpanel)App->sound->PlayFXAudio(BARRACK_SOUND);
				barrackpanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = barrackpanel;
			}
			else if (b_type == ARCHERY_RANGE || b_type == ARCHERY_RANGE_C)
			{
				if (archerypanel->GetActualEntity() != actual_entity || actualpanel != archerypanel)App->sound->PlayFXAudio(ARCHERY_SOUND);
				archerypanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = archerypanel;
			}
			else if (b_type == STABLE || b_type == STABLE_C)
			{
				if (stablepanel->GetActualEntity() != actual_entity || actualpanel != stablepanel)App->sound->PlayFXAudio(STABLE_SOUND);
				stablepanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = stablepanel;
			}
			else if (b_type == BLACKSMITH || b_type == BLACKSMITH_C)
			{
				if (blacksmithpanel->GetActualEntity() != actual_entity || actualpanel != blacksmithpanel)App->sound->PlayFXAudio(BLACKSMITH_SOUND);
				blacksmithpanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = blacksmithpanel;
			}
			else if (b_type == LUMBER_CAMP)
			{
				if (lumbercamppanel->GetActualEntity() != actual_entity || actualpanel != lumbercamppanel)App->sound->PlayFXAudio(LUMBER_CAMP_SOUND);
				lumbercamppanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = lumbercamppanel;
			}
			else if (b_type == MINING_CAMP)
			{
				if (miningcamppanel->GetActualEntity() != actual_entity || actualpanel != miningcamppanel)App->sound->PlayFXAudio(MINING_CAMP_SOUND);
				miningcamppanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = miningcamppanel;
			}
			else if (b_type == MONASTERY)
			{
				if (monasterypanel->GetActualEntity() != actual_entity || actualpanel != monasterypanel)App->sound->PlayFXAudio(MONASTERY_SOUND);
				monasterypanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = monasterypanel;
			}
			else if (b_type == UNIVERSITY_C || b_type == UNIVERSITY_I)
			{
				if (universitypanel->GetActualEntity() != actual_entity || actualpanel != universitypanel)App->sound->PlayFXAudio(UNIVERSITY_SOUND);
				universitypanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = universitypanel;
			}
			else if (b_type == CASTLE)
			{
				if (castlepanel->GetActualEntity() != actual_entity || actualpanel != castlepanel)App->sound->PlayFXAudio(CASTLE_SOUND);
				castlepanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = castlepanel;
			}
			else actualpanel = nullptr;		
		}
		break;
	default: {
		actual_entity = nullptr;
		actualpanel = nullptr;
		}
		break;
	}

	if (actualpanel != nullptr)
	{
		action_screen->DesactivateChids();
		actualpanel->ChangePanelTarget(actual_entity);
		actualpanel->ChangePanelIcons(panel_cells);
		SetButtons();
	}
	else DesactivateButtons();
}

void Action_Panel::SetEntitySelected()
{
	actual_entity = player_selection_panel->GetSelected();
	if (actual_entity == nullptr)
	{
		DesactivateButtons();
		actualpanel = nullptr;
		return;
	}
}

void Action_Panel::SetButtons()
{
	show_cell_info = false;
	for (int count = 0; count < MAX_PANEL_CELLS; count++)
	{
		if (panel_cells[count]->GetTextureBox().w > 1 && panel_cells[count]->GetTextureBox().h > 1)
		{
			panel_buttons[count]->Activate();
		}
		else
		{
			panel_buttons[count]->Desactivate();
		}
	}
}

void Action_Panel::DesactivateButtons()
{
	for (int count = 0; count < MAX_PANEL_CELLS; count++)
	{
		panel_buttons[count]->Desactivate();
	}
}

bool Action_Panel::GetOnAction() const
{
	return on_action;
}

bool Action_Panel::GetIsIn() const
{
	return isin;
}

void Action_Panel::CheckSelected(int selected)
{	
	if (selected == 0 && actualpanel != nullptr)
	{
		on_action = false;
		actual_entity = nullptr;
		action_screen->DesactivateChids();
		actualpanel->SetDefault();
		actualpanel = nullptr;
		return;
	}
	
	if (actualpanel != nullptr)
	{
		if (actualpanel->GetActualEntity() == nullptr || actualpanel->GetActualEntity()->GetLife() == 0)
		{
			if (actualpanel == heropanel) SetPanelType(true);
			else actualpanel->ChangePanelTarget(player_selection_panel->GetSelected());
		}
	}
}

void Action_Panel::HeroIsDead(UNIT_TYPE type)
{
	towncenterpanel->ChampionIsDead(type);
	heropanel->HeroisDead(type);
}

UI_Element * Action_Panel::GetActionScreen() const
{
	return action_screen;
}

UI_Element * Action_Panel::GetHeroSkillTree() const
{
	return heropanel->skill_tree;
}

void Action_Panel::UpgradeCivilizationAge(uint curr_age)
{
	current_age = curr_age;

	towncenterpanel->UpgradeCurrentAge(curr_age);
	barrackpanel->UpgradeCurrentAge(curr_age);
	archerypanel->UpgradeCurrentAge(curr_age);
	stablepanel->UpgradeCurrentAge(curr_age);
	blacksmithpanel->UpgradeCurrentAge(curr_age);
	lumbercamppanel->UpgradeCurrentAge(curr_age);
	miningcamppanel->UpgradeCurrentAge(curr_age);
	monasterypanel->UpgradeCurrentAge(curr_age);
	universitypanel->UpgradeCurrentAge(curr_age);
	castlepanel->UpgradeCurrentAge(curr_age);

	villagerpanel->UpgradeCurrentAge(curr_age);
}

void Action_Panel::UpgradeTecnology(RESEARCH_TECH type)
{
	// update panels acording to type
	towncenterpanel->UpgradeResearch(type);
	barrackpanel->UpgradeResearch(type);
	archerypanel->UpgradeResearch(type);
	stablepanel->UpgradeResearch(type);
	blacksmithpanel->UpgradeResearch(type);
	lumbercamppanel->UpgradeResearch(type);
	miningcamppanel->UpgradeResearch(type);
	monasterypanel->UpgradeResearch(type);
	universitypanel->UpgradeResearch(type);
	castlepanel->UpgradeResearch(type);

	// updata actual panel;
	if (actualpanel != nullptr)
	{
		action_screen->DesactivateChids();
		actualpanel->ChangePanelIcons(panel_cells);
		SetButtons();
	}
}
