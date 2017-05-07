#ifndef _J1_INPUT_MANAGER_H_
#define _J1_INPUT_MANAGER_H_

#include "j1Module.h"
#include <map>

enum INPUT_EVENT
{
	UNKNOWN_INPUT = 0,
	ENTITY_DEBUG_MODE,
	MAP_DEBUG_MODE,
	GENERATE_VILLAGER,
	GENERATE_TREE
};

enum INPUT_STATE
{
	INPUT_NONE = 0,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_REPEAT
};

class j1InputManager : public j1Module
{
public:

	j1InputManager();
	~j1InputManager();

public:

	// Called when before render is available
	bool Awake(pugi::xml_node& );

	/*// Call before first frame
	bool Start();*/

	/*// Called before all Updates
	bool PreUpdate();*/

	/*bool Update(float dt);*/

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:
	
	//Functionality -------------------
	//Used when loading input keys
	INPUT_EVENT StrToInputEvent(const char* str)const;

	void SendInputEvent(int id, INPUT_STATE state);















	/*void SendMouseEvent(int button, e_eventState state);

	void EraseEvent(e_events);
	*/


	INPUT_STATE GetEvent(INPUT_EVENT _event);

	/*const char* GetEventKeyName(e_events _event);

	SDL_Scancode GetEventKey(e_events _event);



	void SetEventToNextKeyPress(e_events _event);

	bool SetEventKey(e_events _event, SDL_Scancode key);



	void UnfreezeInput();

	void FreezeInput();

	bool IsInputFrozen();



	iPoint GetMouseOnScreen() { return mouseScreen; }

	iPoint GetMouseOnWorld() { return mouseWorld; }

	iPoint GetMouseMotion();



	iPoint GetMapSize();

	iPoint GetMapSizeScaled();

	iPoint GetScreenSize() { return{ screenSize.x, screenSize.y }; }

	iPoint GetScreenSizeScale() { return{ screenSize.x / GetScale(), screenSize.y / GetScale() }; }

	int GetScale();

	int GetRealScale();

	void SetScale(uint scale = 2);



	void EnableCursorImage(bool enabled);



	bool clickedUI = false;

	bool hoveringUI = false;*/

private:

	//Map with all the input events mapped
	std::multimap<int, INPUT_EVENT> events_map;

	//Events that are happening during this frame
	std::multimap<INPUT_EVENT, INPUT_STATE> current_events;
	
	/*iPoint mouseScreen;

	iPoint mouseWorld;



	iPoint screenSize;



	bool savingNextKey = false;

	e_events savingEventToChange;*/

};
#endif // _J1_INPUT_MANAGER_H_
