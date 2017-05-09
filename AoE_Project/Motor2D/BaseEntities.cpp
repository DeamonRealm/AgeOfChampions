#include "BaseEntities.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Animator.h"
#include "j1Pathfinding.h"
#include "j1Fonts.h"
#include "j1Map.h"
#include "p2Log.h"
#include "j1EntitiesManager.h"
#include "j1ActionManager.h"
#include "j1Player.h"
#include "j1BuffManager.h"
#include "j1FogOfWar.h"
#include "Actions_Unit.h"

#include "Hud_GamePanel.h"
#include "Hud_MinimapPanel.h"

///Class Entity ---------------------------------
//Constructors ========================
Entity::Entity() :name(""), action_worker()
{

}

Entity::Entity(const Entity& copy) : name(copy.name), position(copy.position), entity_type(copy.entity_type), entity_diplomacy(copy.entity_diplomacy), vision(copy.vision), selection_rect(copy.selection_rect),
icon_rect(copy.icon_rect), max_life(copy.max_life), life(copy.life), life_position(copy.life_position), current_animation(copy.current_animation), action_worker(), blit_color(copy.blit_color)
{

}

//Destructors =========================
Entity::~Entity()
{
	if (current_animation != nullptr)
	{
		delete current_animation;
	}

	current_animation = nullptr;
}


//Functionality =======================
//Select/Deselect -
void Entity::Select()
{
	selected = true;
}

void Entity::Deselect()
{
	selected = false;
}

//Operators -------------
bool Entity::operator == (const Entity& tar)
{
	return (position == tar.position && entity_type == tar.entity_type && tar.life == life);
}

//Update ----------
bool Entity::Update()
{
	action_worker.Update();
	return true;
}

bool Entity::Die()
{
	return true;
}

// Draw -----------
bool Entity::Draw(bool debug)
{
	bool ret = false;

	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), 255, sprite->GetXpivot(), sprite->GetYpivot(), &blit_color);

	return ret;
}

void Entity::DrawLife()
{
	if (entity_type == RESOURCE) return;
	App->render->DrawQuad({ (int)position.x - life_position.x, (int)position.y - life_position.y, life_position.x*2, 2 }, 255, 0, 0);
	App->render->DrawQuad({ (int)position.x - life_position.x, (int)position.y - life_position.y, (life_position.x) * 2 * life / (int)max_life, 2 }, 0, 255, 0);
}

void Entity::CleanAnimation()
{
	if (current_animation != nullptr)
	{
		delete current_animation;
		current_animation = nullptr;
	}
}

void Entity::CleanMapLogic()
{

}

void Entity::SetBlitColor(const SDL_Color new_color)
{
	blit_color = new_color;
}

void Entity::ResetFogAround()
{

}

void Entity::CleanFogAround()
{
	//Collect the new cells in vision area
	in_vision_cells = App->fog_of_war->ClearAlphaLayer(vision);
	if (!in_vision_cells.empty())
	{
		in_vision_tiles = App->fog_of_war->ClearFogLayer(vision, NO_FOG);
		App->fog_of_war->ClearFogLayer(render_area, GRAY_FOG, false);
	}
}

void Entity::CheckFogAround()
{
	this->ResetFogAround();
	in_vision_cells = App->fog_of_war->ClearAlphaLayer(vision, 0);
	in_vision_tiles = App->fog_of_war->ClearFogLayer(vision, NO_FOG);
	App->fog_of_war->ClearFogLayer(render_area, GRAY_FOG, false);
}

//Add Action ------------
void Entity::AddAction(Action * action, TASK_CHANNELS channel)
{
	action_worker.AddAction(action, channel);
}

void Entity::AddPriorizedAction(Action * action, TASK_CHANNELS channel)
{
	action_worker.AddPriorizedAction(action, channel);
}

void Entity::PopAction(Action * action)
{
	action_worker.PopAction(action);
}

//Set Methods -----
void Entity::SetName(const char * name_str)
{
	name = name_str;
}

void Entity::SetPosition(float x, float y, bool insert)
{
	position.x = x;
	position.y = y;
}

void Entity::OnlySetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
}

void Entity::SetEntityType(ENTITY_TYPE type)
{
	entity_type = type;
}

void Entity::SetDiplomacy(DIPLOMACY new_diplomacy)
{
	entity_diplomacy = new_diplomacy;
}

void Entity::SetVision(const Circle & new_vision)
{
	vision = new_vision;
}

void Entity::SetRenderArea(const Circle & new_render_area)
{
	render_area = new_render_area;
}

void Entity::SetMaxLife(uint full_life_val)
{
	max_life = full_life_val;
}

void Entity::SetLife(uint life_val)
{
	life = life_val;
}

void Entity::SetLifeBarPosition(int x, int y)
{
	life_position = { x,y };
}

void Entity::SetAnimation(Animation * anim)
{
	current_animation = anim;
}

void Entity::SetFlipSprite(bool flip)
{
	flip_sprite = flip;
}

void Entity::SetSelectionRect(const SDL_Rect & rect)
{
	selection_rect = rect;
}

void Entity::SetIcon(const SDL_Rect & icon)
{
	icon_rect = icon;
}

// ----------------
//Get Methods -----
const char* Entity::GetName() const
{
	return name.c_str();
}

const fPoint& Entity::GetPosition() const
{
	return position;
}

iPoint Entity::GetPositionRounded() const
{
	return iPoint(position.x, position.y);
}

ENTITY_TYPE Entity::GetEntityType() const
{
	return entity_type;
}

DIPLOMACY Entity::GetDiplomacy() const
{
	return entity_diplomacy;
}

Circle Entity::GetVision() const
{
	return vision;
}

Circle Entity::GetRenderArea() const
{
	return render_area;
}

uint Entity::GetMaxLife() const
{
	return max_life;
}

int Entity::GetLife() const
{
	return life;
}

Animation* Entity::GetAnimation()
{
	return current_animation;
}

bool Entity::GetFlipSprite() const
{
	return flip_sprite;
}

bool Entity::GetIfSelected() const
{
	return selected;
}

const SDL_Rect * Entity::GetSelectionRect() const
{
	return &selection_rect;
}

const SDL_Rect& Entity::GetIcon()const
{
	return icon_rect;
}

ActionWorker * Entity::GetWorker()
{
	return &action_worker;
}
// ----------------
///----------------------------------------------


///Class Unit -----------------------------------
//Constructors ========================
Unit::Unit() :Entity()
{

}

Unit::Unit(const Unit& copy) : Entity(copy), unit_type(copy.unit_type), mark(copy.mark), soft_collider(copy.soft_collider), hard_collider(copy.hard_collider),
speed(copy.speed), action_type(copy.action_type), direction_type(copy.direction_type), attack_hitpoints(copy.attack_hitpoints),
attack_rate(copy.attack_rate), attack_type(copy.attack_type), attack_area(copy.attack_area), defense(copy.defense), unit_class(copy.unit_class),
food_cost(copy.food_cost), wood_cost(copy.wood_cost), gold_cost(copy.gold_cost), population_cost(copy.population_cost), train_time(copy.train_time), unit_experience(copy.unit_experience)
{

}

//Destructors =========================
Unit::~Unit()
{

}

void Unit::SaveAsDef(pugi::xml_node & node)
{
	//Add all this unit attributes at the xml node

	//Name
	std::string name;
	switch (unit_type)
	{
	case MILITIA:				name = "Milita";				break;
	case ARBALEST:				name = "Arbalest";				break;
	case ARCHER:				name = "Archer";				break;
	case CAVALIER:				name = "Cavalier";				break;
	case CAVALRY_ARCHER:		name = "Cavalry Archer";		break;
	case ELITE_SKIRMISHER:		name = "Elite Skirmisher";		break;
	case HEAVY_CAVALRY_ARCHER:	name = "Heavy Cavalry Archer";	break;
	case KNIGHT:				name = "Knight";				break;
	case MONK:					name = "Monk";					break;
	case PALADIN:				name = "Paladin";				break;
	case PIKEMAN:				name = "Pikeman";				break;
	case SPEARMAN:				name = "Spearman";				break;
	case TWO_HANDED_SWORDMAN:	name = "Two Handed Swordman";	break;
	case MAN_AT_ARMS:			name = "Man At Arms";			break;
	case LONG_SWORDMAN:			name = "Long Swordman";			break;
	case CHAMPION:				name = "Champion";				break;
	case CROSSBOWMAN:			name = "Crossbowman";			break;
	case SKIRMISHER:			name = "Skirmisher";			break;
	}
	node.append_attribute("name") = name.c_str();

	/*Type*/	node.append_attribute("unit_type") = App->animator->EnumToUnitStr(unit_type);
	/*Class*/	node.append_attribute("unit_class") = App->animator->EnumToUnitClassStr(unit_class);
	/*Atk Type*/node.append_attribute("attack_type") = App->animator->EnumToAttackStr(attack_type);

	/*Selec X*/ node.append_attribute("selection_x") = selection_rect.x;
	/*Selec Y*/ node.append_attribute("selection_y") = selection_rect.y;
	/*Selec W*/ node.append_attribute("selection_w") = selection_rect.w;
	/*Selec H*/ node.append_attribute("selection_h") = selection_rect.h;

	/*Icon X*/	node.append_attribute("icon_x") = icon_rect.x;
	/*Icon Y*/	node.append_attribute("icon_y") = icon_rect.y;
	/*Icon W*/	node.append_attribute("icon_w") = icon_rect.w;
	/*Icon H*/	node.append_attribute("icon_h") = icon_rect.h;

	/*Vision rad*/	node.append_attribute("vision_rad") = vision.GetRad();
	/*Mark Rad*/	node.append_attribute("mark_rad") = mark.GetRad();
	/*Soft Rad*/	node.append_attribute("soft_rad") = soft_collider.GetRad();
	/*Hard Rad*/	node.append_attribute("hard_rad") = hard_collider.GetRad();

	/*Max Life*/	node.append_attribute("max_life") = max_life;
	/*Life Pos X*/	node.append_attribute("life_pos_x") = life_position.x;
	/*Life Pos Y*/	node.append_attribute("life_pos_y") = life_position.y;
	/*Speed*/		node.append_attribute("speed") = speed;
	/*Atk HitP*/	node.append_attribute("attack_hitpoints") = attack_hitpoints;
	/*Atk Rate*/	node.append_attribute("attack_rate") = attack_rate;
	/*Atk Range*/	node.append_attribute("attack_range") = attack_area.GetRad();
	/*Defense*/		node.append_attribute("defense") = defense;
	/*Food Cost*/	node.append_attribute("food_cost") = food_cost;
	/*Wood Cost*/	node.append_attribute("wood_cost") = wood_cost;
	/*Gold Cost*/	node.append_attribute("gold_cost") = gold_cost;
	/*Popu Cost*/	node.append_attribute("population_cost") = population_cost;
	/*Train Time*/	node.append_attribute("train_time") = train_time;
	/*Experience*/	node.append_attribute("unit_experience") = unit_experience;
}

