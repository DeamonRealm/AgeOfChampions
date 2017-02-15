#ifndef _UI_SCROLL_
#define _UI_SCROLL_

#include "UI_Element.h"
#include "UI_Image.h"

enum SCROLL_TYPE {

	VERTICAL,
	VERTICAL_INV,
	LATERAL,
	LATERAL_INV,
	FREE_DIRECTION

};

class UI_Scroll : public UI_Element {
public:

	UI_Scroll(const SDL_Rect& box, const SDL_Rect& ContentWindow, const UI_Image& ScrollItem, const UI_Image& ScrollBack, SCROLL_TYPE Scroll_Type = VERTICAL, int MaxValue = 0);
	UI_Scroll(const UI_Scroll* copy);
	UI_Scroll();
	~UI_Scroll();

private:

	SCROLL_TYPE				Scroll_Type;
	bool					ScrollSelected;
	
	SDL_Rect				ContentWindow;

	uint					ContentLenght = 0;
	uint					ScrollPosition = 0;
	
	float					Value = 0.0f;
	int						MaxValue = 0;

	//List of the scroll items 
	std::list<UI_Element*>		Items;
	std::list<iPoint>			Items_location;
	
public:

	UI_Image				ScrollItem;
	UI_Image				ScrollBack;

private:

	iPoint		CalculateScrollDesp();
	void		MoveScrollItems();
	uint		UpdateContentLenght(UI_Element* new_item);

public:

	//Game Loop
	void		Draw(bool debug) const;
	bool		CleanUp();
	

	//Functionality
	void		SetScrollableItem(const iPoint position, const SDL_Rect rect, int id = -1);
	void		SetScrollableBack(const iPoint position, const SDL_Rect rect, int id = -1);
	void		SetContentWindow(const SDL_Rect rect);

	void		SetScrollMaxValue(int maxvalue);
	void		SetScrollValue(float scrollvalue);

	void		SetScrollType(SCROLL_TYPE type);
	bool		MoveScroll(int mouse_y_motion, int mouse_x_motion);
	void		GoBottom();

	void		UnselectScroll();

	void		AddScrollItem(UI_Element* new_item);
	void		AddScrollItemAtBottom(UI_Element* new_item);
	uint		GetScrollItemsNum()const;
	UI_Element*	GetScrollItem(uint index);

};
#endif