#pragma once
#include "Sequence.h"

class Selector : public Sequence
{
protected:
	virtual void onInitialize() override
	{
		m_CurrentChild = m_Children.begin();
	}
	virtual Status update()
	{
		// Keep going until a child behavior says its running
		while (true)
		{
			Status s = (*m_CurrentChild)->tick();
			// If child succeeds or keeps running, do the same.
			if (s != BH_FAILURE) return s;
			// Continue search for fallback until the last child.
			if (++m_CurrentChild == m_Children.end())
				return BH_FAILURE;
		}

		return BH_INVALID; // UNEXPECTED LOOP EXIT //
	}
};

