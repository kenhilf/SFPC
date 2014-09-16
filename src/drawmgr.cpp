#include "drawmgr.h"
#include "position.h"
#include "camera.h"
#include "vector2f.h"


//@TODO: Move these into some common math library
namespace Math
{
	float Sign(float value)
	{
		return value >= 0.0f? 1.0f : -1.0f;
	}

	// Rounds value to the nearest (1.0 / divisor) whole number.
	// This function is also known as a "uniform quantizer"
	// (see: http://en.wikipedia.org/wiki/Quantization_(signal_processing)#Rounding_example )
	// For example, if value == 0.59,
	// divisor == 1.0f, will return 1.0
	// divisor == 2.0f, will return 0.5
	float Round(float value, float divisor = 1.0f)
	{
		return Math::Sign(value) * (static_cast<int>((fabs(value) * divisor) + 0.5f) / divisor);
	}
}


DrawMgr::DrawMgr() :
	m_pCamera(NULL),
	m_redLevel(1.0f),
	m_greenLevel(1.0f),
	m_blueLevel(1.0f),
	m_alphaLevel(1.0f)
{

}

void DrawMgr::DrawQuad(const Texture& tex, const Vector2f& worldPos, const DrawSpace::Type drawSpace, 
					   const FlipMode::Type flip, const ScaleMode::Type scaling, 
					   const float scaleWidth, const float scaleHeight)
{
	float x = 0.0f;
	float y = 0.0f;
	if (drawSpace == DrawSpace::Camera)
	{
		const Vector2f pos = m_pCamera->GetPos();
		float camX = pos.x;
		float camY = pos.y;

		x = worldPos.x - camX;
		y = worldPos.y - camY;
	}
	else // drawSpace == Screen
	{
		x = worldPos.x;
		y = worldPos.y;
	}

	const float texDataRatioX = static_cast<float>(tex.m_dataWidth) / tex.m_width;
	const float texDataRatioY = static_cast<float>(tex.m_dataHeight) / tex.m_height;

    const float maxU = texDataRatioX;
    const float maxV = texDataRatioY;

	float top = 0.0f;
	float bottom = maxV;
	float left = 0.0f;
	float right = maxU;

	switch(flip)
	{
		case FlipMode::None:	break;
		case FlipMode::HFlip:	left = maxU; right = 0.0f; break;
		case FlipMode::VFlip:	top = maxV; bottom = 0.0f; break;
		case FlipMode::HVFlip:	left = maxU; right = 0.0f; top = maxV; bottom = 0.0f; break;
	}

	// We render a quad that matches (or is a scale of) the actual data (subimage) contained
	// in the texture.
	float width = 0.0f;
	float height = 0.0f;

	switch(scaling)
	{
		case ScaleMode::None:
			width  = static_cast<float>(tex.m_dataWidth);
			height = static_cast<float>(tex.m_dataHeight);
			break;
		case ScaleMode::Percentage:
			width  = static_cast<float>(tex.m_dataWidth) * scaleWidth;
			height = static_cast<float>(tex.m_dataHeight) * scaleHeight;
			break;
		case ScaleMode::Direct:
			width  = scaleWidth * texDataRatioX;
			height = scaleHeight * texDataRatioY;
			break;
        default:
            assert(false);
            break;
	}

	glBindTexture(GL_TEXTURE_2D, tex.m_texID);

	glBegin(GL_QUADS);
		glTexCoord2f(left, top);     glVertex2f(x, y);				// left top
		glTexCoord2f(left, bottom);  glVertex2f(x, y+height);		// left bottom
		glTexCoord2f(right, bottom); glVertex2f(x+width, y+height);	// right bottom
		glTexCoord2f(right, top);    glVertex2f(x+width, y);		// right top
	glEnd();

	// The glColor4F call here is to set the colors back to the current global levels, one
	// time color changes are made with the one time fns below prior to calling DrawQuad
	glColor4f(m_redLevel, m_greenLevel, m_blueLevel, m_alphaLevel);
}

void DrawMgr::GlobalBlend(const float blend)
{
	m_alphaLevel = blend;
	glColor4f(m_redLevel, m_greenLevel, m_blueLevel, m_alphaLevel);
}

void DrawMgr::GlobalFade(const float fade)
{
	m_redLevel = m_greenLevel = m_blueLevel = fade;
	glColor4f(m_redLevel, m_greenLevel, m_blueLevel, m_alphaLevel);
}

void DrawMgr::GlobalTint(const float r, const float g, const float b)
{
	m_redLevel = r;
	m_greenLevel = g;
	m_blueLevel = b;
	glColor4f(m_redLevel, m_greenLevel, m_blueLevel, m_alphaLevel);
}

void DrawMgr::OneTimeBlend(const float blend)
{
	glColor4f(1.0f * m_redLevel, 1.0f * m_greenLevel, 1.0f * m_blueLevel, blend * m_alphaLevel);
}

void DrawMgr::OneTimeFade(const float fade)
{
	glColor4f(fade * m_redLevel, fade * m_greenLevel, fade * m_blueLevel, 1.0f * m_alphaLevel);
}

void DrawMgr::OneTimeTint(const float r, const float g, const float b)
{
	glColor4f(r * m_redLevel, g * m_greenLevel, b * m_blueLevel, 1.0f * m_alphaLevel);
}

void DrawMgr::ResetRGBA()
{
	m_redLevel = m_greenLevel = m_blueLevel = m_alphaLevel = 1.0f;
	glColor4f(m_redLevel, m_greenLevel, m_blueLevel, m_alphaLevel);
}

void DrawMgr::BeginFrame()
{
	bluGL::Instance().PreDrawGLScene();
}

void DrawMgr::EndFrame()
{
	bluGL::Instance().PostDrawGLScene();
}


//@TODO: glScissor code does not play nice with maximized windows, maximize button
// currently disabled in bluGL::CreateGLWindow()
void DrawMgr::BeginScissor(const int x, const int y, const int w, const int h)
{
	static float hratio = static_cast<float>(ACTUAL_SCREEN_WIDTH)  / GAME_SCREEN_WIDTH;
	static float vratio = static_cast<float>(ACTUAL_SCREEN_HEIGHT) / GAME_SCREEN_HEIGHT;

	glEnable(GL_SCISSOR_TEST);

	glScissor(static_cast<int>(x * hratio), static_cast<int>((GAME_SCREEN_HEIGHT - y - h) * vratio),
			  static_cast<int>(w * hratio), static_cast<int>(h * vratio));
}

void DrawMgr::EndScissor()
{
	glDisable(GL_SCISSOR_TEST);
}

void DrawMgr::BeginRotate(const float rot, const Vector2f& dest, const Vector2f& offset)
{
	const Vector2f pos = m_pCamera->GetPos();
	float camX = pos.x;
	float camY = pos.y;

	if (camX < 0.0f)
		camX = 0.0f;
	if (camY < 0.0f)
		camY = 0.0f;

	float dx = dest.x + offset.x - camX;
	float dy = dest.y + offset.y - camY;

	glPushMatrix();
	glTranslatef(dx, dy, 0.0f);
	glRotatef(rot, 0.0f, 0.0f, 1.0f);
	glTranslatef(-offset.x, -offset.y, 0.0f);
}

void DrawMgr::EndRotate()
{
	glPopMatrix();
}

void DrawMgr::SetCamera(Camera& cam)
{
	m_pCamera = &cam;
}
