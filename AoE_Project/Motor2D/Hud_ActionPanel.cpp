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

void Action_Panel_Elements::SetDataFromXML()
{
	current_age = 1;
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
};

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

		data.SetInfo();

		cell_data.push_back(data);
		
		item = item.next_sibling();
	}

	UpdateCells();
}

void Action_Panel_Elements::UpgradeCurrentAge(uint curr_age)
{
	current_age = current_age;
	UpdateCells();
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
}

bool TownCenterPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 0: {
		if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost,
			panel_icons[i].stone_cost, panel_icons[i].population_cost))
		{
			App->sound->PlayGUIAudio(CLICK_INGAME);
			entitis_panel->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)entitis_panel, VILLAGER, ALLY));
		}
		else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		break;
	case 1: {
		if (got_melechmp == false)
		{
			if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost, panel_icons[i].food_cost, panel_icons[i].gold_cost,
				panel_icons[i].stone_cost, panel_icons[i].population_cost))
			{
				App->sound->PlayGUIAudio(CLICK_INGAME);
				entitis_panel->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)entitis_panel, WARRIOR_CHMP, ALLY));
				got_melechmp = true;
			}
			else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		break;
	default:
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
	case ARCHER_CHMP:
		break;
	case WIZARD_CHMP:
		break;
	default:
		break;
	}
}

// Barrack Panel ---------------------------------------------------------------------------------------------------------

BarrackPanel::BarrackPanel() : Action_Panel_Elements(){}

bool BarrackPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 0: {
		if (player_game_panel_resources->UseResource(panel_icons[i].wood_cost,panel_icons[i].food_cost, panel_icons[i].gold_cost,
			panel_icons[i].stone_cost, panel_icons[i].population_cost))
		{
			entitis_panel->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)entitis_panel, MILITIA, ALLY));
			App->sound->PlayGUIAudio(CLICK_INGAME);
		}
		else App->sound->PlayGUIAudio(ERROR_SOUND);
		}
		break;
	default:
		break;
	}
	return false;
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
		buildingthis->SetPosition(x - App->render->camera.x, y - App->render->camera.y, false);
	}
}

void VillagerPanel::ResetPanel()
{
	villagerisbuilding = VP_NOT_BUILDING;
	SetDataFromXML();
	isbuilding = false;
	buildingthis = nullptr;
}

