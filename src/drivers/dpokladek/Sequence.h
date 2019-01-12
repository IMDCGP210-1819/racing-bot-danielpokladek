#pragma once
#include "Composite.h"

class Sequence : public Composite
{
protected:
	virtual ~Sequence()
	{

	}

	virtual void onInitialize() override
	{
		m_CurrentChild = m_Children.begin();
	}
	virtual Status update() override
	{
		// Keep going until a child behavior says it's running
		for (;;)
		{
			Status s = (*m_CurrentChild)->tick();

			// If child fails or keeps running, do the same
			if (s != BH_SUCCESS)
				return s;								

			// End of array, finished here
			if (++m_CurrentChild == m_Children.end())
				return BH_SUCCESS;
		}

		// Unexpected loop exit
		return BH_INVALID;
	}

	Behaviors::iterator m_CurrentChild;
};

