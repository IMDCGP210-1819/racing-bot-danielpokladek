#pragma once
#include "Node.h"
#include "Behavior.h"

enum Status
{
	BH_INVALID,
	BH_SUCCESS,
	BH_FAILURE,
	BH_RUNNING
};

class Task
{
public:
	Task(Node& node) : m_pNode(&node) {}
	virtual ~Task() {}

	virtual Status update() = 0;
	virtual void onInitialize() {}
	virtual void onTerminate(Status) {}

protected:
	Node * m_pNode;
};