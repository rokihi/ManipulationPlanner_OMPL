// -*-C++-*-
/*!
 * @file  TrajectoryPlannerSVC_impl.cpp
 * @brief Service implementation code of TrajectoryPlanner.idl
 *
 */

#include "TrajectoryPlannerSVC_impl.h"

/*
 * Example implementational code for IDL interface RTC::TrajectoryPlanner
 */
RTC_TrajectoryPlannerSVC_impl::RTC_TrajectoryPlannerSVC_impl()
{
}


RTC_TrajectoryPlannerSVC_impl::~RTC_TrajectoryPlannerSVC_impl()
{

}


/*
 * Methods corresponding to IDL attributes and operations
 */
RTC::RETURN_VALUE RTC_TrajectoryPlannerSVC_impl::planTrajectory(const RTC::JointPose& start, const RTC::JointPose& goal, RTC::JointTrajectory_out trajectory){
	createSampler();

  jSampler->setPlanningMethod(method);
  //not passed arguments(start, goal, trajectory) yet
  jSampler->setStartAndGoal(start, goal);
  if(jSampler->planWithSimpleSetup(trajectory)){
	  delete jSampler;
	  return RTC::RETVAL_OK;
  }

  delete jSampler;
  return RTC::RETVAL_UNKNOWN_ERROR;
}



// End of example implementational code



