#ifndef _faceset_h_
#define _faceset_h_

#include "texture.h"
#include <string>

namespace FaceState
{
	enum Type { EyesOpen, MouthOpen, EyesClosed, NumTypes };
}

class FaceSet
{
public:
	FaceSet(const std::string& filename);
	const Texture& GetFaceFrame(const FaceState::Type state) const;

private:
	void LoadFaceSet(const std::string& filename);

	Texture m_faceFrames[FaceState::NumTypes];
};

#endif // faceset_h_