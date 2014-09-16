#include "audiomgr.h"
#include <assert.h>
#include <audiere.h>
#include <vector>
#include "filepathmgr.h"

namespace AudioMgr
{
using namespace audiere;	

//private vars: static globals, not accessible externally
static OutputStreamPtr g_currSong;
static SoundEffectPtr g_soundEffect;
static AudioDevicePtr g_device;

static float g_volume = 0.5f;

class GameSFX
{
public:
	GameSFX(const std::string& filename, const int delay = 0) :
		m_filename(filename),
		m_delay(delay),
		m_age(0),
		m_bExpired(false)
	{
		std::string soundpath = FilePathMgr::Instance().GetSoundPath() + filename;
		m_sfx = OpenSoundEffect(g_device, soundpath.c_str(), MULTIPLE);
		if (!m_sfx)
			assert(false);
		m_sfx->setVolume(g_volume);				
	}

	bool Update(const double ticks)
	{
		m_age += static_cast<int>(ticks);
		if ((m_age >= m_delay) && (!m_bExpired))
		{
			m_sfx->play();
			m_bExpired = true;
		}
		return m_bExpired;
	}

	bool IsExpired()
	{
		return m_bExpired;
	}

private:
	std::string m_filename;
	double m_delay;
	double m_age;
	SoundEffectPtr m_sfx;
	bool m_bExpired;
};

static std::vector<GameSFX*> g_sfxVector;

void Init()
{
	g_device = OpenDevice();
	if (!g_device)
	{
		assert(false);
	}

	std::string soundpath = FilePathMgr::Instance().GetSoundPath() + "swish.ogg";
	g_soundEffect = OpenSoundEffect(g_device, soundpath.c_str(), MULTIPLE);
	if (!g_soundEffect) 
	{
		//failure
		assert(false);
	}
	g_soundEffect->setVolume(g_volume);
}

void Shutdown()
{
	g_sfxVector.clear();
}

void Update(const double ticks)
{
	// update everything front to back, play whatever is done waiting
	for (unsigned int i = 0; i < g_sfxVector.size(); ++i)
	{
		g_sfxVector[i]->Update(ticks);
	}

	// crawl back to front and delete anything that is expired
/*	for (unsigned int i = g_sfxVector.size(); i > 0; --i)
	{
		if (g_sfxVector[i]->IsExpired())
		{
			g_sfxVector.erase(i);
		}
	}
*/
}

void PlaySong(const std::string& filename)
{
	// Since this file is background music, we don't need to load the
	// whole thing into memory.
	std::string soundpath = FilePathMgr::Instance().GetSoundPath() + filename;
	g_currSong = OpenSound(g_device, soundpath.c_str(), true);
	if (!g_currSong)
	{
		assert(false);
	}

	// Great, we have some opened streams!  What do we do with them?
	// let's start the background music first
	g_currSong->setRepeat(true);
	g_currSong->setVolume(g_volume);
	g_currSong->play();	
}

void StopSong()
{
	g_currSong->stop();
}

void LoopSong(const bool loop)
{
	g_currSong->setRepeat(loop);
}

void PlaySoundEffect(const std::string& filename, const int delay)
{
	g_sfxVector.push_back(new GameSFX(filename, delay));
}

void PlaySwishSound()
{
	g_soundEffect->play();
}

}