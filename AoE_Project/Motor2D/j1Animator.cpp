#include "j1Animator.h"

#include <time.h>
#include <stdlib.h>

#include "j1App.h"
#include "j1FileSystem.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "SDL/include/SDL_rect.h"
#include "p2Log.h"
#include "j1EntitiesManager.h"

///Animation Sprite Class -----------------------
//Constructor ===============
Sprite::Sprite(const SDL_Rect & frame, const iPoint & pivot, int z_cord, uint opacity) : frame(frame), pivot(pivot), z_cord(z_cord), opacity(opacity)
{

}

//Destructor ================
Sprite::~Sprite()
{
}

//Functionality =============
const SDL_Rect * Sprite::GetFrame() const
{
	return &frame;
}
int Sprite::GetXpivot() const
{
	return pivot.x;
}
int Sprite::GetYpivot() const
{
	return pivot.y;
}
int Sprite::GetZ_cord() const
{
	return z_cord;
}
uint Sprite::GetOpacity() const
{
	return opacity;
}
/// ---------------------------------------------



///Animation Class ------------------------------
//Constructor =========================
Animation::Animation()
{

}

//Destructor ==========================
Animation::~Animation()
{
	sprites.clear();
}

void Animation::SetTexture(const SDL_Texture * tex)
{
	texture = (SDL_Texture*)tex;
}

//Functionality =======================
void Animation::SetLoop(bool loop_state)
{
	loop = loop_state;
}

void Animation::SetSpeed(uint new_speed)
{
	speed = new_speed;
}

void Animation::SetId(uint id)
{
	enum_id = id;
}

SDL_Texture * Animation::GetTexture() const
{
	return texture;
}

bool Animation::GetLoop() const
{
	return loop;
}

uint Animation::GetSpeed() const
{
	return speed;
}

const Sprite* Animation::GetCurrentSprite()
{
	if (current_frame == -1)return &sprites[0];

	current_frame = (float)floor(timer.Read() / speed);
	if (current_frame >= sprites.size())
	{
		if (loop)
		{
			loops++;
			current_frame = 0;
			timer.Start();
		}
		else
		{
			loops = 0;
			current_frame = -1;
			return &sprites[0];
		}
	}

	return &sprites[(int)current_frame];
}

const std::vector<Sprite>* Animation::GetAllSprites() const
{
	return &sprites;
}

uint Animation::GetId() const
{
	return enum_id;
}

void Animation::AddSprite(const SDL_Rect & rect, const iPoint & point, const int z, const uint opacity)
{
	sprites.push_back(Sprite(rect, point, z, opacity));
}
/// ---------------------------------------------


/// Animation Block Class -----------------------
//Constructor =========================
Animation_Block::Animation_Block(uint enum_id) :enum_id(enum_id)
{

}

//Destructor ==========================
Animation_Block::~Animation_Block()
{
	ClearAnimationBlocks();
}

//Functionality =======================
void Animation_Block::ClearAnimationBlocks()
{
	while (childs.size() > 0)
	{
		childs.back()->ClearAnimationBlocks();
		childs.pop_back();

	}

	if(animation != nullptr)delete animation;
}

void Animation_Block::SetId(uint id)
{
	enum_id = id;
}

uint Animation_Block::GetId() const
{
	return enum_id;
}

Animation* Animation_Block::GetAnimation() const
{
	return animation;
}

Animation_Block * Animation_Block::GetBlock(int index) const
{
	return childs.at(index);
}

uint Animation_Block::GetChildsNum() const
{
	return childs.size();
}

Animation_Block * Animation_Block::SearchId(uint id) const
{
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (childs[k]->GetId() == id)return childs[k];
	}

	return nullptr;
}

void Animation_Block::SetAnimation(const Animation * new_animation)
{
	if(new_animation != nullptr)animation = (Animation*)new_animation;
}

void Animation_Block::AddAnimationBlock(Animation_Block* new_animation_block)
{
	if(new_animation_block != nullptr)childs.push_back(new_animation_block);
}
/// ---------------------------------------------


//Animator Module -------------------------------
//Constructor =========================
j1Animator::j1Animator()
{
	name = "animator";
}

//Destructor ==========================
j1Animator::~j1Animator()
{
}

