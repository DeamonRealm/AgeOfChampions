#include "Iso_Primitives.h"

#include "j1App.h"
#include "j1Render.h"
#include <math.h>
#include "p2Log.h"

///Class Primitive ------------------------------
//Constructors ==============
Primitive::Primitive()
{

}

Primitive::Primitive(const iPoint& position, const iPoint& displacement, const SDL_Color& color) :position(position), displacement(displacement), color(color)
{

}

Primitive::Primitive(const Primitive& copy) : position(copy.position), displacement(copy.displacement), color(copy.color), x_angle(copy.x_angle)
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

void Primitive::SetDisplacement(const iPoint & desp)
{
	displacement = desp;
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
Circle::Circle(const iPoint& position, uint rad, const iPoint& desplacement) :Primitive(position, desplacement), rad(rad)
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
	return App->render->DrawCircle(position.x + displacement.x, position.y + displacement.y, rad, color.r, color.g, color.b, color.a, x_angle, true);
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
Rectng::Rectng(const iPoint& position, uint width, uint height, const iPoint& desplacement) :Primitive(position,desplacement), width(width), height(height)
{

}

Rectng::Rectng(const Rectng & copy) : Primitive(copy), width(copy.width), height(copy.height)
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
	int diagonal_len = (int)floor(sqrt(((width*0.5f)*(width*0.5f)) + ((height*0.5f)*(height*0.5f))));
	
	//Draw lines with the correct angles and coordinates to form the rotated quad
	iPoint draw_pos(position.x + displacement.x, position.y + displacement.y);
	ret = App->render->DrawLine(draw_pos.x, draw_pos.y - (int)(diagonal_len * sin(x_angle)), draw_pos.x + diagonal_len, draw_pos.y, color.r, color.g, color.b, color.a, true);
	ret = App->render->DrawLine(draw_pos.x, draw_pos.y + (int)(diagonal_len * sin(x_angle)), draw_pos.x + diagonal_len, draw_pos.y, color.r, color.g, color.b, color.a, true);
	ret = App->render->DrawLine(draw_pos.x - diagonal_len, draw_pos.y, draw_pos.x, draw_pos.y - (int)(diagonal_len * sin(x_angle)), color.r, color.g, color.b, color.a, true);
	ret = App->render->DrawLine(draw_pos.x - diagonal_len, draw_pos.y, draw_pos.x, draw_pos.y + (int)(diagonal_len * sin(x_angle)), color.r, color.g, color.b, color.a, true);
	
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
Line::Line(const iPoint & position, const iPoint & position_2, const SDL_Color& color, const iPoint& desplacement) :Primitive(position, desplacement, color), position_2(position_2)
{

}

Line::Line(const Rectng & copy) : Primitive(position, displacement, color), position_2(position_2)
{
}

//Destructors ===============
Line::~Line()
{
}

//Functionality =============
bool Line::Draw()
{
	return App->render->DrawLine(position.x + displacement.x, position.y + displacement.y, position_2.x, position_2.y, color.r, color.g, color.b, color.a);
}

void Line::SetP1(const iPoint & p1)
{
	position = p1;
}

void Line::SetP2(const iPoint & p2)
{
	position_2 = p2;
}

const iPoint& Line::GetP1() const
{
	return position;
}

const iPoint& Line::GetP2() const
{
	return position_2;
}

///Class Triangle -------------------------------
//Triangle isometric primitive
//Constructors ==============
Triangle::Triangle(const iPoint & position, const iPoint & base, uint length, float width_angle, const SDL_Color & color, const iPoint & displacement) :Primitive(position, displacement, color), base(base), length(length), width_angle(width_angle)
{

}

Triangle::Triangle(const Triangle & copy):Primitive(copy), base(copy.base), length(copy.length), width_angle(copy.width_angle)
{

}

//Destructors ===============
Triangle::~Triangle()
{

}

//Functionality =============
bool Triangle::Draw()
{
	return App->render->DrawTriangle(position.x, position.y, base.x, base.y, length, width_angle, x_angle, color.r, color.g, color.b, color.a);
}

void Triangle::SetBase(const iPoint & new_base)
{
	base = new_base;
}

void Triangle::SetLength(uint new_length)
{
	length = new_length;
}

void Triangle::SetAngle(float new_angle)
{
	width_angle = new_angle;
}

const iPoint & Triangle::GetBase() const
{
	return base;
}

uint Triangle::GetLength() const
{
	return length;
}

float Triangle::GetWidthAngle() const
{
	return width_angle;
}

/// ---------------------------------------------