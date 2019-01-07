#pragma once
class Behavior
{
public:
	enum Status
	{
		BH_INVALID,
		BH_SUCCESS,
		BH_FAILURE,
		BH_RUNNING,
		BH_ABORTED
	};

	virtual void onInitialize();			// Called once, immediately before behavior's update method
	virtual Status update() = 0;			// Called exactly once each time the behavior tree updates, until it singnals it has terminated
	virtual void onTerminate(Status);		// Called once, immediately after the previous update signals termination

	Behavior() : m_eStatus(BH_INVALID) {}
	virtual ~Behavior() {}
	Status tick()
	{
		if (m_eStatus != BH_RUNNING)
			onInitialize();

		m_eStatus = update();
		
		if (m_eStatus != BH_RUNNING)
			onTerminate(m_eStatus);
		
		return m_eStatus;
	}
	
	void reset()
	{
		m_eStatus = BH_INVALID;
	}

	void abort()
	{
		onTerminate(BH_ABORTED);
		m_eStatus = BH_ABORTED;
	}

	bool isTerminated() const
	{
		return m_eStatus == BH_SUCCESS || m_eStatus == BH_FAILURE;
	}

	bool isRunning() const
	{
		return m_eStatus == BH_RUNNING;
	}

	Status getStatus() const
	{
		return m_eStatus;
	}

private:
	Status m_eStatus;
};

