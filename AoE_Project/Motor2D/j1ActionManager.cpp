#include "j1ActionManager.h"
#include "Actions_Unit.h"
#include "Actions_Building.h"


///Class Action ---------------------------------
//Base class to build action definitions
//Constructors ========================
Action::Action(Entity* actor, TASK_TYPE type) : actor(actor), type(type)
{

}

//Destructors =========================
Action::~Action()
{
	actor = nullptr;
}

TASK_TYPE Action::GetType()
{
	return type;
}
/// ---------------------------------------------



/// Action Manager ------------------------------
//Action Manager Module
//Constructors ========================
j1ActionManager::j1ActionManager()
{
	name = "action_manager";
}

//Destructors =========================
j1ActionManager::~j1ActionManager()
{

}

//Game Loop ===========================
bool j1ActionManager::CleanUp()
{

	//Freeing space of all actions created

	all_actions.clear();

	return true;
}

//Action Calls ========================
MoveUnitAction* j1ActionManager::MoveAction(Unit * actor, const iPoint& destination, const iPoint& target)
{
	//Generate a new move action definition
	MoveUnitAction* action = new MoveUnitAction(actor, destination, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

MoveUnitAction* j1ActionManager::MoveAction(std::vector<iPoint>* path, Unit * actor, const iPoint& target, Unit* unit_target)
{
	//Generate a new move action definition
	MoveUnitAction* action = nullptr;
	//if unit_target is nullptr move normal
	if (unit_target == nullptr)
		 action = new MoveUnitAction(actor, path, target);
	//else do attack move
	//else
		//MoveUnitAction* action = new MoveUnitAction(actor, path, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

TeleportUnitAction * j1ActionManager::TeleportAction(Unit * actor, const iPoint & displacement)
{
	TeleportUnitAction* action = new TeleportUnitAction(actor, displacement);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

AttackUnitAction* j1ActionManager::AttackToUnitAction(Unit * actor, Unit * target)
{
	//Generate a new attack action definition
	AttackUnitAction* action = new AttackUnitAction(actor, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

AttackBuildingAction * j1ActionManager::AttackToBuildingAction(Unit * actor, Building* target)
{
	//Generate a new move action definition
	AttackBuildingAction* action = new AttackBuildingAction(actor, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

DieUnitAction * j1ActionManager::DieAction(Unit * actor)
{
	//Generate a new die action definition
	DieUnitAction* action = new DieUnitAction(actor);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

DieBuildingAction * j1ActionManager::DieBuildngAction(Building* actor)
{
	//Generate a new die action definition
	DieBuildingAction* action = new DieBuildingAction(actor);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

StunUnitAction * j1ActionManager::StunAction(Unit * actor, uint time)
{
	//Generate a new stun action definition
	StunUnitAction* action = new StunUnitAction(actor, time);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

HealUnitAction * j1ActionManager::HealAction(Unit * actor, Unit * target)
{
	//Generate a new heal action definition
	HealUnitAction* action = new HealUnitAction(actor, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

RecollectVillagerAction* j1ActionManager::RecollectAction(Villager* actor, Resource* target)
{
	//Generate a new move action definition
	RecollectVillagerAction* action = new RecollectVillagerAction(actor, target);

	//Add the new action at the action manager
	all_actions.push_back(action);

	return action;
}

SaveResourcesVillagerAction * j1ActionManager::SaveResourcesAction(Villager * actor, Building* target)
{
	//Generate a new move action definition
	SaveResourcesVillagerAction* action = new SaveResourcesVillagerAction(actor, target);

	//Add the new action at the action manager

	all_actions.push_back(action);

	return action;
}

SpawnUnitAction* j1ActionManager::SpawnAction(ProductiveBuilding * actor, UNIT_TYPE type, DIPLOMACY diplomacy, uint runned_time)
{
	SpawnUnitAction* action = new SpawnUnitAction(actor, type, diplomacy, runned_time);

	all_actions.push_back(action);

	return action;
}

ResearchTecAction * j1ActionManager::ResearchAction(RESEARCH_TECH type, uint r_time, DIPLOMACY diplomacy)
{
	ResearchTecAction* action = new ResearchTecAction(type, r_time, diplomacy);

	all_actions.push_back(action);
	return action;
}

ScannAction * j1ActionManager::ScanAction(Entity * actor)
{
	ScannAction* action = new ScannAction((Unit*)actor);

	all_actions.push_back(action);

	return action;
}

AutoAttackPassiveAction * j1ActionManager::AutoAttackAction(Entity * actor)
{
	AutoAttackPassiveAction* action = new AutoAttackPassiveAction((Unit*)actor);

	all_actions.push_back(action);

	return action;
}

AutoHealPassiveAction * j1ActionManager::AutoHealAction(Entity * actor)
{
	AutoHealPassiveAction* action = new AutoHealPassiveAction((Unit*)actor);

	all_actions.push_back(action);

	return action;
}


void j1ActionManager::SetUnitAutoPassive(Unit * actor)
{
	if (actor->GetUnitType() == MONK || actor->GetUnitType() == WIZARD_CHMP)
	{
		actor->GetWorker()->AddAction(AutoHealAction(actor), PASSIVE);
	}
	else
	{
		actor->GetWorker()->AddAction(AutoAttackAction(actor), PASSIVE);
	}

}

//Methods used in Load/Save -----------
bool j1ActionManager::SaveTask(pugi::xml_node& node, Action* action)
{

	//Save action type
	node.append_attribute("type") = action->GetType();

	switch (action->GetType())
	{
	case TASK_U_MOVE:
		//Save move action goal
		node.append_attribute("goal_x") = ((MoveUnitAction*)action)->GetGoal().x;
		node.append_attribute("goal_y") = ((MoveUnitAction*)action)->GetGoal().y;
		break;
	case TASK_U_TELEPORT:
		//Save teleport action displacement
		node.append_attribute("disp_x") = ((TeleportUnitAction*)action)->GetDisplacement().x;
		node.append_attribute("disp_y") = ((TeleportUnitAction*)action)->GetDisplacement().y;
		break;
	case TASK_U_ATTACK_U:
	{
		//Save attack action target
		Unit* target = ((AttackUnitAction*)action)->GetTarget();
		if (target == nullptr)
		{
			LOG("Error in attack action target");
			return false;
		}
		//Save target unit type
		node.append_attribute("tar_type") = target->GetUnitType();
		//Save target position
		node.append_attribute("tar_pos_x") = target->GetPosition().x;
		node.append_attribute("tar_pos_y") = target->GetPosition().y;
		break;
	}
	case TASK_U_ATTACK_B:
	{
		//Save attack action target
		Building* target = ((AttackBuildingAction*)action)->GetTarget();
		if (target == nullptr)
		{
			LOG("Error in attack action target");
			return false;
		}
		//Save target unit type
		node.append_attribute("tar_type") = target->GetBuildingType();
		//Save target position
		node.append_attribute("tar_pos_x") = target->GetPosition().x;
		node.append_attribute("tar_pos_y") = target->GetPosition().y;
		break;
	}
	case TASK_U_STUN:
		//Save stun time
		node.append_attribute("time") = ((StunUnitAction*)action)->GetTime();
		break;
	case TASK_U_RECOLLECT:
	{
		//Save recollect target characteristics
		Resource* target = ((RecollectVillagerAction*)action)->GetTarget();
		//Save target position
		node.append_attribute("tar_pos_x") = target->GetPosition().x;
		node.append_attribute("tar_pos_y") = target->GetPosition().y;
		//Save target resource type
		node.append_attribute("tar_type") = target->GetResourceType();
		break;
	}
	case TASK_U_SAVE_RESOURCES:
	{
		//Save save resources target characteristics
		Building* target = ((SaveResourcesVillagerAction*)action)->GetTarget();
		//Save target position
		node.append_attribute("tar_pos_x") = target->GetPosition().x;
		node.append_attribute("tar_pos_y") = target->GetPosition().y;
		//Save target resource type
		node.append_attribute("tar_type") = target->GetBuildingType();
		break;
	}
	case TASK_U_SCANN:
		break;
	case TASK_U_AA:
		break;
	case TASK_U_AH:
		break;
	case TASK_B_SPAWN_UNITS:
		//Save unit to spawn type
		node.append_attribute("unit_type") = ((SpawnUnitAction*)action)->GetUnitType();
		//Save unit to spawn diplomacy
		node.append_attribute("unit_diplomacy") = ((SpawnUnitAction*)action)->GetUnitDiplomacy();
		//Save spawn current time 
		node.append_attribute("time") = ((SpawnUnitAction*)action)->GetTime();
		break;
	case TASK_B_RESEARCH:
		//Save research type
		node.append_attribute("research_type") = ((ResearchTecAction*)action)->GetResearchType();
		//Save research diplomacy
		node.append_attribute("research_diplomacy") = ((ResearchTecAction*)action)->GetDiplomacy();
		//Save research time
		node.append_attribute("research_time") = ((ResearchTecAction*)action)->GetResearchTime();
		//Save research current time
		node.append_attribute("res_cur_time") = ((ResearchTecAction*)action)->GetTime();
		break;
	case TASK_NONE:
		LOG("Error Saving Task");
		return false;
		break;
	}

	return true;
}
bool j1ActionManager::LoadTask(pugi::xml_node& node, Entity* actor, TASK_CHANNELS channel)
{
	//Load action type
	TASK_TYPE task_type = (TASK_TYPE)node.attribute("type").as_int();

	//New action pointer
	Action* new_action = nullptr;

	switch (task_type)
	{
	case TASK_U_MOVE:
	{
		//Load move action goal
		iPoint goal;
		goal.x = node.attribute("goal_x").as_int();
		goal.y = node.attribute("goal_y").as_int();
		//Build move action from entity position and goal
		new_action = new MoveUnitAction((Unit*)actor, App->pathfinding->SimpleAstar(actor->GetPositionRounded(), goal), goal);
		//Add the task at the chosen channel
		actor->AddAction(new_action, channel);
		break;
	}
	case TASK_U_TELEPORT:
	{
		//Load teleport action displacement
		iPoint displacement;
		displacement.x = node.attribute("disp_x").as_int();
		displacement.y = node.attribute("disp_y").as_int();
		//Build teleport action from actor and displacement
		new_action = new TeleportUnitAction((Unit*)actor, displacement);
		//Add the task at the chosen channel
		actor->AddAction(new_action, channel);
		break;
	}
	case TASK_U_ATTACK_U:
	{
		//Load target characteristics
		fPoint position = { node.attribute("tar_pos_x").as_float(),node.attribute("tar_pos_y").as_float() };
		UNIT_TYPE unit_type = (UNIT_TYPE)node.attribute("tar_type").as_int();
		//Search attack action target in units list
		Unit* target = nullptr;
		const std::list<Unit*>* units = App->entities_manager->UnitsList();
		std::list<Unit*>::const_iterator unit = units->begin();
		while (unit != units->end())
		{

			if (unit._Ptr->_Myval->GetPosition() == position && unit._Ptr->_Myval->GetUnitType() == unit_type)
			{
				target = unit._Ptr->_Myval;
			}
			unit++;
		}
		if (target == nullptr)
		{
			LOG("Error loading attack action target");
			return false;
		}
		//Build the task with the actor and the target found
		actor->AddAction(App->action_manager->AttackToUnitAction((Unit*)actor, target));
		break;
	}
	case TASK_U_ATTACK_B:
	{
		//Load target characteristics
		fPoint position = { node.attribute("tar_pos_x").as_float(),node.attribute("tar_pos_y").as_float() };
		BUILDING_TYPE unit_type = (BUILDING_TYPE)node.attribute("tar_type").as_int();
		//Search attack action target in units list
		Building* target = nullptr;
		const std::list<Building*>* buildings = App->entities_manager->BuildingList();
		std::list<Building*>::const_iterator building = buildings->begin();
		while (building != buildings->end())
		{

			if (building._Ptr->_Myval->GetPosition() == position && building._Ptr->_Myval->GetBuildingType() == unit_type)
			{
				target = building._Ptr->_Myval;
			}
			building++;
		}
		if (target == nullptr)
		{
			LOG("Error loading attack action target");
			return false;
		}
		//Build the task with the actor and the target found
		actor->AddAction(App->action_manager->AttackToBuildingAction((Unit*)actor, target));
		break;
	}
	case TASK_U_STUN:
		//Load stun time
		actor->AddAction(App->action_manager->StunAction((Unit*)actor, node.attribute("time").as_uint()), channel);
		App->buff_manager->CallBuff((Unit*)actor, TIMED_BUFF, STUN_BUFF);
		break;
	case TASK_U_RECOLLECT:
	{
		//Load target characteristics
		fPoint position = { node.attribute("tar_pos_x").as_float(),node.attribute("tar_pos_y").as_float() };
		RESOURCE_TYPE resource_type = (RESOURCE_TYPE)node.attribute("tar_type").as_int();
		//Iterate resources to find the target
		Resource* target = nullptr;
		const std::list<Resource*>* resources = App->entities_manager->ResourceList();
		std::list<Resource*>::const_iterator resource = resources->begin();
		while (resource != resources->end())
		{
			if (resource._Ptr->_Myval->GetPosition() == position && resource._Ptr->_Myval->GetResourceType() == resource_type)
			{
				target = resource._Ptr->_Myval;
			}
			resource++;
		}
		if (target == nullptr)
		{
			LOG("Error loading recollect action target");
			return false;
		}
		//Build the task with the actor and the target found
		actor->AddAction(App->action_manager->RecollectAction((Villager*)actor, target), channel);
		break;
	}
	break;
	case TASK_U_SAVE_RESOURCES:
	{
		//Load save resources target characteristics
		fPoint position = { node.attribute("tar_pos_x").as_float(),node.attribute("tar_pos_y").as_float() };
		BUILDING_TYPE building_type = (BUILDING_TYPE)node.attribute("tar_type").as_int();
		//Iterate buildings to find the target
		Building* target = nullptr;
		const std::list<Building*>* buildings = App->entities_manager->BuildingList();
		std::list<Building*>::const_iterator building = buildings->begin();
		while (building != buildings->end())
		{
			if (building._Ptr->_Myval->GetPosition() == position && building._Ptr->_Myval->GetBuildingType() == building_type)
			{
				target = building._Ptr->_Myval;
			}
			building++;
		}
		if (target == nullptr)
		{
			LOG("Error loading save resources action target");
			return false;
		}
		//Build the task with the actor and the target found
		actor->AddAction(App->action_manager->SaveResourcesAction((Villager*)actor, target), channel);
		break;
	}
	case TASK_U_SCANN:
		break;
	case TASK_U_AA:
		break;
	case TASK_U_AH:
		break;
	case TASK_B_SPAWN_UNITS:
	{
		//Load unit to spawn type
		UNIT_TYPE unit_type = (UNIT_TYPE)node.attribute("unit_type").as_int();
		//Load unit to spawn diplomacy
		DIPLOMACY diplomacy = (DIPLOMACY)node.attribute("unit_diplomacy").as_int();
		//Build spawn action from unit type, diplomacy and timer runned
		actor->AddAction(App->action_manager->SpawnAction((ProductiveBuilding*)actor, unit_type, diplomacy, node.attribute("time").as_uint()));
		break;
	}
	case TASK_B_RESEARCH:
	{
		//Load research type
		RESEARCH_TECH research_type = (RESEARCH_TECH)node.attribute("research_type").as_int();
		//Load research diplomacy
		DIPLOMACY research_diplomacy = (DIPLOMACY)node.attribute("research_diplomacy").as_int();
		//Load research time
		uint time = MAX(node.attribute("research_time").as_int() - node.attribute("res_cur_time").as_int(), 0);
		//Build research with the loaded characteristics
		actor->AddAction(App->action_manager->ResearchAction(research_type, time, research_diplomacy), channel);
		break;
	}
	case TASK_NONE:
		LOG("Error Saving Task");
		return false;
		break;
	}

	return true;
}
/// ---------------------------------------------



///Action Worker---------------------------------
//Constructor
ActionWorker::ActionWorker() : refresh_rate(500)
{
}

//Destructor
ActionWorker::~ActionWorker()
{
	HardReset();
}

//Worker methods
void ActionWorker::Update()
{
	//Don't update if the worker is paused
	if (paused)
		return;

	//If the worker has a current action execute it
	if (DoWork(&primary_action_queue, &current_primary_action))
	{

		//Reeactive all actions form passive flows when any active action finshes
		if (current_secondary_action != nullptr)	current_secondary_action->Activation();
		if (current_passive_action != nullptr)	current_passive_action->Activation();
	}

	//Secondary and passive actions take place when there is no active_action
	if (current_primary_action == nullptr)
	{
		DoWork(&secondary_action_queue, &current_secondary_action);

		//Make the passive_action logic only work on a refresh rate
		if (refresh_timer.Read() > refresh_rate)
		{
			DoWork(&passive_action_queue, &current_passive_action);
			refresh_timer.Start();
		}
	}
}

bool ActionWorker::DoWork(std::list<Action*>* queue, Action ** current)
{
	//If the worker has a current action execute it
	if (*current != nullptr)
	{
		//If the action ends (execute() == true) erase it
		if ((*current)->Execute())
		{
			(*current)->CleanUp();
			delete (*current);
			(*current) = nullptr;
			return true;
		}
		return false;
	}
	//If the worker has any waiting actions mark the first one as current
	if (!queue->empty() && *current == nullptr)
	{
		(*current) = queue->front();
		queue->pop_front();

		//Delete action if it can't be activated
		if (!(*current)->Activation())
		{
			(*current)->CleanUp();
			delete (*current);
			(*current) = nullptr;
		}
	}
	return false;
}


void ActionWorker::AddAction(Action * action, TASK_CHANNELS channel)
{
	switch (channel)
	{
	case PRIMARY:
		primary_action_queue.push_back(action);
		break;
	case SECONDARY:
		secondary_action_queue.push_back(action);
		break;
	case PASSIVE:
		passive_action_queue.push_back(action);
		break;
	default:
		break;
	}

}



void ActionWorker::AddPriorizedAction(Action * action, TASK_CHANNELS channel)
{
	switch (channel)
	{
	case PRIMARY:
		AddPriorized(action, &primary_action_queue, &current_primary_action);
		break;
	case SECONDARY:
		AddPriorized(action, &secondary_action_queue, &current_secondary_action);
		break;
	case PASSIVE:
		AddPriorized(action, &passive_action_queue, &current_passive_action);
		break;
	default:
		break;
	}
}

void ActionWorker::AddPriorized(Action* action, std::list<Action*>* queue, Action** current)
{
	//Check if there is a current action if it is push it to the front of the queue
	if ((*current) != nullptr)	queue->push_front(*current);

	//Make the new action the current action and activate it
	(*current) = action;
	(*current)->Activation();
}

void ActionWorker::PopAction(Action * action)
{
	if (current_primary_action == action)current_primary_action = nullptr;
	else
	{
		std::list<Action*>::iterator act = primary_action_queue.begin();
		while (act != primary_action_queue.end())
		{
			if (act._Ptr->_Myval == action)
			{
				primary_action_queue.remove(action);
				return;
			}
			act++;
		}
	}
}

void ActionWorker::HardReset()
{
	ResetQueue(&primary_action_queue, &current_primary_action);
	ResetQueue(&secondary_action_queue, &current_secondary_action);
	ResetQueue(&passive_action_queue, &current_passive_action);

	paused = false;
}

void ActionWorker::ResetChannel(TASK_CHANNELS channel)
{
	switch (channel)
	{
	case PRIMARY:
		ResetQueue(&primary_action_queue, &current_primary_action);
		break;
	case SECONDARY:
		ResetQueue(&secondary_action_queue, &current_secondary_action);
		break;
	case PASSIVE:
		ResetQueue(&passive_action_queue, &current_passive_action);
		break;
	default:
		break;
	}

}


void ActionWorker::ResetQueue(std::list<Action*>* queue, Action ** current)
{
	if ((*current) != nullptr)
	{
		(*current)->CleanUp();
		delete (*current);
		(*current) = nullptr;
	}

	Action* to_erase;

	while (!queue->empty())
	{
		to_erase = queue->front();
		to_erase->CleanUp();
		queue->pop_front();
		RELEASE(to_erase);
	}

	queue->clear();
}

TASK_TYPE ActionWorker::GetCurrentActionType() const
{
	if (current_primary_action != nullptr)
	{
		return current_primary_action->GetType();
	}

	return TASK_TYPE::TASK_NONE;
}

Action * ActionWorker::GetCurrentPrimaryAction() const
{
	return current_primary_action;
}

bool ActionWorker::IsBusy(TASK_CHANNELS channel)
{
	switch (channel)
	{

	case PRIMARY:
		if (current_primary_action == nullptr && primary_action_queue.empty())
			return false;
		break;
	case SECONDARY:
		if (current_secondary_action == nullptr && secondary_action_queue.empty())
			return false;
		break;
	case PASSIVE:
		if (current_passive_action == nullptr && passive_action_queue.empty())
			return false;
		break;
	default:
		break;

	}


	return true;;
}

std::list<Action*> ActionWorker::GetActionList(TASK_CHANNELS channel)
{
	std::list<Action*> ret;

	switch (channel)
	{
	case PRIMARY:
		ret = primary_action_queue;
		if (current_primary_action != NULL)ret.push_front(current_primary_action);
		break;
	case SECONDARY:
		ret = secondary_action_queue;
		if (current_secondary_action != NULL)ret.push_front(current_secondary_action);
		break;
	case PASSIVE:
		ret = passive_action_queue;
		if (current_passive_action != NULL)ret.push_front(current_passive_action);
		break;
	default:
		break;
	}


	return ret;
}

std::list<Action*>* ActionWorker::GetActionListPointer(TASK_CHANNELS channel)
{
	std::list<Action*>* ret = nullptr;

	switch (channel)
	{
	case PRIMARY:
		ret = &primary_action_queue;
		break;
	case SECONDARY:
		ret = &secondary_action_queue;
		break;
	case PASSIVE:
		ret = &passive_action_queue;
		break;
	default:
		break;
	}


	return ret;
}

void ActionWorker::Pause()
{
	paused = true;
}
void ActionWorker::Restart()
{
	paused = false;
}
///----------------------------------------------