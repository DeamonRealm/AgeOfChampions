#ifndef __j1APP_H__
#define __j1APP_H__

#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <vector>

#define TIME_TO_ENABLE 10

struct SDL_Texture;

// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1FileSystem;
class j1Scene;
class j1Menu;
class j1Map;
class j1Fonts;
class j1Gui;
class j1Module;
class j1Animator;
class j1SoundManager;
class j1GroupMovement;
class j1EntitiesManager;
class j1BuffManager;
class j1ActionManager;
class j1Player;
class j1AI;
class j1Pathfinding;
class j1FogOfWar;
class j1InputManager;
class j1Video;
class j1Intro;

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	float GetDT() const;

	void LoadGame(const char* file, bool activate_modules = true);
	void SaveGame(const char* file);

	bool IsXMLdir(const char* dir)const;

private:

	// Enable the current active modules
	void EnableActiveModulesNow();

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow();

public:

	// Load the default game
	bool LoadDefaultGame(const char* folder_str);
	void LoadDefaultGameNow();
	void EnableActiveModules();

public:

	// Modules
	j1Window*			win = NULL;
	j1Input*			input = NULL;
	j1InputManager*		input_manager = NULL;
	j1Render*			render = NULL;
	j1Textures*			tex = NULL;
	j1Audio*			audio = NULL;
	j1Scene*			scene = NULL;
	j1Menu*				menu = NULL;
	j1FileSystem*		fs = NULL;
	j1Map*				map = NULL;
	j1Fonts*			font = NULL;
	j1Gui*				gui = NULL;
	j1Animator*			animator = NULL;
	j1SoundManager*		sound = NULL;
	j1GroupMovement*	group_move = NULL;
	j1EntitiesManager*	entities_manager = NULL;
	j1BuffManager*		buff_manager = NULL;
	j1ActionManager*	action_manager = NULL;
	j1Player*			player = NULL;
	j1AI*				AI = NULL;
	j1Pathfinding*		pathfinding = NULL;
	j1FogOfWar*			fog_of_war = NULL;
	j1Video*			video = NULL;
	j1Intro*			intro = NULL;

	// Add a new module to handle
	void		AddModule(j1Module* module);
	//Get Module pointer by name
	j1Module*	GetModule(const  std::string* module_name);
	//Get number of modules
	uint		GetModulesNum()const;
	//Get module of position x in array
	j1Module*	GetModuleAt(uint index)const;

public:

	//Get config xml file
	pugi::xml_node GetConfigXML()const;

private:

	pugi::xml_node config_node;

	std::list<j1Module*>	modules;
	std::vector<j1Module*>	modules_to_enable;
	uint enable_index = 0;

	int						argc = 0;
	char**					args = nullptr;

	std::string				title;
	std::string				organization;

public:

	mutable bool			want_to_save = false;	/*True when save function called*/
	bool					want_to_load = false;	/*True when load function called*/
	bool					want_to_load_default = false;
	std::string				load_game;	/*Folder where the game is loaded*/
	std::string				save_game;	/*Folder where the game is saved*/

private:

	j1PerfTimer			ptimer;
	uint64				frame_count = 0;
	j1Timer				startup_time;
	j1Timer				frame_time;
	j1Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	float				dt = 0.0f;
	int					capped_ms = -1;

	bool				want_to_quit = false;
	bool				want_to_enable = false;
	SDL_Texture*		load_screen = nullptr;

public:

	void SetQuit();
	bool GetQuit()const;

	bool	debug_mode = false;
	bool	map_debug_mode = false;
	
	bool	paused = false;

};

extern j1App* App;

#endif