bool VillagerPanel::ActivateCell(int i)
{
	switch (i)
	{
	case 0:{
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
		else if (villagerisbuilding == VP_MILITARY && player_game_panel_resources->UseResource(panel_icons[i].wood_cost,
			panel_icons[i].food_cost,panel_icons[i].gold_cost,panel_icons[i].stone_cost,panel_icons[i].population_cost))
		{
			App->sound->PlayGUIAudio(CLICK_INGAME);
			buildingthis = App->entities_manager->GenerateBuilding(BARRACK,ALLY,true);
			int x = 0, y = 0;
			App->input->GetMousePosition(x, y);
			buildingthis->SetPosition(x - App->render->camera.x, y - App->render->camera.y, false);
			isbuilding = true;
			return true;
		}
		}
		break;
	case 1: {
		if (villagerisbuilding == VP_NOT_BUILDING){
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
				buildingthis->SetPosition(x - App->render->camera.x, y - App->render->camera.y, true);
			}
			else {
				App->entities_manager->DeleteEntity(buildingthis);
				player_game_panel_resources->AddResource(175, GP_STONE);
			}
			isbuilding = false;
			buildingthis = nullptr;
			return false;
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

	mele_champion.reserve(MAX_SKILLS_LEARNED);
	mele_champion.push_back({ 651,38, 36, 36 });
	mele_champion.push_back({ 651,0,36,36 });
	mele_champion.push_back({ 651,114,36,36 });
	mele_champion.push_back({ 651,76,36,36 });
	// must Change
	mele_champion.push_back({ 576,477, 36, 36 });
	mele_champion.push_back({ 612,477,36,36 });

	skills.reserve(MAX_SKILLS_LEARNED);
	skills_buttons.reserve(MAX_SKILLS_LEARNED);
	for (int i = 0; i < MAX_SKILLS_LEARNED; i++)
	{
		skills.push_back((UI_Image*)App->gui->GenerateUI_Element(IMG));
		skills[i]->SetBox({ 0, 0, 67, 61 });
		skills[i]->ChangeTextureId(ICONS);
		skills[i]->ChangeTextureRect(mele_champion[i]);
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
}

HeroPanel::~HeroPanel()
{
	skills.clear();
	skills_buttons.clear();
}

void HeroPanel::ResetPanel()
{
	entitis_panel = nullptr;
	champion_selected = NO_UNIT;
	
	skill_tree->Desactivate();
	skill_tree->DesactivateChids();
	
	for (int i = 0; i < MAX_SKILLS_LEARNED; i++)
	{
		skills_buttons[i]->button_state = UP;
	}
	SetDataFromXML();

	activate_skill = -1;
}

bool HeroPanel::ActivateCell(int i)
{
	if (entitis_panel == nullptr) return false;
	switch (i)
	{
	case 0: {
		if (champion_selected == WARRIOR_CHMP && cell_lvl[i] != 0)
		{
			App->sound->PlayGUIAudio(CLICK_INGAME);
			((Warrior*)entitis_panel)->Hability_A();
			return false;
		}
		}
		break;
	case 1: 
		{
		if (champion_selected == WARRIOR_CHMP && cell_lvl[i] != 0)
		{
			activate_skill = 1;	
			App->sound->PlayGUIAudio(CLICK_INGAME);
			((Warrior*)entitis_panel)->PrepareAbility_B();
			return true;
		}
		}
		break;
	case 2:
		break;
	case 3: {
		if (skill_tree->GetActiveState() == true)
		{
			App->sound->PlayGUIAudio(CLICK_INGAME);
			skill_tree->Desactivate();
			skill_tree->DesactivateChids();
		}
		else {
			App->sound->PlayGUIAudio(CLICK_INGAME);
			skill_tree->Activate();
			skill_tree->ActivateChilds();
		}
		}
		break;
	case 4:
		{
		App->sound->PlayGUIAudio(CLICK_INGAME);
		entitis_panel->SetLife(0);
		((Unit*)entitis_panel)->AddPriorizedAction(App->action_manager->DieAction((Unit*)entitis_panel));
		}
	default:	break;
	}
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
					if (cell_lvl[i / 2] != 0)return false;
					LearnSkill(i);
					App->sound->PlayGUIAudio(CLICK_INGAME);
					if (i / 2 == 0)((Champion*)entitis_panel)->SetAbility_A((i % 2));
					if (i / 2 == 1)((Champion*)entitis_panel)->SetAbility_B((i % 2));
					return true;
				}
			}
		}
		break;
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
		
		if (activate_skill == 1)((Warrior*)entitis_panel)->Hability_B(x, y);
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
	if (champion_selected == WARRIOR_CHMP)
	{
		if (i / 2 != 2) // can't learn skill 3 yet
		{
			cell_lvl[i / 2] = (i % 2)+1;
			UpdateCells();
		}
	}
}

void HeroPanel::ChangePanelTarget(Entity * new_target)
{
	activate_skill = -1;
	entitis_panel = new_target;
	champion_selected = ((Unit*)entitis_panel)->GetUnitType();
	
}

// ACTION PANEL  ================================================================================================

Action_Panel::Action_Panel() : action_rect({ 37, 624, 200, 123 }), panel_pos({ 37,624 }), isin(false)
{
	// Create Panels
	towncenterpanel = new TownCenterPanel();
	barrackpanel = new BarrackPanel();
	unitpanel = new UnitPanel();
	heropanel = new HeroPanel();
	villagerpanel = new VillagerPanel();


	//Load Panels Data from loaded folder
	char* buffer = nullptr;
	int size = App->fs->Load("gui/Hud_Action_Panel_Data.xml", &buffer);
	pugi::xml_document panel_data;
	pugi::xml_parse_result result = panel_data.load_buffer(buffer, size);
	RELEASE_ARRAY(buffer);

	pugi::xml_node unit_node = panel_data.first_child();
	towncenterpanel->LoadPanelFromXML(unit_node.child("Buildings").child("TownCenter"));
	barrackpanel->LoadPanelFromXML(unit_node.child("Buildings").child("Barrack"));
	villagerpanel->LoadPanelFromXML(unit_node.child("Units").child("Villager"));
	unitpanel->LoadPanelFromXML(unit_node.child("Units").child("Unit"));
	heropanel->LoadPanelFromXML(unit_node.child("Units").child("Champion"));

	// Set GUI Elements
	App->gui->SetDefaultInputTarget((j1Module*)App->player);

	action_screen = App->gui->GenerateUI_Element(UNDEFINED);
	action_screen->SetBox(action_rect);
	action_screen->SetLayer(20);
	action_screen->Activate();
	action_screen->SetInputTarget((j1Module*)App->player);

	cell_information = (UI_String*)App->gui->GenerateUI_Element(STRING);
	cell_information->SetBoxPosition(10, 570);
	cell_information->SetString("Potato Information");
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
	delete villagerpanel;
	delete unitpanel;
	delete heropanel;

	return false;
}

