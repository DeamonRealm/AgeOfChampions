#include "Iso_Primitives.h"

#include "j1App.h"
#include "j1Render.h"
#include <math.h>

///Class Primitive ------------------------------
//Constructors ==============
Primitive::Primitive()
{

}

Primitive::Primitive(const iPoint& position, const SDL_Color& color) :position(position), color(color)
{

}

Primitive::Primitive(const Primitive& copy) : position(copy.position), color(copy.color), x_angle(copy.x_angle)
{

}

//Destructors ===============
Primitive::~Primitive()
{

}

//Functionality =============
bool Primitive::Draw()
{
	return true;
}

void Primitive::SetPosition(const iPoint & pos)
{
	position = pos;
}

void Primitive::SetXAngle(float angle)
{
	x_angle = angle;
}

void Primitive::SetColor(const SDL_Color & rgba)
{
	color = rgba;
}

iPoint Primitive::GetPosition() const
{
	return position;
}
float Primitive::GetXAngle() const
{
	return x_angle;
}
SDL_Color Primitive::GetColor() const
{
	return color;
}
/// ---------------------------------------------


///Class Circle ---------------------------------
//Constructors ==============
Circle::Circle(const iPoint& position, uint rad) :Primitive(position), rad(rad)
{

}

Circle::Circle(const Circle & copy) : Primitive(copy), rad(copy.rad)
{

}

//Destructors ===============
Circle::~Circle()
{

}

//Functionality =============
bool Circle::Draw()
{
	//Just draw the circle mark
	return App->render->DrawCircle(position.x, position.y, rad, color.r, color.g, color.b, color.a, x_angle, true);
}

void Circle::SetRad(uint r)
{
	rad = r;
}

uint Circle::GetRad() const
{
	return rad;
}
/// ---------------------------------------------


///Class Rectangle ------------------------------
//Constructors ==============
Rectng::Rectng(const iPoint& position, uint width, uint height) :Primitive(position), width(width), height(height)
{

}

Rectng::Rectng(const Rectng & copy) : Primitive(copy.position, copy.color), width(copy.width), height(copy.height)
{

}

//Destructors ===============
Rectng::~Rectng()
{

}


//Functionality =============
bool Rectng::Draw()
{
	bool ret = false;

	//Calculate the diagonal distance from the center to the vertex
	int diagonal_len = floor(sqrt(((width*0.5f)*(width*0.5f)) + ((height*0.5f)*(height*0.5f))));
	
	//Draw lines with the correct angles and coordinates to form the rotated quad
	ret = App->render->DrawLine(position.x, position.y - (diagonal_len * sin(x_angle)), position.x + diagonal_len, position.y, color.r, color.g, color.b, color.a, true);
	ret = App->render->DrawLine(position.x, position.y + (diagonal_len * sin(x_angle)), position.x + diagonal_len, position.y, color.r, color.g, color.b, color.a, true);
	ret = App->render->DrawLine(position.x - diagonal_len, position.y, position.x, position.y - (diagonal_len * sin(x_angle)), color.r, color.g, color.b, color.a, true);
	ret = App->render->DrawLine(position.x - diagonal_len, position.y, position.x, position.y + (diagonal_len * sin(x_angle)), color.r, color.g, color.b, color.a, true);
	
	return true;
}

void Rectng::SetWidth(uint w)
{
	width = w;
}

void Rectng::SetHeight(uint h)
{
	height = h;
}

uint Rectng::GetWidth() const
{
	return width;
}

uint Rectng::GetHeight() const
{
	return height;
}
/// ---------------------------------------------


///Class Line -----------------------------------
//Constructors ==============
Line::Line(const iPoint & position, const iPoint & position_2, const SDL_Color& color) :Primitive(position,color), position_2(position_2)
{

}

Line::Line(const Rectng & copy) : Primitive(position, color), position_2(position_2)
{
}

//Destructors ===============
Line::~Line()
{
}

//Functionality =============
bool Line::Draw()
{
	return App->render->DrawLine(position.x, position.y, position_2.x, position_2.y, color.r, color.g, color.b, color.a);
}

void Line::SetP1(const iPoint & p1)
{
	position = p1;
}

void Line::SetP2(const iPoint & p2)
{
	position_2 = p2;
}

const iPoint & Line::GetP1() const
{
	return position;
}

const iPoint & Line::GetP2() const
{
	return position_2;
}
