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

#include "Actions_Unit.h"

#include "Hud_GamePanel.h"

///Class Entity ---------------------------------
//Constructors ========================
Entity::Entity() :name(""), action_worker(new ActionWorker())
{
}

Entity::Entity(const Entity& copy) : name(copy.name), position(copy.position), entity_type(copy.entity_type), entity_diplomacy(copy.entity_diplomacy), vision(copy.vision), selection_rect(copy.selection_rect),
icon_rect(copy.icon_rect), max_life(copy.max_life), life(copy.life), current_animation(copy.current_animation), action_worker(new ActionWorker())
{

}

//Destructors =========================
Entity::~Entity()
{

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
	return (myself == tar.myself && position == tar.position && entity_type == tar.entity_type && tar.life == life);
}

//Update ----------
bool Entity::Update()
{
	action_worker->Update();
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
	ret = App->render->CallBlit(current_animation->GetTexture(), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetXpivot(), sprite->GetYpivot());

	return ret;
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

//Add Action ------------
void Entity::AddAction(Action * action)
{
	action_worker->AddAction(action);
}

void Entity::AddPasiveAction(Action * action)
{
	action_worker->AddPassiveAction(action);
}

void Entity::AddSecondaryAction(Action * action)
{
	action_worker->AddSecondaryAction(action);
}

void Entity::AddPriorizedAction(Action * action)
{
	action_worker->AddPriorizedAction(action);
}

void Entity::PopAction(Action * action)
{
	action_worker->PopAction(action);
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

void Entity::SetMaxLife(uint full_life_val)
{
	max_life = full_life_val;
}

void Entity::SetLife(uint life_val)
{
	life = life_val;
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

const SDL_Rect * Entity::GetSelectionRect() const
{
	return &selection_rect;
}

const SDL_Rect& Entity::GetIcon()const
{
	return icon_rect;
}

ActionWorker * Entity::GetWorker() const
{
	return action_worker;
}

Entity ** Entity::GetMe()
{
	return &myself;
}


// ----------------
///----------------------------------------------


///Class Unit -----------------------------------
//Constructors ========================
Unit::Unit() :Entity()
{

}

Unit::Unit(const Unit& copy) : Entity(copy), unit_type(copy.unit_type), mark(copy.mark),soft_collider(copy.soft_collider),hard_collider(copy.hard_collider), view_area(copy.view_area),
speed(copy.speed), action_type(copy.action_type), direction_type(copy.direction_type), attack_hitpoints(copy.attack_hitpoints), attack_bonus(copy.attack_bonus), siege_hitpoints(copy.siege_hitpoints),
attack_rate(copy.attack_rate), attack_type(copy.attack_type), attack_area(copy.attack_area), defense(copy.defense), defense_bonus(copy.defense_bonus), armor(copy.armor), armor_bonus(copy.armor_bonus),
food_cost(copy.food_cost), wood_cost(copy.wood_cost), gold_cost(copy.gold_cost), population_cost(copy.population_cost), train_time(copy.train_time)
{

}

//Destructors =========================
Unit::~Unit()
{

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
	if (selected)ret = mark.Draw();
	
	if (debug) {
		if (selected)
			attack_area.Draw();
			soft_collider.Draw();
			hard_collider.Draw();
			vision.Draw();
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
	ret = App->render->CallBlit(((DiplomaticAnimation*)current_animation)->GetTexture(entity_diplomacy), position.x, position.y, sprite->GetFrame(), flip_sprite, -position.y - sprite->GetZ_cord(), sprite->GetOpacity(), sprite->GetXpivot(), sprite->GetYpivot());

	return ret;
}


//Actions ---------
bool Unit::Move(std::vector<iPoint>* path, const iPoint& target) ///Returns true when it ends
{
	//Check if the unit have an assigned path
	if (path == nullptr)
	{
		LOG("Error path not found!");
		return true;
	}


	//Build goal path point
	iPoint goal = path->back();
	iPoint location = iPoint(position.x, position.y);
	float x_step = 0.0f;
	float y_step = 0.0f;
	int path_size = path->size();
	//Update goal node and animation direction
	if (location.DistanceTo(goal) < 2)
	{

		//if we have a collision with other unit and we have lower priority reduction of speed

		//Look in the next update if there is an error
		std::vector<Unit*> other_units;
		App->entities_manager->units_quadtree.CollectCandidates(other_units, vision);
		other_units.size();
		if (future_position != iPoint(-1, -1) && !App->pathfinding->IsWalkable(App->map->WorldToMap(future_position.x, future_position.y)))
		{
			CorrectPath(path);
			if (path->empty())
			{
				return true;
			}
			goal = NextGoal(path);
			NewPosition(goal, x_step, y_step);

			//Add the calculated values at the unit & mark position
			SetPosition(x_step, y_step);

			return false;
		}
		int collisions = 0;
		while (!other_units.empty()) {
			Unit* other_unit = other_units.back();
			other_units.pop_back();

			if (other_unit == this)
			{
				continue;
			}
			int other_path_size = 0;
			std::vector<iPoint>* other_path = nullptr;
			if (this != other_unit) {

			}
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
				if (path->size() < 2) {
					if (target != iPoint(-1, -1))
						Repath(path, target);
					else
						Repath(path, *(path->begin()));
					return false;
				}

			}
			break;
			case COLLISION_MOVE:

				if (other_path_size < 2 && path_size < 2 && future_position == other_unit->GetPositionRounded()) {
					if (location.DistanceTo(goal) < other_unit->GetPositionRounded().DistanceTo(goal))
						other_unit->Repath(other_path, *(other_path->begin()));
					else
						Repath(path, *(path->begin()));

				}
				else
				{
					if (mutable_speed == 0 && other_unit->mutable_speed == 0)
					{
						if (location.DistanceTo(goal) < other_unit->GetPositionRounded().DistanceTo(goal))
							other_unit->mutable_speed -= 0.2;
						else
							mutable_speed -= 0.2;
					}
				}

				collisions++;
				break;
			case FUTURE_COLLISION_MOVE:
				/*
				if (other_path_size != 0) {
				if (future_position == other_unit->future_position && future_position != iPoint(-1, -1)) {
				if (other_path_size <= 2 && path_size <= 2) {
				if (location.DistanceTo(goal) < other_unit->GetPositionRounded().DistanceTo(goal))
				other_unit->Repath(*(other_path->begin()));
				else
				Repath(*(path->begin()));

				}
				else
				{
				if (mutable_speed == 0 && other_unit->mutable_speed == 0)
				{
				if (location.DistanceTo(goal) < other_unit->GetPositionRounded().DistanceTo(goal))
				other_unit->mutable_speed -= 0.2;
				else
				mutable_speed -= 0.2;
				}
				}

				}
				}
				collisions++;
				*/
				break;




			}
		}
		if (collisions == 0 && mutable_speed != 0.0f) {
			mutable_speed = 0.0f;
		}

		if (path->size() == 1)
		{

			//Set unit at the goal pixel position
			SetPosition((float)goal.x, (float)goal.y);
			//Stop idle walk animation
			action_type = IDLE;
			App->animator->UnitPlay(this);

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
		Focus(goal);
	}

	//Calculate the X/Y values that the unit have to move 
	//checking the goal location and the unit movement speed
	NewPosition(goal, x_step, y_step);

	//Add the calculated values at the unit & mark position
	SetPosition(x_step, y_step);

	return false;
}

void Unit::Repath(std::vector<iPoint>* path,const iPoint & destination)
{
	iPoint new_destination = FindWalkableCell(destination);
	std::vector<iPoint>* new_path;
	new_path=App->pathfinding->SimpleAstar(GetPositionRounded(), new_destination);
	path->clear();
	if (new_path != nullptr) {
		path->insert(path->end(), new_path->begin(), new_path->end());
	}
}

void Unit::CorrectPath(std::vector<iPoint>* path)
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
	Focus(goal);
	return goal;
}

void Unit::NewPosition(const iPoint& goal, float & position_x, float & position_y)
{
	iPoint location = iPoint(position.x, position.y);

	int norm = location.DistanceTo(goal);
	float x_step = GetSpeed() * (App->GetDT() * 100) * (goal.x - location.x) / norm;
	float y_step = GetSpeed() * (App->GetDT() * 100) * (goal.y - location.y) / norm;

	//Add the calculated values at the unit & mark position
	position_x = position.x + x_step;
	position_y = position.y + y_step;

}

iPoint Unit::FindWalkableCell(const iPoint & center)
{
	iPoint cell;
	iPoint pos = App->map->WorldToMap(center.x, center.y);
	std::vector<Unit*> other_units;
	App->entities_manager->units_quadtree.CollectCandidates(other_units, vision);
	int i = 1;
	while (i<=5) {
		// south
		cell.create(pos.x, pos.y + i);
		if (App->pathfinding->IsWalkable(cell) && UnitHere(other_units, cell))
		{
			return App->map->MapToWorld(cell.x, cell.y);;
		}

		// north
		cell.create(pos.x, pos.y - i);
		if (App->pathfinding->IsWalkable(cell) && UnitHere(other_units, cell))
		{
			return App->map->MapToWorld(cell.x, cell.y);;
		}

		// east
		cell.create(pos.x + i, pos.y);
		if (App->pathfinding->IsWalkable(cell) && UnitHere(other_units, cell))
		{
			return App->map->MapToWorld(cell.x, cell.y);;
		}

		// west
		cell.create(pos.x - i, pos.y);
		if (App->pathfinding->IsWalkable(cell) && UnitHere(other_units, cell))
		{
			return App->map->MapToWorld(cell.x, cell.y);;
		}

		// south-east
		cell.create(pos.x + i, pos.y + i);
		if (App->pathfinding->IsWalkable(cell) && UnitHere(other_units, cell))
		{
			return App->map->MapToWorld(cell.x, cell.y);;
		}
		// south-west
		cell.create(pos.x - i, pos.y + i);
		if (App->pathfinding->IsWalkable(cell) && UnitHere(other_units, cell))
		{
			return App->map->MapToWorld(cell.x, cell.y);;
		}
		// north-east
		cell.create(pos.x + i, pos.y - i);
		if (App->pathfinding->IsWalkable(cell) && UnitHere(other_units, cell))
		{
			return App->map->MapToWorld(cell.x, cell.y);;
		}
		// north-west
		cell.create(pos.x - i, pos.y - i);
		if (App->pathfinding->IsWalkable(cell) && UnitHere(other_units, cell))
		{
			return App->map->MapToWorld(cell.x, cell.y);;
		}
		i++;
	}
}

bool Unit::UnitHere(std::vector<Unit*> other_units, const iPoint& cell)
{
	iPoint destination = App->map->MapToWorldCenter(cell.x,cell.y);
	iPoint other_unit_cell;
	for(int i = 0;i<other_units.size();i++)
	{
		
		Unit* other = other_units[i];
		if (this == other)
		{
			continue;
		}
		other_unit_cell = App->map->WorldToMap(other->GetPositionRounded().x, other->GetPositionRounded().y);
		if (other_unit_cell == cell) {
			return false;
		}
		/*
		if (other->action_type == IDLE || other->action_type == ATTATCK)
		{
			if (sqrt((other->GetPositionRounded().x - destination.x) * (other->GetPositionRounded().x - destination.x) + (other->GetPositionRounded().y - destination.y) * (other->GetPositionRounded().y - destination.y)) < (this->hard_collider.GetRad() + other->hard_collider.GetRad()))
			{
				return false;
			}
		}
		else if (other->action_type == WALK) {
			if (sqrt((other->GetPositionRounded().x - destination.x) * (other->GetPositionRounded().x - destination.x) + (other->GetPositionRounded().y - destination.y) * (other->GetPositionRounded().y - destination.y)) < (this->hard_collider.GetRad() + other->hard_collider.GetRad()))
			{
				return false;
			}
		}
		*/
	}
	
	
	return true;
}

bool Unit::UnitHere(const iPoint & destination, int radius)
{


	std::vector<Unit*> other_units;
	Circle area;
	area.SetRad(radius);

	area.SetPosition(destination);
	App->entities_manager->units_quadtree.CollectCandidates(other_units, area);
	if (!other_units.empty()) {
		if (other_units.size() == 1 && *(other_units.begin()) == this)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}


iPoint Unit::FindWalkableAdjacent(const iPoint & center)
{
	iPoint cell;
	iPoint pos = App->map->WorldToMap(center.x, center.y);
	iPoint goal;

	int radius = GetSoftCollider().GetRad();
	Circle checker;
	checker.SetRad(radius);

	// south
	cell.create(pos.x, pos.y + 1);
	checker.SetPosition(App->map->MapToWorldCenter(cell.x, cell.y));
	goal = attack_area.NearestPoint(&checker);


		if (App->pathfinding->IsWalkable(cell) && !UnitHere(goal,radius))
		{
			return goal;
		}

		// north
		cell.create(pos.x, pos.y - 1);
		checker.SetPosition(App->map->MapToWorldCenter(cell.x, cell.y));
		goal = attack_area.NearestPoint(&checker);

		if (App->pathfinding->IsWalkable(cell) && !UnitHere(goal, radius))
		{
			return goal;
		}


		// east
		cell.create(pos.x + 1, pos.y);
		checker.SetPosition(App->map->MapToWorldCenter(cell.x, cell.y));
		goal = attack_area.NearestPoint(&checker);


		if (App->pathfinding->IsWalkable(cell) && !UnitHere(goal, radius))
		{
			return goal;
		}


		// west
		cell.create(pos.x - 1, pos.y);
		checker.SetPosition(App->map->MapToWorldCenter(cell.x, cell.y));
		goal = attack_area.NearestPoint(&checker);


		if (App->pathfinding->IsWalkable(cell) && !UnitHere(goal, radius))
		{
			return goal;
		}

		
		// south-east
		cell.create(pos.x + 1, pos.y + 1);
		checker.SetPosition(App->map->MapToWorldCenter(cell.x, cell.y));
		goal = attack_area.NearestPoint(&checker);


		if (App->pathfinding->IsWalkable(cell) && !UnitHere(goal, radius))
		{
			return goal;
		}

		// south-west
		cell.create(pos.x - 1, pos.y + 1);
		checker.SetPosition(App->map->MapToWorldCenter(cell.x, cell.y));
		goal = attack_area.NearestPoint(&checker);


		if (App->pathfinding->IsWalkable(cell) && !UnitHere(goal, radius))
		{
			return goal;
		}

		// north-east
		cell.create(pos.x + 1, pos.y - 1);
		checker.SetPosition(App->map->MapToWorldCenter(cell.x, cell.y));
		goal = attack_area.NearestPoint(&checker);

		
		if (App->pathfinding->IsWalkable(cell) && !UnitHere(goal, radius))
		{
			return goal;
		}

		// north-west
		cell.create(pos.x - 1, pos.y - 1);
		checker.SetPosition(App->map->MapToWorldCenter(cell.x, cell.y));
		goal = attack_area.NearestPoint(&checker);


		if (App->pathfinding->IsWalkable(cell) && !UnitHere(goal, radius))
		{
			return goal;
		}

	
		return iPoint(-1, -1);
}

Unit** Unit::FindNewTarget()
{
std::vector<Unit*> other_units;
App->entities_manager->units_quadtree.CollectCandidates(other_units, vision);
App->entities_manager->OrganizeByNearest(other_units, this->GetVision());

Unit* unit;
iPoint goal;
while (!other_units.empty())
{
	unit = other_units.back();
	other_units.pop_back();

	if(unit->GetDiplomacy()==ALLY||unit->GetAction()==DIE ||unit->GetAction()==DISAPPEAR)
	{
		continue;
	}
	goal = FindWalkableAdjacent(unit->GetPositionRounded());
	if (goal != iPoint(-1, -1)) 
	{

		SetInteractionTarget(unit);
		return (Unit**)unit->GetMe();
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
bool Unit::AttackUnit(Unit** target)
{
	//Check if the target is in the attack area
	if ((*target) == nullptr)
	{
		return true;
	}
	if (!attack_area.Intersects(((Unit*)(*target))->GetAttackArea()))
	{

		iPoint goal = attack_area.NearestPoint(((Unit*)(*target))->GetAttackArea());

		if (UnitHere(goal,GetSoftCollider().GetRad()))
		{
			goal = FindWalkableAdjacent(((Unit*)(*target))->GetPositionRounded());
			if (goal == iPoint(-1, -1))
			{
				Unit** new_target;
				new_target = FindNewTarget();
				if (new_target != nullptr) {
					this->AddPriorizedAction((Action*)App->action_manager->AttackToUnitAction(this, new_target));
					return true;
				}
				return true;
			}
		}
	
			std::vector<iPoint>* path = App->pathfinding->SimpleAstar(GetPositionRounded(), goal);
			if (path == nullptr)return true;
			this->AddPriorizedAction((Action*)App->action_manager->MoveAction(path, this));
			return false;
	
	
	}


	//Control action rate
	if (action_timer.Read() < attack_rate)return false;
	
	//App->sound->PlayFXAudio(SWORD_ATTACK_SOUND);

	//Set unit attack animation
	if (action_type != ATTATCK)
	{
		action_type = ATTATCK;
		Focus((*target)->GetPositionRounded());
		App->animator->UnitPlay(this);
	}

	if ((*target)->GetLife() <= 0)
	{
		ACTION_TYPE act = ((Unit*)(*target))->action_type;
		if (this->action_type == ATTATCK)
		{
			action_type = IDLE;
			App->animator->UnitPlay(this);
		}
		if(act != DIE && act != DISAPPEAR)(*target)->AddPriorizedAction((Action*)App->action_manager->DieAction((Unit*)(*target)));
		return true;
	}
	//Calculate the attack & apply the value at the target life points
	((Unit*)(*target))->life -= MIN(((Unit*)(*target))->life, attack_hitpoints);

	//Reset action timer
	action_timer.Start();

	return false;
}

bool Unit::AttackBuilding(Building ** target)
{
	//Check if the target is in the attack area
	if (!attack_area.Intersects((*target)->GetInteractArea()))
	{

		iPoint goal = attack_area.NearestPoint((*target)->GetInteractArea());
		std::vector<iPoint>* path = App->pathfinding->SimpleAstar(GetPositionRounded(), goal);
		if (path == nullptr)return true;
		this->AddSecondaryAction((Action*)App->action_manager->MoveAction(path, this));
	}

	//Control action rate
	if (action_timer.Read() < attack_rate)return false;

	//Set unit attack animation
	if (action_type != ATTATCK)
	{
		action_type = ATTATCK;
		Focus((*target)->GetPositionRounded());
		App->animator->UnitPlay(this);
	}

	if ((*target)->GetLife() <= 0)
	{
		ACTION_TYPE act = (*target)->GetActionType();
		if (this->action_type == ATTATCK)
		{
			action_type = IDLE;
			App->animator->UnitPlay(this);
		}
		if (act != DIE && act != DISAPPEAR)(*target)->AddPriorizedAction((Action*)App->action_manager->DieBuildngAction((*target)));
		return true;
	}

	//Calculate the attack & apply the value at the target life points
	((Unit*)(*target))->life -= MIN(((Unit*)(*target))->life, attack_hitpoints);

	//Reset action timer
	action_timer.Start();

	return false;
}

bool Unit::Cover()
{
	return ((HabitableBuilding*)interaction_target)->CoverUnit(this);
}

void Unit::DirectDamage(uint damage)
{
	//Deal damage to the unit
	life -= MIN(life, damage);
	if (life <= 0)
	{
		action_worker->AddPriorizedAction(App->action_manager->DieAction(this));
	}
}

bool Unit::Die()
{
	if (GetDiplomacy() == DIPLOMACY::ALLY)
	{
		App->player->game_panel->IncreaseDeathAllies();
	}
	if (GetDiplomacy() == DIPLOMACY::ENEMY)
	{
		//App->player->game_panel->IncreaseDeathEnemies();

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


	if (action_type != DIE && action_type != DISAPPEAR)
	{
		App->buff_manager->RemoveTargetBuffs(this);
		action_type = DIE;
		if (this->GetDiplomacy() == ALLY) App->player->game_panel->IncressPopulation(-1, false);
		App->entities_manager->units_quadtree.Exteract(this,&this->GetPosition());
		App->animator->UnitPlay(this);
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
			GetWorker()->Reset();
			App->entities_manager->DeleteEntity(this);
			return true;
		}
	}
	return false;
}

void Unit::Stun(uint time)
{
	action_worker->AddPriorizedAction(App->action_manager->StunAction(this, time));
}

COLLISION_TYPE Unit::CheckColision(const Unit * current, const Unit * other)
{
	if (current == other)
	{
		return NO_COLLISION;
	}
	if(other->action_type == IDLE||other->action_type == ATTATCK)
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

//Bonus -----------
void Unit::AddBonus(BONUS_TYPE type, uint type_id, uint bonus, bool defence)
{
	Bonus* new_bonus = new Bonus(type, type_id, bonus);
	if (defence) defence_bonuses.push_back(new_bonus);
	else defence_bonuses.push_back(new_bonus);
}

//Set Methods -----
void Unit::SetPosition(float x, float y, bool insert)
{
	//Extract the units to push it with the new position later
	if(insert)App->entities_manager->units_quadtree.Exteract(this,&position);

	//Set unit position
	position.x = x;
	position.y = y;
	iPoint pos(position.x, position.y);
	//Set unit vision position
	vision.SetPosition(pos);
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
	attack_buff = atk_buff;
}

void Unit::SetUnitType(UNIT_TYPE type)
{
	unit_type = type;
}

void Unit::SetInteractionTarget(const Entity * target)
{
	interaction_target = (Entity*)target;
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

void Unit::SetViewArea(uint area_val)
{
	view_area = area_val;
}

void Unit::SetSpeed(float speed_val)
{
	speed = speed_val;
}

void Unit::SetAction(ACTION_TYPE action_val)
{
	action_type = action_val;
}

void Unit::SetDirection(DIRECTION_TYPE direction_val)
{
	direction_type = direction_val;
}

void Unit::SetAttackDelay(uint atk_delay)
{
	attack_delay = atk_delay;
}

void Unit::SetAttackHitPoints(uint atk_val)
{
	attack_hitpoints = atk_val;
}

void Unit::SetAttackBonus(uint atk_bonus)
{
	attack_bonus = atk_bonus;
}

void Unit::SetSiegeHitPoints(uint siege_val)
{
	siege_hitpoints = siege_val;
}

void Unit::SetAttackRate(uint atk_rate)
{
	attack_rate = atk_rate;
}

void Unit::SetAttackType(ATTACK_TYPE atk_type)
{
	attack_type = atk_type;
}

void Unit::SetAttackArea(const Circle & atk_area)
{
	attack_area = atk_area;
}

void Unit::SetDefense(uint def)
{
	defense = def;
}

void Unit::SetDefenseBonus(uint def_bonus)
{
	defense_bonus = def_bonus;
}

void Unit::SetDefenseBuff(float def_buff)
{
	defense_buff = def_buff;
}

void Unit::SetArmor(uint arm)
{
	armor = arm;
}

void Unit::SetArmorBonus(uint arm_bonus)
{
	armor_bonus = arm_bonus;
}

void Unit::SetArmorBuff(float arm_buff)
{
	armor_buff = arm_buff;
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

// ----------------
//Get Methods -----
UNIT_TYPE Unit::GetUnitType()const
{
	return unit_type;
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

const Entity * Unit::GetInteractionTarget()
{
	return interaction_target;
}

uint Unit::GetViewArea()const
{
	return view_area;
}

float Unit::GetSpeed()const
{
	return speed+mutable_speed;
}

ACTION_TYPE Unit::GetAction()const
{
	return action_type;
}

DIRECTION_TYPE Unit::GetDirection()const
{
	return direction_type;
}

uint Unit::GetAttackDelay() const
{
	return attack_delay;
}

uint Unit::GetAttackHitPoints()const
{
	return attack_hitpoints;
}

uint Unit::GetAttackBonus()const
{
	return attack_bonus;
}

float Unit::GetAttackBuff() const
{
	return attack_buff;
}

uint Unit::GetSiegeHitPoints()const
{
	return siege_hitpoints;
}

uint Unit::GetAttackRate()const
{
	return attack_rate;
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

uint Unit::GetDefense()const
{
	return defense;
}

uint Unit::GetDefenseBonus() const
{
	return defense_bonus;
}

float Unit::GetDefenseBuff() const
{
	return defense_buff;
}

uint Unit::GetArmor() const
{
	return armor;
}

uint Unit::GetArmorBonus() const
{
	return armor_bonus;
}

float Unit::GetArmorBuff() const
{
	return armor_buff;
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
	if (action_worker->GetCurrentActionType() != TASK_U_MOVE)
		return nullptr;

	return ((MoveUnitAction*)action_worker->GetCurrentAction())->GetPath();
}
// ----------------
///----------------------------------------------


///Class Resource -------------------------------
//Constructors ========================
Resource::Resource() :Entity()
{

}

Resource::Resource(const Resource& copy) : Entity(copy), resource_type(copy.resource_type), mark(copy.mark),interact_area(copy.interact_area)
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

	const std::vector<Sprite>* sprites = current_animation->GetAllSprites();

	uint size = sprites->size();
	for (uint k = 0; k < size; k++)
	{
		ret = App->render->CallBlit(current_animation->GetTexture(), position.x - sprites->at(k).GetXpivot(), position.y - sprites->at(k).GetYpivot(), sprites->at(k).GetFrame(), false, -position.y - sprites->at(k).GetZ_cord(), sprites->at(k).GetOpacity());
		if (!ret)break;
	}

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
		this->CleanMapLogic();
		return false;
	}
	else
	{
		life -= *value;
		if (resource_type == GOLD_ORE && life < 500)
		{
			resource_type = TINY_GOLD_ORE;
			App->animator->ResourcePlay(this);
		}
		else if (resource_type == STONE_ORE && life < 500)
		{
			resource_type = TINY_STONE_ORE;
			App->animator->ResourcePlay(this);
		}

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

	//Set resource interaction area position
	interact_area.SetPosition(iPoint(position.x, position.y));

	//Set resource mark position
	mark.SetPosition(iPoint(position.x, position.y));

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
	if (building_type == RUBBLE_THREE || building_type == BARRACK)
	{
		world_coords.x -= 1;
		world_coords.y -= 1;
	}

	//Change walk & construction logic maps
	App->map->ChangeConstructionMap(world_coords, width_in_tiles, height_in_tiles, 1);
	App->map->ChangeLogicMap(world_coords, width_in_tiles, height_in_tiles, 1);
}

bool Building::CheckZone(int x, int y)
{
	//Check the construction tiles
	iPoint world_coords = App->map->WorldToMap(position.x, position.y);
	world_coords.x -= 1;
	world_coords.y -= 1;
	if (App->map->CheckConstructionMap(world_coords, width_in_tiles - 1, height_in_tiles))
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

		action_type = DISAPPEAR;
		if (building_type == TOWN_CENTER)building_type = RUBBLE_FOUR;
		else if (building_type == BARRACK)building_type = RUBBLE_THREE;

		App->entities_manager->buildings_quadtree.Exteract(this, &this->position);
		App->animator->BuildingPlay(this);
		current_animation->Reset();
	}


	else
	{
		current_animation->GetCurrentSprite();
		if (current_animation->IsEnd())
		{
			
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
		ret = App->render->CallBlit(current_animation->GetTexture(), position.x - sprites->at(k).GetXpivot(), position.y - sprites->at(k).GetYpivot(), sprites->at(k).GetFrame(), false, -position.y - sprites->at(k).GetZ_cord(), sprites->at(k).GetOpacity());
		if (!ret)break;
	}

	return ret;
}

void Building::SetPosition(float x, float y, bool insert)
{
	//Set building position fixing it in the tiles coordinates ( center position in the selected tile)
	iPoint map_coords = App->map->WorldToMap(x, y);
	iPoint world_coords = App->map->MapToWorld(map_coords.x, map_coords.y);
	position.x = world_coords.x;
	position.y = world_coords.y - (App->map->data.tile_height + 1) * 0.5f;

	if (!insert)return;

	//Calculate the upper tile of the building zone
	iPoint upper_tile(map_coords.x, map_coords.y);

	//Update the logic & construction map
	//Check if the building is a town center to respect the build exception
	if (building_type == TOWN_CENTER)
	{
		position.y = world_coords.y - (App->map->data.tile_height + 1) * 0.5f;
		upper_tile.x -= 1;
		upper_tile.y -= 1;
		App->map->ChangeLogicMap(upper_tile, width_in_tiles - 2, height_in_tiles - 2, 0);
	}
	else if(building_type == BARRACK)
	{
		position.y = world_coords.y - (App->map->data.tile_height + 1);
		upper_tile.x -= 2;
		upper_tile.y -= 2;
		App->map->ChangeLogicMap(upper_tile, width_in_tiles, height_in_tiles, 0);
	}

	//Update construction map
	App->map->ChangeConstructionMap(upper_tile, width_in_tiles, height_in_tiles, 0);


	//Set interaction area rectangle position
	interact_area.SetPosition(iPoint(position.x, position.y));

	//Set building mark position
	mark.SetPosition(iPoint(position.x, position.y));

	//Add building at the correct quad tree
	App->entities_manager->buildings_quadtree.Insert(this, &position);
}

//Set Methods ---------------
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

const Rectng & Building::GetMark() const
{
	return mark;
}

const Rectng* Building::GetInteractArea() const
{
	return &interact_area;
}

//Get Methods ---------------
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

///----------------------------------------------