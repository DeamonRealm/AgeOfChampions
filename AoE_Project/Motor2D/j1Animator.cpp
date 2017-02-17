#include "j1Animator.h"

#include "j1App.h"
#include "j1FileSystem.h"
#include "j1Render.h"
#include "j1Textures.h"

#include "SDL/include/SDL_rect.h"
#include "p2Log.h"

///Animation Class ------------------------------
Animation::Animation()
{

}

Animation::~Animation()
{
	frames.clear();
	pivots.clear();
}

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

bool Animation::GetLoop() const
{
	return loop;
}

uint Animation::GetSpeed() const
{
	return speed;
}

const SDL_Rect& Animation::GetCurrentFrame()
{
	if (current_frame == -1)return frames[0];

	current_frame = (int)floor(timer.Read() / speed);
	if (current_frame >= frames.size())
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
			return frames[0];
		}
	}

	return frames[(int)current_frame];
}

const iPoint& Animation::GetCurrentPivot() const
{
	return pivots.at((int)current_frame);
}

uint Animation::GetId() const
{
	return enum_id;
}

void Animation::AddFrame(const SDL_Rect & rect, const iPoint & point)
{
	frames.push_back(rect);
	pivots.push_back(point);
}
/// ---------------------------------------------


/// Animation Block Class -----------------------
Animation_Block::Animation_Block(uint enum_id) :enum_id(enum_id)
{

}

Animation_Block::~Animation_Block()
{
	ClearAnimationBlocks();
}

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

	//Load animations folder from config.xml
	std::string anim_folder = config.child("folder").child_value();
	std::string load_folder = name + "/" + anim_folder;

	//Load animations data from loaded folder
	char* buffer = nullptr;
	int size = App->fs->Load(load_folder.c_str(), &buffer);
	pugi::xml_document animations_data;
	pugi::xml_parse_result result = animations_data.load_buffer(buffer, size);
	RELEASE(buffer);

	//Check result of the buffer loaded
	if (result == NULL)
	{
		LOG("Error loading animator data: %s", result.description());
		return false;
	}

	//Load Animations data
	//Node focused at any unit node
	pugi::xml_node unit_node = animations_data.child("TextureAtlas").child("unit");
	std::string unit_enum;
	Animation_Block* unit_anim_block = nullptr;
	//Node focused at any unit action node
	pugi::xml_node action_node;
	std::string action_enum;
	Animation_Block* action_anim_block = nullptr;
	//Animation blocks to allocate the action different directions
	std::string direction_enum;
	Animation_Block* dir_0_anim_block = nullptr;
	Animation_Block* dir_1_anim_block = nullptr;
	Animation_Block* dir_2_anim_block = nullptr;
	Animation_Block* dir_3_anim_block = nullptr;
	Animation_Block* dir_4_anim_block = nullptr;
	//Current sprite node 
	pugi::xml_node sprite;
	Animation* dir_0_anim = nullptr;
	Animation* dir_1_anim = nullptr;
	Animation* dir_2_anim = nullptr;
	Animation* dir_3_anim = nullptr;
	Animation* dir_4_anim = nullptr;

	//Iterate all unit nodes
	while (unit_node != NULL)
	{
		//Build new unit animation block
		unit_anim_block = new Animation_Block();
		//Get current unit enum
		unit_enum = unit_node.attribute("id").as_string();
		unit_anim_block->SetId(Str_to_UnitEnum(&unit_enum));

		//Iterate all unit action nodes
		action_node = unit_node.first_child();
		while (action_node != NULL)
		{
			//Build new action animation block
			action_anim_block = new Animation_Block();
			//Get current action enum
			action_enum = action_node.attribute("enum").as_string();
			action_anim_block->SetId(Str_to_ActionEnum(&action_enum));

			//Build new action direction animation blocks
			dir_0_anim_block = new Animation_Block(SOUTH);
			dir_1_anim_block = new Animation_Block(SOUTH_WEST);
			dir_2_anim_block = new Animation_Block(WEST);
			dir_3_anim_block = new Animation_Block(NORTH_WEST);
			dir_4_anim_block = new Animation_Block(NORTH);

			//Iterate all direction sprite nodes
			dir_0_anim = new Animation();
			dir_1_anim = new Animation();
			dir_2_anim = new Animation();
			dir_3_anim = new Animation();
			dir_4_anim = new Animation();
			sprite = action_node.first_child();
			
			while (sprite != NULL)
			{
				//Load sprite rect
				SDL_Rect rect = { sprite.attribute("x").as_int(),sprite.attribute("y").as_int(),sprite.attribute("w").as_int(),sprite.attribute("h").as_int() };
				//Load sprite pivot
				float pX = sprite.attribute("pX").as_float() * rect.w;
				pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
				float pY = sprite.attribute("pY").as_float() * rect.h;
				pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);
					
				//Get current action direction enum
				direction_enum = sprite.attribute("direction").as_string();
				//Add sprite at correct animation
				switch (Str_to_DirectionEnum(&direction_enum))
				{
				case SOUTH:			dir_0_anim->AddFrame(rect, iPoint(pX, pY));		break;
				case SOUTH_WEST:	dir_1_anim->AddFrame(rect, iPoint(pX, pY));		break;
				case WEST:			dir_2_anim->AddFrame(rect, iPoint(pX, pY));		break;
				case NORTH_WEST:	dir_3_anim->AddFrame(rect, iPoint(pX, pY));		break;
				case NORTH:			dir_4_anim->AddFrame(rect, iPoint(pX, pY));		break;
				}

				sprite = sprite.next_sibling();
			}
			//Add animations to direction blocks
			dir_0_anim_block->SetAnimation(dir_0_anim);
			dir_1_anim_block->SetAnimation(dir_1_anim);
			dir_2_anim_block->SetAnimation(dir_2_anim);
			dir_3_anim_block->SetAnimation(dir_3_anim);
			dir_4_anim_block->SetAnimation(dir_4_anim);
			//Add direction blocks to action block
			action_anim_block->AddAnimationBlock(dir_0_anim_block);
			action_anim_block->AddAnimationBlock(dir_1_anim_block);
			action_anim_block->AddAnimationBlock(dir_2_anim_block);
			action_anim_block->AddAnimationBlock(dir_3_anim_block);
			action_anim_block->AddAnimationBlock(dir_4_anim_block);

			//Add action block to unit block
			unit_anim_block->AddAnimationBlock(action_anim_block);

			action_node = action_node.next_sibling();
		}

		//Add unit animation block to module vector
		animation_blocks.push_back(unit_anim_block);

		unit_node = unit_node.next_sibling();
		
	}

	//Release load document data
	animations_data.reset();

	return true;
}

