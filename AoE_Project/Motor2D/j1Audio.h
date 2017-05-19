#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "j1Timer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	void VolumeUp();
	void VolumeDown();

	void ResetAudioSystem();

private:

	_Mix_Music*				music = NULL;
	std::list<Mix_Chunk*>	fx;
	j1Timer					go_down_time;
	bool					go_down = false;
	uint					volume = 65;

};

#endif // __j1AUDIO_H__