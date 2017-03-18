#include "Hud_SelectionPanel.h"

//Modules
#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"

//GUI Elements
#include "UI_String.h"


//Entityes Elements
#include "BaseEntities.h"

//Entity Profile Constructor =====================================================
Entity_Profile::Entity_Profile() : element(nullptr), e_type(NO_ENTITY), isenemy(false), life_update(0), u_attack(0), u_deffence(0), u_range(0),
u_capacity(0), u_resources(0), m_capacity(0), m_life(0)
{
	name = (UI_String*)App->gui->GenerateUI_Element(STRING);
	name->SetColor({ 0,0,0,255 });

	diplomacy = (UI_String*)App->gui->GenerateUI_Element(STRING);
	diplomacy->SetColor({ 255,0,0,0 });
	diplomacy->SetString("Enemy");

	//life
	life = (UI_String*)App->gui->GenerateUI_Element(STRING);
	life->SetColor({ 0,0,0,255 });

	//Units and Buildings
	attack = (UI_String*)App->gui->GenerateUI_Element(STRING);
	attack->SetColor({ 0,0,0,255 });

	deffence = (UI_String*)App->gui->GenerateUI_Element(STRING);
	deffence->SetColor({ 0,0,0,255 });

	range = (UI_String*)App->gui->GenerateUI_Element(STRING);
	range->SetColor({ 0,0,0,255 });

	//Buildings
	capacity = (UI_String*)App->gui->GenerateUI_Element(STRING);
	capacity->SetColor({ 0,0,0,255 });

	//resources
	resources = (UI_String*)App->gui->GenerateUI_Element(STRING);
	resources->SetColor({ 0,0,0,255 });

}

//Entity Profile Destructor ======================================================
Entity_Profile::~Entity_Profile()
{
}

//Functionality ==================================================================
void Entity_Profile::SetEntity(Entity * entity_selected)
{
	element = entity_selected;

	name->SetString((char*)element->GetName());

	life_update = element->GetLife();
	if (life_update > 0)
	{
		m_life = element->GetMaxLife();
		profile_text.clear();
		profile_text = App->gui->SetStringFromInt(life_update);
		profile_text = profile_text + "/" + App->gui->SetStringFromInt(m_life);
		life->SetString((char*)profile_text.c_str());
	}

	if (element->GetDiplomacy() == ENEMY)
	{
		isenemy = true;
	}
	else isenemy = false;

	e_type = element->GetEntityType();
	if (e_type == UNIT)
	{
		u_attack = ((Unit*)element)->GetAttackHitPoints();
		if (u_attack > 0)attack->SetString(App->gui->SetStringFromInt(u_attack));

		u_deffence = ((Unit*)element)->GetDefense();
		if (u_deffence > 0)deffence->SetString(App->gui->SetStringFromInt(u_deffence));

		u_range = ((Unit*)element)->GetAttackRange();
		if (u_range > 0) range->SetString(App->gui->SetStringFromInt(u_range));
	}
	else if (e_type == BUILDING)
	{
		u_capacity = ((Building*)element)->GetCurrentUnits();
		m_capacity = ((Building*)element)->GetUnitsCapacity();
		profile_text.clear();
		profile_text = App->gui->SetStringFromInt(u_capacity);
		profile_text = profile_text + "/" + App->gui->SetStringFromInt(m_capacity);
		capacity->SetString((char*)profile_text.c_str());
	}
	else if (e_type = RESOURCE)
	{
		u_resources = ((Resource*)element)->GetMaxResources();
		resources->SetString(App->gui->SetStringFromInt(u_resources));
	}
}

Entity * Entity_Profile::GetEntity()
{
	return element;
}

void Entity_Profile::DrawProfile() const
{
	//Draw profile background
	App->render->DrawQuad({ 338 - App->render->camera.x, 632 - App->render->camera.y, 39, 38 }, 148, 148, 148);

	//Draw profile icon
	App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 634 - App->render->camera.y, &element->GetIcon());

	name->DrawAt(337, 610);
	//civilization->DrawAt(472, 640)
	if (isenemy) diplomacy->DrawAt(472, 660);

	//Draw life
	if (life_update != 0)
	{
		App->render->DrawQuad({ 340 - App->render->camera.x, 670 - App->render->camera.y, 36, 3 }, 255, 0, 0);
		App->render->DrawQuad({ 340 - App->render->camera.x, 670 - App->render->camera.y, 36 * (m_life / life_update), 3 }, 0, 255, 0);

		life->DrawAt(380, 660);
	}

	SDL_Rect rect = { 0,0,0,0 };
	if (e_type == UNIT)
	{
		if (u_attack > 0)
		{
			rect = { 372,287,36,23 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 680 - App->render->camera.y, &rect);
			attack->DrawAt(390, 680);
		}
		if (u_deffence > 0)
		{
			rect = { 336,287,36,23 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 700 - App->render->camera.y, &rect);
			deffence->DrawAt(390, 700);
		}
		if (u_range > 0)
		{
			rect = { 300,287,36,21 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 720 - App->render->camera.y, &rect);
			range->DrawAt(390, 720);
		}
	}
	else if (e_type == BUILDING)
	{
		if (m_capacity != 0)
		{
			rect = { 624,287,36,21 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 680 - App->render->camera.y, &rect);
			capacity->DrawAt(390, 680);
		}
		if (u_attack > 0)
		{
			rect = { 372,287,36,23 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 700 - App->render->camera.y, &rect);
			attack->DrawAt(390, 700);
		}
		if (u_range > 0)
		{
			rect = { 300,287,36,21 };
			App->render->Blit(App->gui->Get_UI_Texture(ICONS), 340 - App->render->camera.x, 720 - App->render->camera.y, &rect);
			range->DrawAt(390, 720);
		}
	}
	else if (e_type == RESOURCE)
	{
		resources->DrawAt(390, 680);
	}
}

void Entity_Profile::UpdateStats()
{
	if (element->GetLife() != life_update)
	{
		life_update = element->GetLife();
		profile_text.clear();
		profile_text = App->gui->SetStringFromInt(life_update);
		profile_text = profile_text + "/" + App->gui->SetStringFromInt(element->GetMaxLife());
		life->SetString((char*)profile_text.c_str());
	}
	if (e_type == BUILDING)
	{
		if (u_capacity != ((Building*)element)->GetCurrentUnits())
		{
			u_capacity = ((Building*)element)->GetCurrentUnits();
			profile_text.clear();
			profile_text = App->gui->SetStringFromInt(u_capacity);
			profile_text = profile_text + "/" + App->gui->SetStringFromInt(m_capacity);
			capacity->SetString((char*)profile_text.c_str());
		}
	}
	if (e_type == RESOURCE)
	{
		if (u_resources != ((Resource*)element)->GetMaxResources())
		{
			u_resources = ((Resource*)element)->GetMaxResources();
			resources->SetString(App->gui->SetStringFromInt(u_resources));
		}
	}
}


Selection_Panel::Selection_Panel()
{
}

Selection_Panel::~Selection_Panel()
{
}

bool Selection_Panel::Start()
{
	return true;
}

bool Selection_Panel::PreUpdate()
{
	return true;
}

bool Selection_Panel::PostUpdate()
{
	return true;
}

bool Selection_Panel::CleanUp()
{
	return true;
}

bool Selection_Panel::Draw()
{
	return true;
}