bool j1Animator::Start()
{
	int size = 0;
	test_texture = App->tex->Load("animator/militia.png");

	
	Animation_Block* block = animation_blocks.at(0)->GetBlock(0);
	test = block->GetBlock(0)->GetAnimation();

	block = animation_blocks.at(0)->GetBlock(1);
	test_1 = block->GetBlock(1)->GetAnimation();

	block = animation_blocks.at(0)->GetBlock(2);
	test_2 = block->GetBlock(2)->GetAnimation();

	block = animation_blocks.at(0)->GetBlock(3);
	test_3 = block->GetBlock(3)->GetAnimation();

	block = animation_blocks.at(0)->GetBlock(4);
	test_4 = block->GetBlock(3)->GetAnimation();

	return true;
}

bool j1Animator::PostUpdate()
{
	SDL_Rect rect = test->GetCurrentFrame();
	iPoint pivot = test->GetCurrentPivot();
	App->render->Blit(test_texture, 50 - pivot.x, 350 - pivot.y, &rect);

	rect = test_1->GetCurrentFrame();
	pivot = test_1->GetCurrentPivot();
	App->render->Blit(test_texture, 150 - pivot.x, 350 - pivot.y, &rect);

	rect = test_2->GetCurrentFrame();
	pivot = test_2->GetCurrentPivot();
	App->render->Blit(test_texture, 250 - pivot.x, 350 - pivot.y, &rect);

	rect = test_3->GetCurrentFrame();
	pivot = test_3->GetCurrentPivot();
	App->render->Blit(test_texture, 350 - pivot.x, 350 - pivot.y, &rect);

	rect = test_4->GetCurrentFrame();
	pivot = test_4->GetCurrentPivot();
	App->render->Blit(test_texture, 450 - pivot.x, 350 - pivot.y, &rect);

	return true;
}
bool j1Animator::CleanUp()
{
	//Clean the block childs
	while (animation_blocks.size() > 0)
	{
		animation_blocks.back()->ClearAnimationBlocks();
		animation_blocks.pop_back();
	}

	return true;
}

UNIT_TYPE j1Animator::Str_to_UnitEnum(const std::string* str) const
{
	if (*str == "militia") return MILITIA;
	return NO_UNIT;
}

ACTION_TYPE j1Animator::Str_to_ActionEnum(const std::string* str) const
{
	if (*str == "attack")return ATTATCK;
	return NO_ACTION;
}

DIRECTION_TYPE j1Animator::Str_to_DirectionEnum(const std::string* str) const
{
	if (*str == "north")return NORTH;
	if (*str == "north-east")return NORTH_EAST;
	if (*str == "south-east")return EAST;
	if (*str == "east")return SOUTH_EAST;
	if (*str == "south")return SOUTH;
	if (*str == "south-west")return SOUTH_WEST;
	if (*str == "west")return WEST;
	if (*str == "north-west")return NORTH_WEST;
	return NO_DIRECTION;
}
