#pragma once
#include "Behavior.h"
#include "AI.h"

#include <stdio.h>
#include <iostream>

class BrakeNode : public Behavior
{
	AI *ai;
	tCarElt *car;
	//tTrackSeg *segptr;

	//float currentSpeedSqr;
	//float mu;
	//float maxLookAheadDist;
	//float allowedSpeed;
	//float lookAheadDist;

	int m_iInitializeCalled;
	int m_iTerminateCalled;
	int m_iUpdateCalled;
	Status m_eReturnStatus;
	Status m_eTerminateStatus;

public:
	BrakeNode(AI *_ai)
		: m_iInitializeCalled(0)
		, m_iTerminateCalled(0)
		, m_iUpdateCalled(0)
		, m_eReturnStatus(BH_RUNNING)
		, m_eTerminateStatus(BH_INVALID)
		, ai(_ai)
	{}
	
	virtual ~BrakeNode()
	{

	}

	virtual void onInitialize()
	{
		++m_iInitializeCalled;
		car = ai->carReference;
	}

	virtual void onTerminate(Status s)
	{
		++m_iTerminateCalled;
		m_eTerminateStatus = s;
	}

	virtual Status update() override
	{
		++m_iUpdateCalled;

		BlackboardFloatType *brakeEntry = (BlackboardFloatType*)ai->blackboard.at(ai->brakeKey);
		BlackboardFloatType *accelEntry = (BlackboardFloatType*)ai->blackboard.at(ai->accelKey);
		BlackboardBoolType *stuckEntry = (BlackboardBoolType*)ai->blackboard.at(ai->stuckKey);

		if (stuckEntry->GetValue() == false)
		{
			tTrackSeg *segptr			= car->_trkPos.seg;
			float currentSpeedSqr		= car->_speed_x*car->_speed_x;
			float mu					= segptr->surface->kFriction;
			float maxLookAheadDist		= currentSpeedSqr / (2.0*mu*G);
			float lookAheadDist			= ai->getDistToSegEnd();

			float allowedSpeed	= ai->getAllowedSpeed(segptr);

			if (allowedSpeed < car->_speed_x)
				brakeEntry->SetValue(1.0);

			segptr = segptr->next;
			while (lookAheadDist < maxLookAheadDist)
			{
				allowedSpeed = ai->getAllowedSpeed(segptr);
				if (allowedSpeed < car->_speed_x)
				{
					float allowedSpeedSqr = allowedSpeed * allowedSpeed;
					float brakeDist = ai->mass*(currentSpeedSqr - allowedSpeedSqr) /
										(2.0*(mu*ai->G*ai->mass + allowedSpeedSqr*(ai->CA*mu + ai->CW)));

					if (brakeDist > lookAheadDist)
					{
						brakeEntry->SetValue(1.0);
						accelEntry->SetValue(0.0);
						return BH_SUCCESS;
					}
				}

				lookAheadDist += segptr->length;
				segptr = segptr->next;
			}

			brakeEntry->SetValue(0.0f);
			return BH_FAILURE;
		}
		else
		{
			return BH_FAILURE;
		}

		return BH_INVALID;
	}
};

