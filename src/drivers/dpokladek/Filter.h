#pragma once

#include "Sequence.h"

class Filter : public Sequence
{
public:
	void addCondition(Behavior* condition)
	{
		m_Children.insert(m_Children.begin(), condition);
	}

	void addAction(Behavior* action)
	{
		m_Children.insert(m_Children.begin(), action);
	}
};

