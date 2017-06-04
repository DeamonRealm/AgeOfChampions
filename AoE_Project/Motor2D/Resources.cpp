#include "Resources.h"

#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1Animator.h"
#include "j1Render.h"
#include "j1Map.h"

///Class Tree -----------------------------------
//Constructors ========================
Tree::Tree() :Resource()
{

}

Tree::Tree(const Tree & copy) : Resource(copy), cortex(copy.cortex)
{

}

//Destructors =========================
Tree::~Tree()
{

}

uint Tree::GetCortex() const
{
	return cortex;
}

void Tree::SetCortex(uint val)
{
	cortex = val;
}

bool Tree::ExtractResources(uint* value)
{
	if (life == 0)return false;
	//Check the tree cortex
	else if (cortex > 0)
	{
		cortex -= MIN(cortex, *value);
		if (cortex == 0)
		{
			resource_type = TREE_CUT;
			App->animator->ResourcePlay(this);
		}
	}

	//Check the tree resources
	else if (life <= *value)
	{
		*value = life;
		life = 0;

		//Now the tree is only a static chop that can't interact 
		resource_type = CHOP;
		App->animator->ResourcePlay(this);
		App->entities_manager->resources_quadtree.Exteract(&this->position);
		App->entities_manager->AddDeadResource(this);

		//Change walk & construction logic maps
		iPoint world_coords = App->map->WorldToMap(position.x, position.y);
		App->map->ChangeConstructionMap(world_coords,1,1, 1);
		App->map->ChangeLogicMap(world_coords, 1, 1, 1);

		this->current_animation->Reset();
		return true;
	}
	else
	{
		life -= *value;
	}

	return true;
}


void Tree::SaveAsDef(pugi::xml_node & node)
{
	//Add all this resource attributes at the xml node

	//Name
	node.append_attribute("name") = "Tree";

	/*Type*/	node.append_attribute("resource_type") = App->animator->EnumToResourceStr(resource_type);
	/*Selec X*/ node.append_attribute("selection_x") = selection_rect.x;
	/*Selec Y*/ node.append_attribute("selection_y") = selection_rect.y;
	/*Selec W*/ node.append_attribute("selection_w") = selection_rect.w;
	/*Selec H*/ node.append_attribute("selection_h") = selection_rect.h;

	/*Icon X*/	node.append_attribute("icon_x") = icon_rect.x;
	/*Icon Y*/	node.append_attribute("icon_y") = icon_rect.y;
	/*Icon W*/	node.append_attribute("icon_w") = icon_rect.w;
	/*Icon H*/	node.append_attribute("icon_h") = icon_rect.h;

	/*Mark X*/	node.append_attribute("mark_x") = mark.GetPosition().x;
	/*Mark Y*/	node.append_attribute("mark_y") = mark.GetPosition().y;
	/*Mark W*/	node.append_attribute("mark_w") = mark.GetWidth();

	/*Interact rad*/	node.append_attribute("interaction_rad") = interact_area.GetRad();
	/*Vision rad*/		node.append_attribute("vision_rad") = vision.GetRad();

	/*Max Resouces*/	node.append_attribute("max_resources") = max_life;
	/*Cortex Value*/	node.append_attribute("cortex") = cortex;
}

bool Tree::Draw(bool debug)
{

	bool ret = false;
	//Draw Resource Mark
	if (selected)
	{
		ret = mark.Draw();
	}
	if (debug)
	{
		interact_area.Draw();
	}

	/*if (debug) {
	//Draw Entity Selection Rect
	App->render->DrawQuad({ (int)floor(position.x + selection_rect.x - selection_rect.w * 0.5f),(int)position.y + selection_rect.y, selection_rect.w,-selection_rect.h }, 50, 155, 255, 100, true);

	//Draw axis lines to check the center of the unit (tool used during the sprites allocation)
	int length = 55;
	iPoint p1 = { (int)position.x, (int)position.y - length };
	iPoint p2 = { (int)position.x, (int)position.y + length };
	SDL_Color color = { 50,255,200,255 };
	Line y_axis(p1, p2, color);
	p1.x -= length;
	p1.y += length;
	p2.x += length;
	p2.y -= length;
	Line x_axis(p1, p2, color);
	y_axis.Draw();
	x_axis.Draw();
	}*/

	if (this->resource_type != RESOURCE_TYPE::CHOP)
	{
		const std::vector<Sprite>* sprites = current_animation->GetAllSprites();

		uint size = sprites->size();
		for (uint k = 0; k < size; k++)
		{
			ret = App->render->CallBlit(current_animation->GetTexture(), position.x - sprites->at(k).GetXpivot(), position.y - sprites->at(k).GetYpivot(), sprites->at(k).GetFrame(), false, -position.y - sprites->at(k).GetZ_cord(), sprites->at(k).GetOpacity(), 0, 0, blit_color);
			if (!ret)break;
		}

	}
	else
	{
		const Sprite* sprite = current_animation->GetCurrentSprite();

		ret = App->render->CallBlit(current_animation->GetTexture(), position.x - sprite->GetXpivot(), position.y - sprite->GetYpivot(), sprite->GetFrame(), false, -position.y - sprite->GetZ_cord(), sprite->GetOpacity(), 0, 0, blit_color);

		if (current_animation->IsEnd())return false;
	}
	return ret;
}


