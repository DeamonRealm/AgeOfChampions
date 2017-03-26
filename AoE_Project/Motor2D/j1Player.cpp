#include "j1Player.h"

#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Animator.h"

#include "j1App.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "j1EntitiesManager.h"

#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_String.h"

//Hud Elements
#include "Hud_SelectionPanel.h"
#include "Hud_GamePanel.h"
#include "Hud_ActionPanel.h"

//j1Player Constructor ============================================================
j1Player::j1Player()
{
	name = "player";
}

//j1Player Destructor ============================================================
j1Player::~j1Player()
{
}

bool j1Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player");
	bool ret = true;


	return ret;
}

bool j1Player::Start()
{
	// Hud Background
	game_hud = (UI_Image*) App->gui->GenerateUI_Element(IMG);
	game_hud->ChangeTextureId(HUD);
	game_hud->SetLayer(10);
	App->gui->PushScreen(game_hud);

	// HUD Panels
	game_panel = new Game_Panel();
	selection_panel = new Selection_Panel();
	action_panel = new Action_Panel();

	// Setting Game Panel Resources
	game_panel->AddResource(200, GP_WOOD);
	game_panel->AddResource(200, GP_MEAT);
	game_panel->AddResource(100, GP_GOLD);
	game_panel->AddResource(200, GP_STONE);

	game_panel->IncressPopulation(45, true);

	// Entities Build ------------------------------------- only for testing recolection
	tree = App->entities_manager->GenerateResource(RESOURCE_TYPE::TREE);
	tree->SetPosition(130, 650);
	
	berry_bush = App->entities_manager->GenerateResource(RESOURCE_TYPE::BERRY_BUSH);
	berry_bush->SetPosition(440, 380);

	gold_ore = App->entities_manager->GenerateResource(RESOURCE_TYPE::GOLD_ORE);
	gold_ore->SetPosition(300, 480);

	stone_ore = App->entities_manager->GenerateResource(RESOURCE_TYPE::STONE_ORE);
	stone_ore->SetPosition(200, 480);

	return true;
}

bool j1Player::PreUpdate()
{
	int x, y;
	App->input->GetMousePosition(x, y);

	selection_panel->PreUpdate();
	action_panel->PreUpdate();

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (selection_panel->PointisInViewport(x, y))
		{
			selection_panel->Handle_Input(MOUSE_LEFT_BUTTON_DOWN);
			action_panel->SetPanelType(selection_panel->GetSelected());
		}
		if (action_panel->GetIsIn()) action_panel->Handle_Input(MOUSE_LEFT_BUTTON_DOWN);
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (selection_panel->PointisInViewport(x, y)) selection_panel->Handle_Input(MOUSE_RIGHT_BUTTON);		
	}

	//Generate a town center in the mouse coordinates
	if(App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		Building* center = App->entities_manager->GenerateBuilding(BUILDING_TYPE::TOWN_CENTER);
		center->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		center->SetDiplomacy(ALLY);
		
		game_panel->IncressPopulation(15, true);
	}
	//Generate Villager in the mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(VILLAGER);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		new_unit->SetDiplomacy(ALLY);
		
		game_panel->IncressPopulation(1, false);
	}

	//Generate a Militia unit in the mouse coordinates
	if(App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(MILITIA);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		new_unit->SetDiplomacy(ALLY);
		
		game_panel->IncressPopulation(1, false);
	}
	//Generate a Arbalest unit in the mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(ARBALEST);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		new_unit->SetDiplomacy(ALLY);

		game_panel->IncressPopulation(1, false);
	}

	return true;
}

bool j1Player::PostUpdate()
{
	game_hud->Draw(false);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		selection_panel->Handle_Input(MOUSE_LEFT_BUTTON_REPEAT);
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		selection_panel->Handle_Input(MOUSE_LEFT_BUTTON_UP);
	}

	// Draw Game Panel (HUD)
	game_panel->Draw();

	// Draw Selected Units
	selection_panel->Draw();

	// Draw Action Panel
	action_panel->Draw();

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

	return true;
}

bool j1Player::DoAction(ACTION type)
{
	switch (type)
	{
	case LCLICK: 
		break;
	case RCLICK:
	/*	if (UpperEntity != nullptr)
		{
			if (UpperEntity->GetEntityType() == RESOURCE)
			{
				((Unit*)Selected->GetEntity())->SetInteractionTarget(UpperEntity);
				return true;
			}
			else if (UpperEntity->GetEntityType() == UNIT)
			{
				//((Unit*)Selected->GetEntity())->SetInteractionTarget(UpperEntity);
				return true;
			}
		}*/
		break;
	}
	return false;
}
