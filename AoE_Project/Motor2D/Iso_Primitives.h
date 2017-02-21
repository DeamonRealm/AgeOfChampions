#ifndef _ISO_PRIMITVES_
#define _ISO_PRIMITVES_

#include"p2Defs.h"
#include "p2Point.h"
#include"SDL_image/include/SDL_image.h"

enum COLLIDER_TYPE
{
	COLLIDER_NO_UI,
	COLLIDER_NO_BASE,
	COLLIDER_UNIT_UI,
	COLLIDER_UNIT_BASE,
	COLLIDER_BUILDING_UI,
	COLLIDER_BUILDING_BASE,
	COLLIDER_RESOURCE_UI,
	COLLIDER_RESOURCE_BASE
};

///Class Primitive ------------------------------
//Balse primitive class
class Primitive
{
public:

	Primitive(const iPoint& position, const SDL_Color& color = {});
	Primitive(const Primitive& copy);
	~Primitive();

protected:

	iPoint		position = {0,0};
	float		x_angle = 0.0f;
	SDL_Color	color = { 0,0,0,0 };

public:

	//Functionality ---------
	//Draw
	virtual bool	Draw();
	//Set Methods
	void			SetPosition(const iPoint& pos);
	void			SetXAngle(float angle);
	void			SetColor(const SDL_Color& rgba);
	//Get Methods
	iPoint			GetPosition()const;
	float			GetXAngle()const;
	SDL_Color		GetColor()const;
};
/// ---------------------------------------------


///Class Circle ---------------------------------
//Circle isometric primitive
class Circle : public Primitive
{
public:

	Circle(iPoint position = { 0,0 }, uint rad = 0);
	Circle(const Circle& copy);
	~Circle();

private:

	uint rad = 0;

public:

	//Functionality ---------
	//Draw
	bool	Draw();
	//Set Methods
	void	SetRad(uint r);
	//Get Methods
	uint	GetRad()const;
};
/// ---------------------------------------------


///Class Rectangle ------------------------------
//Rectangle isometric primitive
class Rectng : public Primitive
{
public:

	Rectng(iPoint position = {0,0},uint width = 0, uint height = 0);
	Rectng(const Rectng& copy);
	~Rectng();

private:

	uint width = 0;
	uint height = 0;

public:

	//Functionality ---------
	//Draw
	bool	Draw();
	//Set Methods
	void	SetWidth(uint w);
	void	SetHeight(uint h);
	//Get Methods
	uint	GetWidth()const;
	uint	GetHeight()const;
};
/// ---------------------------------------------
#endif // _ISO_PRIMITVES_
