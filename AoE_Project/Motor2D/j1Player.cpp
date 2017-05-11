#include "j1Player.h"

#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1InputManager.h"
#include "j1Animator.h"

#include "j1App.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "j1EntitiesManager.h"

#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_String.h"

#include "Actions_Unit.h"
#include "Actions_Building.h"

//Hud Elements
#include "Hud_SelectionPanel.h"
#include "Hud_GamePanel.h"
#include "Hud_ActionPanel.h"
#include "Hud_MinimapPanel.h"


#include "j1AI.h"

//j1Player Constructor ============================================================
j1Player::j1Player()
{
	name = "player";
}

//j1Player Destructor ============================================================
j1Player::~j1Player()
{
}

void j1Player::Init()
{
	active = false;
	enabled = false;
}

bool j1Player::Enable()
{
	// Setting Game Panel Resources
	game_panel->AddResource(500, GP_WOOD);
	game_panel->AddResource(500, GP_MEAT);
	game_panel->AddResource(500, GP_GOLD);
	game_panel->AddResource(500, GP_STONE);
	game_panel->IncressPopulation(5, true);

	selection_panel->Enable();
	game_panel->Enable();
	action_panel->Enable();
	minimap_panel->Enable();

	return true;
}

void j1Player::Disable()
{
	active = false;
	enabled = false;
	selection_panel->Disable();
	game_panel->Disable();
	action_panel->Disable();
	minimap_panel->Disable();
}

void j1Player::Reset()
{
	game_panel->Reset();
	selection_panel->Reset();
	action_panel->Reset();
	minimap_panel->Reset();
}

bool j1Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player");
	bool ret = true;


	return ret;
}

bool j1Player::Start()
{
	App->gui->SetDefaultInputTarget(this);
	// Hud Background
	game_hud = (UI_Image*) App->gui->GenerateUI_Element(IMG);
	game_hud->ChangeTextureId(HUD);
	game_hud->SetLayer(1);


	// HUD Panels
	game_panel = new Game_Panel();
	selection_panel = new Selection_Panel();
	action_panel = new Action_Panel();
	minimap_panel = new Minimap_Panel();

	action_panel->SetSelectionPanelPointer(selection_panel);
	action_panel->SetGamePanelPointer(game_panel);

	game_hud->AddChild(selection_panel->GetViewport());
	game_hud->AddChild(action_panel->GetHeroSkillTree());
	action_panel->GetHeroSkillTree()->SetLayer(5);
	game_hud->AddChild(action_panel->GetActionScreen());
	game_hud->AddChild(game_panel->GetExitMenu());
	game_hud->AddChild(minimap_panel->minimap_background);
	minimap_panel->minimap_background->SetLayer(20);
	App->gui->PushScreen(game_hud);

	return true;
}

