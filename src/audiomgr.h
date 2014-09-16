#ifndef _audiomgr_h_
#define _audiomgr_h_

#include <string>

// using Tony's singleton replacement pattern for hiding private vars
namespace AudioMgr
{
	void Init();
	void Shutdown();
	void Update(const float ticks);

	void PlaySong(const std::string& filename);
	void StopSong();
	void LoopSong(const bool loop);

	void PlaySoundEffect(const std::string& filename, const int delay = 0);
}

#endif //_audiomgr_h_