#include "audiomgr.h"
#include <assert.h>
#include <audiere.h>
#include <list>
#include "filepathmgr.h"

namespace AudioMgr
{
using namespace audiere;	

//private vars: static globals, not accessible externally
static OutputStreamPtr g_currSong;
static AudioDevicePtr g_device;

static float g_sfxVolume = 0.75f;
static float g_musicVolume = 0.33f;

class GameSFX
{
public:
	GameSFX(const std::string& filename, const int delay = 0) :
		m_filename(filename),
		m_length(0.0f),
		m_delay(static_cast<float>(delay)),
		m_age(0.0f),
		m_bHasPlayed(false),
		m_bExpired(false)
	{
		const std::string soundpath = FilePathMgr::Instance().GetSoundPath() + filename;

		SampleSource* source = OpenSampleSource(soundpath.c_str(), FF_AUTODETECT);
		
		// Compute sfx length in seconds
		{
			assert(source->isSeekable()); // Can't retrieve length if not seekable
			int channelCount = 0;
			int sampleRate = 0;
			SampleFormat sampleFormat;
			source->getFormat(channelCount, sampleRate, sampleFormat);
			int numSamples = source->getLength();

			m_length = static_cast<float>(numSamples) / sampleRate;
		}

		m_sfx = OpenSoundEffect(g_device, source, MULTIPLE);

		if (!m_sfx)
			assert(false);
		m_sfx->setVolume(g_sfxVolume);				
	}

	bool Update(const float ticks)
	{
		m_age += static_cast<int>(ticks);
		//@TODO: padding the sound's lifetime by 200ms, otherwise tons of sounds going off
		// at once (open 4dir menu and just mash space fast) can throw an exception 
		// somehow.  Adding 0 or 100 is insufficient, but 200 seems to give it enough time
		// to keep itself together.
		if ((m_bHasPlayed) && (m_age >= (m_delay + static_cast<int>(m_length * 1000) + 200)))
		{
			m_bExpired = true;
		}
		if ((m_age >= m_delay) && (!m_bHasPlayed))
		{
			m_sfx->play();
			m_bHasPlayed = true;
		}
		return m_bExpired;
	}

	bool IsExpired() const
	{
		return m_bExpired;
	}

private:
	std::string m_filename;
	float m_length; // seconds
	float m_delay;
	float m_age;
	SoundEffectPtr m_sfx;
	bool m_bHasPlayed;
	bool m_bExpired;
};

typedef std::list<GameSFX*> GameSFXList;
static GameSFXList g_sfxList;

void Init()
{
	g_device = OpenDevice();
	if (!g_device)
	{
		assert(false);
	}
}

void Shutdown()
{
	GameSFXList::iterator iter;
	for (iter = g_sfxList.begin(); iter != g_sfxList.end(); ++iter)
	{
		delete (*iter);
	}
	g_sfxList.clear();
}

void Update(const float ticks)
{
	GameSFXList::iterator iter;
	for (iter = g_sfxList.begin(); iter != g_sfxList.end(); ++iter)
	{
		(*iter)->Update(ticks);
	}

	for (iter = g_sfxList.begin(); iter != g_sfxList.end();  )
	{
		if ((*iter)->IsExpired())
		{
			delete (*iter);
			iter = g_sfxList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void PlaySong(const std::string& filename)
{
	const std::string soundpath = FilePathMgr::Instance().GetSoundPath() + filename;
	g_currSong = OpenSound(g_device, soundpath.c_str(), true);
	if (!g_currSong)
	{
		assert(false);
	}

	g_currSong->setRepeat(true);
	g_currSong->setVolume(g_musicVolume);
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
	g_sfxList.push_back(new GameSFX(filename, delay));
}

}