void Action_Panel::Enable()
{
	on_action = false;
	
	action_screen->Activate();
	action_screen->DesactivateChids();

	towncenterpanel->ResetPanel();
	barrackpanel->ResetPanel();
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
		break;
	default:
		break;
	}
}

void Action_Panel::ActivateCell(int i)
{
	if (actualpanel != nullptr) actualpanel->ActivateCell(i);
}

bool Action_Panel::Draw()
{
	if (actualpanel == nullptr) return false;
	for (int count = 0; count < MAX_PANEL_CELLS; count++)
	{
		if(panel_cells[count]->GetTextureBox().w > 1) panel_cells[count]->Draw(false);
	}
	if (show_cell_info)
	{
		cell_information->Draw(false);
	}
	return true;
}

void Action_Panel::SetSelectionPanelPointer(Selection_Panel * selection_panel)
{
	player_selection_panel = selection_panel;
}

void Action_Panel::SetGamePanelPointer(Game_Panel * game_panel)
{
	player_game_panel = game_panel;
}

void Action_Panel::SetPanelType()
{
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

	if (actualpanel == heropanel && u_type != WARRIOR_CHMP)
	{
		heropanel->skill_tree->Desactivate();
		heropanel->skill_tree->DesactivateChids();
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
				if (villagerpanel->GetActualEntity() != actual_entity) villagerpanel->ResetPanel();
				villagerpanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = villagerpanel;
			}
			else if (u_type == WARRIOR_CHMP)
			{
				if (actualpanel == heropanel && heropanel->GetActualEntity()!= nullptr)return;
				if (heropanel->GetActualEntity() != actual_entity) heropanel->ResetPanel();
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
				if(towncenterpanel->GetActualEntity() != actual_entity || actualpanel != towncenterpanel) App->sound->PlayFXAudio(TOWN_CENTER_SELECTED_SOUND);
				towncenterpanel->ChangePlayerGamePanel(player_game_panel);
				actualpanel = towncenterpanel;
			}
			else if (b_type == BARRACK)
			{
				if (barrackpanel->GetActualEntity() != actual_entity || actualpanel != barrackpanel)App->sound->PlayFXAudio(BARRACK_SOUND);
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
		action_screen->DesactivateChids();
		actualpanel->ChangePanelTarget(actual_entity);
		actualpanel->ChangePanelIcons(panel_cells);
		SetButtons();
	}
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
	if (selected == 0)
	{
		on_action = false;
		actual_entity = nullptr;
		action_screen->DesactivateChids();
		if (actualpanel == heropanel)
		{
			heropanel->skill_tree->Desactivate();
			heropanel->skill_tree->DesactivateChids();
		}
		actualpanel = nullptr;
		return;
	}
	if (actualpanel != nullptr)
	{
		if (actualpanel->GetActualEntity() == nullptr || actualpanel->GetActualEntity()->GetLife() == 0)
		{
			actualpanel->ChangePanelTarget(player_selection_panel->GetSelected());
		}
	}
}

void Action_Panel::HeroIsDead(UNIT_TYPE type)
{
	towncenterpanel->ChampionIsDead(type);
	heropanel->ResetPanel();
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
	towncenterpanel->UpgradeCurrentAge(curr_age);
	barrackpanel->UpgradeCurrentAge(curr_age);
	
	villagerpanel->UpgradeCurrentAge(curr_age);
	if (actualpanel != nullptr)
	{
		actualpanel->ChangePanelIcons(panel_cells);
		SetButtons();
	}
}
