#define _CRT_SECURE_NO_WARNINGS

#include <iostream>		
#include <sstream> 
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1InputManager.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Menu.h"
#include "j1FileSystem.h"
#include "j1Map.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "j1Animator.h"
#include "j1EntitiesManager.h"
#include "j1App.h"
#include "j1Player.h"
#include "j1Pathfinding.h"
#include "j1SoundManager.h"
#include "j1GroupMovement.h"
#include "j1ActionManager.h"
#include "j1BuffManager.h"
#include "j1AI.h"
#include "j1FogOfWar.h"
#include "j1Video.h"
#include "j1Intro.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{

	PERF_START(ptimer);

	input = new j1Input();
	input_manager = new j1InputManager();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	menu = new j1Menu();
	fs = new j1FileSystem();
	map = new j1Map();
	font = new j1Fonts();
	gui = new j1Gui();
	animator = new j1Animator();
	sound = new j1SoundManager();
	group_move = new j1GroupMovement();
	entities_manager = new j1EntitiesManager();
	buff_manager = new j1BuffManager();
	player = new j1Player();
	AI = new j1AI();
	pathfinding = new j1Pathfinding();
	action_manager = new j1ActionManager();
	fog_of_war = new j1FogOfWar();
	video = new j1Video();
	intro = new j1Intro();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(fs);
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(font);
	AddModule(pathfinding);
	AddModule(gui);
	AddModule(animator);
	AddModule(sound);
	AddModule(group_move);
	AddModule(action_manager);
	AddModule(entities_manager);
	AddModule(buff_manager);
	AddModule(fog_of_war);
	AddModule(player);
	AddModule(AI);
	AddModule(video);

	// scene last
	AddModule(scene);
	AddModule(menu);
	AddModule(intro);
	
	// render last to swap buffer
	AddModule(render);
	AddModule(input_manager);

	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	for (std::list<j1Module*>::iterator item = modules.begin(); item != modules.end(); item++)
	{
		RELEASE(item._Ptr->_Myval);
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;

	config = LoadConfig(config_file);

	//Load App config data
	if (config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title = app_config.child("title").child_value();
		organization = app_config.child("organization").child_value();

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		if (cap > 0)
		{
			capped_ms = 1000 / cap;
		}
	}

	//Call modules Awake method
	if (ret == true)
	{
		for (std::list<j1Module*>::iterator item = modules.begin(); item != modules.end() && ret == true; item++)
		{
			ret = item._Ptr->_Myval->Awake(config.child(item._Ptr->_Myval->name.c_str()));
			modules.end();
		}
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(ptimer);
	bool ret = true;
	std::list<j1Module*>::iterator item = modules.begin();

	while (item != modules.end() != NULL && ret == true)
	{
		ret = item._Ptr->_Myval->Start();
		item++;
	}
	startup_time.Start();

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;

	if (want_to_enable)
	{
		EnableActiveModulesNow();
	}
	
	PrepareUpdate();

	if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if (ret == true)
	{
		ret = PreUpdate();
	}

	if (ret == true)
	{
		ret = DoUpdate();
	}

	if (ret == true)
	{
		ret = PostUpdate();
	}

	

	FinishUpdate();
	return ret;
}

void j1App::EnableActiveModulesNow()
{
	if (modules_to_enable[enable_index]->Enable())enable_index++;

	uint size = modules_to_enable.size();

	if (enable_index == size)
	{
		for (uint k = 0; k < size; k++)
		{
			modules_to_enable[k]->enabled = true;
		}
		want_to_enable = false;
		modules_to_enable.clear();
		if(enable_index > 2)App->menu->Disable();
		enable_index = 0;
		
	}
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load("config.xml", &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE_ARRAY(buf);

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if (want_to_save == true && !want_to_enable)
	{
		SavegameNow();
	}

	if (want_to_load == true && !want_to_enable)
	{
		LoadGameNow();
	}

	if (want_to_load_default && !want_to_enable)
	{
		LoadDefaultGameNow();
	}

	// Framerate calculations --

	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[50];

	if (debug_mode || map_debug_mode)
	{
		sprintf_s(title, 50, "Age Of Champions || DT: %u || FPS: %u", last_frame_ms, frames_on_last_update);
	}
	else
	{
		sprintf_s(title, 50, "Age Of Champions", frames_on_last_update);
	}

	App->win->SetTitle(title);

	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		j1PerfTimer t;
		SDL_Delay(capped_ms - last_frame_ms);
	}
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end())
	{
		if (item._Ptr->_Myval->active && item._Ptr->_Myval->enabled) ret = item._Ptr->_Myval->PreUpdate();

		item++;
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end())
	{
		if (item._Ptr->_Myval->active && item._Ptr->_Myval->enabled)ret = item._Ptr->_Myval->Update(dt);

		item++;
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end() && ret)
	{
		if (item._Ptr->_Myval->active && item._Ptr->_Myval->enabled)ret = item._Ptr->_Myval->PostUpdate();

		item++;
	}

	if (ret)ret = !want_to_quit;

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	tex->UnLoad(load_screen);


	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.end();
	item--;

	while (item != modules.begin() && ret == true)
	{
		ret = item._Ptr->_Myval->CleanUp();
		item--;
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.c_str();
}

// ---------------------------------------
float j1App::GetDT() const
{
	return dt;
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.c_str();
}

// Load / Save
void j1App::LoadGame(const char* file,bool activate_modules)
{
	if (activate_modules)
	{
		//Clean all the previous data
		App->map->Disable();
		App->gui->ChangeMouseTexture(DEFAULT);
		App->player->Disable();
		App->animator->Disable();
		App->entities_manager->Disable();
		App->fog_of_war->Disable();
		App->buff_manager->Disable();
		App->AI->Disable();
		App->pathfinding->Disable();
		App->video->Disable();

		//Reactivate all the modules (only say active but data is not loaded in the same loop)
		App->map->Active();
		App->player->Active();
		App->animator->Active();
		App->entities_manager->Active(); /*This load the entities animations textures*/
		App->fog_of_war->Active();
		App->buff_manager->Active(); /*This load the particles textures*/
		App->AI->Active();
		App->action_manager->Active();
		App->pathfinding->Active();
		App->video->Active();
		App->scene->Enable();
		
		want_to_enable = true;

		EnableActiveModules();
	}
	else
	{
		App->pathfinding->Reset();
		App->fog_of_war->Reset();
		App->entities_manager->Reset();
		App->player->Reset();
		App->AI->Reset();
	}

	want_to_load = true;
	load_game = std::string(fs->GetSaveDirectory()) + std::string(file);
}

// ---------------------------------------
void j1App::SaveGame(const char* file)
{
	want_to_save = true;
	save_game = file;
}

// ---------------------------------------

bool j1App::IsXMLdir(const char * dir) const
{
	uint len = strlen(dir);
	if (len < 4)return false;
	bool format_zone = false;
	char* temp = new char[strlen(dir)];
	uint j = 0;
	for (uint k = 0; k < len; k++)
	{
		if (dir[k] == '.')format_zone = true;
		else if (format_zone)
		{
			temp[j] = dir[k];
			j++;
		}

	}
	temp[j] = '\0';
	std::string str = temp;
	delete temp;
	if (str == "xml")return true;

	return false;
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	char* buffer;
	uint size = fs->Load(load_game.c_str(), &buffer);

	if (size > 0)
	{
		pugi::xml_document data;
		pugi::xml_node root;

		pugi::xml_parse_result result = data.load_buffer(buffer, size);
		RELEASE(buffer);

		if (result != NULL)
		{
			LOG("Loading new Game State from %s...", load_game.c_str());

			root = data.child("game_state");

			std::list<j1Module*>::const_iterator item = modules.begin();
			ret = true;

			while (item != modules.end() && ret == true)
			{
				ret = item._Ptr->_Myval->Load(root.child(item._Ptr->_Myval->name.c_str()));
				item++;
			}

			data.reset();
			if (ret == true)
				LOG("...finished loading");
			else
				LOG("...loading process interrupted with error on module %s", (item._Ptr->_Myval != NULL) ? item._Ptr->_Myval->name.c_str() : "unknown");
		}
		else
			LOG("Could not parse game state xml file %s. pugi error: %s", load_game.c_str(), result.description());
	}
	else
		LOG("Could not load game state xml file %s", load_game.c_str());

	want_to_load = false;

	return ret;
}

bool j1App::SavegameNow()
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.c_str());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->Save(root.append_child(item._Ptr->_Myval->name.c_str()));
		item++;
	}

	if (ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		fs->Save(save_game.c_str(), stream.str().c_str(), stream.str().length());
		LOG("... finished saving correctly", save_game.c_str());
	}
	else
		LOG("Save process halted from an error in module %s", (item._Ptr->_Myval != NULL) ? item._Ptr->_Myval->name.c_str() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}

