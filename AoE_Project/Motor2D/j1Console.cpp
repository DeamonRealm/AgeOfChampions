#include "j1Console.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"

//UI Elements
#include "UI_Scroll.h"
#include "UI_String.h"
#include "UI_Text_Box.h"

//Constructors ============================================
j1Console::j1Console()
{
	name = "console";
}


//Destructors =============================================
j1Console::~j1Console()
{

}

//Game Loop ===============================================
bool j1Console::Awake(pugi::xml_node& config)
{
	LOG("Console Awake");
	App->gui->SetDefaultInputTarget(this);

	//Load console window size
	console_area.x = config.child("area").attribute("x").as_int();
	console_area.y = config.child("area").attribute("y").as_int();
	console_area.w = config.child("area").attribute("w").as_int();
	console_area.h = config.child("area").attribute("h").as_int();
	//Load console window color
	console_color.r = config.child("area_color").attribute("r").as_int();
	console_color.g = config.child("area_color").attribute("g").as_int();
	console_color.b = config.child("area_color").attribute("b").as_int();
	console_color.a = config.child("area_color").attribute("a").as_int();

	//Load Console Text Box
	SDL_Rect input_box;
	input_box.x = config.child("input_box").attribute("x").as_int();
	input_box.y = config.child("input_box").attribute("y").as_int();
	input_box.w = config.child("input_box").attribute("w").as_int();
	input_box.h = config.child("input_box").attribute("h").as_int();
	
	//Load Console Text Font
	font = App->font->Load(config.child("console_font").attribute("file").as_string(), config.child("console_font").attribute("size").as_int());
	font_color.r = config.child("font_color").attribute("r").as_int();
	font_color.g = config.child("font_color").attribute("g").as_int();
	font_color.b = config.child("font_color").attribute("b").as_int();
	font_color.a = config.child("font_color").attribute("a").as_int();
	
	//Load Text Cursor
	int cursor_w = config.child("cursor").attribute("w").as_int();
	int cursor_h = config.child("cursor").attribute("h").as_int();
	
	LOG("Console Config data Loaded");
	
	
	//Build Console Text
	console_input_box = (UI_Text_Box*)App->gui->GenerateUI_Element(TEXT_BOX);
	console_input_box->SetTextColor({ font_color.r,font_color.g,font_color.b,font_color.a });
	console_input_box->SetTextFont(font);
	console_input_box->SetCursorSize(cursor_w, cursor_h);
	console_input_box->SetCursorColor({ 255,255,255,255 });
	console_input_box->SetBox(input_box);

	//Build Console Labels Scroll
	console_labels_scroll = (UI_Scroll*)App->gui->GenerateUI_Element(SCROLL);
	iPoint item_location = { console_area.w - 28,20 };
	iPoint back_location = { console_area.w - 25,20 };
	SDL_Rect scroll_area = { console_area.x,console_area.y,console_area.w,console_area.y - (console_area.y - input_box.y) };
	console_labels_scroll->SetScrollableItem({ item_location.x,item_location.y }, { 1000,880,19,20 });
	console_labels_scroll->SetScrollableBack({ back_location.x,back_location.y }, { 985,874,13,149 });
	console_labels_scroll->SetBox(scroll_area);
	console_labels_scroll->SetContentWindow({ scroll_area.x, scroll_area.y, item_location.x, input_box.y });
	console_labels_scroll->SetScrollType(VERTICAL);
	console_labels_scroll->SetScrollMaxValue(500);

	LOG("Console UI Build");

	return true;
}

bool j1Console::Start()
{
	//Now is able to generate texture for console scroll labels
	can_texturize_strings = true;
	UpdateConsoleLabels();

	//Add Console Commands
	AddCommand("get", this);
	AddCommand("set", this);
	AddCommand("help", this);

	LOG("Console Labels Generated");
	return true;
}

bool j1Console::PostUpdate()
{
	if (console_input_box->GetActiveState() == false)return true;

	//Update Console Input Box
	console_input_box->HandleInput();
	console_labels_scroll->HandleInput();
	
	//UpdateConsoleLabels();

	//Draw Console Area
	App->render->DrawQuad({ console_area.x - App->render->camera.x,console_area.y -App->render->camera.y,console_area.w,console_area.h }, console_color.r, console_color.g, console_color.b, console_color.a);

	//Draw console labels
	console_labels_scroll->Draw(false);

	//Draw Console Input Box
	console_input_box->Draw(true);
	
	return true;
}

