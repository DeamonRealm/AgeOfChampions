#include "Hud_GamePanel.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1Menu.h"
#include "j1AI.h"
#include "j1EntitiesManager.h"

#include "BaseEntities.h"

//UI
#include "UI_String.h"
#include "UI_Button.h"
#include "UI_Element.h"

// Constructor ================================================================
Game_Panel::Game_Panel() : wood(0), meat(0), gold(0), stone(0), population(0), max_population(5),
wood_width(0), meat_width(0), gold_width(0), stone_width(0), population_width(0)
{
	player_resources.reserve(MAX_GAME_RESOURCES);

	UI_String* resource_text;
	int i = 0;
	while (i < MAX_GAME_RESOURCES)
	{
		resource_text = (UI_String*)App->gui->GenerateUI_Element(STRING);
		resource_text->SetColor({ 255, 255, 255, 255 });
		resource_text->SetBoxPosition(73 + 77 * i, 5);
		player_resources.push_back(resource_text);
		i++;
	}

	App->gui->SetDefaultInputTarget((j1Module*)App->player);

	//Exit Menu 

	exit_menu_screen = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	exit_menu_screen->SetBox({ 0,0,App->win->screen_surface->w, 23});
	exit_menu_screen->SetInputTarget((j1Module*)App->player);
	exit_menu_screen->SetLayer(3);

	exit_menu_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	exit_menu_button->SetBox({ 1306,3,50,17 });
	exit_menu_button->SetTexON({ 250,193, 50,17 }, ICONS);
	exit_menu_button->SetTexOFF({ 200,193, 50, 17 }, ICONS);
	exit_menu_button->SetTexOVER({ 200,193,50,17 }, ICONS);
	exit_menu_screen->AddChild(exit_menu_button);

	exit_menu_image = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	exit_menu_image->SetBox({536,150,243,345});
	exit_menu_image->ChangeTextureId(TEXTURE_ID::MENU_PAUSE);
	exit_menu_image->ChangeTextureRect({ 0,0,294,300 });
	exit_menu_image->Desactivate();
	exit_menu_screen->AddChild(exit_menu_image);

	exit_to_main_menu = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	exit_to_main_menu->SetBox({38, 38, 220, 30});
	exit_to_main_menu->SetTexOFF({295, 0,220,30}, MENU_PAUSE);
	exit_to_main_menu->SetTexOVER({ 295, 0,220,30 }, MENU_PAUSE);
	exit_to_main_menu->SetTexON({295, 31,220,30}, MENU_PAUSE);
	exit_to_main_menu->Desactivate();
	exit_menu_image->AddChild(exit_to_main_menu);

	cancel_game_menu = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	cancel_game_menu->SetBox({ 38, 230, 220, 30 });
	cancel_game_menu->SetTexOFF({ 295, 0,220,30 }, MENU_PAUSE);
	cancel_game_menu->SetTexOVER({ 295, 0,220,30 }, MENU_PAUSE);
	cancel_game_menu->SetTexON({ 295, 31,220,30 }, MENU_PAUSE);
	cancel_game_menu->Desactivate();
	exit_menu_image->AddChild(cancel_game_menu);


	//Final Menu

	final_menu_image = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	final_menu_image->SetBox({ 536,150,227 ,305 });
	final_menu_image->ChangeTextureId(TEXTURE_ID::FINAL_MENU);
	final_menu_image->ChangeTextureRect({ 0,0,227,305 });
	final_menu_image->Desactivate();
	exit_menu_screen->AddChild(final_menu_image);

	// Menu Text
	resource_text = (UI_String*)App->gui->GenerateUI_Element(STRING);
	resource_text->SetColor({ 255, 255, 255, 255 });
	resource_text->SetString("Quit Current Game");
	resource_text->Desactivate();
	resource_text->SetBox({ 58, 6,0,0 });
	exit_to_main_menu->AddChild(resource_text);

	resource_text = (UI_String*)App->gui->GenerateUI_Element(STRING);
	resource_text->SetColor({ 255, 255, 255, 255 });
	resource_text->SetString("Cancel");
	resource_text->Desactivate();
	resource_text->SetBox({ 90, 6,0,0 });
	cancel_game_menu->AddChild(resource_text);

	final_str = (UI_String*)App->gui->GenerateUI_Element(STRING);
	final_str->SetColor({ 0, 0, 0, 255 });
	final_str->SetString("YOU WIN !!!");
	final_str->Activate();
	final_str->SetBox({ 83, 50,0,0 });
	final_menu_image->AddChild(final_str);

	units_dead = (UI_String*)App->gui->GenerateUI_Element(STRING);
	units_dead->SetColor({ 0, 0, 0, 255 });
	units_dead->SetString("Enemy's Killed: ");
	units_dead->Activate();
	units_dead->SetBox({ 65, 70,0,0 });
	final_menu_image->AddChild(units_dead);

	your_units = (UI_String*)App->gui->GenerateUI_Element(STRING);
	your_units->SetColor({ 0, 0, 0, 255 });
	your_units->SetString("Ally Units alive: ");
	your_units->Activate();
	your_units->SetBox({ 60, 90,0,0 });
	final_menu_image->AddChild(your_units);

	exit_menu_screen->Desactivate();
}