bool j1App::LoadDefaultGame(const char* folder_str)
{
	//Clean all the previous data
	App->map->Disable();
	App->gui->ChangeMouseTexture(DEFAULT);
	App->player->Disable();
	App->animator->Disable();
	App->entities_manager->Disable();
	App->fog_of_war->Disable();
	App->buff_manager->Disable();
	App->AI->Disable();
	App->pathfinding->Disable();
	App->video->Disable();

	//Reactivate all the modules (only say active but data is not loaded in the same loop)
	App->map->Active();
	App->player->Active();
	App->animator->Active();
	App->entities_manager->Active(); /*This load the entities animations textures*/
	App->fog_of_war->Active();
	App->buff_manager->Active(); /*This load the particles textures*/
	App->AI->Active();
	App->pathfinding->Active();
	App->video->Active();

	App->scene->Enable();

	App->EnableActiveModules();

	load_game = folder_str;
	want_to_load_default = true;

	return true;
}

void j1App::LoadDefaultGameNow()
{
	bool ret = true;
	pugi::xml_document scene_doc;
	if (!App->fs->LoadXML(load_game.c_str(), &scene_doc))
	{
		LOG("Error Loading Scene!");
		return;
	}

	if (scene_doc != NULL)
	{

		pugi::xml_node game_node = scene_doc.child("game_state");

		LOG("Loading new Game State from %s...", load_game.c_str());

		std::list<j1Module*>::const_iterator item = modules.begin();
		ret = true;

		while (item != modules.end() && ret == true)
		{
			ret = item._Ptr->_Myval->Load(game_node.child(item._Ptr->_Myval->name.c_str()));
			item++;
		}

		scene_doc.reset();

		if (ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item._Ptr->_Myval != NULL) ? item._Ptr->_Myval->name.c_str() : "unknown");
	}
	else
		LOG("Could not load game state xml file %s", load_game.c_str());

	want_to_load_default = false;
}

void j1App::EnableActiveModules()
{
	std::list<j1Module*>::const_iterator item = modules.begin();
	while (item != modules.end())
	{
		if (item._Ptr->_Myval->active && !item._Ptr->_Myval->enabled) modules_to_enable.push_back(item._Ptr->_Myval);

		item++;
	}

	want_to_enable = true;
}

j1Module * j1App::GetModule(const std::string* module_name)
{
	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end())
	{
		if (*module_name == item._Ptr->_Myval->name.c_str())
		{
			return item._Ptr->_Myval;
		}

		item++;
	}
	return nullptr;
}

uint j1App::GetModulesNum() const
{
	return modules.size();
}

j1Module * j1App::GetModuleAt(uint index)const
{
	std::list<j1Module*>::const_iterator item = modules.begin();

	for (uint k = 0; k < index; k++)
		item++;

	return item._Ptr->_Myval;
}

pugi::xml_node j1App::GetConfigXML() const
{
	return config_node;
}

void j1App::SetQuit()
{
	want_to_quit = true;
}

bool j1App::GetQuit() const
{
	return want_to_quit;
}