//Game Loop ===========================
bool j1Animator::Awake(pugi::xml_node& config)
{
	return true;
}

bool j1Animator::Start()
{
	return true;
}

bool j1Animator::PostUpdate()
{
	return true;
}

bool j1Animator::CleanUp()
{
	//Clean the unit blocks
	uint size = unit_blocks.size();

	for (uint k = 0; k < size; k++)
	{
		unit_blocks[k]->ClearAnimationBlocks();
	}
	unit_blocks.clear();
	//Clean the building blocks
	size = building_blocks.size();

	for (uint k = 0; k < size; k++)
	{
		building_blocks[k]->ClearAnimationBlocks();
	}
	building_blocks.clear();
	//Clean the building blocks
	size = resource_blocks.size();

	for (uint k = 0; k < size; k++)
	{
		resource_blocks[k]->ClearAnimationBlocks();
	}
	resource_blocks.clear();

	return true;
}


//Methods that transform strings to enums (used when loading data from xml)
ENTITY_TYPE j1Animator::StrToEntityEnum(const char * str) const
{
	if (strcmp(str, "unit") == 0)		return UNIT;
	if (strcmp(str, "resource") == 0)	return RESOURCE;
	if (strcmp(str, "building") == 0)	return BUILDING;
	return NO_ENTITY;
}

UNIT_TYPE j1Animator::StrToUnitEnum(const char* str) const
{
	if (strcmp(str, "spearman") == 0)				return SPEARMAN;
	if (strcmp(str, "pikeman") == 0)				return PIKEMAN;
	if (strcmp(str, "paladin") == 0)				return PALADIN;
	if (strcmp(str, "monk") == 0)					return MONK;
	if (strcmp(str, "knight") == 0)					return KNIGHT;
	if (strcmp(str, "heavy_cavalry_archer") == 0)	return HEAVY_CAVALRY_ARCHER;
	if (strcmp(str, "elite_skirmisher") == 0)		return ELITE_SKIRMISHER;
	if (strcmp(str, "cavalry_archer") == 0)			return CAVALRY_ARCHER;
	if (strcmp(str, "cavalier") == 0)				return CAVALIER;
	if (strcmp(str, "militia") == 0)				return MILITIA;
	if (strcmp(str, "arbalest") == 0)				return ARBALEST;
	if (strcmp(str, "archer") == 0)					return ARCHER;
	if (strcmp(str, "villager") == 0)				return VILLAGER;
	return NO_UNIT;
}

ACTION_TYPE j1Animator::StrToActionEnum(const char* str) const
{
	if (strcmp(str, "attack") == 0)		return ATTATCK;
	if (strcmp(str, "die") == 0)		return DIE;
	if (strcmp(str, "disappear") == 0)	return DISAPPEAR;
	if (strcmp(str, "idle") == 0)		return IDLE;
	if (strcmp(str, "walk") == 0)		return WALK;
	return NO_ACTION;
}

ITEM_TYPE j1Animator::StrToItemEnum(const char* str) const
{
	if (strcmp(str, "axe") == 0)		return AXE;
	if (strcmp(str, "basket") == 0)		return BASKET;
	if (strcmp(str, "bow") == 0)		return BOW;
	if (strcmp(str, "hammer") == 0)		return HAMMER;
	if (strcmp(str, "pick") == 0)		return PICK;
	if (strcmp(str, "plow") == 0)		return PLOW;
	if (strcmp(str, "fish") == 0)		return FISH;
	if (strcmp(str, "gold") == 0)		return GOLD;
	if (strcmp(str, "stone") == 0)		return STONE;
	if (strcmp(str, "wood") == 0)		return WOOD;
	if (strcmp(str, "meat") == 0)		return MEAT;
	return NO_ITEM;
}

DIRECTION_TYPE j1Animator::StrToDirectionEnum(const char* str) const
{
	if (strcmp(str, "north") == 0)			return NORTH;
	if (strcmp(str, "north-east") == 0)		return NORTH_EAST;
	if (strcmp(str, "south-east") == 0)		return EAST;
	if (strcmp(str, "east") == 0)			return SOUTH_EAST;
	if (strcmp(str, "south") == 0)			return SOUTH;
	if (strcmp(str, "south-west") == 0)		return SOUTH_WEST;
	if (strcmp(str, "west") == 0)			return WEST;
	if (strcmp(str, "north-west") == 0)		return NORTH_WEST;
	return NO_DIRECTION;
}

