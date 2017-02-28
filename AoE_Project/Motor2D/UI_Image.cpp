#include "UI_Image.h"
#include "j1Render.h"

//Constructors ============================================
UI_Image::UI_Image(const SDL_Rect& box, const iPoint pivot, const SDL_Rect& texture_rect, int texture_id) : UI_Element(box, IMG), pivot(pivot), texture_rect(texture_rect), texture_id(texture_id) {}

UI_Image::UI_Image(const UI_Image* copy) : UI_Element(copy->box, IMG), pivot(copy->pivot), texture_rect(copy->texture_rect), texture_id(copy->texture_id) {}

UI_Image::UI_Image() : UI_Element({ 0,0,0,0 }, IMG), pivot(0,0), texture_rect({ 0,0,0,0 }) {}

//Destructors =============================================
UI_Image::~UI_Image()
{

}


//Game Loop ===============================================
void UI_Image::Draw(bool debug) const
{
	//This Draw
	if(debug)App->render->DrawQuad({ box.x - App->render->camera.x, box.y - App->render->camera.y, box.w, box.h }, 150, 50, 0);

	//Draw from Atlas
	if (texture_id == -1)App->render->Blit(((SDL_Texture*)App->gui->GetAtlas()), box.x - App->render->camera.x, box.y - App->render->camera.y, &texture_rect);
	
	//Draw from other textures
	else {
		
		//Undefined draw size
		if (texture_rect.w == 0 || texture_rect.h == 0)App->render->Blit(App->gui->Get_UI_Texture(texture_id), box.x - App->render->camera.x - pivot.x, box.y - App->render->camera.y - pivot.y);
		//Defined draw size
		else App->render->Blit(App->gui->Get_UI_Texture(texture_id), box.x - App->render->camera.x - pivot.x, box.y - App->render->camera.y - pivot.y, &texture_rect);

	}

	//Childs Draw
	DrawChilds(debug);
}

//Functionality ===========================================
SDL_Rect UI_Image::AdjustBox()
{
	
	box.w = texture_rect.w;
	box.h = texture_rect.h;

	if (box.w == 0 || box.h == 0) {
	
		int w, h;
		SDL_QueryTexture(App->gui->Get_UI_Texture(this->texture_id), NULL, NULL, &w, &h);

		box.w = w;
		box.h = h;
	}

	return box;
}

void UI_Image::DrawAt(int x, int y) const
{
	x += this->box.x;
	y += this->box.y;

		//Draw from Atlas
		if (texture_id == -1)App->render->Blit(((SDL_Texture*)App->gui->GetAtlas()), x - App->render->camera.x, y - App->render->camera.y, &texture_rect);

		//Draw from other textures
		else {

			//Undefined draw size
			if (texture_rect.w == 0 || texture_rect.h == 0)App->render->Blit(App->gui->Get_UI_Texture(texture_id), x - App->render->camera.x - pivot.x, y - App->render->camera.y - pivot.y);
			//Defined draw size
			else App->render->Blit(App->gui->Get_UI_Texture(texture_id), x - App->render->camera.x - pivot.x, y - App->render->camera.y - pivot.y, &texture_rect);

		}
}

void UI_Image::ChangeTextureRect(SDL_Rect new_rect)
{
	texture_rect = new_rect;
}

void UI_Image::ChangeTextureId(int id)
{
	texture_id = id;
}

iPoint UI_Image::GetPivot() const
{
	return pivot;
}
