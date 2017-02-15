#include "j1Animatior.h"

#include "SDL/include/SDL_rect.h"

///Animation Class ------------------------------
Animation::Animation()
{

}

Animation::~Animation()
{
	frames.clear();
	pivots.clear();
}

void Animation::SetLoop(bool loop_state)
{
	loop = loop_state;
}

void Animation::SetSpeed(uint new_speed)
{
	speed = new_speed;
}

bool Animation::GetLoop() const
{
	return loop;
}

uint Animation::GetSpeed() const
{
	return speed;
}

const SDL_Rect& Animation::GetCurrentFrame()
{
	if (current_frame == -1)return frames[0];

	current_frame = (int)floor(timer.Read() / speed);
	if (current_frame > frames.size())
	{
		if (loop)
		{
			loops++;
			current_frame = 0;
			timer.Start();
		}
		else
		{
			loops = 0;
			current_frame = -1;
			return frames[0];
		}
	}

	return frames[(int)current_frame];
}

void Animation::AddFrame(const SDL_Rect & rect, const iPoint & point)
{
	frames.push_back(rect);
	pivots.push_back(point);
}
