#pragma once

#include "Behavior.h"
#include <vector>

const size_t k_MaxChildrenPerComposite = 2;

class Composite : public Behavior
{
public:
	void addChild(Behavior* child)
	{
		m_Children.push_back(child);
	}

	void removeChild(Behavior*);
	void clearChildren();

protected:
	typedef std::vector<Behavior*> Behaviors;
	Behaviors m_Children;
};

