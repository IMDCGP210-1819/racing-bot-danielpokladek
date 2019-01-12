#pragma once
#include "Sequence.h"

class Selector : public Sequence
{
protected:
	virtual ~Selector()
	{
		
	}

	virtual void onInitialize() override
	{
		m_CurrentChild = m_Children.begin();
	}
	virtual Status update()
	{
		// Keep going until a child behavior says its running
		for (;;)
		{
			Status s = (*m_CurrentChild)->tick();

			// If child succeeds or keeps running, do the same.
			if (s != BH_FAILURE)
				return s;

			// Hit the end of the array, it didn't end well...
			if (++m_CurrentChild == m_Children.end())
				return BH_FAILURE;
		}

		return BH_INVALID; // UNEXPECTED LOOP EXIT //
	}

	Behaviors::iterator m_Current;
};