BUILDING_TYPE j1Animator::StrToBuildingEnum(const char* str) const
{
	if (strcmp(str, "town_center") == 0)	return TOWN_CENTER;
	return NO_BUILDING;
}

ATTACK_TYPE j1Animator::StrToAttackEnum(const char * str) const
{
	if (strcmp(str, "melee") == 0)		return MELEE;
	if (strcmp(str, "distance") == 0)	return DISTANCE;
	return NO_ATTACK;
}

RESOURCE_TYPE j1Animator::StrToResourceEnum(const char * str) const
{
	if (strcmp(str, "tree") == 0)		return TREE;
	if (strcmp(str, "tree_cut") == 0)	return TREE_CUT;
	if (strcmp(str, "chop") == 0)		return CHOP;
	if (strcmp(str, "berry_bush") == 0)	return BERRY_BUSH;
	if (strcmp(str, "gold_ore") == 0)	return GOLD_ORE;
	if (strcmp(str, "tiny_gold_ore") == 0)	return TINY_GOLD_ORE;
	if (strcmp(str, "stone_ore") == 0)	return STONE_ORE;
	if (strcmp(str, "tiny_stone_ore") == 0)	return TINY_STONE_ORE;
	return NO_RESOURCE;
}

//Functionality =======================
bool j1Animator::LoadUnitBlock(const char* xml_folder)
{
	//Load animations data from folders
	//XML load
	LOG("Loading: %s", xml_folder);
	std::string load_folder = name + "/" + xml_folder;
	pugi::xml_document animations_data;
	if (!App->fs->LoadXML(load_folder.c_str(), &animations_data)) return false;
	
	//Check if the loaded XML have villager structure & if true villager is loaded
	if (strcmp(animations_data.first_child().name(), "villager") == 0) return LoadVillagerBlock(&animations_data);

	//Texture load
	load_folder = name + "/" + animations_data.child("TextureAtlas").attribute("imagePath").as_string();
	SDL_Texture* texture = App->tex->Load(load_folder.c_str());

	//Load Animations data
	//Node focused at any unit node
	pugi::xml_node unit_node = animations_data.child("TextureAtlas").child("unit");
	//Node focused at any unit action node
	pugi::xml_node action_node;
	//Node focused at any direction action node
	pugi::xml_node direction_node;
	//Current sprite node 
	pugi::xml_node sprite;

	//Build new unit animation block
	Animation_Block* unit_anim_block = new Animation_Block();
	//Get unit enum
	unit_anim_block->SetId(StrToUnitEnum(unit_node.attribute("id").as_string()));

	//Iterate all unit action nodes
	action_node = unit_node.first_child();
	while (action_node != NULL)
	{
		//Build new action animation block
		Animation_Block* action_anim_block = new Animation_Block();
		//Get current action enum
		action_anim_block->SetId(StrToActionEnum(action_node.attribute("enum").as_string()));
		//Get current action animation speed
		uint speed = action_node.attribute("speed").as_uint();

		//Focus direction of the current action
		direction_node = action_node.first_child();

		while (direction_node != NULL) {
			
			//Build new direction animation block
			Animation_Block* direction_anim_block = new Animation_Block();
			//Get direction block direction enum
			direction_anim_block->SetId(StrToDirectionEnum(direction_node.attribute("enum").as_string()));

			//Build direction block animation
			Animation* animation = new Animation();
			//Set animation speed
			animation->SetSpeed(speed);
			//Set animation texture
			animation->SetTexture(texture);

			//Focus sprite node at first sprite of direction node
			sprite = direction_node.first_child();

			while (sprite != NULL)
			{
				//Load sprite rect
				SDL_Rect rect = { sprite.attribute("x").as_int(),sprite.attribute("y").as_int(),sprite.attribute("w").as_int(),sprite.attribute("h").as_int() };
				//Load sprite pivot
				float pX = sprite.attribute("pX").as_float() * rect.w;
				pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
				float pY = sprite.attribute("pY").as_float() * rect.h;
				pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);

				//Add built sprite at the current animation
				animation->AddSprite(rect, iPoint((int)pX, (int)pY));

				//Get next sprite node
				sprite = sprite.next_sibling();
			}

			//Add built animation to the direction block
			direction_anim_block->SetAnimation(animation);

			//Add built direction block to the action block
			action_anim_block->AddAnimationBlock(direction_anim_block);

			//Get next direction node
			direction_node = direction_node.next_sibling();
		}

		//Add action block at the unit block
		unit_anim_block->AddAnimationBlock(action_anim_block);

		//Get action next node
		action_node = action_node.next_sibling();

	}

	//Add unit animation block to module vector
	unit_blocks.push_back(unit_anim_block);

	//Release loaded document data
	animations_data.reset();

	return true;
}