bool Unit::operator<(const Unit & target) const
{
	return distance_to_target < target.distance_to_target;
}

//Functionality =======================
//Draw ------------
bool Unit::Draw(bool debug)
{
	bool ret = false;

	//Draw Entity Mark
	if (selected)
	{
		ret = mark.Draw();
	}

	if (debug) {
		if (selected)
			attack_area.Draw();
		soft_collider.Draw();
		hard_collider.Draw();
		vision.Draw();
		render_area.Draw();
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
	if (path != nullptr)DrawPath();

	}*/

	//Draw Entity Current animation frame
	const Sprite* sprite = current_animation->GetCurrentSprite();
	ret = App->render->CallBlit(((DiplomaticAnimation*)current_animation)->GetTexture(entity_diplomacy), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot(), &blit_color);

	return ret;
}


//Actions ---------
bool Unit::Move(std::vector<iPoint>*& path, const iPoint& target, Unit* unit_target) ///Returns true when it ends
{
	//Check if the unit have an assigned path
	if (path == nullptr)
	{
		LOG("Error path not found!");
		return true;
	}
	last_position = GetPositionRounded();
	/*
	if (unit_target != nullptr)
	{
		if (current_distance == 0)
			current_distance = last_position.DistanceTo(*(path->begin()));
		int target_ditance = last_position.DistanceTo(unit_target->GetPositionRounded());

		if (current_distance + MAX_DISTANCE  > target_ditance|| current_distance - MAX_DISTANCE<target_ditance)
		{
			current_distance = target_ditance;
			iPoint goal = this->attack_area.NearestPoint(&unit_target->soft_collider);
			std::vector<iPoint>* new_path;
			new_path = App->pathfinding->SimpleAstar(GetPositionRounded(), goal);
			if (new_path == nullptr)return true;
			if (new_path != nullptr) {
				path->clear();
				delete path;
				path = new_path;
				current_distance = last_position.DistanceTo(*(path->begin()));
			}
		}
	}
	*/
	//Build goal path point
	iPoint goal = path->back();
	iPoint location = iPoint(position.x, position.y);
	float x_step = 0.0f;
	float y_step = 0.0f;
	int path_size = path->size();
	//Update goal node and animation direction
	if (location.DistanceTo(goal) < 2)
	{
		if (future_position != iPoint(-1, -1) && !App->pathfinding->IsWalkable(App->map->WorldToMap(future_position.x, future_position.y)))
		{
			CorrectPath(path);
			if (path->empty())
			{
				delete path;
				path = nullptr;
				return true;
			}
			goal = NextGoal(path);
			NewPosition(goal, x_step, y_step);
			//Add the calculated values at the unit & mark position
			SetPosition(x_step, y_step);
			return false;
		}
		//Look in the next update if there is an error
		std::vector<Unit*> other_units;
		App->entities_manager->units_quadtree.CollectCandidates(other_units, vision);
		other_units.size();
		int collisions = 0;
		while (!other_units.empty()) {
			Unit* other_unit = other_units.back();
			other_units.pop_back();
			if (other_unit == this)	continue;			
			int other_path_size = 0;
			std::vector<iPoint>* other_path = nullptr;
			if (other_unit->GetPath() != nullptr)
			{
				other_path = other_unit->GetPath();
				other_path_size = other_path->size();
			}
			switch (CheckColision(this, other_unit))
			{
			case NO_COLLISION:
				break;
			case COLLISION_IDLE:
			{
				if (unit_target != nullptr)
					if (unit_target == other_unit)
						return true;
				if (path->size() < 2) {
					if (target != iPoint(-1, -1))
					{
						if (!Repath(path, target)) return true;
					}
					else
					{
						if (!Repath(path, *(path->begin()))) return true;
					}
					return false;
				}
			}
			break;
			case COLLISION_MOVE:
				if (unit_target != nullptr)
					if (unit_target == other_unit)
						return true;
				//if we have a collision with other unit and we have lower priority reduction of speed
				if (other_path_size < 2 && path_size < 2 && future_position == other_unit->GetPositionRounded())
				{
					if (location.DistanceTo(goal) < other_unit->GetPositionRounded().DistanceTo(goal))
					{
						if (!other_unit->Repath(other_path, *(other_path->begin())))	return true;
					}
					else
					{
						if (!Repath(path, *(path->begin()))) return true;
					}
				}
				else
				{
					if (mutable_speed == 0 && other_unit->mutable_speed == 0)
					{
						if (location.DistanceTo(goal) < other_unit->GetPositionRounded().DistanceTo(goal))
							other_unit->mutable_speed -= 0.1;
						else
							mutable_speed -= 0.1;
					}
				}
				collisions++;
				break;
			case FUTURE_COLLISION_MOVE:			
				break;
			}
		}
		if (unit_target != nullptr&&collisions == 0)
		{
			int target_ditance = last_position.DistanceTo(unit_target->GetPositionRounded());
			LOG("target_ditance %i", target_ditance);
			if (MAX_DISTANCE > target_ditance)
				return true;
		}
		if (collisions == 0 && mutable_speed != 0.0f) 
			mutable_speed = 0.0f;		
		if (path->size() == 1)
		{
			//Set unit at the goal pixel position
			SetPosition((float)goal.x, (float)goal.y);
			//Delete unit path
			delete path;
			path = nullptr;
			return true;
		}
		//Set the unit next tile goal
		goal = NextGoal(path);
	}
	//Check actor animation
	if (action_type != WALK)
	{
		action_type = WALK;
		Focus(goal, false);
	}
	//Calculate the X/Y values that the unit have to move 
	//checking the goal location and the unit movement speed
	NewPosition(goal, x_step, y_step);
	//Add the calculated values at the unit & mark position
	SetPosition(x_step, y_step);
	return false;
}

void Unit::ResetDistance()
{
	current_distance = 0;
}


iPoint Unit::Spawn(const iPoint& position)
{
	int radius = GetSoftCollider().GetRad();
	int doble_radius = radius * 2;
	iPoint unit_position = position;
	iPoint unit_map_position = App->map->WorldToMap(unit_position.x, unit_position.y);
	if (UnitHere(unit_position, doble_radius) || !App->pathfinding->IsWalkable(unit_map_position) )
		unit_position=FindSpawnCell(unit_position);

	return unit_position;
}

bool Unit::Repath(std::vector<iPoint>*& path, const iPoint & destination)
{
	iPoint new_destination = FindWalkableCell(destination);
	std::vector<iPoint>* new_path;
	new_path = App->pathfinding->SimpleAstar(GetPositionRounded(), new_destination);
	if (new_destination == iPoint(-1, -1)|| !App->pathfinding->IsWalkable(App->map->WorldToMap(new_destination.x, new_destination.y)))
	{
		//LOG("NOOOOOO");
	}
	if (new_path != nullptr) {

		path->clear();
		delete path;

		path = new_path;
		return true;
	}
	return false;

}



void Unit::CorrectPath(std::vector<iPoint>*& path)
{
	std::vector<iPoint>* new_path;
	path->pop_back();
	iPoint next_goal;
	for (int i = path->size() - 1; i >= 0; i--) {
		if (App->pathfinding->IsWalkable(App->map->WorldToMap(path->at(i).x, path->at(i).y))) {
			next_goal = path->at(i);
			break;
		}
		else
			path->pop_back();

	}
	if (path->empty())
	{
	
		return;
	}
	new_path = App->pathfinding->SimpleAstar(GetPositionRounded(), next_goal);
	if (new_path != nullptr) {
		LOG("PATH SIZE %i", new_path->size());
		path->insert(path->end(), new_path->begin(), new_path->end());
		delete new_path;
	}
}

iPoint Unit::NextGoal(std::vector<iPoint>* path)
{
	iPoint goal;
	path->pop_back();

	if (path->size() >= 2)	SetFutureAction(*(path->rbegin() + 1));

	else					SetFutureAction(*(path->rbegin()));

	goal = path->back();

	//Focus the unit at the next goal
	Focus(goal, false);
	return goal;
}

void Unit::NewPosition(const iPoint& goal, float & position_x, float & position_y)
{
	iPoint location = iPoint(position.x, position.y);

	int norm = location.DistanceTo(goal);
	float x_step = GetTotalSpeed() * (App->GetDT() * 100) * (goal.x - location.x) / norm;
	float y_step = GetTotalSpeed() * (App->GetDT() * 100) * (goal.y - location.y) / norm;

	this->distance_walked.x += x_step;
	this->distance_walked.y += y_step;

	//Add the calculated values at the unit & mark position
	position_x = position.x + x_step;
	position_y = position.y + y_step;

}





