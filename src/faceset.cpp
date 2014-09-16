#include "faceset.h"
#include "filepathmgr.h"

FaceSet::FaceSet(const std::string& filename)
{
	LoadFaceSet(filename);
}

const Texture& FaceSet::GetFaceFrame(const FaceState::Type state) const
{
	return m_faceFrames[state];
}

void FaceSet::LoadFaceSet(const std::string& filename)
{
	Image buffer;
	const std::string imagepath = FilePathMgr::Instance().GetImagePath() + filename;
	buffer.LoadBMP(imagepath.c_str());

	for (int i = 0; i < FaceState::NumTypes; i++)
	{
		m_faceFrames[i].LoadSubImage(buffer, (i*48), 0, 48, 64);
	}
}