bool j1Animator::LoadVillagerBlock(pugi::xml_document* doc)
{
	//Node pointing to the first villager item
	pugi::xml_node item_node = doc->child("villager").first_child();
	//Node for the action data
	pugi::xml_node action_node;
	//Node for the direction data
	pugi::xml_node direction_node;
	//Current sprite node 
	pugi::xml_node sprite;

	//String of the module data folder direction
	std::string dir_str = name + "/";

	//Build new unit animation block
	Animation_Block* unit_anim_block = new Animation_Block();
	//Get unit enum
	unit_anim_block->SetId(VILLAGER);

	//Iterate all items
	while (item_node != NULL)
	{
		//Load the item animations texture
		std::string folder = dir_str + item_node.first_child().attribute("imagePath").as_string();
		SDL_Texture* texture = App->tex->Load(folder.c_str());

		//Allocate the item animation block
		Animation_Block* item_animation_block = new Animation_Block();

		//Set item animation block id
		item_animation_block->SetId(App->animator->StrToItemEnum(item_node.attribute("id").as_string()));

		//Focus the action node at the first item action
		action_node = item_node.child("action");

		//Iterate all the actions
		while (action_node != NULL)
		{

			//Build new action animation block
			Animation_Block* action_anim_block = new Animation_Block();
			//Get current action enum
			action_anim_block->SetId(StrToActionEnum(action_node.attribute("enum").as_string()));
			//Get current action animation speed
			uint speed = action_node.attribute("speed").as_uint();

			//Focus direction of the current action
			direction_node = action_node.first_child();

			while (direction_node != NULL) {

				//Build new direction animation block
				Animation_Block* direction_anim_block = new Animation_Block();
				//Get direction block direction enum
				direction_anim_block->SetId(StrToDirectionEnum(direction_node.attribute("enum").as_string()));

				//Build direction block animation
				Animation* animation = new Animation();
				//Set animation speed
				animation->SetSpeed(speed);
				//Set animation texture
				animation->SetTexture(texture);

				//Focus sprite node at first sprite of direction node
				sprite = direction_node.first_child();

				while (sprite != NULL)
				{
					//Load sprite rect
					SDL_Rect rect = { sprite.attribute("x").as_int(),sprite.attribute("y").as_int(),sprite.attribute("w").as_int(),sprite.attribute("h").as_int() };
					//Load sprite pivot
					float pX = sprite.attribute("pX").as_float() * rect.w;
					pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
					float pY = sprite.attribute("pY").as_float() * rect.h;
					pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);

					//Add built sprite at the current animation
					animation->AddSprite(rect, iPoint((int)pX, (int)pY));

					//Get next sprite node
					sprite = sprite.next_sibling();
				}

				//Add built animation to the direction block
				direction_anim_block->SetAnimation(animation);

				//Add built direction block to the action block
				action_anim_block->AddAnimationBlock(direction_anim_block);

				//Get next direction node
				direction_node = direction_node.next_sibling();

			}
			//Add action block in the item block
			item_animation_block->AddAnimationBlock(action_anim_block);

			//Get next action node
			action_node = action_node.next_sibling();
		}

		//Add item animation block at the unit animation block
		unit_anim_block->AddAnimationBlock(item_animation_block);

		//Get next item node
		item_node = item_node.next_sibling();
	}

	//Add unit animation block to module vector
	unit_blocks.push_back(unit_anim_block);

	//Release loaded document data
	doc->reset();

	return true;
}

