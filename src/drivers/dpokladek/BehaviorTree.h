#pragma once
#include "Behavior.h"
#include <stdio.h>
#include <deque>

const size_t k_MaxBehaviorTreeMemory = 8192;

class BehaviorTree
{
public:
	BehaviorTree() : m_pBuffer(new uint8_t[k_MaxBehaviorTreeMemory]), m_iOffset(0) {}
	~BehaviorTree() { delete[] m_pBuffer; }

	/** Update method, used to traverse the BehaviorTree.
	* It also serves as the entry point to the tree. */
	void tick();
	bool step();
	//void start(Behavior& bh, BehaviorObserver* observer);
	//void stop(Behavior& bh, Status result);

	template <typename T>
	T& allocate()
	{
		T* node = new ((void*)((uintptr_t)m_pBuffer + m_iOffset)) T;
		m_iOffset += sizeof(T);
		return *node;
	}

protected:
	std::deque<Behavior*> m_Behaviors;

	Behavior* m_pRoot;
	uint8_t* m_pBuffer;
	size_t m_iOffset;
};