bool j1Console::CleanUp()
{
	can_texturize_strings = false;

	uint var_count = console_variables.size();
	for (uint k = 0; k < var_count; k++)
	{
		delete console_variables[k];
	}
	uint labels_count = console_labels.size();
	for (uint k = 0; k < labels_count; k++)
	{
		delete console_labels[k];
	}
	delete console_input_box;
	delete console_labels_scroll;

	return true;
}

//Functionality ===========================================
void j1Console::GUI_Input(UI_Element * target, GUI_INPUT input)
{
	int x, y;
	App->input->GetMouseMotion(x, y);
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
		break;
	case MOUSE_LEFT_BUTTON_REPEAT:
		if (target == console_labels_scroll)
		{
			console_labels_scroll->MoveScroll(x, y);
		}
		break;
	case MOUSE_LEFT_BUTTON_UP:
		if (target == console_labels_scroll)
		{
			console_labels_scroll->UnselectScroll();
		}
		break;
	case MOUSE_RIGHT_BUTTON:
		break;
	case BACKSPACE:
		break;
	case SUPR:
		break;
	case MOUSE_IN:
		break;
	case MOUSE_OUT:
		break;
	case ENTER:
		if (target == console_input_box)
		{
			//Get data from input
			Cvar* target_cvar = GetCvarfromInput(console_input_box->GetText());
			std::string input = GetValuefromInput(console_input_box->GetText());
			Command* command = GetCommandfromInput(console_input_box->GetText());

			//Send command to each module or print error str
			if (command == NULL)
			{
				GenerateConsoleLabel("Error Command Input: %s", console_input_box->GetText());
			}
			else
			{
				if (command->GetModuleTarget() != nullptr)
				{
					command->GetModuleTarget()->Console_Command_Input(command, target_cvar, &input);
				}
				else
				{
					App->Console_Command_Input(command, target_cvar, &input);
				}
			}
			
			//Clean console box
			console_input_box->SetText("");
			//Go at bottom of scroll
			console_labels_scroll->GoBottom();
		}
		break;
	case TAB:
		if (target == console_input_box)
		{
			AutoCompleteInputBox(console_input_box->GetText());
		}
		break;
	}
}

// Functionality =========================================
void j1Console::ChangeConsoleState()
{
	active = !active;
	if (active)
	{
		App->gui->ItemSelected = console_input_box;
		console_input_box->Activate();
		console_labels_scroll->Activate();
	}
	else
	{
		console_input_box->SetText("");
		console_input_box->Desactivate();
		console_labels_scroll->Desactivate();
	}
}

//Console UI interaction --------------------
void j1Console::GoScrollBottom()
{
	console_labels_scroll->GoBottom();
}

