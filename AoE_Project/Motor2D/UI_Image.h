#ifndef _UI_IMG_
#define _UI_IMG_

#include "UI_Element.h"
#include "p2Point.h"

class UI_Image : public UI_Element {
public:

	UI_Image(const SDL_Rect& box, const iPoint pivot, const SDL_Rect& texture_rect = {0,0,0,0}, int texture_id = -1);
	UI_Image(const UI_Image* copy);
	UI_Image();

	~UI_Image();

private:

	SDL_Rect	texture_rect;
	iPoint		pivot;
	int			texture_id;

public:

	//Game Loop
	void				Draw(bool debug) const;

	//Functionality
	SDL_Rect			AdjustBox();
	void				DrawAt(int x, int y)const;
	void				ChangeTextureRect(SDL_Rect new_rect);
	void				ChangeTextureId(int id);
	iPoint				GetPivot()const;
};

#endif // __UI_IMG__