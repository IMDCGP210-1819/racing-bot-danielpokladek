#pragma once

#include "Composite.h"

class Sequence : public Composite
{
protected:
	Behaviors::iterator m_CurrentChild;
	virtual void onInitialize() override
	{
		m_CurrentChild = m_Children.begin();
	}
	virtual Status update() override
	{
		while (true)									// Keep going until a child behavior says it's running
		{
			Status s = (*m_CurrentChild)->tick();

			if (s != BH_SUCCESS)						// If child fails or keeps running, do the same
				return s;								

			if (++m_CurrentChild == m_Children.end())	// End of array, finished here
				return BH_SUCCESS;
		}
		return BH_INVALID;								// Unexpected loop exit
	}
};