bool j1Console::AutoCompleteInputBox(char* input)
{
	//Get input str length
	uint char_num = strlen(input);
	if (char_num > 30)return false;
	uint input_step = 1;
	uint init = 0;

	//Get input current step
	for (uint k = 0; k < char_num; k++)
	{
		if (input[k] == '/')
		{
			if (input_step < 2)
			{
				input_step = 2;
				init = k + 1;
			}
			else return false;
		}
		else if (input[k] == '.')
		{
			if (input_step < 3)
			{
				input_step = 3;
				init = k + 1;
			}
			else return false;
		}
		else if (input[k] == '=')
		{
			if (input_step < 4)
			{
				input_step = 4;
				init = k + 1;
			}
			else return false;
		}
	}

	//Build the str of the step input
	if (char_num == init)return false;
	char* temp = new char[char_num + 1 - init];
	uint h = 0;
	for (uint j = init; j < char_num; j++)
	{
		temp[h] = input[j];
		h++;
	}
	temp[h] = '\0';
	std::string str = temp;
	delete temp;
	if (strlen(str.c_str()) > 7)return false;
	//Try to auto complete the step
	uint elements_num = 0;
	uint equal_chars = 0;
	uint perf_index = 0;
	bool ret = false;
	uint eq = 0;
	switch (input_step)
	{
	case 1:
		//Check command auto complete
		elements_num = commands.size();
		for (uint k = 0; k < elements_num; k++)
		{
			eq = CompareChars(str.c_str(),commands[k]->GetCommandStr()->c_str());
			if (eq == commands[k]->GetCommandStr()->length())
			{
				return false;
			}
			else if (equal_chars < eq)
			{
				equal_chars = eq;
				perf_index = k;
				ret = true;
			}
		}
		if (!ret)return false;
		console_input_box->DeleteTextSegment(init, char_num);
		console_input_box->PushTextSegment(commands[perf_index]->GetCommandStr()->c_str(), init);
		break;

	case 2:
		
		//Check module auto complete
		elements_num = App->GetModulesNum();
		for (uint k = 0; k < elements_num; k++)
		{
			eq = CompareChars(str.c_str(),App->GetModuleAt(k)->name.c_str());
			if (equal_chars < eq)
			{
				equal_chars = eq;
				perf_index = k;
				ret = true;
			}
		}
		
		if (CompareChars(str.c_str(),"app") > equal_chars)
		{
			console_input_box->DeleteTextSegment(init, char_num);
			console_input_box->PushTextSegment("app", init);
			console_input_box->SetCursorPos(strlen(console_input_box->GetText()));
			return true;
		}

		else {
			if (!ret)return false;
				console_input_box->DeleteTextSegment(init, char_num);
				console_input_box->PushTextSegment(App->GetModuleAt(perf_index)->name.c_str(), init);
				input = console_input_box->GetText();
				uint k = 0;
				k++;
				k += 9;
		}
		
		break;

	case 3:
		//Check cvar auto complete
		bool find;
		j1Module* cvar_module;
		cvar_module = GetModulefromInput(input, find);
		if (!find)return false;
		elements_num = console_variables.size();
		for (uint k = 0; k < elements_num; k++)
		{
			eq = CompareChars(str.c_str(),console_variables[k]->GetCvarName()->c_str());
			if (eq == console_variables[k]->GetCvarName()->length())
			{
				return false;
			}
			if (equal_chars < eq && console_variables[k]->GetCvarModule() == cvar_module)
			{
				equal_chars = eq;
				perf_index = k;
				ret = true;
			}
		}
		
		if (!ret)return false;
		console_input_box->DeleteTextSegment(init, char_num);
		console_input_box->PushTextSegment(console_variables[perf_index]->GetCvarName()->c_str(), init);
		break;

	case 4:
		//Check input type auto complete
		break;

	}
	//Place the cursor at the end of the new str
	console_input_box->SetCursorPos(console_input_box->GetTextLength());

	return ret;
}

//Console Output ----------------------------
void j1Console::AddConsoleText(char* new_text)
{
	UI_String* label = new UI_String({ 0,0,0,0 }, new_text, font_color, font);
	if (!can_texturize_strings)
	{
		console_labels.push_back(label);
	}
	else
	{
		console_labels.push_back(label);
		UpdateConsoleLabels();
		console_labels_scroll->GoBottom();
	}
}

char * j1Console::GenerateConsoleLabel(char * new_text,...)
{
	static char tmp_string[1000];
	static va_list ap;

	va_start(ap,new_text);
	vsprintf_s(tmp_string, 1000, new_text, ap);
	va_end(ap);

	UI_String* label = new UI_String({ 0,0,0,0 }, tmp_string, font_color, font);
	console_labels.push_back(label);
	UpdateConsoleLabels();

	return tmp_string;
}

void j1Console::UpdateConsoleLabels()
{
	uint labels_num = console_labels.size();
	uint scroll_labels_num = console_labels_scroll->GetScrollItemsNum();
	if (scroll_labels_num)scroll_labels_num--;

	if (scroll_labels_num == labels_num)return;
	
	for (uint k = scroll_labels_num; k < labels_num; k++)
	{
		console_labels_scroll->AddScrollItemAtBottom(console_labels.at(k));
		console_labels[k]->SetFont(font);
		console_labels[k]->SetColor(font_color);
		console_labels[k]->GenerateTexture();
		console_labels[k]->AdjustBox();
	}
}

