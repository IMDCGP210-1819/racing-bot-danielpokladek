#pragma once
#include "Composite.h"

class Parallel : public Composite
{
public:
	enum Policy {
		RequireOne,
		RequireAll
	};
	
	Parallel(Policy successPolicy, Policy failurePolicy)
		: m_eSuccessPolicy(successPolicy)
		, m_eFailurePolicy(failurePolicy)
	{}

	virtual ~Parallel() {}

protected:
	Policy m_eSuccessPolicy;
	Policy m_eFailurePolicy;

	virtual Status update() override
	{
		size_t iSuccessCount = 0, iFailureCount = 0;

		for (Behaviors::iterator it = m_Children.begin(); it != m_Children.end(); ++it)
		{
			Behavior& b = **it;
			if (!b.isTerminated())
			{
				b.tick();
			}

			if (b.getStatus() == BH_SUCCESS)
			{
				++iSuccessCount;
				if (m_eSuccessPolicy == RequireOne)
				{
					return BH_SUCCESS;
				}
			}

			if (b.getStatus() == BH_FAILURE)
			{
				++iFailureCount;
				if (m_eFailurePolicy == RequireOne)
				{
					return BH_FAILURE;
				}
			}
		}

		if (m_eFailurePolicy == RequireAll && iFailureCount == m_Children.size())
			return BH_FAILURE;
		
		if (m_eSuccessPolicy == RequireAll && iSuccessCount == m_Children.size())
			return BH_SUCCESS;

		return BH_RUNNING;
	}

	// Once the termination criteria are fulfilled, all other running behaviors also must be terminated.
	virtual void onTerminate(Status)
	{
		for (Behaviors::iterator it = m_Children.begin(); it != m_Children.end(); ++it)
		{
			Behavior& b = **it;
			if (b.isRunning())
			{
				b.abort();
			}
		}
	}
};