#ifndef _UI_ELEMENT_
#define _UI_ELEMENT_

#include "p2Point.h"
#include "p2Defs.h"
#include "j1App.h"
#include "j1Gui.h"
#include "SDL\include\SDL_rect.h"

enum UI_TYPE {

	UNDEFINED,
	BUTTON,
	TEXT_BOX,
	STRING,
	IMG,
	SCROLL,
	POPUP_MENU,
	FIXED_BUTTON
};

class UI_Element {
public:

	UI_Element(const SDL_Rect& box, UI_TYPE ui_type = UNDEFINED, bool IsActive = true);
	UI_Element(const UI_Element* copy);
	UI_Element();

	~UI_Element();

protected:

	SDL_Rect				box;
	uint					layer = 0;
	uint					tab_num = 0;
	mutable bool			IsActive = true;
	UI_TYPE					ui_type;

	UI_Element*				parent = nullptr;

	j1Module*				input_target;

public:

	std::list<UI_Element*>		childs;

public:

	//Game Loop
	virtual bool	Update();
	bool			UpdateChilds();
	virtual bool	CleanUp();
	bool			CleanUpChilds();
	virtual void	Draw(bool debug) const;
	virtual void	DrawAt(int x, int y)const;
	void			DrawChilds(bool debug)const;
	virtual void	HandleInput();

	//Functionality
	//Box functions
	bool				MouseIsIn(int x = 0, int y = 0)const;
	void				SetBoxPosition(int new_pos_x, int new_pos_y);
	virtual void		MoveBox(int x_vel, int y_vel);
	void				ResizeBox(const iPoint& new_size);
	void				SetBox(SDL_Rect new_box);
	virtual SDL_Rect	AdjustBox();
	SDL_Rect*			GetBox()const;
	bool				RectIsIn(const SDL_Rect* target, int x_vel, int y_vel, bool x_axis = false)const;

	//Tab/Layer functions
	void				SetTabable();
	uint				GetTabNum()const;
	void				SetLayer(uint new_layer);
	uint				GetLayer()const;
	UI_TYPE				GetUItype()const;

	//Active/Desactive functions
	void				Activate();
	void				Desactivate();
	bool				GetActiveState()const;

	//Tree functions
	void				AddChild(UI_Element* child, uint start_layer = 0);
	void				Delete_Child(UI_Element* child);
	void				Delete_Child(uint index);
	UI_Element*			SetParent(const UI_Element* parent);
	UI_Element*			GetParent()const;

	// Input taget functions
	void				SetInputTarget(j1Module* target);
	j1Module*			GetInputTarget()const;
};

#endif // __UI_ELEMENT__