//Console Input -----------------------------
Cvar * j1Console::GetCvarfromInput(char * input) const
{

	//String position of input elements
	uint name_init = 0;
	uint name_end = 0;

	//Get the var name start and end position
	uint char_num = strlen(input) + 1;
	for (uint k = 0; k < char_num; k++)
	{
		if (input[k] == '.' && name_init == 0)name_init = k + 1;
		else if ((input[k] == '\0' || input[k] == '=') && name_end == 0)name_end = k;
	}

	if (name_init == 0)return nullptr;

	//Build the module & cvar name
	char* cvar_name = new char[10];
	char* module_name = new char[10];
	
	//Copy the cvar name in a new string
	uint j = 0;
	for (uint pos = name_init; pos < name_end; pos++)
	{
		cvar_name[j] = input[pos];
		j++;
	}
	cvar_name[j] = '\0';
	std::string name_str = cvar_name;
	delete cvar_name;

	//Search the Cvar with the name build
	uint cvar_num = console_variables.size();
	for (uint k = 0; k < cvar_num; k++)
	{
		if (*console_variables[k]->GetCvarName() == name_str.c_str())return console_variables[k];
	}
	return nullptr;
}

char* j1Console::GetValuefromInput(char * input) const
{
	uint char_num = strlen(input);
	char* value = new char[char_num];
	uint init = 0;
	uint val_pos = 0;
	
	for (uint k = 0; k < char_num; k++)
	{
		if (input[k] == '=')init = k;
		if (k > init && init != 0)
		{
			value[val_pos] = input[k];
			val_pos++;
		}
	}
	char* temp = value;
	delete value;
	return temp;
}

Command* j1Console::GetCommandfromInput(char* input)const
{
	//Build the command string
	const uint char_num = strlen(input);
	char* command_type = new char[char_num];

	for (uint k = 0; k <= char_num; k++)
	{
		if (input[k] == '/' || k == char_num)
		{
			command_type[k] = '\0';
			break;
		}
		command_type[k] = input[k];
	}
	
	//Search the commands in the array
	uint command_num = commands.size();
	for (uint k = 0; k < command_num; k++)
	{
		if (strcmp(commands[k]->GetCommandStr()->c_str(),command_type) == 0)
		{
			delete command_type;
			return commands[k];
		}
	}

	delete command_type;
	return nullptr;
}

j1Module * j1Console::GetModulefromInput(char * input, bool& find)
{
	//String position of input elements
	uint module_init = 0;
	uint module_end = 0;

	//Get module name start and end position
	uint char_num = strlen(input) + 1;
	for (uint k = 0; k < char_num; k++)
	{
		if (input[k] == '/' && module_init == 0)module_init = k + 1;
		else if (input[k] == '.' && module_end == 0)module_end = k;
	}

	if (module_end < module_init || module_end == 0 || module_init == 0)
	{
		find = false;
		return nullptr;
	}


	//Build module name string
	char* module_str = new char[module_end - module_init];
	uint j = 0;
	for (uint k = module_init; k < module_end; k++)
	{
		module_str[j] = input[k];
		j++;
	}
	module_str[j] = '\0';
	std::string module_name = module_str;
	delete module_str;

	//Find the module with the name build
	if (module_name == "app")
	{
		find = true;
		return nullptr;
	}
	uint num_modules = App->GetModulesNum();
	for (uint k = 0; k < num_modules; k++)
	{
		if (module_name == App->GetModuleAt(k)->name.c_str())
		{
			find = true;
			return App->GetModuleAt(k);
		}
	}
	return nullptr;
}

uint j1Console::CompareChars(const char * str_a, const char * str_b) const
{
	//Equal chars counter
	uint equal_chars = 0;

	//Iterate str and count equal chars
	uint chars_num = MIN(strlen(str_a),strlen(str_b));
	for (uint k = 0; k < chars_num; k++)
	{
		if (str_a[k] == str_b[k])equal_chars++;
	}

	//Return equal chars
	return equal_chars;
}

//Transformations ---------------------------
char * j1Console::CvarTypetoString(C_VAR_TYPE cvar_type) const
{
	switch (cvar_type)
	{
	case INT_VAR:		return "integer";	break;
	case FLOAT_VAR:		return "float";		break;
	case CHAR_VAR:		return "character";	break;
	case BOOLEAN_VAR:	return "boolean";	break;
	case UNDEF:			return "undefined";	break;
	}
}

