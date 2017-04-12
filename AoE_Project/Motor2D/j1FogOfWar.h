#ifndef _FOG_OF_WAR_
#define _FOG_OF_WAR_

#include "j1Module.h"
#include "Iso_Primitives.h"

class j1FogOfWar : public j1Module
{
public:

	j1FogOfWar();
	~j1FogOfWar();


	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PostUpdate();

private:

	/*Fog Layer*/					std::vector<bool> fog_layer;
	/*Fog Surface*/					SDL_Surface* fog_surface = nullptr;
	/*Stamp to erase fog surface*/	SDL_Surface* fog_eraser = nullptr;

public:

	uint	GetFogID(int x, int y)const;
	void	ClearFogZone(Circle zone);
	void	ClearFogTile(int x, int y);

};

#endif