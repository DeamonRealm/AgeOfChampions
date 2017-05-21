#ifndef _ISO_PRIMITVES_
#define _ISO_PRIMITVES_

#include"p2Defs.h"
#include "p2Point.h"
#include "j1Timer.h"

#include"SDL_image/include/SDL_image.h"

class Rectng;

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
//Base primitive class
class Primitive
{
public:

	Primitive();
	Primitive(const iPoint& position, const iPoint& displacement = { 0,0 } , const SDL_Color& color = { 0,0,0,255 });
	Primitive(const Primitive& copy);
	~Primitive();

protected:

	iPoint		position = { 0,0 };
	iPoint		displacement = { 0,0 };
	float		x_angle = 0.54f;
	SDL_Color	color = { 0,0,0,255 };

public:

	//Functionality ---------
	//Draw
	virtual bool	Draw();
	//Set Methods
	void			SetPosition(const iPoint& pos);
	void			SetDisplacement(const iPoint& desp);
	void			SetXAngle(float angle);
	void			SetColor(const SDL_Color& rgba);
	//Get Methods
	iPoint			GetPosition()const;
	iPoint			GetDisplacement()const;
	float			GetXAngle()const;
	SDL_Color		GetColor()const;
};
/// ---------------------------------------------


///Class Circle ---------------------------------
//Circle isometric primitive
class Circle : public Primitive
{
public:

	Circle(const iPoint& position = { 0,0 }, uint rad = 0, const iPoint& displacement = { 0,0 });
	Circle(const Circle& copy);
	~Circle();

private:

	uint rad = 0;

public:

	//Functionality ---------
	//Draw
	bool	Draw();
	//Shape functions
	bool	IsIn(const fPoint* loc) const;
	bool	Intersects(const SDL_Rect* rect) const;
	bool	Intersects(const Circle* target)const;
	bool	Intersects(const Rectng* target)const;
	bool	Intersects(const iPoint* point)const;
	bool	Overlap(const Circle* target)const;
	iPoint  NearestPoint(const Circle* target)const;
	iPoint	NearestPoint(const Rectng* target)const;

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

	Rectng(const iPoint& position = {0,0},uint width = 0, uint height = 0, const iPoint& displacement = { 0,0 });
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

///Class PivotedRect ----------------------------
class PivotedRect : public Primitive
{
public:

	PivotedRect(const iPoint& origin = { 0,0 }, const iPoint& goal = { 0,0 }, uint width = 0, uint height = 0);
	PivotedRect(const PivotedRect& copy);
	~PivotedRect();

private:

	iPoint	goal = { 0,0 };
	uint	width = 0;
	uint	height = 0;
	uint	pivot_distance = 0;

	iPoint vertex[4];

public:

	//Functionality ---------
	//Draw -------------
	bool	Draw();

	//Set Methods ------
	void	SetGoal(const iPoint& new_goal);
	void	SetWidth(uint new_width);
	void	SetHeight(uint new_height);
	void	SetPivotDistance(uint new_pivot_distance);

	void	CalculateVertex();
	bool	IsIn(const fPoint* loc) const;
	bool	Intersects(const SDL_Rect* rect)const;

	uint	GetWidth() const;
	uint	GetHeight() const;
	uint	GetPivotDistance() const;

};
/// ---------------------------------------------

///Class Line -----------------------------------
//Line isometric primitive
class Line : public Primitive
{
public:

	Line(const iPoint& position , const iPoint& position_2, const SDL_Color& color , const iPoint& displacement = { 0,0 });
	Line(const Rectng& copy);
	~Line();

private:

	iPoint position_2 = { 0,0 };

public:

	//Functionality ---------
	//Draw
	bool	Draw();
	//Set Methods
	void			SetP1(const iPoint& p1);
	void			SetP2(const iPoint& p2);
	//Get Methods
	const iPoint&	GetP1()const;
	const iPoint& 	GetP2()const;
};
/// ---------------------------------------------

///Class Triangle -------------------------------
//Triangle isometric primitive
class Triangle : public Primitive
{
public:

	Triangle(const iPoint& position = { 0,0 } , const iPoint& base = { 0,0 }, uint length = 0, float width_angle = 0, const iPoint& displacement = { 0,0 });
	Triangle(const Triangle& copy);
	~Triangle();

private:

	iPoint base = { 0,0 }; /*Used for calculate the other vertex*/
	uint length = 0;
	float width_angle = 0;
	
	iPoint v_A = { 0,0 };	
	iPoint v_B = { 0,0 };

public:

	//Functionality ---------
	//Draw
	bool			Draw();
	//Shape functions
	bool			IsIn(const fPoint* loc) const;
	bool			Intersects(const SDL_Rect* rect) const;
	void			CalculateVertex();
	//Set Methods
	void			SetBase(const iPoint& new_base);
	void			SetLength(uint new_length);
	void			SetWidthAngle(float new_angle);
	//Get Methods
	const iPoint&	GetBase()const;
	uint			GetLength()const;
	float			GetWidthAngle()const;

};
/// ---------------------------------------------

/// Class Alpha Cell ----------------------------
class AlphaCell
{
public:

	AlphaCell(const iPoint& position = { 0,0 }, unsigned short alpha = 255);
	~AlphaCell();

public:

	iPoint			position = { 0,0 };
	unsigned short	alpha = 255;
	bool			locked = false;
	j1Timer			unlock_timer;
	
};
/// ---------------------------------------------
#endif // _ISO_PRIMITVES_
