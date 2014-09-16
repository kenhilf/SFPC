#ifndef _gameobject_h_
#define _gameobject_h_

#include <vector>
#include "position.h"
#include "drawmgr.h"
#include "vector2f.h"

namespace LayerSpace
{
	enum Type { Background, Overlay, Foreground, Window, Top, UpdateOnly, NumTypes };
}

class GameObject
{
friend class GameObjectMgr;

public:
	GameObject(const LayerSpace::Type ls = LayerSpace::Background);
	virtual ~GameObject() {}

	void Update(const float ticks); 
	void Draw(DrawMgr& drawMgr);
	void Physics(const float ticks);

	void AddChild(GameObject& obj);
	bool RemoveChild(GameObject& obj);
	int GetNumChildren() const;
	GameObject& GetChild(const int index);
	bool HasParent() const;
	GameObject& GetParent();
	const GameObject& GetParent() const;

	void SetLocalPos(const Vector2f& pos);
	void SetLocalPos(const Position& pos);
	const Vector2f& GetLocalPos() const;
	void SetWorldPos(const Vector2f& pos);
	void SetWorldPos(const Position& pos);
	const Vector2f& GetWorldPos() const;

	void SetLayerSpace(const LayerSpace::Type ls);
	LayerSpace::Type GetLayerSpace() const;

	bool IsUpdated() const;

protected:
	virtual void DoDraw(DrawMgr& drawMgr) {}
	virtual void DoUpdate(const float ticks) {}
	virtual void DoPhysics(const float ticks) {}
	virtual void ProcessInput() {}

	typedef std::vector<GameObject*> GameObjectPtrList;
	GameObjectPtrList m_children;
	GameObject* m_pParent;
	LayerSpace::Type m_layerSpace;

private:
	Vector2f m_localPos;
	mutable Vector2f m_worldPos;

	bool m_bUpdated;
};

bool CompareLayerSpace(GameObject* lhs, GameObject* rhs);

#endif //_gameobject_h_