// Destructor ==============================================
Game_Panel::~Game_Panel()
{
	CleanUp();
}

// FUNCTIONALITY ===========================================
bool Game_Panel::CleanUp()
{
	hud_game_text.clear();

	player_resources.clear();

	return true;
}

bool Game_Panel::PreUpdate()
{
	if (lose)
	{

		final_menu_image->Activate();
	}


	if (win)
	{
		final_menu_image->Activate();
	}
	return true;
}

bool Game_Panel::PostUpdate()
{
	

	return true;
}

bool Game_Panel::Draw()
{
	for (int i = 0; i < MAX_GAME_RESOURCES; i++)
	{
		player_resources[i]->Draw(false);
	}

	return true;
}

void Game_Panel::Handle_Input(UI_Element * ui_element, GUI_INPUT ui_input)
{
	switch (ui_input)
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
	

		if (ui_element == exit_menu_button)
		{
			final_menu_image->Desactivate();
			final_menu_image->DesactivateChids();
			lose = false;
			win = false;

			if (exit_menu_image->GetActiveState()) {
				exit_menu_image->Desactivate();
				exit_menu_image->DesactivateChids();
			}
			else
			{
				exit_menu_image->Activate();
				exit_menu_image->ActivateChilds();
			}			
		}
	}

	break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		break;
	case MOUSE_LEFT_BUTTON_UP:
	{
		if (ui_element == exit_to_main_menu)
		{
			App->scene->Disable();
			App->player->Disable();
			App->AI->Disable();
			App->entities_manager->Disable();
			App->menu->Enable();
		}
		if (ui_element == cancel_game_menu)
		{
			exit_menu_image->Desactivate();
			exit_menu_image->DesactivateChids();
		}
	}
		break;
	case MOUSE_RIGHT_BUTTON:
		break;
	case MOUSE_IN:
		break;
	case MOUSE_OUT:
		break;
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

void Game_Panel::Enable()
{
	exit_menu_screen->Activate();
	exit_menu_button->Activate();
	final_menu_image->ActivateChilds();
	win = false;
	lose = false;
	game_ended = false;
}

void Game_Panel::Disable()
{
	population = 0;
	wood = 0;
	meat = 0;
	gold = 0;
	stone = 0;
	max_population = 5;
	all_resources = 0;
	player_death_units = 0;
	player_death_enemies = 0;
	player_all_units = 0;

	exit_menu_screen->Desactivate();
	exit_menu_screen->DesactivateChids();
}

bool Game_Panel::AddResource(int amount, PLAYER_RESOURCES resource_type)
{
	bool ret = false;
	
	int height = 0;

	if (amount > 0)
		all_resources += amount;

	switch (resource_type)
	{
	case GP_WOOD:
		if (wood + amount >= 0)
		{
			wood += amount;
			player_resources[resource_type]->SetString(App->gui->SetStringFromInt(wood));
			App->font->CalcSize(player_resources[resource_type]->GetString(), wood_width, height);
			player_resources[resource_type]->SetBoxPosition(73 + 77 * resource_type - wood_width, 5);
			ret = true;
		}
		break;
	case GP_MEAT:
		if (meat + amount >= 0)
		{
			meat += amount;
			player_resources[resource_type]->SetString(App->gui->SetStringFromInt(meat));
			App->font->CalcSize(player_resources[resource_type]->GetString(), meat_width, height);
			player_resources[resource_type]->SetBoxPosition(73 + 77 * resource_type - meat_width, 5);
			ret = true;
		}
		break;
	case GP_GOLD:
		if (gold + amount >= 0)
		{
			gold += amount;
			player_resources[resource_type]->SetString(App->gui->SetStringFromInt(gold));
			App->font->CalcSize(player_resources[resource_type]->GetString(), gold_width, height);
			player_resources[resource_type]->SetBoxPosition(73 + 77 * resource_type - gold_width, 5);
			ret = true;
		}
		break;
	case GP_STONE:
		if (stone + amount >= 0)
		{
			stone += amount;
			player_resources[resource_type]->SetString(App->gui->SetStringFromInt(stone));
			App->font->CalcSize(player_resources[resource_type]->GetString(), stone_width, height);
			player_resources[resource_type]->SetBoxPosition(73 + 77 * resource_type - stone_width, 5);
			ret = true;
		}
		break;
	default:
		break;
	}
	return ret;
}

