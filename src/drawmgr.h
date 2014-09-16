#ifndef _drawmgr_h_
#define _drawmgr_h_

#include "texture.h"
#include "bluGL.h"
class Vector2f;
class Position;
class Camera;

namespace DrawSpace
{
	enum Type { Screen, Camera };
}

namespace FlipMode
{
	enum Type { None, HFlip, VFlip, HVFlip };
}

namespace ScaleMode
{
	enum Type { None, Percentage, Direct };
}

class DrawMgr
{
public:
	DrawMgr();
	void DrawQuad(const Texture& tex, const Vector2f& worldPos, const DrawSpace::Type drawSpace, 
				  const FlipMode::Type flip = FlipMode::None, 
				  const ScaleMode::Type scaling = ScaleMode::None, 
				  const float scaleWidth = 1.0f, const float scaleHeight = 1.0f);
	void GlobalBlend(const float blend = 1.0f);
	void GlobalFade(const float fade = 1.0f);
	void GlobalTint(const float r = 1.0f, const float g = 1.0f, const float b = 1.0f);
	void OneTimeBlend(const float blend);
	void OneTimeFade(const float fade);
	void OneTimeTint(const float r, const float g, const float b);
	void ResetRGBA();
	void BeginFrame();
	void EndFrame();
	void BeginScissor(const int x, const int y, const int w, const int h);
	void EndScissor();
	void BeginRotate(const float rot, const Vector2f& dest, const Vector2f& offset);
	void EndRotate();

	void SetCamera(Camera& cam);

private:
	Camera* m_pCamera;

	float m_redLevel;
	float m_greenLevel;
	float m_blueLevel;
	float m_alphaLevel;
};

#endif //_drawmgr_h_