bool j1Animator::LoadBuildingBlock(const char* xml_folder)
{
	//Load animations data from folders
	//XML load
	LOG("Loading: %s", xml_folder);
	std::string load_folder = name + "/" + xml_folder;
	pugi::xml_document build_anim_data;
	if (!App->fs->LoadXML(load_folder.c_str(), &build_anim_data))return false;
	//Texture load
	load_folder = name + "/" + build_anim_data.child("TextureAtlas").attribute("imagePath").as_string();
	SDL_Texture* texture = App->tex->Load(load_folder.c_str());

	//Focus building id
	pugi::xml_node build_node = build_anim_data.child("TextureAtlas").child("building");
	
	//Allocate building animation block data
	Animation_Block* building_block = new Animation_Block();
	building_block->SetId((uint)StrToBuildingEnum(build_node.attribute("id").as_string()));

	//Iterate all building actions
	pugi::xml_node action_node = build_node.first_child();
	while (action_node != NULL)
	{
		//Allocate action animation block data
		Animation_Block* action_block = new Animation_Block();
		action_block->SetId((uint)StrToActionEnum(action_node.attribute("enum").as_string()));

		//Build animation
		Animation* anim = new Animation();
		anim->SetSpeed(action_node.attribute("speed").as_uint());
		anim->SetTexture(texture);

		//Iterate all action sprites
		pugi::xml_node sprite = action_node.first_child();
		while (sprite != NULL)
		{
			//Load sprite rect
			SDL_Rect rect = { sprite.attribute("x").as_int(),sprite.attribute("y").as_int(),sprite.attribute("w").as_int(),sprite.attribute("h").as_int() };
			//Load sprite pivot
			float pX = sprite.attribute("pX").as_float() * rect.w;
			pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
			float pY = sprite.attribute("pY").as_float() * rect.h;
			pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);
			//Load sprite opacity
			uint opacity = sprite.attribute("opacity").as_uint();

			//Add sprite at animation
			anim->AddSprite(rect, iPoint(pX, pY), sprite.attribute("z").as_int(),opacity);

			//Focus next animation sprite
			sprite = sprite.next_sibling();
		}

		//Set animation of action block
		action_block->SetAnimation(anim);

		//Add build action to building animation block
		building_block->AddAnimationBlock(action_block);
		
		//Focus next action node
		action_node = action_node.next_sibling();
	}

	//Add loaded building animation to buildings vector
	building_blocks.push_back(building_block);

	//Release loaded document data
	build_anim_data.reset();

	return true;
}

bool j1Animator::LoadResourceBlock(const char * xml_folder)
{
	//Load animations data from folders
	//XML load
	LOG("Loading: %s", xml_folder);
	std::string load_folder = name + "/" + xml_folder;
	pugi::xml_document resource_anim_data;
	if (!App->fs->LoadXML(load_folder.c_str(), &resource_anim_data))return false;
	//Texture load
	load_folder = name + "/" + resource_anim_data.child("TextureAtlas").attribute("imagePath").as_string();
	SDL_Texture* texture = App->tex->Load(load_folder.c_str());

	//Node to a resource type
	pugi::xml_node resource_node = resource_anim_data.first_child().first_child();
	while (resource_node != NULL)
	{
		//Create a pointer to the new resource AnimationBlock
		Animation_Block* resource_block = new Animation_Block();
		resource_block->SetId(StrToResourceEnum(resource_node.attribute("id").as_string()));

		//Focus the first element of the current resource
		pugi::xml_node element_node = resource_node.first_child();
		while (element_node != NULL)
		{
			//Build an animation block for the current element
			Animation_Block* element_block = new Animation_Block();
			
			//Build current element animation
			Animation* anim = new Animation();
			anim->SetLoop(false);

			//Focus the first sprite of the current element
			pugi::xml_node sprite = element_node.first_child();
			while (sprite != NULL)
			{
				//Load sprite rect
				SDL_Rect rect = { sprite.attribute("x").as_int(),sprite.attribute("y").as_int(),sprite.attribute("w").as_int(),sprite.attribute("h").as_int() };
				//Load sprite pivot
				float pX = sprite.attribute("pX").as_float() * rect.w;
				pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
				float pY = sprite.attribute("pY").as_float() * rect.h;
				pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);
				//Load sprite opacity
				uint opacity = sprite.attribute("opacity").as_uint();
				if (opacity == 0)opacity = 255;

				//Add sprite at animation
				anim->AddSprite(rect, iPoint(pX, pY), sprite.attribute("z").as_int(), opacity);

				//Focus next animation sprite
				sprite = sprite.next_sibling();
			}

			//Set built animation texture
			anim->SetTexture(texture);

			//Set animation of the current element block
			element_block->SetAnimation(anim);

			//Add element block to the resource childs list
			resource_block->AddAnimationBlock(element_block);

			//Focus the next element node
			element_node = element_node.next_sibling();
		}

		//Add resource block built at the resources vector
		resource_blocks.push_back(resource_block);

		resource_node = resource_node.next_sibling();
	}

	return true;
}

