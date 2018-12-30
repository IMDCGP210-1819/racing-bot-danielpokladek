#pragma once

#include "Behavior.h"
#include <vector>

class Composite : public Behavior
{
public:
	void addChild(Behavior*);
	void removeChild(Behavior*);
	void clearChildren();
protected:
	typedef std::vector<Behavior*> Behaviors;
	Behaviors m_Children;
};

