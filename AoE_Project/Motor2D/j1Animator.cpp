#include "j1Animator.h"

#include "j1App.h"
#include "j1FileSystem.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "SDL/include/SDL_rect.h"
#include "p2Log.h"
#include "BaseEntities.h"

///Animation Sprite Class -----------------------
//Constructor ===============
Sprite::Sprite(const SDL_Rect & frame, const iPoint & pivot, int z_cord) : frame(frame), pivot(pivot), z_cord(z_cord)
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

void Animation::AddSprite(const SDL_Rect & rect, const iPoint & point, const int z)
{
	sprites.push_back(Sprite(rect, point, z));
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
	//Load Civilization Test
	bool ret = LoadCivilization("Teutones.xml");

	return ret;
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
UNIT_TYPE j1Animator::StrToUnitEnum(const char* str) const
{
	if (strcmp(str, "militia") == 0)			return MILITIA;
	if (strcmp(str, "arbalest") == 0)			return ARBALEST;
	if (strcmp(str, "villager") == 0)			return VILLAGER;
	if (strcmp(str, "villager_pick") == 0)		return VILLAGER_PICK;
	if (strcmp(str, "villager_plow") == 0)		return VILLAGER_PLOW;
	if (strcmp(str, "villager_bow") == 0)		return VILLAGER_BOW;
	if (strcmp(str, "villager_basket") == 0)	return VILLAGER_BASKET;
	if (strcmp(str, "villager_axe") == 0)		return VILLAGER_AXE;
	if (strcmp(str, "villager_hammer") == 0)	return VILLAGER_HAMMER;
	if (strcmp(str, "villager_carry") == 0)		return VILLAGER_CARRY;
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

//Functionality =======================
bool j1Animator::LoadCivilization(const char* folder)
{
	j1Timer time;
	time.Start();
	LOG("---- Loading %s...", folder);

	//Load civilization data from loaded folder
	char* buffer = nullptr;
	std::string load_folder = name + "/" + folder;
	int size = App->fs->Load(load_folder.c_str(), &buffer);
	pugi::xml_document civilization_data;
	pugi::xml_parse_result result = civilization_data.load_buffer(buffer, size);
	RELEASE(buffer);

	//Check result of the buffer loaded
	if (result == NULL)
	{
		LOG("Error loading civilization data: %s", result.description());
		return false;
	}

	//Boolean to check the correct file loads
	bool ret = true;
	std::string tex_folder;
	std::string tex_file;
	//Load civilization units list
	pugi::xml_node unit_node = civilization_data.child("data").child("units").first_child();
	while (unit_node != NULL)
	{
		if (!ret)break;
		ret = LoadUnitBlock(unit_node.attribute("xml").as_string(), unit_node.attribute("spritesheet").as_string());
		unit_node = unit_node.next_sibling();
	}
	//Load civilization buildings list
	pugi::xml_node building_node = civilization_data.child("data").child("buildings").first_child();
	while (building_node != NULL)
	{
		if (!ret)break;
		ret = LoadBuildingBlock(building_node.attribute("xml").as_string(), building_node.attribute("spritesheet").as_string());
		building_node = building_node.next_sibling();
	}

	LOG("---- %s loaded in %.3f", folder, time.ReadSec());

	return ret;
}

bool j1Animator::LoadUnitBlock(const char* xml_folder, const char* tex_folder)
{
	//Load animations data from folders
	//XML load
	LOG("Loading: %s", xml_folder);
	char* buffer = nullptr;
	std::string load_folder = name + "/" + xml_folder;
	int size = App->fs->Load(load_folder.c_str(), &buffer);
	pugi::xml_document animations_data;
	pugi::xml_parse_result result = animations_data.load_buffer(buffer, size);
	RELEASE(buffer);
	//Texture load
	load_folder = name + "/" + tex_folder;
	SDL_Texture* texture = App->tex->Load(load_folder.c_str());

	//Check result of the buffer loaded
	if (result == NULL || texture == nullptr)
	{
		LOG("Error loading %s data: %s", xml_folder, result.description());
		return false;
	}

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

bool j1Animator::LoadBuildingBlock(const char* xml_folder, const char* tex_folder)
{
	//Load animations data from folders
	//XML load
	LOG("Loading: %s", xml_folder);
	char* buffer = nullptr;
	std::string load_folder = name + "/" + xml_folder;
	int size = App->fs->Load(load_folder.c_str(), &buffer);
	pugi::xml_document build_anim_data;
	pugi::xml_parse_result result = build_anim_data.load_buffer(buffer, size);
	RELEASE(buffer);
	//Texture load
	load_folder = name + "/" + tex_folder;
	SDL_Texture* texture = App->tex->Load(load_folder.c_str());

	//Check result of the buffer loaded
	if (result == NULL || texture == nullptr)
	{
		LOG("Error loading %s data: %s", xml_folder, result.description());
		return false;
	}

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

			//Add sprite at animation
			anim->AddSprite(rect, iPoint(pX, pY), sprite.attribute("z").as_int());

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

Animation * j1Animator::BuildingPlay(const BUILDING_TYPE unit, const ACTION_TYPE action, const DIRECTION_TYPE direction) const
{
	Animation_Block* block = nullptr;

	//Iterate all blocks of childs vector
	uint size = building_blocks.size();
	for (uint k = 0; k < size; k++)
	{
		//Pointer to the current block
		block = building_blocks[k];

		//Compare block unit id
		if (block->GetId() == unit)
		{
			//Compare block action id
			block = block->SearchId(action);
			//If action block is found search the correct direction block or return unidirectional action
			if (direction == NO_DIRECTION)return block->GetAnimation(); 
			if (block != nullptr)block = block->SearchId(direction);
			//If direction block is found returns the block animation
			if (block != nullptr)return block->GetAnimation();
		}
	}

	return nullptr;
}
