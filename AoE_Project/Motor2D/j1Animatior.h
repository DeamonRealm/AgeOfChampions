#ifndef _J1ANIMATOR_H_
#define _J1ANIMATOR_H_

#include "j1Module.h"
#include "j1Timer.h"
#include "p2Point.h"

struct SDL_Rect;

///Animation Class ------------------------------
//Class that contains the animation basic necessary data
class Animation
{
public:

	Animation();

	~Animation();

private:

	std::vector<SDL_Rect>	frames;
	std::vector<iPoint>		pivots;
	float					current_frame = 0;
	int						loops = 0;
	bool					loop = true;
	uint					speed = 300;
	j1Timer					timer;

public:

	//Set Methods -----------
	void	SetLoop(bool loop_state);
	void	SetSpeed(uint new_speed);

	//Get Methods -----------
	bool			GetLoop()const;
	uint			GetSpeed()const;
	const SDL_Rect&	GetCurrentFrame();

	//Add New frame with pivot defined
	void AddFrame(const SDL_Rect& rect, const iPoint& point);

};
/// ---------------------------------------------

/// Animation Block Class -----------------------
//Block that contains a vector of animations 

/// ---------------------------------------------

#endif // !_J1ANIMATOR_H_
