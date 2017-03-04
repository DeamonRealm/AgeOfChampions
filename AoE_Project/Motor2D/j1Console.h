#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "j1Module.h"
#include "C_var.h"
#include "Command.h"
#include "SDL\include\SDL_rect.h"

class UI_Text_Box;
class UI_Scroll;
class UI_String;
struct _TTF_Font;

class j1Console : public j1Module {
public:

	j1Console();
	~j1Console();

public:

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PostUpdate();

	bool CleanUp();

private:

	SDL_Rect				console_area;
	SDL_Color				console_color;
	UI_Text_Box*			console_input_box;

	UI_Scroll*				console_labels_scroll;
	std::vector<UI_String*>	console_labels;
	bool					can_texturize_strings = false;
	
	std::vector<Cvar*>		console_variables;
	std::vector<Command*>	commands;
	
	_TTF_Font*				font;
	SDL_Color				font_color;

public:

	// Receive UI input and work with it
	void	GUI_Input(UI_Element* target, GUI_INPUT input);

	//Active / Deactivate the Console
	void	ChangeConsoleState();

	//Console UI interaction --------------------
	//Set Console Scroll at bottom to see the last label
	void	GoScrollBottom();
	//Auto complete input string checking cvars & commands
	bool	AutoCompleteInputBox(char* input);

	//Console Output ----------------------------
	//Add a text at text list of scroll
	void		AddConsoleText(char* new_text);
	//Generate log text without send it to the debug output
	char*		GenerateConsoleLabel(char* new_text,...);
	//Texturize the console labels and push it to the scroll
	void		UpdateConsoleLabels();

	//Console Input -----------------------------
	//Get Cvar from input string
	Cvar*		GetCvarfromInput(char* input)const;
	//Get value from input string
	char*		GetValuefromInput(char* input)const;
	//Get command from input string
	Command*	GetCommandfromInput(char* input)const;

	//Get module from input(app = nullptr)
	j1Module*				GetModulefromInput(char* input, bool& find);
	//Compare input strings
	uint					CompareChars(const char* str_a, const char* str_b)const;

	//Transformations ---------------------------
	//Cvar type transformations
	char*					CvarTypetoString(C_VAR_TYPE cvar_type)const;
	C_VAR_TYPE				StringtoCvarType(const std::string* string)const;

	//Console Variables Creation ----------------
	//Add Console Variable
	Cvar* AddCvar(const char* name, const char* description,const char* value, C_VAR_TYPE cvar_type, j1Module* module_target, bool only_read);
	//Load Console Variable
	Cvar* LoadCvar(const char* name, const char* description,const char* value, C_VAR_TYPE cvar_type, j1Module* module_target, bool only_read);

	//Console Commands Creation -----------------
	Command* AddCommand(const char* command_str, j1Module* module_target);


	//Handle Console Input ----------------------
	void Console_Command_Input(Command* command, Cvar* cvar, std::string* input);

};
#endif