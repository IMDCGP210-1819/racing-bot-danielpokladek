#pragma once

#include "Behavior.h"

class Decorator : public Behavior
{
protected:
	Behavior* m_pChild;
public:
	Decorator(Behavior* child) : m_pChild(child) {}
};