bool Unit::UnitHere(const iPoint & destination, int radius)
{
	std::vector<Unit*> other_units;
	Circle area;
	area.SetPosition(destination);

	area.SetRad(radius);
	App->entities_manager->units_quadtree.CollectCandidates(other_units, area);
	area.SetRad(this->GetHardCollider().GetRad());
	if (!other_units.empty()) {
		for (int i = 0; i < other_units.size(); i++)
		{
			if (other_units[i] == this) {
				continue;
			}
			if (other_units[i]->GetHardCollider().Overlap(&area))
			{
				return true;
			}
		}

	}

	return false;
}
bool Unit::SurroundedUnit()
{
	return iPoint(-1, -1) == FindWalkableAdjacent(this->GetPositionRounded());
}
iPoint Unit::FindWalkableCell(const iPoint & center)
{
	iPoint cell;
	iPoint cell_1;
	iPoint cell_2;
	iPoint pos = center;
	iPoint goal;
	iPoint cell_map1;
	iPoint cell_map2;

	int radius = GetSoftCollider().GetRad();
	int doble_radius = radius * 2;
	iPoint unit_location = this->GetPositionRounded();
	int i = 1;
	bool look_1 = false;
	bool look_2 = false;
	int look = rand() % 2;
	//LOG("%i", look);
	switch (direction_type)
	{
	case NORTH:
	//	LOG("NORTH");
		while (look_1 != true || look_2 != true) {
			switch (look)
			{
			case 0:
				cell_1.create(pos.x + radius, pos.y);
				cell_map1 = App->map->WorldToMap(cell_1.x, cell_1.y);
				if (App->pathfinding->IsWalkable(cell_map1) || !UnitHere(cell_1, doble_radius))
				{
					direction_type = EAST;
					App->animator->UnitPlay(this);

					return cell_1;
				}
				look_1 = true;
				look = 1;
				break;
			case 1:
				cell_2.create(pos.x - radius, pos.y);
				cell_map2 = App->map->WorldToMap(cell_2.x, cell_2.y);
				if (App->pathfinding->IsWalkable(cell_map2) || !UnitHere(cell_2, doble_radius))
				{
					direction_type = WEST;
					App->animator->UnitPlay(this);
					return cell_2;
				}
				look_2 = true;
				look = 0;
				break;
			default:
				break;
			}
		}
		

			
		
		break;
	case NORTH_EAST:
	//	LOG("NORTH_EAST");
		while (look_1 != true || look_2 != true) {
			switch (look)
			{
			case 0:
				cell_1.create(pos.x + radius, pos.y + radius);
				cell_map1 = App->map->WorldToMap(cell_1.x, cell_1.y);
				if (App->pathfinding->IsWalkable(cell_map1) || !UnitHere(cell_1, doble_radius))
				{
					direction_type = SOUTH_EAST;
					App->animator->UnitPlay(this);
					return cell_1;
				}
				look_1 = true;
				look = 1;
				break;
			case 1:

				cell_2.create(pos.x - radius, pos.y - radius);
				cell_map2 = App->map->WorldToMap(cell_2.x, cell_2.y);
				if (App->pathfinding->IsWalkable(cell_map2) || !UnitHere(cell_2, doble_radius))
				{
					direction_type = NORTH_WEST;
					App->animator->UnitPlay(this);
					return cell_2;
				}
				look_2 = true;
				look = 0;
				break;
			default:
				break;
			}
		}
		break;
	case EAST:
	//	LOG("EAST");
		while (look_1 != true || look_2 != true) {
			switch (look)
			{
			case 0:
				cell_1.create(pos.x, pos.y + radius);
				cell_map1 = App->map->WorldToMap(cell_1.x, cell_1.y);
				if (App->pathfinding->IsWalkable(cell_map1) || !UnitHere(cell_1, doble_radius))
				{
					direction_type = SOUTH;
					App->animator->UnitPlay(this);
					return cell_1;
				}

				look_1 = true;
				look = 1;
				break;
			case 1:
				cell_2.create(pos.x, pos.y - radius);
				cell_map2 = App->map->WorldToMap(cell_2.x, cell_2.y);
				if (App->pathfinding->IsWalkable(cell_map2) || !UnitHere(cell_2, doble_radius))
				{
					direction_type = NORTH;
					App->animator->UnitPlay(this);
					return cell_2;
				}
				look_2 = true;
				look = 0;
				break;
			default:
				break;
			}
		}		
		break;
	case SOUTH_EAST:
		//LOG("SOUTH_EAST");
		while (look_1 != true || look_2 != true) {
			switch (look)
			{
			case 0:
				cell_1.create(pos.x + radius, pos.y - radius);
				cell_map1 = App->map->WorldToMap(cell_1.x, cell_1.y);
				if (App->pathfinding->IsWalkable(cell_map1) || !UnitHere(cell_1, doble_radius))
				{
					direction_type = NORTH_EAST;
					App->animator->UnitPlay(this);
					return cell_1;
				}

				look_1 = true;
				look = 1;
				break;
			case 1:
				cell_2.create(pos.x - radius, pos.y + radius);
				cell_map2 = App->map->WorldToMap(cell_2.x, cell_2.y);
				if (App->pathfinding->IsWalkable(cell_map2) || !UnitHere(cell_2, doble_radius))
				{
					direction_type = SOUTH_WEST;
					App->animator->UnitPlay(this);
					return cell_2;
				}
				look_2 = true;
				look = 0;
				break;
			default:
				break;
			}
		}	
		break;
	case SOUTH:
		//LOG("SOUTH");
		while ((look_1 != true || look_2 != true)) {
			switch (look)
			{
			case 0:
				cell_1.create(pos.x + radius, pos.y);
				cell_map1 = App->map->WorldToMap(cell_1.x, cell_1.y);
				if (App->pathfinding->IsWalkable(cell_map1) || !UnitHere(cell_1, doble_radius))
				{
					direction_type = EAST;
					App->animator->UnitPlay(this);
					return cell_1;
				}
				look_1 = true;
				look = 1;
				break;
			case 1:
				cell_2.create(pos.x - radius, pos.y);
				cell_map2 = App->map->WorldToMap(cell_2.x, cell_2.y);
				if (App->pathfinding->IsWalkable(cell_map2) || !UnitHere(cell_2, doble_radius))
				{
					direction_type = WEST;
					App->animator->UnitPlay(this);
					return cell_2;
				}
				look_2 = true;
				look = 0;
				break;
			default:
				break;
			}

		}	
		break;
	case SOUTH_WEST:
		//LOG("SOUTH_WEST");
		while (look_1 != true || look_2 != true) {
			switch (look)
			{
			case 0:
				cell_1.create(pos.x + radius, pos.y + radius);
				cell_map1 = App->map->WorldToMap(cell_1.x, cell_1.y);
				if (App->pathfinding->IsWalkable(cell_map1) || !UnitHere(cell_1, doble_radius))
				{
					direction_type = SOUTH_EAST;
					App->animator->UnitPlay(this);
					return cell_1;
				}
				look_1 = true;
				look = 1;
				break;
			case 1:
				cell_2.create(pos.x - radius, pos.y - radius);
				cell_map2 = App->map->WorldToMap(cell_2.x, cell_2.y);
				if (App->pathfinding->IsWalkable(cell_map2) || !UnitHere(cell_2, doble_radius))
				{
					direction_type = NORTH_WEST;
					App->animator->UnitPlay(this);
					return cell_2;
				}
				look_2 = true;
				look = 0;
				break;
			default:
				break;
			}
		}
		break;
	case WEST:
		//LOG("WEST");
		while (look_1 != true || look_2 != true) {
			switch (look)
			{
			case 0:
				cell_1.create(pos.x, pos.y + radius);
				cell_map1 = App->map->WorldToMap(cell_1.x, cell_1.y);
				if (App->pathfinding->IsWalkable(cell_map1) || !UnitHere(cell_1, doble_radius))
				{
					direction_type = SOUTH;
					App->animator->UnitPlay(this);
					return cell_1;
				}
				look_1 = true;
				look = 1;
				break;
			case 1:
				cell_2.create(pos.x, pos.y - radius);
				cell_map2 = App->map->WorldToMap(cell_2.x, cell_2.y);
				if (App->pathfinding->IsWalkable(cell_map2) || !UnitHere(cell_2, doble_radius))
				{
					direction_type = NORTH;
					App->animator->UnitPlay(this);
					return cell_2;
				}
				look_2 = true;
				look = 0;
				break;
			default:
				break;
			}
		}
		break;
	case NORTH_WEST:
		//LOG("NORTH_WEST");
		while (look_1 != true || look_2 != true) {
			switch (look)
			{
			case 0:
				cell_1.create(pos.x + radius, pos.y - radius);
				cell_map1 = App->map->WorldToMap(cell_1.x, cell_1.y);
				if (App->pathfinding->IsWalkable(cell_map1) || !UnitHere(cell_1, doble_radius))
				{
					direction_type = NORTH_EAST;
					App->animator->UnitPlay(this);
					return cell_1;
				}
				look_1 = true;
				look = 1;
				break;
			case 1:
				cell_2.create(pos.x - radius, pos.y + radius);
				cell_map2 = App->map->WorldToMap(cell_2.x, cell_2.y);
				if (App->pathfinding->IsWalkable(cell_map2) || !UnitHere(cell_2, doble_radius))
				{
					direction_type = SOUTH_WEST;
					App->animator->UnitPlay(this);
					return cell_2;
				}
				look_2 = true;
				look = 0;
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}

	if (unit_location.DistanceTo(cell_1) <= unit_location.DistanceTo(cell_2))
	{
		if (App->pathfinding->IsWalkable(cell_map1))
		{
			this->Focus(cell_1, false);
			return cell_1;
		}
			
	}
	else 
	{
		if (App->pathfinding->IsWalkable(cell_map2))
		{
			this->Focus(cell_2, false);
			return cell_2;
		}
	}
}

iPoint Unit::FindSpawnCell(const iPoint & center)
{
	iPoint cell;
	iPoint pos = center;
	iPoint cell_map;

	int radius = GetSoftCollider().GetRad();
	int doble_radius = radius * 2;
	iPoint unit_location = this->GetPositionRounded();
	int i = 1;
	while (i <= 5) {
		// south
		cell.create(pos.x, pos.y + radius*i);
		cell_map = App->map->WorldToMap(cell.x, cell.y);

		if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(cell, doble_radius))
		{
			return cell;
		}

		// north
		cell.create(pos.x, pos.y - radius*i);
		cell_map = App->map->WorldToMap(cell.x, cell.y);

		if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(cell, doble_radius))
		{
			return cell;
		}

		// east
		cell.create(pos.x + radius*i, pos.y);
		cell_map = App->map->WorldToMap(cell.x, cell.y);

		if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(cell, doble_radius))
		{
			return cell;
		}

		// west
		cell.create(pos.x - radius*i, pos.y);
		cell_map = App->map->WorldToMap(cell.x, cell.y);

		if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(cell, doble_radius))
		{
			return cell;
		}

		// south-east
		cell.create(pos.x + radius*i, pos.y + radius*i);
		cell_map = App->map->WorldToMap(cell.x, cell.y);

		if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(cell, doble_radius))
		{
			return cell;
		}
		// south-west
		cell.create(pos.x - radius*i, pos.y + radius*i);
		cell_map = App->map->WorldToMap(cell.x, cell.y);

		if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(cell, doble_radius))
		{
			return cell;
		}
		// north-east
		cell.create(pos.x + radius*i, pos.y - radius*i);
		cell_map = App->map->WorldToMap(cell.x, cell.y);

		if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(cell, doble_radius))
		{
			return cell;
		}
		// north-west
		cell.create(pos.x - radius*i, pos.y - radius*i);
		cell_map = App->map->WorldToMap(cell.x, cell.y);

		if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(cell, doble_radius))
		{
			return cell;
		}
		i++;
	}
	return iPoint(-1, -1);
}

