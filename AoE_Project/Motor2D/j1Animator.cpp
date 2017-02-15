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
	//Node focused at any unit action node
	pugi::xml_node action_node = unit_node.first_child();
	//Node focused at any actions sprite node
	pugi::xml_node sprite;

	//Iterate all unit nodes
	/*while (unit_node != NULL)
	{
		//Iterate all unit action nodes
		while (action_node != NULL)
		{
			sprite = action_node.first_child();
			while (sprite != NULL)
			{
				//Iterate all action sprite nodes
				sprite = sprite.next_sibling();

				/*
					TODO: Iterate all directions of actions (now are not defined in xml)
				
			}
			action_node = action_node.next_sibling();
		}
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
