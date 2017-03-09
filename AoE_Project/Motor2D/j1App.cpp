#define _CRT_SECURE_NO_WARNINGS

#include <iostream>		
#include <sstream> 
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Menu.h"
#include "j1FileSystem.h"
#include "j1Map.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "j1Console.h"
#include "j1Animator.h"
#include "j1EntitiesManager.h"
#include "j1App.h"
#include "j1Player.h"
#include "j1Pathfinding.h"


// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{

	PERF_START(ptimer);

	input = new j1Input();
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
	console = new j1Console();
	animator = new j1Animator();
	entities_manager = new j1EntitiesManager();
	player = new j1Player();
	pathfinding = new j1Pathfinding();

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
	AddModule(entities_manager);
	AddModule(player);

	// scene last
	AddModule(scene);
	AddModule(menu);


	AddModule(console);

	// render last to swap buffer
	AddModule(render);

	

	PERF_PEEK(ptimer);

}

// Destructor
j1App::~j1App()
{
	// release modules
	for(std::list<j1Module*>::iterator item = modules.end(); item != modules.end(); item--)
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
	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title = app_config.child("title").child_value();
		organization = app_config.child("organization").child_value();

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		if(cap > 0)
		{
			capped_ms = 1000 / cap;
		}
	}

	//Call modules Awake method
	if(ret == true)
	{
		for(std::list<j1Module*>::iterator item = modules.begin(); item != modules.end() && ret == true; item++)
		{
			ret = item._Ptr->_Myval->Awake(config.child(item._Ptr->_Myval->name.c_str()));
			modules.end();
		}
	}

	//Load Cvars 
	pugi::xml_node module_node = app_config;
	while (module_node != NULL)
	{

		pugi::xml_node cvar = module_node.child("c_vars").first_child();
		while (cvar != NULL)
		{
			//Load CVar data
			std::string name = cvar.attribute("name").value();
			std::string description = cvar.attribute("description").value();
			std::string value = cvar.attribute("value").value();
			std::string type = cvar.attribute("type").value();
			C_VAR_TYPE cv_type = App->console->StringtoCvarType(&type);
			std::string module = module_node.name();
			j1Module* cv_module = App->GetModule(&module);

			bool only_read = cvar.attribute("only_read").as_bool();
			
			//Build CVar
			Cvar* cv = App->console->LoadCvar(name.c_str(), description.c_str(), value.c_str(), cv_type, cv_module, only_read);

			LOG("-- %s -- CVar added at %s", cv->GetCvarName()->c_str(), module.c_str());
			
			//Next cvar
			cvar = cvar.next_sibling();
		}

		//Next module
		module_node = module_node.next_sibling();
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

	while(item != modules.end() != NULL && ret == true)
	{
		ret = item._Ptr->_Myval->Start();
		item++;
	}
	startup_time.Start();

	PERF_PEEK(ptimer);

	//Add Console Commands
	console->AddCommand("quit", nullptr);
	console->AddCommand("save", nullptr);
	console->AddCommand("load", nullptr);

	//Add Console Cvars
	save_dir = App->console->AddCvar("save_dir", "Directory where game data is saved", "game_save.xml", C_VAR_TYPE::CHAR_VAR, nullptr, false);
	load_dir = App->console->AddCvar("load_dir", "Directory from app load data", "game_save.xml", C_VAR_TYPE::CHAR_VAR, nullptr, false);


	//Desactivate Console 
	console->active = false;

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	//Activate/Deactivate debug mode
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		debug_mode = !debug_mode;
	}

	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if (ret == true)
	{
		BROFILER_CATEGORY("PrepareUpdate", Profiler::Color::Aqua);
		BROFILER_FRAME("PreUpdate");
		ret = PreUpdate();
	}

	if (ret == true)
	{
		BROFILER_CATEGORY("PrepareUpdate", Profiler::Color::Coral);
		BROFILER_FRAME("DoUpdate");
		ret = DoUpdate();
	}

	if (ret == true)
	{
		BROFILER_CATEGORY("PrepareUpdate", Profiler::Color::Green);
		BROFILER_FRAME("PostUpdate");
		ret = PostUpdate();
	}

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load("config.xml", &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE(buf);

	if(result == NULL)
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
	if(want_to_save == true)
		SavegameNow();

	if(want_to_load == true)
		LoadGameNow();

	// Framerate calculations --

	if(last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[256];
	sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %u Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %lu ",
			  avg_fps, (unsigned long)last_frame_ms, frames_on_last_update, dt, seconds_since_startup, frame_count);
	App->win->SetTitle(title);

	if(capped_ms > 0 && last_frame_ms < capped_ms)
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

	while(item != modules.end())
	{
		if(item._Ptr->_Myval->active) ret = item._Ptr->_Myval->PreUpdate();

		item++;
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.begin();

	while(item != modules.end())
	{
		if(item._Ptr->_Myval->active)ret = item._Ptr->_Myval->Update(dt);

		item++;
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.begin();

	while(item != modules.end() && ret)
	{
		if(item._Ptr->_Myval->active)ret = item._Ptr->_Myval->PostUpdate();

		item++;
	}

	if(ret)ret = !want_to_quit;

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	PERF_START(ptimer);
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.end();
	item--;

	while(item != modules.begin() && ret == true)
	{
		ret = item._Ptr->_Myval->CleanUp();
		item--;
	}


	saved_games.clear();
	

	PERF_PEEK(ptimer);
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
	if(index < argc)
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
void j1App::LoadGame(const char* file)
{
	bool ret = false;
	std::list<std::string*>::const_iterator item = saved_games.begin();

	while (item != saved_games.end())
	{
		if (*item._Ptr->_Myval == file)
		{
			ret = true;
			break;
		}
		item++;
	}
	if (ret)
	{
		want_to_load = true;
		load_game = std::string(fs->GetSaveDirectory()) + std::string(file);
	}
	else LOG("Load Directory is no available!");
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	std::list<std::string*>::const_iterator item = saved_games.begin();
	bool exist = false;
	while (item != saved_games.end())
	{
		if (*item._Ptr->_Myval == file)exist = true;
		item++;
	}
	if (!exist)
	{
		std::string* new_file_str = new std::string(file);
		saved_games.push_back(new_file_str);

	}

	want_to_save = true;
	save_game = file;
}

// ---------------------------------------
void j1App::GetSaveGames(std::list<std::string*>& list_to_fill) const
{
	list_to_fill = saved_games;
}

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

	if(size > 0)
	{
		pugi::xml_document data;
		pugi::xml_node root;

		pugi::xml_parse_result result = data.load_buffer(buffer, size);
		RELEASE(buffer);

		if(result != NULL)
		{
			LOG("Loading new Game State from %s...", load_game.c_str());

			root = data.child("game_state");

			std::list<j1Module*>::const_iterator item = modules.begin();
			ret = true;

			while(item != modules.end() && ret == true)
			{
				ret = item._Ptr->_Myval->Load(root.child(item._Ptr->_Myval->name.c_str()));
				item++;
			}

			data.reset();
			if(ret == true)
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

	while(item != modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->Save(root.append_child(item._Ptr->_Myval->name.c_str()));
		item++;
	}

	if(ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		fs->Save(save_game.c_str(), stream.str().c_str(), stream.str().length());
		LOG("... finished saving", save_game.c_str());
	}
	else
		LOG("Save process halted from an error in module %s", (item._Ptr->_Myval != NULL) ? item._Ptr->_Myval->name.c_str() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
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

void j1App::Console_Command_Input(Command * command, Cvar * cvar, std::string * input)
{
	if (*command->GetCommandStr() == "quit")
	{
		SetQuit();
	}
	else if (*command->GetCommandStr() == "save")
	{
		SaveGame(save_dir->GetValueString()->c_str());
		
	}
	else if (*command->GetCommandStr() == "load")
	{
		LoadGame(load_dir->GetValueString()->c_str());
	}
}

void j1App::Console_Cvar_Input(Cvar * cvar, Command* command_type, std::string * input)
{
	//Set command
	if (*command_type->GetCommandStr() == "set")
	{
		//Maxfps cvar
		if (*cvar->GetCvarName() == "maxfps")
		{
			if (cvar->GetValueAsNum() < 1)cvar->SetValue("-1");
			else if (cvar->GetValueAsNum() > 120)cvar->SetValue("120");

			//Set cvar value
			cvar->SetValue(input->c_str());

			//Set new ms delay
			capped_ms = (int)(1000 / cvar->GetValueAsNum());

		}
		//Save_dir cvar
		else if (*cvar->GetCvarName() == "save_dir")
		{
			if (strlen(input->c_str()) > 5 && IsXMLdir(input->c_str()))
			{
				//Set new save directory
				App->save_game = input->c_str();
				
				//Set cvar value
				cvar->SetValue(input->c_str());
			}
			else App->console->GenerateConsoleLabel("Invalid Save Directory: %s", input->c_str(),cvar->GetCvarName()->c_str());

		}

		else if (*cvar->GetCvarName() == "load_dir")
		{
			if (strlen(input->c_str()) > 5 && IsXMLdir(input->c_str()))
			{
				//Set new save directory
				App->load_game = input->c_str();

				//Set cvar value
				cvar->SetValue(input->c_str());
			}
			else App->console->GenerateConsoleLabel("Invalid Load Directory: %s", input->c_str(), cvar->GetCvarName()->c_str());

		}
		//Unknown cvar
		else
		{
			App->console->GenerateConsoleLabel("Cvar id Error at module Render");
			return;
		}

	}
}

void j1App::SetQuit()
{
	want_to_quit = true;
}