iPoint Unit::FindWalkableAdjacent(const iPoint & center)
{
	iPoint cell;
	iPoint pos = center;
	iPoint goal;
	iPoint cell_map;
	int radius = GetSoftCollider().GetRad();
	int doble_radius = radius * 2;
	Circle target;
	target.SetRad(radius);
	target.SetPosition(center);
	Circle checker;
	checker.SetRad(radius);
	// south
	cell.create(pos.x, pos.y + doble_radius);
	checker.SetPosition(cell);
	goal = checker.NearestPoint(&target);
	cell_map = App->map->WorldToMap(cell.x, cell.y);
	if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(goal, radius))
	{
		return goal;
	}

	// north
	cell.create(pos.x, pos.y - doble_radius);
	checker.SetPosition(cell);
	goal = checker.NearestPoint(&target);

	if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(goal, radius))
	{
		return goal;
	}


	// east
	cell.create(pos.x + doble_radius, pos.y);
	checker.SetPosition(cell);
	goal = checker.NearestPoint(&target);


	if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(goal, radius))
	{
		return goal;
	}


	// west
	cell.create(pos.x - doble_radius, pos.y);
	checker.SetPosition(cell);
	goal = checker.NearestPoint(&target);


	if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(goal, radius))
	{
		return goal;
	}


	// south-east
	cell.create(pos.x + doble_radius, pos.y + doble_radius);
	checker.SetPosition(cell);
	goal = checker.NearestPoint(&target);


	if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(goal, radius))
	{
		return goal;
	}

	// south-west
	cell.create(pos.x - doble_radius, pos.y + doble_radius);
	checker.SetPosition(cell);
	goal = checker.NearestPoint(&target);


	if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(goal, radius))
	{
		return goal;
	}

	// north-east
	cell.create(pos.x + doble_radius, pos.y - doble_radius);
	checker.SetPosition(cell);
	goal = checker.NearestPoint(&target);


	if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(goal, radius))
	{
		return goal;
	}

	// north-west
	cell.create(pos.x - doble_radius, pos.y - doble_radius);
	checker.SetPosition(cell);
	goal = checker.NearestPoint(&target);


	if (App->pathfinding->IsWalkable(cell_map) && !UnitHere(goal, radius))
	{
		return goal;
	}


	return iPoint(-1, -1);
}

Unit* Unit::FindNewTarget()
{
	std::vector<Unit*> other_units;
	App->entities_manager->units_quadtree.CollectCandidates(other_units, vision);
	App->entities_manager->OrganizeByNearest(other_units, this->GetVision());

	Unit* unit = nullptr;
	iPoint goal;
	while (!other_units.empty())
	{
		unit = other_units.back();
		other_units.pop_back();

		if (unit->GetDiplomacy() == this->GetDiplomacy() || unit->GetAction() == DIE || unit->GetAction() == DISAPPEAR)
		{
			continue;
		}
		goal = FindWalkableAdjacent(unit->GetPositionRounded());
		if (goal != iPoint(-1, -1))
		{
			return unit;
		}
	}
	return nullptr;
}

void Unit::Focus(const iPoint & target, bool play)
{
	//Calculate the directional vector
	iPoint dir_point = target - iPoint(position.x, position.y);
	bool directed = false;
	//Find the correct direction in relation of the goal and the location
	if (abs(dir_point.x) < FOCUS_MARGIN)
	{
		if (dir_point.y > 0 && direction_type != SOUTH)
		{
			direction_type = DIRECTION_TYPE::SOUTH;
			directed = true;
		}
		else if (dir_point.y <= 0 && direction_type != NORTH)
		{
			direction_type = DIRECTION_TYPE::NORTH;
			directed = true;
		}
	}
	else if (abs(dir_point.y) < FOCUS_MARGIN)
	{
		if (dir_point.x > 0 && direction_type != EAST)
		{
			direction_type = DIRECTION_TYPE::EAST;
			directed = true;
		}
		else if (dir_point.x <= 0 && direction_type != WEST)
		{
			direction_type = DIRECTION_TYPE::WEST;
			directed = true;
		}
	}
	else if (dir_point.x >= 0 && dir_point.y >= 0 && direction_type != SOUTH_EAST)
	{
		direction_type = DIRECTION_TYPE::SOUTH_EAST;
		directed = true;
	}
	else if (dir_point.x <= 0 && dir_point.y >= 0 && direction_type != SOUTH_WEST)
	{
		direction_type = DIRECTION_TYPE::SOUTH_WEST;
		directed = true;
	}
	else if (dir_point.x >= 0 && dir_point.y <= 0 && direction_type != NORTH_EAST)
	{
		direction_type = DIRECTION_TYPE::NORTH_EAST;
		directed = true;
	}
	else if (dir_point.x <= 0 && dir_point.y <= 0 && direction_type != NORTH_WEST)
	{
		direction_type = DIRECTION_TYPE::NORTH_WEST;
		directed = true;
	}

	//Set the unit animation with the new direction
	if (play)
	{
		App->animator->UnitPlay(this);
	}
	else if (directed)
	{
		App->animator->UnitPlay(this);
	}
}

DIRECTION_TYPE Unit::LookDirection(const iPoint & from, const iPoint & to)
{
	//Calculate the directional vector
	iPoint dir_point = from - to;

	//Find the correct direction in relation of the goal and the location
	if (abs(dir_point.x) < FOCUS_MARGIN)
	{
		if (dir_point.y > 0)return SOUTH;
		else return NORTH;
	}
	else if (abs(dir_point.y) < FOCUS_MARGIN)
	{
		if (dir_point.x > 0)return EAST;
		else return WEST;
	}
	else if (dir_point.x >= 0 && dir_point.y >= 0)
	{
		return SOUTH_EAST;
	}
	else if (dir_point.x <= 0 && dir_point.y >= 0)
	{
		return SOUTH_WEST;
	}
	else if (dir_point.x >= 0 && dir_point.y <= 0)
	{
		return NORTH_EAST;
	}
	else if (dir_point.x <= 0 && dir_point.y <= 0)
	{
		return NORTH_WEST;
	}
	else {
		return NO_DIRECTION;
	}
}
bool Unit::AttackUnit(Unit* target)
{
	bool ret = false;
	//Check if the target is in the attack area
	if (target == nullptr || target->GetAction() == DIE || target->GetAction() == DISAPPEAR)
	{
		return true;
	}
	if (!attack_area.Overlap(&target->GetSoftCollider()))
	{

		iPoint goal = attack_area.NearestPoint(&target->GetSoftCollider());
		iPoint target_pos(-1,-1);
		if (GetAttackType() == MELEE)
		{
			target_pos = target->GetPositionRounded();
		}
		App->pathfinding->PushPath(this, goal, target_pos,true,PRIMARY,target);
/*
		std::vector<iPoint>* path = App->pathfinding->SimpleAstar(GetPositionRounded(), goal);
		if (path == nullptr)return true;
		this->AddPriorizedAction((Action*)App->action_manager->MoveAction(path, this));
*/
		return false;


	}

	//Control action rate
	if (action_timer.Read() < attack_rate)return false;

	//App->sound->PlayFXAudio(SWORD_ATTACK_SOUND);
	if (attack_type == DISTANCE)
	{
		const Sprite* sprite = this->current_animation->GetCurrentSprite();
		iPoint arrow_position(this->GetPositionRounded().x, this->GetPositionRounded().y-sprite->GetYpivot());
		iPoint arrow_destination(target->GetPositionRounded().x, target->GetPositionRounded().y);
		ShotArrow(arrow_position, arrow_destination);
	}
		//Set unit attack animation
	if (action_type != ATTATCK)
	{
		action_type = ATTATCK;
		Focus(target->GetPositionRounded());
		App->animator->UnitPlay(this);
	}

	//Calculate the attack & apply the value at the target life points
	//((Unit*)(*target))->life -= MIN(((Unit*)(*target))->life, attack_hitpoints);
	ret = target->DirectDamage(attack_hitpoints);
	if (ret || target->GetLife() <= 0)
	{
		ret = true;
		if (this->action_type == ATTATCK)
		{
			action_type = IDLE;
			App->animator->UnitPlay(this);
		}
	}
	//Reset action timer
	action_timer.Start();

	return ret;
}

