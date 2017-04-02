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
	game_hud->SetLayer(1);


	// HUD Panels
	game_panel = new Game_Panel();
	selection_panel = new Selection_Panel();
	action_panel = new Action_Panel();
	action_panel->SetSelectionPanelPointer(selection_panel);

	// Setting Game Panel Resources
	game_panel->AddResource(200, GP_WOOD);
	game_panel->AddResource(200, GP_MEAT);
	game_panel->AddResource(100, GP_GOLD);
	game_panel->AddResource(200, GP_STONE);

	game_panel->IncressPopulation(45, true);

	game_hud->AddChild(selection_panel->GetViewport());
	game_hud->AddChild(action_panel->GetHeroSkillTree());
	action_panel->GetHeroSkillTree()->SetLayer(5);
	game_hud->AddChild(game_panel->GetExitMenu());
	App->gui->PushScreen(game_hud);
	return true;
}

bool j1Player::PreUpdate()
{

	App->gui->CalculateUpperElement(game_hud);

	int x, y;
	App->input->GetMousePosition(x, y);

	selection_panel->PreUpdate();
	action_panel->PreUpdate();

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (selection_panel->GetInViewport())
		{
			selection_panel->Handle_Input(MOUSE_LEFT_BUTTON_DOWN);
			action_panel->SetPanelType();
		}
		if (action_panel->GetIsIn()) action_panel->Handle_Input(MOUSE_LEFT_BUTTON_DOWN);
	}
	
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (selection_panel->GetInViewport()) selection_panel->Handle_Input(MOUSE_RIGHT_BUTTON);		
	}

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
	{
		selection_panel->Handle_Input(UP_ARROW);
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
	{
		selection_panel->Handle_Input(DOWN_ARROW);
	}

	// Action Panel Check Selected
	action_panel->CheckSelected(selection_panel->GetSelectedSize());

	//Building spawn unit
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		if (selection_panel->PointisInViewport(x, y)) selection_panel->Handle_Input(ENTER);
	}
	//Generate a town center in the mouse coordinates
	if(App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		Building* center = App->entities_manager->GenerateBuilding(BUILDING_TYPE::TOWN_CENTER,ALLY);
		center->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		center->SetDiplomacy(ALLY);
		
		game_panel->IncressPopulation(15, true);
	}
	//Generate Villager in the mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(VILLAGER, ALLY);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
		
		game_panel->IncressPopulation(1, false);
	}

	//Generate a Militia unit in the mouse coordinates
	if(App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(MILITIA, ENEMY);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);

		//game_panel->IncressPopulation(1, false);
	}
	//Generate a Arbalest unit in the mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(ARBALEST, ALLY);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);

		game_panel->IncressPopulation(1, false);
	}
	//Generate Tree resource in mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Entity* new_unit = App->entities_manager->GenerateResource(TREE);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
	}
	//Generate BerryBush resource in mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Entity* new_unit = App->entities_manager->GenerateResource(BERRY_BUSH);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
	}
	//Generate Stone Ore resource in mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Entity* new_unit = App->entities_manager->GenerateResource(STONE_ORE);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
	}
	//Generate Gold Ore resource in mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Entity* new_unit = App->entities_manager->GenerateResource(GOLD_ORE);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
	}

	//Generate Warrior unit in mouse coordinates
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && game_panel->CheckPopulation())
	{
		Unit* new_unit = App->entities_manager->GenerateUnit(WARRIOR_CHMP, ALLY);
		new_unit->SetPosition(x - App->render->camera.x, y - App->render->camera.y);
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
		action_panel->SetPanelType();
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
			action_panel->Handle_Input(target, MOUSE_LEFT_BUTTON_DOWN);
	}
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:

		break;
	case MOUSE_RIGHT_BUTTON:
		break;
	case MOUSE_IN: if (App->gui->upper_element == selection_panel->GetViewport()->GetLayer() && selection_panel->GetInViewport() != true)
	{
		selection_panel->Handle_Input(target, MOUSE_IN);
	}
				   break;
	case MOUSE_OUT:  if (App->gui->upper_element != selection_panel->GetViewport()->GetLayer() && selection_panel->GetInViewport() == true)
	{
		selection_panel->Handle_Input(selection_panel->GetViewport(), MOUSE_OUT);
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
