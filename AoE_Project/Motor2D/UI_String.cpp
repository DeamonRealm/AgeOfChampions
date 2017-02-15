#define _CRT_SECURE_NO_WARNINGS
#include "UI_String.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Textures.h"

//Contructors =============================================
UI_String::UI_String(const SDL_Rect& box, char * text, const SDL_Color& text_color, _TTF_Font * text_font) : UI_Element(box, STRING), text(text), text_font(text_font), text_color(text_color) {}

UI_String::UI_String(const UI_String* copy) : UI_Element(copy->box, STRING), text(copy->text), text_font(copy->text_font), text_texture(copy->text_texture), text_color(copy->text_color) {}

UI_String::UI_String() : UI_Element({0,0}, STRING), text(""), text_font(nullptr) {}

//Destructor ==============================================
UI_String::~UI_String()
{

}


//Game Loop ===============================================
bool UI_String::Start()
{
	GenerateTexture();
	return true;
}

void UI_String::Draw(bool debug) const
{
	//This Draw
	
	if (debug)App->render->DrawQuad({ box.x - App->render->camera.x,box.y - App->render->camera.y,box.w,box.h }, 255, 255, 255);
	if(text_texture != nullptr)App->render->Blit(text_texture, box.x - App->render->camera.x, box.y - App->render->camera.y);

	//Childs Draw
	DrawChilds(debug);
}


//Functionality ===========================================
char * UI_String::GetString() const
{
	return (char*)text.c_str();
}

uint UI_String::GetLenght()const
{
	return uint(text.length());
}

void UI_String::SetString(char * new_text)
{
	text = new_text;
	if(strlen(new_text) > 0)text_texture = App->font->Print(text.c_str(), text_color, text_font);
}

void UI_String::PushString(const char * new_text, uint position)
{
	text.insert(position, new_text);
	text_texture = App->font->Print(text.c_str(), text_color, text_font);
}

bool UI_String::DeleteChar(uint position)
{
	if (text.length() == 0) return false;

	text.erase(position,1);

	if(text.length() > 0)text_texture = App->font->Print(text.c_str(), text_color, text_font);
	return true;
}

bool UI_String::DeleteSegment(uint start, uint end)
{
	//Check for bad input
	if (start > text.length() || end > text.length() || end < start)return false;
	
	//Delete str chars
	bool ret = true;
	for (uint k = start; k < end; k++)
	{
		text.erase(start,1);
		if (!ret)break;
	}

	//Generate str texture if it contain alive chars
	if (text.length() == 0)text_texture = nullptr;
	else if (ret)text_texture = App->font->Print(text.c_str(), text_color, text_font);

	return ret;
}

uint UI_String::GetPixelLenght(uint end) const
{
	std::string segment = text.substr(0, end);
	int x, y;
	App->font->CalcSize(segment.c_str(), x, y, text_font);
	return x;
}

uint UI_String::GetPixelHeightLenght() const
{
	int x, y;
	App->font->CalcSize("T", x, y, text_font);
	return y;
}

void UI_String::DrawAt(int x, int y) const
{
	if(text_texture != nullptr)App->render->Blit(text_texture, box.x + x - App->render->camera.x, box.y + y - App->render->camera.y);
}

bool UI_String::TokenizeString(uint margin)
{
	


	int w, h;
	int w_size = 0, h_size = 0;
	char* copy = (char*)text.c_str();
	char* token = strtok(copy, "\n");
	SDL_Texture* texture = nullptr;
	std::list<char*>		texttokens;
	std::list<SDL_Texture*>	texttextures;

	while (token != NULL) {

		texture = App->font->Print(token, text_color, App->font->default);
		texttextures.push_back(texture);
		SDL_QueryTexture(texture, NULL,NULL, &w, &h);
		texttokens.push_back(token);
		h_size += h + margin;
		if (w > w_size)w_size = w;
		token = strtok(NULL, "\n");

	}
	
	text_texture = SDL_CreateTexture(App->render->renderer, SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_TARGET, w_size, h_size);
	SDL_SetRenderTarget(App->render->renderer, text_texture);

	uint k = 0;
	std::list<SDL_Texture*>::iterator texture_item = texttextures.begin();
	while (texture_item._Ptr != NULL)
	{
		
		App->render->Blit(texture_item._Ptr->_Myval, 0, (h * k) + margin);
		texture_item++;
		k++;
	}

	SDL_SetTextureBlendMode(text_texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(App->render->renderer, NULL);
	return true;
}

bool UI_String::GenerateTexture()
{
	if (text.length() == NULL)return false;
	if(strlen(text.c_str()) > 0)text_texture = App->font->Print(this->text.c_str(), text_color, text_font);
	if(text_texture != nullptr)return true;
	return false;
}

SDL_Rect UI_String::AdjustBox()
{
	int w, h;
	SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);
	box.w = w;
	box.h = h;

	return box;
}

bool UI_String::SetFont(char* font_dir, uint size)
{
	_TTF_Font* new_font = App->font->Load(font_dir, size);
	if (new_font == NULL)return false;
	else
	{
		text_font = new_font;
		return true;
	}
}

bool UI_String::SetFont(_TTF_Font * new_font)
{
	if (new_font == nullptr)return false;
	
	text_font = new_font;
	return true;
}

void UI_String::SetColor(SDL_Color new_color)
{
	text_color = new_color;
}