bool Unit::HealUnit(Unit* target)
{
	bool ret = false;

	//Check if the target is in the attack area
	if (target == nullptr)
	{
		return true;
	}


	if (!attack_area.Overlap(&target->GetSoftCollider()))
	{

		iPoint goal = attack_area.NearestPoint(&target->GetSoftCollider() + attack_area.GetRad());
		App->pathfinding->PushPath(this, goal,target->GetPositionRounded());
		/*
		std::vector<iPoint>* path = App->pathfinding->SimpleAstar(GetPositionRounded(), goal);
		if (path == nullptr)return true;

		this->AddPriorizedAction((Action*)App->action_manager->MoveAction(path, this));
		*/
		return false;
	}


	//Control action rate
	if (action_timer.Read() < attack_rate)return false;

	//App->sound->PlayFXAudio(SWORD_ATTACK_SOUND);

	//Set unit attack animation
	if (action_type != ATTATCK)
	{
		action_type = ATTATCK;
		App->animator->UnitPlay(this);
	}
	Focus(target->GetPositionRounded());

	//Calculate the attack & apply the value at the target life points
	target->Heal(attack_hitpoints);

	//Reset action timer
	action_timer.Start();

	if (target->GetLife() >= target->GetMaxLife()) ret = true;
	return ret;
}

bool Unit::AttackBuilding(Building * target)
{
	if (target == nullptr)
	{
		return true;
	}

	//Check if the target is in the attack area
	if (!attack_area.Intersects((target)->GetInteractArea())&& target->GetLife() > 0)
	{

		iPoint goal = attack_area.NearestPoint(target->GetInteractArea());
		LOG("X: %i || Y: %i", goal.x, goal.y);
		App->pathfinding->PushPath(this, goal);

		return false;
	}

	//Control action rate
	if (action_timer.Read() < attack_rate)return false;

	//Set unit attack animation
	if (action_type != ATTATCK)
	{
		action_type = ATTATCK;
		Focus(target->GetPositionRounded());
		App->animator->UnitPlay(this);
	}

	if (target->GetLife() == 0)
	{
		ACTION_TYPE act = target->GetActionType();
		if (this->action_type == ATTATCK)
		{
			action_type = IDLE;
			App->animator->UnitPlay(this);
		}
		if (act != DIE && act != DISAPPEAR)target->AddPriorizedAction((Action*)App->action_manager->DieBuildngAction(target));
		return true;
	}

	//Calculate the attack & apply the value at the target life points
	target->SetLife(target->GetLife() - MIN(target->GetLife(), attack_hitpoints));

	//Reset action timer
	action_timer.Start();

	return false;
}

bool Unit::Cover()
{
	return true;
}

bool Unit::DirectDamage(uint damage)
{
	bool ret = false;
	if (unit_protected == true && tank != nullptr)
	{
		tank->DirectDamage(1);
		return false;
	}
	//Deal damage to the unit
	life -= MIN(life, damage);
	if (life <= 0)
	{
		ACTION_TYPE act = action_type;
		if (act != DIE && act != DISAPPEAR) {
			action_worker.HardReset();
			action_worker.AddAction(App->action_manager->DieAction(this));
			ret = true;
		}
	}
	return ret;
}

void Unit::Heal(uint heal)
{
	//Deal damage to the unit
	life = MIN(max_life, life + heal);
}

bool Unit::Die()
{
	if (action_type != DIE && action_type != DISAPPEAR)
	{
	if (GetDiplomacy() == DIPLOMACY::ALLY)
	{
		App->player->game_panel->IncreaseDeathAllies();
		App->entities_manager->GetExperienceFromUnit(unit_experience, DIPLOMACY::ALLY);
	}
	if (GetDiplomacy() == DIPLOMACY::ENEMY)
	{
		//App->player->game_panel->IncreaseDeathEnemies();
		App->entities_manager->GetExperienceFromUnit(unit_experience, DIPLOMACY::ENEMY);

		std::list<Unit*>::const_iterator it = App->entities_manager->UnitsList()->begin();
		bool lastenemy = true;
		while (it != App->entities_manager->UnitsList()->end())
		{
			if (it._Ptr->_Myval->GetDiplomacy() == ENEMY && it._Ptr->_Myval->GetEntityType() == UNIT && it._Ptr->_Myval != this)
				lastenemy = false;
			it++;
		}
		if (lastenemy && !App->debug_mode) App->player->game_panel->DoWin();
	}
		App->buff_manager->RemoveTargetBuffs(this);
		action_type = DIE;
		if (this->GetDiplomacy() == ALLY) App->player->game_panel->IncressPopulation(-1, false);
		App->entities_manager->AddDeathUnit(this);
		App->animator->UnitPlay(this);
		App->fog_of_war->ReleaseEntityFog(this);
	}
	else if (current_animation->IsEnd())
	{
		if (action_type == DIE)
		{
			action_type = DISAPPEAR;
			App->animator->UnitPlay(this);
		}
		else
		{
			if (GetDiplomacy() == DIPLOMACY::ENEMY)
			{
				App->player->game_panel->IncreaseDeathEnemies();
			}
			App->entities_manager->DeleteEntity(this);
			return true;
		}
	}
	return false;
}

bool Unit::Resurrect()
{

	if (action_type == DIE || action_type == DISAPPEAR)
	{
		this->Heal(5);
		App->entities_manager->ResurrectUnit(this);
		action_type = IDLE;
		App->animator->UnitPlay(this);
		action_worker.HardReset();
		App->action_manager->SetUnitAutoPassive(this);
	}

	return true;
}

void Unit::Stun(uint time)
{
	action_worker.AddPriorizedAction(App->action_manager->StunAction(this, time));
}

COLLISION_TYPE Unit::CheckColision(const Unit * current, const Unit * other)
{
	if (current == other)
	{
		return NO_COLLISION;
	}
	if (other->action_type == IDLE || other->action_type == ATTATCK)
	{
		if (sqrt((other->GetPosition().x - current->GetPosition().x) * (other->GetPosition().x - current->GetPosition().x) + (other->GetPosition().y - current->GetPosition().y) * (other->GetPosition().y - current->GetPosition().y)) < (current->hard_collider.GetRad() + other->hard_collider.GetRad()))
		{
			return COLLISION_IDLE;
		}
		return NO_COLLISION;
	}
	else if (other->action_type == WALK) {
		if (sqrt((other->future_position.x - current->future_position.x) * (other->future_position.x - current->future_position.x) + (other->future_position.y - current->future_position.y) * (other->future_position.y - current->future_position.y)) < (current->hard_collider.GetRad() + other->hard_collider.GetRad()))
		{
			return FUTURE_COLLISION_MOVE;
		}
		if (sqrt((other->GetPosition().x - current->GetPosition().x) * (other->GetPosition().x - current->GetPosition().x) + (other->GetPosition().y - current->GetPosition().y) * (other->GetPosition().y - current->GetPosition().y)) < (current->hard_collider.GetRad() + other->hard_collider.GetRad()))
		{
			return COLLISION_MOVE;
		}
		return NO_COLLISION;
	}
	return NO_COLLISION;
}

void Unit::ResetFogAround()
{
	//Reset unit locked cells
	uint size = in_vision_cells.size();
	for (uint k = 0; k < size; k++)
	{
		in_vision_cells[k]->locked = false;
	}

	//Reset unit locked tiles
	size = in_vision_tiles.size();
	for (uint k = 0; k < size; k++)
	{
		in_vision_tiles[k]->locked = false;
		in_vision_tiles[k]->type = GRAY_FOG;
	}

	in_vision_cells.clear();
	in_vision_tiles.clear();
	this->distance_walked.y = this->distance_walked.x = 0.01f;
}

void Unit::SetProtection(Warrior * warrior)
{
	unit_protected = true;
	tank = warrior;
}

void Unit::QuitProtection()
{
	unit_protected = false;
	tank = nullptr;

}

//Set Methods -----
void Unit::SetPosition(float x, float y, bool insert)
{
	//Extract the units to push it with the new position later
	if (insert)App->entities_manager->units_quadtree.Exteract(this, &position);

	//Set unit position
	position.x = x;
	position.y = y;
	iPoint pos(position.x, position.y);
	//Set unit vision position
	vision.SetPosition(pos);
	//Set unit render area position
	render_area.SetPosition(pos);

	//Check unit fog
	SDL_Point point = { pos.x,pos.y };
	int dist_mult = 5;
	if (SDL_PointInRect(&point, &App->render->camera_viewport))dist_mult = 2;
	if (entity_diplomacy == ALLY && (this->distance_walked.x == 0.00f || abs(distance_walked.x) + abs(distance_walked.y) > App->fog_of_war->alpha_cell_size * dist_mult))App->fog_of_war->CheckEntityFog(this);

	//Set unit mark position
	mark.SetPosition(pos);
	//Set soft_collider mark position
	soft_collider.SetPosition(pos);
	//Set hard_collider mark position
	hard_collider.SetPosition(pos);
	//Set unit attack area position
	attack_area.SetPosition(pos);

	//Add the unit with the correct position in the correct quad tree
	if (insert)
	{
		App->entities_manager->units_quadtree.Insert(this, &position);
	}
}

