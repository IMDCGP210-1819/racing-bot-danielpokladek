#pragma once
#include "Decorator.h"
class Succeder : public Decorator
{
public:
	Succeder(Behavior *child)
		: Decorator(child)
	{
	}

	void setCount(int count)
	{
		m_iLimit = count;
	}

	void onInitialize()
	{
		m_iCounter = 0;
	}

	Status update()
	{
		m_pChild->tick();

		return BH_SUCCESS;
	}

protected:
	int m_iLimit;
	int m_iCounter;
};