bool j1Player::PreUpdate()
{
	App->gui->CalculateUpperElement(game_hud);
	if (App->paused)return true;
	
	int x, y;
	App->input->GetMousePosition(x, y);

	selection_panel->PreUpdate();
	action_panel->PreUpdate();
	game_panel->PreUpdate();
	minimap_panel->PreUpdate();

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (selection_panel->GetInViewport() && !action_panel->GetOnAction())
		{
			selection_panel->Handle_Input(MOUSE_LEFT_BUTTON_DOWN);
		}
		action_panel->Handle_Input(MOUSE_LEFT_BUTTON_DOWN);
	}
	
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (selection_panel->GetInViewport() && !action_panel->GetOnAction())
		{
			selection_panel->Handle_Input(MOUSE_RIGHT_BUTTON);
		}
		action_panel->Handle_Input(MOUSE_RIGHT_BUTTON);
	}

	if (App->debug_mode)
	{
		//Generate a town center in the mouse coordinates
		if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		{
			Building* center = App->entities_manager->GenerateBuilding(BUILDING_TYPE::TOWN_CENTER, ALLY);
			center->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
			center->SetDiplomacy(ALLY);

			game_panel->IncressPopulation(15, true);
		}

		if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		{
			App->AI->GenerateDebugVillager();
		}
		//Generate Villager in the mouse coordinates
		if (App->input_manager->GetEvent(GENERATE_VILLAGER) == INPUT_DOWN)
		{
			Unit* new_unit = App->entities_manager->GenerateUnit(VILLAGER, ALLY);
			new_unit->SetPosition((float)x - App->render->camera.x,(float) y - App->render->camera.y);

			game_panel->IncressPopulation(1, false);
		}

		//Generate a Militia unit in the mouse coordinates
		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
		{
			Building* test_building = App->entities_manager->GenerateBuilding(BUILDING_TYPE::TOWN_CENTER, ENEMY);
			test_building->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
			
			
			/*Unit* new_unit = App->entities_manager->GenerateUnit(PALADIN, ALLY);
			new_unit->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);*/
		}
		//Generate a Arbalest unit in the mouse coordinates
		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN /*&& game_panel->CheckPopulation()*/)
		{
			Unit* new_unit = App->entities_manager->GenerateUnit(MILITIA, ENEMY);
			new_unit->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
			new_unit->AddAction(App->action_manager->AutoAttackAction(new_unit), TASK_CHANNELS::PASSIVE);
			game_panel->IncressPopulation(1, false);
		}
		//Generate Tree resource in mouse coordinates
		if (App->input_manager->GetEvent(GENERATE_TREE) == INPUT_DOWN)
		{
			Entity* new_unit = App->entities_manager->GenerateResource(TREE);
			new_unit->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
		}
		//Generate BerryBush resource in mouse coordinates
		if (App->input_manager->GetEvent(GENERATE_BUSH) == INPUT_DOWN)
		{
			Entity* new_unit = App->entities_manager->GenerateResource(BERRY_BUSH);
			new_unit->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
		}
		//Generate Stone Ore resource in mouse coordinates
		if (App->input_manager->GetEvent(GENERATE_STONE) == INPUT_DOWN)
		{
			Entity* new_unit = App->entities_manager->GenerateResource(STONE_ORE);
			new_unit->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
		}
		//Generate Gold Ore resource in mouse coordinates
		if (App->input_manager->GetEvent(GENERATE_GOLD) == INPUT_DOWN)
		{
			Entity* new_unit = App->entities_manager->GenerateResource(GOLD_ORE);
			new_unit->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
		}

		//Generate Warrior unit in mouse coordinates
		if (App->input_manager->GetEvent(GENERATE_WARRIOR) == INPUT_DOWN)
		{
			Unit* new_unit = App->entities_manager->GenerateUnit(WARRIOR_CHMP, ALLY);
			new_unit->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
		}
		//Generate Wizard unit in mouse coordinates
		if (App->input_manager->GetEvent(GENERATE_WIZARD) == INPUT_DOWN)
		{
			Unit* new_unit = App->entities_manager->GenerateUnit(WIZARD_CHMP, ALLY);
			new_unit->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
		}

		//Generate Archer unit in mouse coordinates
		if (App->input_manager->GetEvent(GENERATE_HUNTER) == INPUT_DOWN)
		{
			Unit* new_unit = App->entities_manager->GenerateUnit(ARCHER_CHMP, ALLY);
			new_unit->SetPosition((float)x - App->render->camera.x, (float)y - App->render->camera.y);
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
	{
		App->entities_manager->UpgradeUnit(MILITIA, ARBALEST, ALLY);
	}

	// Skills
	if (App->input_manager->GetEvent(SET_HABILITY_A) == INPUT_DOWN)
	{
		action_panel->ActivateCell(0);
	}
	if (App->input_manager->GetEvent(SET_HABILITY_B) == INPUT_DOWN)
	{
		action_panel->ActivateCell(1);
	}
	if (App->input_manager->GetEvent(SET_HABILITY_C) == INPUT_DOWN)
	{
		action_panel->ActivateCell(2);
	}

	return true;
}