C_VAR_TYPE j1Console::StringtoCvarType(const std::string* string) const
{
	if			(*string == "integer")			return INT_VAR;
	else if		(*string == "float")			return FLOAT_VAR;
	else if		(*string == "character")		return CHAR_VAR;
	else if		(*string == "boolean")			return BOOLEAN_VAR;
	else return UNDEF;
}

//Console Variables Creation ----------------
Cvar* j1Console::AddCvar(const char* name, const char* description,const char* value, C_VAR_TYPE cvar_type, j1Module* module_target, bool only_read)
{
	//Check if the cvar already exist
	uint num = console_variables.size();
	for (uint k = 0; k < num; k++)
	{
		if (*console_variables[k]->GetCvarName() == name)return nullptr;
	}

	//Create the new cvar
	Cvar* new_cvar = new Cvar(name, description, (char*)value, cvar_type, module_target, only_read);

	//Add it to the cvars array
	console_variables.push_back(new_cvar);

	return new_cvar;
}

Cvar* j1Console::LoadCvar(const char* name, const char* description,const char* value, C_VAR_TYPE cvar_type, j1Module* module_target, bool only_read)
{
	//Create the new cvar
	uint cvars_num = console_variables.size();
	for (uint k = 0; k < cvars_num; k++)
	{
		if (console_variables[k]->GetCvarName()->c_str() == name)return nullptr;
	}

	//Create the new Cvar
	Cvar* new_cvar = new Cvar(name, description, (char*)value, cvar_type, module_target, only_read);
	console_variables.push_back(new_cvar);

	return new_cvar;
}

//Console Commands Creation -----------------
Command * j1Console::AddCommand(const char * command_str, j1Module * module_target)
{
	//Check if the command already exist
	uint commands_num = commands.size();
	for (uint k = 0; k < commands_num; k++)
	{
		if (commands[k]->GetCommandStr()->c_str() == command_str)
		{
			return nullptr;
		}
	}

	//Create the new command
	Command* new_command = new Command(command_str, module_target);
	commands.push_back(new_command);
	
	LOG("- %s - command added", new_command->GetCommandStr()->c_str());

	return new_command;
}

//Handle Console Input ----------------------
void j1Console::Console_Command_Input(Command * command, Cvar * cvar, std::string * input)
{
	//Get command
	if (*command->GetCommandStr() == "get")
	{
		//Check cvar
		if (cvar == nullptr)
		{
			App->console->GenerateConsoleLabel("Error set command Cvar is NULL");
			return;
		}
		//Show cvar value
		GenerateConsoleLabel("%s: %s", cvar->GetCvarName()->c_str(), cvar->GetValueString()->c_str());
	}

	//Set command
	else if (*command->GetCommandStr() == "set")
	{
		//Check cvar
		if (cvar == nullptr)
		{
			App->console->GenerateConsoleLabel("Error set command Cvar is NULL");
			return;
		}
		
		//Update cvar state
		if (cvar->GetCvarModule() != nullptr)
		{
			cvar->GetCvarModule()->Console_Cvar_Input(cvar, command, input);
		}
		else
		{
			App->Console_Cvar_Input(cvar, command, input);
		}

		//Show cvar value
		GenerateConsoleLabel("-- %s = %s", cvar->GetCvarName()->c_str(),cvar->GetValueString()->c_str());
	}
	//Help command
	else if (*command->GetCommandStr() == "help")
	{
		std::string module;
		std::string only_read;
		uint c_var_num = console_variables.size();
		for (uint k = 0; k < c_var_num; k++)
		{
			//Build the cvar module name
			if (console_variables[k]->GetCvarModule() == nullptr)module = "app";
			else module = console_variables[k]->GetCvarModule()->name;
			
			//Build the cvar mutable name
			if (console_variables[k]->GetOnlyReadState())only_read = "const";
			else only_read = "mutable";

			//Show cvar data on console scroll
			//name | module | description | type | value | edit state 
			GenerateConsoleLabel("-- %s -- | -- %s -- | -- %s -- | -- %s -- | -- %s -- | -- %s -- ",
				console_variables[k]->GetCvarName()->c_str(), module.c_str(), console_variables[k]->GetCvarDescription()->c_str(),
				CvarTypetoString(console_variables[k]->GetCvarType()), console_variables[k]->GetValueString()->c_str(),only_read.c_str());
		}
	}

}
