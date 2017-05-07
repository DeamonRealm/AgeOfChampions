#include "j1InputManager.h"


#include "j1App.h"



#include "j1Render.h"

#include "j1Input.h"
#include "j1Window.h"
#include "j1Map.h"



// Constructors =================================
j1InputManager::j1InputManager()
{
	name = "input_manager";
}

// Destructors ==================================
j1InputManager::~j1InputManager()
{

}


//Game Loop =====================================
// Called when before render is available
bool j1InputManager::Awake(pugi::xml_node& config)
{
	//Node where first key is located
	pugi::xml_node key_node = config.child("key");

	while (key_node != NULL)
	{
		//Get the current key node scancode id
		SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
		scancode = SDL_GetScancodeFromName(key_node.attribute("id").as_string());

		if (scancode != SDL_SCANCODE_UNKNOWN)
		{
			//Map pair where new key is allocated
			std::pair<int, INPUT_EVENT> new_key;

			//Build pair with the loaded data
			new_key.first = scancode;
			new_key.second = StrToInputEvent(key_node.attribute("event").as_string());
			if (new_key.second == 0)
			{
				LOG("Error Loading Input Key!");
			}
			else
			{
				this->events_map.insert(new_key);
			}
		}

		//Focus the next key node
		key_node = key_node.next_sibling();
	}

	return true;

	/*
	for (pugi::xml_node path = config.child("key"); path; path = path.next_sibling("key"))

	{

		int scancode = 0;

		if (path.attribute("forcedID").as_int() == 1)

		{

			scancode = path.attribute("id").as_int();

		}

		else

		{

			scancode = SDL_GetScancodeFromName(path.attribute("id").as_string());

		}

		if (scancode != SDL_SCANCODE_UNKNOWN)

		{

			std::pair<int, e_events> toPush;

			toPush.first = scancode;

			toPush.second = static_cast<e_events>(path.attribute("event").as_int());



			eventsList.insert(toPush);

		}

	}
	*/



}



// Call before first frame
/*
bool M_InputManager::Start()

{

	App->win->GetWindowSize(&screenSize.x, &screenSize.y);

	LOG("INPUT size %d %d", screenSize.x, screenSize.y);

	clickedUI = false;

	hoveringUI = false;



	return true;

}*/



// Called before all Updates

/*bool M_InputManager::PreUpdate()

{

	return true;

}
*/


/*bool M_InputManager::Update(float dt)

{

	//TO CHANGE: Is this going to be something useful? We can remove render.h otherwise

	int x, y;

	App->input->GetMousePosition(x, y);

	mouseScreen = { x, y };

	mouseWorld = App->render->ScreenToWorld(x, y);



	return true;

}
*/


// Called after all Updates
bool j1InputManager::PostUpdate()
{
	current_events.clear();
	return true;
}

// Called before quitting
bool j1InputManager::CleanUp()
{
	current_events.clear();
	events_map.clear();
	return true;
}


//Functionality ==================================
INPUT_EVENT j1InputManager::StrToInputEvent(const char * str) const
{
	if (strcmp(str, "enti_debug") == 0)			return INPUT_EVENT::ENTITY_DEBUG_MODE;
	if (strcmp(str, "map_debug") == 0)			return INPUT_EVENT::MAP_DEBUG_MODE;
	if (strcmp(str, "generate_villager") == 0)	return INPUT_EVENT::GENERATE_VILLAGER;
	if (strcmp(str, "generate_tree") == 0)		return INPUT_EVENT::GENERATE_TREE;
	return UNKNOWN_INPUT;
}

void j1InputManager::SendInputEvent(int id, INPUT_STATE state)
{
	//Search the event in the map
	std::map<int, INPUT_EVENT>::const_iterator key_event = events_map.find(id);

	
	while (key_event != events_map.end())
	{

		//If the event is found we add it to the current events map
		if (key_event->first == id)
		{
			std::pair<INPUT_EVENT, INPUT_STATE> key_pressed;
			key_pressed.first = key_event->second;
			key_pressed.second = state;
			current_events.insert(key_pressed);
		}
		key_event++;
	}
}