bool Game_Panel::UseResource(int amount_wood, int amount_food, int amount_gold, int amount_stone, int used_population)
{
	if(wood-amount_wood >= 0 && meat-amount_food >= 0 && gold - amount_gold >= 0 && stone - amount_stone >= 0 &&  population + used_population <= max_population)
	{
		if (amount_wood != 0) AddResource(-amount_wood, GP_WOOD);
		if (amount_food != 0) AddResource(-amount_food, GP_MEAT);
		if (amount_gold != 0) AddResource(-amount_gold, GP_GOLD);
		if (amount_stone != 0) AddResource(-amount_stone, GP_STONE);
		if (used_population != 0) IncressPopulation(used_population);
		return true;
	}
	else return false;
}

bool Game_Panel::IncressPopulation(int amount, bool increase_max)
{
	int height = 0;
	if (increase_max)
	{
		if (max_population + amount <= MAX_POPULATION) max_population += amount;
		else max_population = MAX_POPULATION;
		hud_game_text.clear();
		hud_game_text = App->gui->SetStringFromInt(population);
		hud_game_text = hud_game_text + "/" + App->gui->SetStringFromInt(max_population);
	}
	else
	{
		if (amount > 0)
			IncreaseAllUnits();
		
		if (population + amount <= MAX_POPULATION && population + amount >= 0)
		{
			population += amount;
			hud_game_text.clear();
			hud_game_text = App->gui->SetStringFromInt(population);
			hud_game_text = hud_game_text + "/" + App->gui->SetStringFromInt(max_population);
		}
		else return false;
	}
	player_resources[4]->SetString((char*)hud_game_text.c_str());
	App->font->CalcSize(player_resources[4]->GetString(), population_width, height);
	player_resources[4]->SetBoxPosition(381 - population_width, 5);

	return true;
}

bool Game_Panel::CheckPopulation() const
{
	if (population == max_population) return false;
	else return true;
}

int Game_Panel::GetAllUnitScore()
{
	return player_all_units;
}

int Game_Panel::GetDeathUnitScore()
{
	return player_death_units;
}

int Game_Panel::GetResourceScore()
{
	return all_resources;
}

UI_Element * Game_Panel::GetExitMenu()
{
	return exit_menu_screen;
}

void Game_Panel::IncreaseDeathEnemies()
{
	player_death_enemies++;
}

void Game_Panel::IncreaseDeathAllies()
{
	player_death_units++;
}

void Game_Panel::IncreaseAllUnits()
{
	player_all_units++;
}

void Game_Panel::IncreaseAllResources()
{
	all_resources++;
}

void Game_Panel::DoLose()
{
	if (!game_ended)
	{
		game_ended = true;
		lose = true;
	}
	final_str->SetString("YOU LOSE!!!");

	std::string str = "Enemies Killed ";
	str += App->gui->SetStringFromInt(player_death_enemies);


	units_dead->SetString((char*)str.c_str());
	
	str.clear();
	str = "Ally Units alive ";
	str += App->gui->SetStringFromInt(population);

	your_units->SetString((char*)str.c_str());
	
}

void Game_Panel::DoWin()
{
	if (!game_ended)
	{
		win = true;
	}
	final_str->SetString("YOU WIN!!!");

	std::string str = "Enemies Killed ";
	str = str + App->gui->SetStringFromInt(1+player_death_enemies);

	units_dead->SetString((char*)str.c_str());
	
	str.clear();
	str = "Ally Units alive ";
	str = str + App->gui->SetStringFromInt(population);

	your_units->SetString((char*)str.c_str());
}

