#pragma once
class BehaviorTree
{
protected:
	//Behavior* m_pRoot;
public:
	/** Update method, used to traverse the BehaviorTree.
	  * It also serves as the entry point to the tree. */
	void Tick();
};