bool j1Animator::UnitPlay(Unit* target)
{
	DIRECTION_TYPE direction = target->GetDirection();
	switch (direction)
	{
	case NORTH:	target->SetFlipSprite(false);	break;
	
	case NORTH_EAST:
		direction = NORTH_WEST;
		target->SetFlipSprite(true);
		break;
	
	case EAST:
		direction = WEST;
		target->SetFlipSprite(true);
		break;
	
	case SOUTH_EAST:
		direction = SOUTH_WEST;
		target->SetFlipSprite(true);
		break;

	case SOUTH:			target->SetFlipSprite(false);	break;
	case SOUTH_WEST:	target->SetFlipSprite(false);	break;
	case WEST:			target->SetFlipSprite(false);	break;
	case NORTH_WEST:	target->SetFlipSprite(false);	break;

	}
	Animation_Block* block = nullptr;

	//Iterate all blocks of childs vector
	uint size = unit_blocks.size();
	for (uint k = 0; k < size; k++)
	{
		//Pointer to the current block
		block = unit_blocks[k];

		//Compare block unit id
		if (block->GetId() == target->GetUnitType())
		{
			if (block->GetId() == VILLAGER)
			{
				//If the unit is a villager we need to check the item that is carrying
				block = block->SearchId(((Villager*)target)->GetItemType());
			}
			//Compare block action id
			block = block->SearchId(target->GetAction());
			//If action block is found search the correct direction block
			if (block != nullptr)block = block->SearchId(direction);
			//If direction block is found returns the block animation
			if (block != nullptr)
			{
				target->SetAnimation(block->GetAnimation());
				return true;
			}
		}
	}
	
	return false;
}

bool j1Animator::BuildingPlay(Building* target)
{
	Animation_Block* block = nullptr;

	//Iterate all blocks of childs vector
	uint size = building_blocks.size();
	for (uint k = 0; k < size; k++)
	{
		//Pointer to the current block
		block = building_blocks[k];

		//Compare block unit id
		if (block->GetId() == target->GetBuildingType())
		{
			//Compare block action id
			block = block->SearchId(IDLE);
			//If action block is found search the correct direction block or return unidirectional action
			if (target->GetDirectionType() == NO_DIRECTION)
			{
				target->SetAnimation(block->GetAnimation());
				return true;
			}
			if (block != nullptr)block = block->SearchId(target->GetDirectionType());
			//If direction block is found returns the block animation
			if (block != nullptr)
			{
				target->SetAnimation(block->GetAnimation());
				return true;
			}
		}
	}

	return false;
}

bool j1Animator::ResourcePlay(Resource * target)
{
	//Initialize random numbers generator
	time_t t;
	srand(time(&t));

	//Iterate the resource animations vector
	uint size = resource_blocks.size();
	for (uint k = 0; k < size; k++)
	{
		//Compare resource type id
		if (resource_blocks[k]->GetId() == target->GetResourceType())
		{
			//Generate a random number to select one of the possible animations
			uint rand_index = rand() % resource_blocks[k]->GetChildsNum();
			//Choose the child with the random index and get its animation
			Animation* anim = resource_blocks[k]->GetBlock(rand_index)->GetAnimation();
			if (anim != nullptr)
			{
				target->SetAnimation(anim);
				return true;
			}
			else
			{
				LOG("Error getting resource animation");
				return false;
			}
		}
	}
	return false;
}