void Unit::SetFutureAction(const iPoint & position)
{
	future_position = position;
}

void Unit::SetAttackBuff(float atk_buff)
{
	attack_buff += atk_buff;
}

void Unit::SetUnitType(UNIT_TYPE type)
{
	unit_type = type;
}

void Unit::SetUnitClass(UNIT_CLASS type)
{
	unit_class = type;
}

void Unit::SetMark(const Circle & new_mark)
{
	mark = new_mark;
}

void Unit::SetSoftCollider(const Circle & new_soft_collider)
{
	soft_collider = new_soft_collider;
}

void Unit::SetHardCollider(const Circle & new_hard_collider)
{
	hard_collider = new_hard_collider;
}

void Unit::SetSpeed(float speed_val)
{
	speed = speed_val;
}

void Unit::SetSpeedBuff(float speed_val)
{
	speed_buff = speed_buff;
}

void Unit::SetAction(ACTION_TYPE action_val)
{
	action_type = action_val;
}

void Unit::SetDirection(DIRECTION_TYPE direction_val)
{
	direction_type = direction_val;
}

void Unit::SetAttackHitPoints(uint atk_val)
{
	attack_hitpoints = atk_val;
}

void Unit::SetAttackRate(uint atk_rate)
{
	attack_rate = atk_rate;
}

void Unit::SetAttackRateBuff(uint atk_rate)
{
	attack_rate_buff = atk_rate;
}

void Unit::SetAttackType(ATTACK_TYPE atk_type)
{
	attack_type = atk_type;
}

void Unit::SetAttackArea(const Circle & atk_area)
{
	attack_area = atk_area;
}

void Unit::SetAttackAreaBuff(uint atk_range)
{
	attack_area_buff = atk_range;
}

void Unit::SetDefense(uint def)
{
	defense = def;
}

void Unit::SetDefenseBuff(float def_buff)
{
	defense_buff += def_buff;
}

void Unit::SetVisionBuff(float vis_buff)
{
	vision_buff = vis_buff;
}

void Unit::SetLifeBuff(float hp_buff)
{
	if (life - hp_buff < 0)
	{
		life = 1;
	}
	life += hp_buff;
	life_buff += hp_buff;
}

void Unit::SetFoodCost(uint food_cst)
{
	food_cost = food_cst;
}

void Unit::SetWoodCost(uint wood_cst)
{
	wood_cost = wood_cst;
}

void Unit::SetGoldCost(uint coin_cst)
{
	gold_cost = coin_cst;
}

void Unit::SetPopulationCost(uint poblation_cst)
{
	population_cost = poblation_cst;
}

void Unit::SetTrainTime(uint train_time_val)
{
	train_time = train_time_val;
}
void Unit::SetExp(uint experience)
{
	exp = experience;
}

void Unit::SetUpgrade(Unit * upgraded)
{
	// From Base Entity
	name.clear();
	name = upgraded->GetName();
	vision.SetRad(upgraded->GetVision().GetRad());
	selection_rect = *upgraded->GetSelectionRect();
	icon_rect = upgraded->GetIcon();
	if (life == max_life)
	{
		life = upgraded->GetLife();
	}
	max_life = upgraded->GetMaxLife();
	life_position = upgraded->life_position;

	// Unit
	unit_type = upgraded->GetUnitType();
	soft_collider.SetRad(upgraded->GetSoftCollider().GetRad());
	hard_collider.SetRad(upgraded->GetHardCollider().GetRad());
	speed = upgraded->GetSpeed();

	int curr_frame = current_animation->GetCurrentFrame();
	CleanAnimation();
	App->animator->UnitPlay(this);
	current_animation->SetCurrentFrame(curr_frame);
	attack_hitpoints = upgraded->attack_hitpoints;
	attack_rate = upgraded->attack_rate;
	attack_type = upgraded->attack_type;

	defense = upgraded->defense;

	attack_area.SetRad(upgraded->attack_area.GetRad());
}

void Unit::SetUnitExperience(uint value)
{
	unit_experience = value;
}

// ----------------
//Get Methods -----
UNIT_TYPE Unit::GetUnitType()const
{
	return unit_type;
}

UNIT_CLASS Unit::GetUnitClass() const
{
	return unit_class;
}

const Circle& Unit::GetMark() const
{
	return mark;
}

const Circle & Unit::GetSoftCollider() const
{
	return soft_collider;
}

const Circle & Unit::GetHardCollider() const
{
	return hard_collider;
}

float Unit::GetSpeed()const
{
	return speed;
}

float Unit::GetSpeedBuff() const
{
	return speed_buff;
}

float Unit::GetTotalSpeed() const
{
	return speed + mutable_speed + speed_buff;
}

ACTION_TYPE Unit::GetAction()const
{
	return action_type;
}

DIRECTION_TYPE Unit::GetDirection()const
{
	return direction_type;
}

uint Unit::GetAttackHitPoints()const
{
	return attack_hitpoints;
}

float Unit::GetAttackBuff() const
{
	return attack_buff;
}

uint Unit::GetAttackRate()const
{
	return attack_rate;
}

uint Unit::GetAttackRateBuff() const
{
	return attack_rate_buff;
}

ATTACK_TYPE Unit::GetAttackType()const
{
	return attack_type;
}

uint Unit::GetAttackRange()const
{
	return attack_area.GetRad();
}


const Circle * Unit::GetAttackArea() const
{
	return &attack_area;
}
uint Unit::GetAttackAreaBuff()const
{
	return attack_area_buff;
}
uint Unit::GetDefense()const
{
	return defense;
}

float Unit::GetDefenseBuff() const
{
	return defense_buff;
}

uint Unit::GetMaxLife() const
{
	return max_life + life_buff;
}
uint Unit::GetBaseMaxLife() const
{
	return max_life;
}

uint Unit::GetFoodCost() const
{
	return food_cost;
}

uint Unit::GetWoodCost() const
{
	return wood_cost;
}

uint Unit::GetGoldCost() const
{
	return gold_cost;
}

uint Unit::GetPopulationCost() const
{
	return population_cost;
}

uint Unit::GetTrainTime() const
{
	return train_time;
}

uint Unit::GetExp() const
{
	return exp;
}

std::vector<iPoint>* Unit::GetPath() const
{
	if (action_worker.GetCurrentActionType() != TASK_U_MOVE)
		return nullptr;

	return ((MoveUnitAction*)action_worker.GetCurrentPrimaryAction())->GetPath();
}
iPoint Unit::GetFuturePosition() const
{
	return future_position;
}
float Unit::GetVisionBuff() const
{
	return vision_buff;
}
float Unit::GetLifeBuff() const
{
	return life_buff;
}
void Unit::ShotArrow(iPoint start, iPoint goal)
{
	Arrow* arrow = new Arrow();
	arrow->position = start;
	arrow->destination = goal;
		SDL_QueryTexture(App->animator->arrow, NULL, NULL, &arrow->arrow_rect.w, &arrow->arrow_rect.h);

		//SPEED
		arrow->PrepareArrow(arrow->position, arrow->destination);

		//PUSH ARROW TO ARROW LIST
		App->entities_manager->AddArrow(arrow);
	
}
// ----------------
///----------------------------------------------


///Class Resource -------------------------------
//Constructors ========================
Resource::Resource() :Entity()
{

}

Resource::Resource(const Resource& copy) : Entity(copy), resource_type(copy.resource_type), mark(copy.mark), interact_area(copy.interact_area)
{

}

//Destructors =========================
Resource::~Resource()
{

}

//Functionality =======================
void Resource::CleanMapLogic()
{
	//Set resource position fixing it in the tiles coordinates
	iPoint world_coords = App->map->WorldToMap(position.x, position.y);

	//Change walk & construction logic maps
	App->map->ChangeConstructionMap(world_coords, 1, 1, 1);
	App->map->ChangeLogicMap(world_coords, 1, 1, 1);
}

void Resource::SaveAsDef(pugi::xml_node & node)
{
	//Add all this resource attributes at the xml node
	
	//Name
	std::string name;
	switch (resource_type)
	{
	case BERRY_BUSH:	name = "Berry Bush";	break;
	case GOLD_ORE:		name = "Gold Ore";		break;
	case STONE_ORE:		name = "Stone Ore";		break;
	}
	node.append_attribute("name") = name.c_str();

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

}

bool Resource::Draw(bool debug)
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

	const Sprite* sprite = current_animation->GetCurrentSprite();

	ret = App->render->CallBlit(current_animation->GetTexture(), position.x - sprite->GetXpivot(), position.y - sprite->GetYpivot(), sprite->GetFrame(), false, -position.y - sprite->GetZ_cord(), sprite->GetOpacity(), 0, 0, &blit_color);

	if (current_animation->IsEnd())return false;

	return ret;
}

bool Resource::ExtractResources(uint* value)
{
	if (life <= 0)
	{
		return false;
	}
	if (life <= *value)
	{
		*value = life;
		life = 0;
		App->entities_manager->DeleteEntity(this);
		App->entities_manager->resources_quadtree.Exteract(&position);
		App->entities_manager->resources.remove(this);
		this->CleanMapLogic();
		return false;
	}
	else
	{
		life -= *value;
		return true;
	}
}