bool j1Player::PostUpdate()
{
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_DOWN)
	{
		game_panel->ActivatePauseMenu();
	}

	// Draw Selected Entities Life
	selection_panel->DrawLife();

	game_hud->Draw(false);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if(!action_panel->GetOnAction()) selection_panel->Handle_Input(MOUSE_LEFT_BUTTON_REPEAT);
		action_panel->Handle_Input(MOUSE_LEFT_BUTTON_REPEAT);
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		
		if(!action_panel->GetOnAction()) selection_panel->Handle_Input(MOUSE_LEFT_BUTTON_UP);
		action_panel->Handle_Input(MOUSE_LEFT_BUTTON_UP);
		action_panel->SetPanelType();
	}
	
	// Draw Selected Units
	selection_panel->Draw();

	// Draw Action Panel
	action_panel->Draw();

	// Draw Game Panel (HUD)
	game_panel->Draw();

	// Draw Minimap
	minimap_panel->Draw();

	//Draw Mouse Last one
	if (SDL_ShowCursor(-1) == 0) App->gui->DrawMouseTexture();

	return true;
}

bool j1Player::CleanUp()
{
	//Delete HUD
	delete game_panel;
	delete selection_panel;
	delete action_panel;
	delete minimap_panel;

	return true;
}

bool j1Player::Load(pugi::xml_node & data)
{
	// Player Load ---------------------------
	//Player node
	pugi::xml_node player_node = data.child("player_data");

	if (player_node == NULL) return true;

	game_panel->Load(player_node);
	selection_panel->Load(player_node);
	action_panel->Load(player_node);
	minimap_panel->Load(player_node);

	return true;
}

bool j1Player::Save(pugi::xml_node &data) const
{
	// Player Save ---------------------------
	//Player node
	pugi::xml_node player_node = data.append_child("player_data");

	game_panel->Save(player_node);
	selection_panel->Save(player_node);
	action_panel->Save(player_node);
	minimap_panel->Save(player_node);

	return true;
}

void j1Player::GUI_Input(UI_Element* target, GUI_INPUT input)
{
	game_panel->Handle_Input(target, input);

	switch (input)
	{
	case UP_ARROW:
		break;
	case DOWN_ARROW:
		break;
	case LEFT_ARROW:
		break;
	case RIGHT_ARROW:
		break;
	case MOUSE_LEFT_BUTTON_DOWN:
		{
			minimap_panel->Handle_Input(target, input);
			action_panel->Handle_Input(target, input);
		}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:
		{
			action_panel->Handle_Input(target, input);
		}
		break;
	case MOUSE_RIGHT_BUTTON:
		{
			minimap_panel->Handle_Input(target, input);
		}
		break;
	case MOUSE_IN:
		{
			if (App->gui->upper_element == selection_panel->GetViewport()->GetLayer() && selection_panel->GetInViewport() != true)
			{
				selection_panel->Handle_Input(target, input);
			}
			action_panel->Handle_Input(target, input);
			minimap_panel->Handle_Input(target, input);
		}
		break;
	case MOUSE_OUT: 
		{
			if (App->gui->upper_element != selection_panel->GetViewport()->GetLayer() && selection_panel->GetInViewport() == true)
			{
				selection_panel->Handle_Input(selection_panel->GetViewport(), input);
			}
			action_panel->Handle_Input(target, input);
			minimap_panel->Handle_Input(target, input);
		}
	case SUPR:
		break;
	case BACKSPACE:
		break;
	case ENTER:
		break;
	case TAB:
		break;
	default:
		break;
	}
}

void j1Player::UpgradeCivilization(RESEARCH_TECH type) 
{
	switch (type)
	{
	case NO_TECH:
		break;
	case TC_FEUDAL: {
		action_panel->UpgradeCivilizationAge(2);
		action_panel->UpgradeTecnology(type);
		return;
		}
		break;
	case TC_CASTLE:	{
		action_panel->UpgradeCivilizationAge(3);
		action_panel->UpgradeTecnology(type);
		return;
		}
		break;
	case TC_IMPERIAL: {
		action_panel->UpgradeCivilizationAge(4);
		action_panel->UpgradeTecnology(type);
		return;
		}
		break;
	default: action_panel->UpgradeTecnology(type);
		break;
	}
	selection_panel->UpdateSelected();
}