/*
void M_InputManager::SendMouseEvent(int button, e_eventState state)

{

	std::pair<e_events, e_eventState> toPush;

	switch (button)

	{

	case SDL_BUTTON_LEFT:

	{

		toPush.first = E_LEFT_CLICK; break;

	}

	case SDL_BUTTON_RIGHT:

	{

		toPush.first = E_RIGHT_CLICK; break;

	}

	case SDL_BUTTON_MIDDLE:

	{

		toPush.first = E_MID_CLICK; break;

	}

	default:

	{

		return;

	}

	}



	toPush.second = state;

	currentEvents.insert(toPush);

}



void M_InputManager::EraseEvent(e_events eventToErase)

{

	if (currentEvents.empty() == false)

	{

		std::map<e_events, e_eventState>::iterator tmp = currentEvents.find(eventToErase);

		if (tmp != currentEvents.end())

		{

			currentEvents.erase(tmp);

		}

	}

}
*/


INPUT_STATE j1InputManager::GetEvent(INPUT_EVENT _event)
{
	if (!current_events.empty())
	{
		std::map<INPUT_EVENT, INPUT_STATE>::iterator item = current_events.find(_event);
		if (item != current_events.end())
		{
			return item->second;
		}
	}
	return INPUT_NONE;
}



/*const char* M_InputManager::GetEventKeyName(e_events _event)

{

	if (eventsList.empty() == false)

	{

		std::map<int, e_events>::iterator tmp = eventsList.begin();

		while (tmp != eventsList.end())

		{

			if (tmp->second == _event)

			{

				return SDL_GetScancodeName(static_cast<SDL_Scancode>(tmp->first));

			}

			tmp++;

		}

	}

	return "";

}



SDL_Scancode M_InputManager::GetEventKey(e_events _event)

{

	if (eventsList.empty() == false)

	{

		std::map<int, e_events>::iterator tmp = eventsList.begin();

		while (tmp != eventsList.end())

		{

			if (tmp->second == _event)

			{

				return static_cast<SDL_Scancode>(tmp->first);

			}

			tmp++;

		}

	}

	return SDL_SCANCODE_UNKNOWN;

}



void M_InputManager::SetEventToNextKeyPress(e_events _event)

{

	savingNextKey = true;

	savingEventToChange = _event;

}



bool M_InputManager::SetEventKey(e_events _event, SDL_Scancode key)

{

	if (eventsList.empty() == false)

	{

		std::map<int, e_events>::iterator tmp = eventsList.begin();

		while (tmp != eventsList.end())

		{

			if (tmp->second == _event)

			{

				std::pair<int, e_events> toPush;

				toPush.first = key;

				toPush.second = _event;



				eventsList.erase(tmp);



				eventsList.insert(toPush);



				std::pair<e_events, e_eventState> notification;

				notification.first = E_UPDATED_KEY;

				notification.second = EVENT_DOWN;

				currentEvents.insert(notification);



				LOG("Succesfully assigned %s key to event %i", SDL_GetScancodeName(key), _event);

				return true;

			}

			tmp++;

		}



		std::pair<int, e_events> toPush;

		toPush.first = key;

		toPush.second = _event;

		eventsList.insert(toPush);



		std::pair<e_events, e_eventState> notification;

		notification.first = E_UPDATED_KEY;

		notification.second = EVENT_DOWN;

		currentEvents.insert(notification);

		LOG("Created new event %s to %i", SDL_GetScancodeName(key), _event);

		return true;

	}

	return false;

}



void M_InputManager::UnfreezeInput()

{

	App->input->UnFreezeInput();

}



void M_InputManager::FreezeInput()

{

	App->input->FreezeInput();

}





bool M_InputManager::IsInputFrozen()

{

	return App->input->GetInputState();

}



iPoint M_InputManager::GetMouseMotion()

{

	iPoint  tmp;

	App->input->GetMouseMotion(tmp.x, tmp.y);

	return tmp;

}



iPoint M_InputManager::GetMapSize()

{

	return{ App->map->data.width * App->map->data.tile_width, App->map->data.height * App->map->data.tile_height };

}

iPoint M_InputManager::GetMapSizeScaled()

{

	iPoint ret = GetMapSize();

	ret.x *= GetScale();

	ret.y *= GetScale();

	return ret;

}



int M_InputManager::GetScale()

{

	return App->win->GetScale();

}





int M_InputManager::GetRealScale()

{

	return App->win->GetRealScale();

}



void M_InputManager::SetScale(uint scale)

{

	App->win->SetScale(scale);

}



void M_InputManager::EnableCursorImage(bool enabled)

{

	if (enabled)

	{

		App->input->EnableCursorImage();

	}

	else

	{

		App->input->DisableCursorImage();

	}

}*/