void Resource::SetPosition(float x, float y, bool insert)
{
	//Set resource position fixing it in the tiles coordinates
	iPoint world_coords = App->map->WorldToMap(x, y);
	iPoint coords = App->map->MapToWorld(world_coords.x, world_coords.y);
	position.x = coords.x;
	position.y = coords.y;
	iPoint pos(position.x, position.y);

	//Set resource interaction area position
	interact_area.SetPosition(pos);

	//Set resource mark position
	mark.SetPosition(pos);

	//Set resource vision position
	vision.SetPosition(pos);

	//Set resource render area position
	render_area.SetPosition(pos);

	//Change walk & construction logic maps
	App->map->ChangeConstructionMap(world_coords, 1, 1, 0);
	App->map->ChangeLogicMap(world_coords, 1, 1, 0);

	//Add Resource at the correct quad tree
	if (insert)
		App->entities_manager->resources_quadtree.Insert(this, &position);
}

void Resource::SetMark(const Rectng & rectangle)
{
	mark = rectangle;
}

void Resource::SetInteractArea(const Circle & area)
{
	interact_area = area;
}

void Resource::SetResourceType(RESOURCE_TYPE type)
{
	resource_type = type;
}

const Rectng& Resource::GetMark() const
{
	return mark;
}

const Circle * Resource::GetInteractArea() const
{
	return &interact_area;
}

RESOURCE_TYPE Resource::GetResourceType() const
{
	return resource_type;
}
///----------------------------------------------


///Class Building -------------------------------
//Constructors ========================
Building::Building() :Entity()
{

}

Building::Building(const Building& copy) : Entity(copy), mark(copy.mark), building_type(copy.building_type),
width_in_tiles(copy.width_in_tiles), height_in_tiles(copy.height_in_tiles), interact_area(copy.interact_area)
{

}

//Destructors =========================
Building::~Building()
{

}

//Functionality =======================
void Building::CleanMapLogic()
{
	//Set resource position fixing it in the tiles coordinates
	iPoint world_coords = App->map->WorldToMap(position.x, position.y);
	if (building_type == RUBBLE_FOUR || building_type == RUBBLE_THREE || building_type == BARRACK || building_type == BARRACK_C || building_type == ARCHERY_RANGE || building_type == ARCHERY_RANGE_C)
	{
		world_coords.x -= 1;
		world_coords.y -= 1;
	}

	//Change walk & construction logic maps
	App->map->ChangeConstructionMap(world_coords, width_in_tiles + 1, height_in_tiles + 1, 1);
	App->map->ChangeLogicMap(world_coords, width_in_tiles, height_in_tiles, 1);
}

void Building::ResetFogAround()
{
	//Reset unit locked cells
	uint size = in_vision_cells.size();
	for (uint k = 0; k < size; k++)
	{
		in_vision_cells[k]->alpha = MID_ALPHA;
		in_vision_cells[k]->locked = false;
	}

	//Reset unit locked tiles
	size = in_vision_tiles.size();
	for (uint k = 0; k < size; k++)
	{
		in_vision_tiles[k]->locked = false;
		in_vision_tiles[k]->type = GRAY_FOG;
	}

	in_vision_cells.clear();
	in_vision_tiles.clear();
}

void Building::DiscoverFogAround()
{
	if (entity_diplomacy != ALLY) return;
	//Calculate the zone that have to be discovered
	Circle zone = vision;
	zone.SetRad(width_in_tiles * App->map->data.tile_width);

	//Clear the discover zone around the building
	App->fog_of_war->ClearFogLayer(render_area, GRAY_FOG, false);
	App->fog_of_war->ClearAlphaLayer(zone, MID_ALPHA, false);
}

void Building::SaveAsDef(pugi::xml_node & node)
{
	//Add all this building attributes at the xml node

	//Name
	std::string name;
	switch (building_type)
	{
	case TOWN_CENTER_C:
	case TOWN_CENTER:	name = "Town Center";	break;
	case BARRACK_C:
	case BARRACK:		name = "Barrack";		break;
	case ARCHERY_RANGE_C:
	case ARCHERY_RANGE:	name = "Archery Range";	break;
	case BLACKSMITH_C:
	case BLACKSMITH:	name = "BlackSmith";	break;
	case STABLE_C:
	case STABLE:		name = "Stable";		break;
	case HOUSE_AI:
	case HOUSE_BI:
	case HOUSE_CI:
	case HOUSE_A:		
	case HOUSE_B:		
	case HOUSE_C:		name = "House";			break;
	case LUMBER_CAMP:	name = "Lumber Camp";	break;
	case MINING_CAMP:	name = "Mining Camp";	break;
	case MONASTERY:		name = "Monastery";		break;
	case UNIVERSITY_C:
	case UNIVERSITY_I:	name = "University";	break;
	case CASTLE:		name = "Castle";		break;
	}
	node.append_attribute("name") = name.c_str();

	/*Type*/	node.append_attribute("building_type") = App->animator->EnumToBuildingStr(building_type);
	/*Selec X*/ node.append_attribute("selection_x") = selection_rect.x;
	/*Selec Y*/ node.append_attribute("selection_y") = selection_rect.y;
	/*Selec W*/ node.append_attribute("selection_w") = selection_rect.w;
	/*Selec H*/ node.append_attribute("selection_h") = selection_rect.h;

	/*Icon X*/	node.append_attribute("icon_x") = icon_rect.x;
	/*Icon Y*/	node.append_attribute("icon_y") = icon_rect.y;
	/*Icon W*/	node.append_attribute("icon_w") = icon_rect.w;
	/*Icon H*/	node.append_attribute("icon_h") = icon_rect.h;

	/*Mark X*/	node.append_attribute("mark_x") = mark.GetDisplacement().x;
	/*Mark Y*/	node.append_attribute("mark_y") = mark.GetDisplacement().y;
	/*Mark W*/	node.append_attribute("mark_w") = mark.GetWidth();

	/*Interact X*/	node.append_attribute("interaction_area_x") = interact_area.GetPosition().x;
	/*Interact Y*/	node.append_attribute("interaction_area_y") = interact_area.GetPosition().y;
	/*Interact W*/	node.append_attribute("interaction_area_w") = interact_area.GetWidth();

	/*Tiles W*/		node.append_attribute("width_in_tiles") = width_in_tiles;
	/*Tiles H*/		node.append_attribute("height_in_tiles") = height_in_tiles;

	/*Vision rad*/	node.append_attribute("vision_rad") = vision.GetRad();

	/*Max Life*/		node.append_attribute("max_life") = max_life;
	/*Life Pos X*/	node.append_attribute("life_pos_x") = life_position.x;
	/*Life Pos Y*/	node.append_attribute("life_pos_y") = life_position.y;
	/*Units Capacity*/	node.append_attribute("units_capacity") = ((ProductiveBuilding*)this)->GetUnitsCapacity();
	/*Units Spawn X*/	node.append_attribute("units_spawn_x") = ((ProductiveBuilding*)this)->GetSpawnPoint().x;
	/*Units Spawn Y*/	node.append_attribute("units_spawn_y") = ((ProductiveBuilding*)this)->GetSpawnPoint().y;
	/*Prod Capacity*/	node.append_attribute("production_capacity") = ((ProductiveBuilding*)this)->GetProductionCapacity();

}

bool Building::CheckZone(int x, int y)
{
	//Check the construction tiles
	iPoint world_coords = App->map->WorldToMap(position.x, position.y);
	if (App->fog_of_war->GetFogID(world_coords.x, world_coords.y) == DARK_FOG && entity_diplomacy == ALLY)return false;

	switch (building_type)
	{
	case TOWN_CENTER:
	case HOUSE_A:
	case HOUSE_B:
	case HOUSE_C:
	case LUMBER_CAMP:
	case MINING_CAMP:
	case TOWN_CENTER_C:
	case HOUSE_AI:
	case HOUSE_BI:
	case HOUSE_CI:
		break;

	default:
		world_coords.x -= 1;
		world_coords.y -= 1;
		break;
	}
	
	/*if (building_type != HOUSE_A && building_type != HOUSE_B && building_type != HOUSE_C && building_type != TOWN_CENTER)
	{
		world_coords.x -= 1;
		world_coords.y -= 1;
	}*/

	if (App->map->CheckConstructionMap(world_coords, width_in_tiles + 1, height_in_tiles + 1))
	{
		//Check unit quadtree for units in build zone
		Circle check_area;
		check_area.SetRad(width_in_tiles * App->map->data.tile_height);
		check_area.SetPosition(GetPositionRounded());
		std::vector<Unit*> in_zone;
		App->entities_manager->units_quadtree.CollectCandidates(in_zone, check_area);
		if (in_zone.empty())
		{
			return true;
		}
	}
	return false;
}

