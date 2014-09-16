#include "gameobject.h"
#include "stlex.h"
#include <assert.h>

//////////////////////
// GameObject		//
//////////////////////

GameObject::GameObject(const LayerSpace::Type ls) :
	m_pParent(NULL),
	m_layerSpace(ls),
	m_bUpdated(false)
{

}

void GameObject::Draw(DrawMgr& drawMgr) 
{
	DoDraw(drawMgr);

	GameObjectPtrList::iterator iter = m_children.begin(); 
	for ( ; iter != m_children.end(); ++iter) 
	{
		(*iter)->Draw(drawMgr);
	}
}

void GameObject::Update(const float ticks)
{
	DoUpdate(ticks);

	GameObjectPtrList::iterator iter = m_children.begin(); 
	for ( ; iter != m_children.end(); ++iter) 
	{
		(*iter)->Update(ticks);
	}
}

void GameObject::Physics(const float ticks)
{
	DoPhysics(ticks);

	GameObjectPtrList::iterator iter = m_children.begin(); 
	for ( ; iter != m_children.end(); ++iter) 
	{
		(*iter)->Physics(ticks);
	}
}

void GameObject::AddChild(GameObject& obj)
{
	obj.m_pParent = this;
	m_children.push_back(&obj);
}

bool GameObject::RemoveChild(GameObject& obj)
{
	return EraseFromSTLContainer(m_children, &obj);
}

int GameObject::GetNumChildren() const
{
	return static_cast<int>(m_children.size()); 
}

GameObject& GameObject::GetChild(const int index)
{
	assert(index <= GetNumChildren());

	return *m_children[index];
}

bool GameObject::HasParent() const
{
	if (m_pParent == NULL)
		return false;
	else
		return true;
}

GameObject& GameObject::GetParent()
{
	assert(HasParent());

	return *m_pParent;
}

const GameObject& GameObject::GetParent() const
{
	assert(HasParent());

	return *m_pParent;
}

void GameObject::SetLocalPos(const Vector2f& pos)
{
	m_localPos = pos;
}

void GameObject::SetLocalPos(const Position& pos)
{
	m_localPos = Vector2f(pos.x, pos.y);
}

const Vector2f& GameObject::GetLocalPos() const
{
	return m_localPos;
}

void GameObject::SetWorldPos(const Vector2f& pos)
{
	Vector2f parentWorldPos;
	if (HasParent())
	{
		parentWorldPos = GetParent().GetWorldPos();
	}
	m_localPos = pos - parentWorldPos;
}

void GameObject::SetWorldPos(const Position& pos)
{
	Vector2f parentWorldPos;
	if (HasParent())
	{
		parentWorldPos = GetParent().GetWorldPos();
	}
	m_localPos.x = static_cast<float>(pos.x) - parentWorldPos.x;
	m_localPos.y = static_cast<float>(pos.y) - parentWorldPos.y;
}

const Vector2f& GameObject::GetWorldPos() const
{
	m_worldPos = m_localPos;
	if (HasParent())
	{
		m_worldPos += GetParent().GetWorldPos();
	}
	return m_worldPos;
}

void GameObject::SetLayerSpace(const LayerSpace::Type ls)
{
	m_layerSpace = ls;
}

LayerSpace::Type GameObject::GetLayerSpace() const
{
	return m_layerSpace;
}

bool GameObject::IsUpdated() const
{
	return m_bUpdated;
}

//////////////////
// Other		//
//////////////////

bool CompareLayerSpace(GameObject* lhs, GameObject* rhs)
{
	return (lhs->GetLayerSpace() < rhs->GetLayerSpace());
}
