#include "j1Animator.h"

#include "j1App.h"
#include "j1FileSystem.h"

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
	if (current_frame > frames.size())
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

void Animation::AddFrame(const SDL_Rect & rect, const iPoint & point)
{
	frames.push_back(rect);
	pivots.push_back(point);
}
/// ---------------------------------------------


/// Animation Block Class -----------------------
template<class ENUM>
Animation_Block<ENUM>::Animation_Block()
{

}

template<class ENUM>
Animation_Block<ENUM>::Animation_Block(const ENUM & type) :type(type)
{

}

template<class ENUM>
Animation_Block<ENUM>::~Animation_Block()
{
	animations.clear();
	ClearAnimationBlocks();
}

template<class ENUM>
void Animation_Block<ENUM>::ClearAnimationBlocks()
{
	while (childs.size() > 0)
	{
		childs.back()->ClearAnimationBlocks();
		childs.pop_back();

	}

	animations.clear();
}

template<class ENUM>
bool Animation_Block<ENUM>::SetEnum(const ENUM & new_enum)
{
	return type = new_enum;
}

template<class ENUM>
ENUM Animation_Block<ENUM>::GetEnum() const
{
	return type;
}

template<class ENUM>
void Animation_Block<ENUM>::AddAnimation(const Animation * new_animation)
{
	if(new_animation != nullptr)animations.push_back(new_animation);
}

template<class ENUM>
void Animation_Block<ENUM>::AddAnimationBlock(Animation_Block* new_animation_block)
{
	if(new_animation_block != nullptr)animation_blocks.push_back(new_animation_block);
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
	std::string unit_enum = unit_node.attribute("enum").as_string();
	Unit_Animation_Block*	unit_anim_block = nullptr;
	//Node focused at any unit action node
	pugi::xml_node action_node;
	std::string action_enum = action_node.attribute("enum").as_string();
	Action_Animation_Block*	action_anim_block = nullptr;
	//Node focused at any actions sprite node
	pugi::xml_node sprite;
	Animation* animation = nullptr;

	//Iterate all unit nodes
	/*while (unit_node != NULL)
	{
		unit_anim_block = new Unit_Animation_Block();

		//Iterate all unit action nodes
		action_node = unit_node.first_child();
		while (action_node != NULL)
		{
			action_anim_block = new Action_Animation_Block();

			//Iterate all action sprite nodes
			animation = new Animation();

			sprite = action_node.first_child();
			while (sprite != NULL)
			{
				animation->AddFrame()

				sprite = sprite.next_sibling();
			}

			unit_anim_block->AddAnimationBlock((Unit_Animation_Block*)action_anim_block);
			action_node = action_node.next_sibling();
		}

		animation_blocks.push_back(unit_anim_block);
		unit_node = unit_node.next_sibling();
		
	}*/

	//Release load document data
	animations_data.reset();

	return true;
}

bool j1Animator::Start()
{
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

UNIT_TYPE j1Animator::Str_to_UnitEnum(const char * str) const
{
	if (str == "militia") return MILITIA;
	return NO_UNIT;
}

ACTION_TYPE j1Animator::Str_to_ActionEnum(const char * str) const
{
	if (str == "attack")return ATTATCK;
	return NO_ACTION;
}

DIRECTION_TYPE j1Animator::Str_to_DirectionEnum(const char * str) const
{
	if (str == "north")return NORTH;
	return NO_DIRECTION;
}