bool Building::Die()
{
	if (action_type != DISAPPEAR)
	{
		if (building_type == TOWN_CENTER)
		{
			//Do lose condition
			App->player->game_panel->DoLose();
		}
		//Remove from minimap
		iPoint pos = GetPositionRounded();
		App->player->minimap_panel->RemoveBuildingToPrint(pos.x, pos.y, entity_diplomacy);

		action_type = DISAPPEAR;
		switch (building_type)
		{
		case TOWN_CENTER:
			building_type = RUBBLE_FOUR;
			break;
		case BARRACK:
			building_type = RUBBLE_THREE;
			break;
		case ARCHERY_RANGE:
			building_type = RUBBLE_FOUR;
			break;
		case BLACKSMITH:
		case STABLE:
			building_type = RUBBLE_THREE;
			break;
		case HOUSE_A:
		case HOUSE_B:
		case HOUSE_C:
		case LUMBER_CAMP:
		case MINING_CAMP:
			building_type = RUBBLE_TWO;
			break;
		case TOWN_CENTER_C:
			building_type = RUBBLE_FOUR;
			break;
		case BARRACK_C:
			building_type = RUBBLE_THREE;
			break;
		case ARCHERY_RANGE_C:
			building_type = RUBBLE_FOUR;
			break;
		case BLACKSMITH_C:
		case STABLE_C:
			building_type = RUBBLE_THREE;
			break;
		case HOUSE_AI:
		case HOUSE_BI:
		case HOUSE_CI:
			building_type = RUBBLE_TWO;
			break;
		case MONASTERY:
			building_type = RUBBLE_THREE;
			break;
		case UNIVERSITY_C:
		case UNIVERSITY_I:
		case CASTLE:
			building_type = RUBBLE_FOUR;
			break;
		}

		App->entities_manager->AddDeadBuilding(this);
		App->animator->BuildingPlay(this);
		current_animation->Reset();
	}


	else
	{
		current_animation->GetCurrentSprite();
		if (current_animation->IsEnd())
		{
			//App->entities_manager->RemoveDeathBuilding(this);
			this->CleanMapLogic();
			App->entities_manager->DeleteEntity(this);
			return true;
		}
	}

	return false;
}

//Draw ----------------------
bool Building::Draw(bool debug)
{
	bool ret = false;

	//Debug Draw
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
	int length = 200;
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

	//Get all sprites of the current animation
	const std::vector<Sprite>*	sprites = current_animation->GetAllSprites();

	uint size = sprites->size();
	for (uint k = 0; k < size; k++)
	{
		ret = App->render->CallBlit(((DiplomaticAnimation*)current_animation)->GetTexture(entity_diplomacy), position.x - sprites->at(k).GetXpivot(), position.y - sprites->at(k).GetYpivot(), sprites->at(k).GetFrame(), false, -position.y - sprites->at(k).GetZ_cord(), sprites->at(k).GetOpacity(), 0, 0, &blit_color);
		if (!ret)break;
	}

	return ret;
}

bool Building::Update()
{
	action_worker.Update();
	if (!fog_discovered && building_type == ALLY)
	{
		DiscoverFogAround();
		fog_discovered = true;
	}
	return true;
}

//Set Methods ---------------
void Building::SetPosition(float x, float y, bool insert)
{
	//Set building position fixing it in the tiles coordinates ( center position in the selected tile)
	iPoint map_coords = App->map->WorldToMap(x, y);
	iPoint world_coords = App->map->MapToWorld(map_coords.x, map_coords.y);
	position.x = world_coords.x;
	position.y = world_coords.y;

	if (!insert)return;

	//Calculate the upper tile of the building zone
	iPoint upper_tile(map_coords.x - 1, map_coords.y - 1);

	//Update the logic & construction map
	//Check if the building is a town center to respect the build exception
	switch (building_type)
	{
	case TOWN_CENTER:
		position.y -= App->map->data.tile_height * 0.5f;
		App->map->ChangeLogicMap(upper_tile, width_in_tiles - 2, height_in_tiles - 2, 0);
		break;
	case TOWN_CENTER_C:
		position.y += App->map->data.tile_height * 0.5f;
		App->map->ChangeLogicMap(upper_tile, width_in_tiles - 2, height_in_tiles - 2, 0);
		break;
	case BARRACK:
	case BARRACK_C:
	case ARCHERY_RANGE:
	case ARCHERY_RANGE_C:
	case BLACKSMITH:
	case BLACKSMITH_C:
	case STABLE:
	case STABLE_C:
	case MONASTERY:
		App->map->ChangeLogicMap(upper_tile, width_in_tiles, height_in_tiles, 0);
		break;
	case UNIVERSITY_C:
	case UNIVERSITY_I:
	case CASTLE:
		position.y += (App->map->data.tile_height + 1) * 0.5f;
		App->map->ChangeLogicMap(upper_tile, width_in_tiles, height_in_tiles, 0);
		break;
	case HOUSE_A:
	case HOUSE_AI:
	case HOUSE_B:
	case HOUSE_BI:
	case HOUSE_C:
	case HOUSE_CI:
	case LUMBER_CAMP:
	case MINING_CAMP:
		position.y -= (App->map->data.tile_height + 1) * 0.5f;
		App->map->ChangeLogicMap(upper_tile, width_in_tiles, height_in_tiles, 0);
		break;
	}

	//Update construction map
	App->map->ChangeConstructionMap(upper_tile, width_in_tiles + 1, height_in_tiles + 1, 0);

	iPoint pos(position.x, position.y);

	//Clear fog around building vision zone
	vision.SetPosition(pos);

	//Set building render area position
	render_area.SetPosition(pos);

	//Set interaction area rectangle position
	interact_area.SetPosition(pos);

	//Set building mark position
	mark.SetPosition(iPoint(position.x, position.y));

	//Add building at the correct quad tree
	App->entities_manager->buildings_quadtree.Insert(this, &position);

	//Set Position in Minimap
	iPoint map_pos = this->GetPositionRounded();
	App->player->minimap_panel->SetBuildingToPrint(map_pos.x, map_pos.y, entity_diplomacy);

	if (entity_diplomacy != ALLY)return;

	//Collect all the units around to release the fog
	std::vector<Unit*> coll_units;
	Circle coll_area = render_area;
	coll_area.SetRad(coll_area.GetRad() + 400);
	uint size = App->entities_manager->units_quadtree.CollectCandidates(coll_units, coll_area);
	for (uint k = 0; k < size; k++)
	{
		coll_units[k]->ResetFogAround();
	}
	this->CleanFogAround();
	for (uint k = 0; k < size; k++)
	{
		coll_units[k]->CleanFogAround();
	}
}

void Building::OnlySetPosition(float x, float y)
{
	//Set building position fixing it in the tiles coordinates ( center position in the selected tile)
	iPoint pos = App->map->WorldToMap(x, y);
	pos = App->map->MapToWorld(pos.x, pos.y);
	switch (building_type)
	{

	case TOWN_CENTER:
	case LUMBER_CAMP:
	case MINING_CAMP:
		pos.y -= App->map->data.tile_height * 0.5f;
		break;
	case HOUSE_A:
	case HOUSE_B:
	case HOUSE_C:
		pos.y -= (App->map->data.tile_height + 1) * 0.5f;
		break;
	case RUBBLE_THREE:
		break;
	case RUBBLE_FOUR:
		break;
	}

	position.x = pos.x;
	position.y = pos.y;
}

void Building::SetMark(const Rectng& rectangle)
{
	mark = rectangle;
}

void Building::SetInteractArea(const Rectng& rectangle)
{
	interact_area = rectangle;
}

void Building::SetWidthInTiles(uint width)
{
	width_in_tiles = width;
}

void Building::SetHeightInTiles(uint height)
{
	height_in_tiles = height;
}

void Building::SetBuildingType(BUILDING_TYPE type)
{
	building_type = type;
}

void Building::SetActionType(ACTION_TYPE type)
{
	action_type = type;
}

void Building::SetDirectionType(DIRECTION_TYPE type)
{
	direction_type = type;
}

void Building::SetFogDiscovered(bool val)
{
	fog_discovered = val;
}

void Building::SetUpgrade(Building * new_building)
{
	// From Base Entity
	name.clear();
	name = new_building->GetName();
	vision.SetRad(new_building->GetVision().GetRad());
	selection_rect = *new_building->GetSelectionRect();
	icon_rect = new_building->GetIcon();
	if (life == max_life)
	{
		life = new_building->GetLife();
	}
	max_life = new_building->GetMaxLife();
	life_position = new_building->life_position;

	building_type = new_building->GetBuildingType();

	CleanAnimation();
	App->animator->BuildingPlay(this);

	mark = new_building->GetMark();
	iPoint pos(position.x, position.y);
	mark.SetPosition(pos);
	interact_area = *new_building->GetInteractArea();
	interact_area.SetPosition(pos);

}

//Get Methods ---------------
const Rectng & Building::GetMark() const
{
	return mark;
}

const Rectng* Building::GetInteractArea() const
{
	return &interact_area;
}

BUILDING_TYPE Building::GetBuildingType() const
{
	return building_type;
}

ACTION_TYPE Building::GetActionType() const
{
	return action_type;
}

DIRECTION_TYPE Building::GetDirectionType() const
{
	return direction_type;
}

bool Building::GetFogDiscovered() const
{
	return fog_discovered;
}
Arrow::Arrow()
{
}
Arrow::~Arrow()
{
}
///-----------------------------------------------
bool Arrow::Draw()
{

	NewPosition(destination);

		App->render->CallBlit(App->animator->arrow, position.x, position.y, &arrow_rect, flip, 0, 255, pivot_x, pivot_y, nullptr,angle);
	if (position == destination)return true;
	return false;
}



void Arrow::NewPosition(iPoint goal)
{
	iPoint location = position;

	int norm = location.DistanceTo(goal);
	float x_step = speed * (App->GetDT() * 100) * (goal.x - location.x) / norm;
	float y_step = speed * (App->GetDT() * 100) * (goal.y - location.y) / norm;


	//Add the calculated values at the unit & mark position
	position.x += x_step;
	position.y += y_step;
}

void Arrow::PrepareArrow(iPoint start, iPoint goal)
{
	int deltaY = start.y - goal.y;
	int deltaX = start.x - goal.x;
	angle = atan2(deltaY, deltaX)*(180 / 3.14)+180;
	//angle = 
//	if (angle >= 0 && angle < 90 || angle < 270 && angle >= 360)
	//	flip = true;
}

bool Arrow::operator==(const Arrow & other) const
{
	return this->destination == other.destination && this->angle == other.angle&&this->position == other.position;
}


///---------------------------------------------