// -*-C++-*-
/*!
 * @file  TrajectoryPlannerSVC_impl.h
 * @brief Service implementation header of TrajectoryPlanner.idl
 *
 */

#include "TrajectoryPlannerSkel.h"
#include "MotionPlanner.h"

#ifndef TRAJECTORYPLANNERSVC_IMPL_H
#define TRAJECTORYPLANNERSVC_IMPL_H
 
/*!
 * @class TrajectoryPlannerSVC_impl
 * Example class implementing IDL interface Manipulation::TrajectoryPlanner
 */
class RTC_TrajectoryPlannerSVC_impl
 : public virtual POA_Manipulation::TrajectoryPlanner,
   public virtual PortableServer::RefCountServantBase
{
 private:
   // Make sure all instances are built on the heap by making the
   // destructor non-public
   //virtual ~TrajectoryPlannerSVC_impl();

  int method = 1;
  JointStateSampler* jSampler;

 public:
  /*!
   * @brief standard constructor
   */
   RTC_TrajectoryPlannerSVC_impl();
  /*!
   * @brief destructor
   */
   virtual ~RTC_TrajectoryPlannerSVC_impl();

   void createSampler(){jSampler = new JointStateSampler();}

   Manipulation::RETURN_VALUE planTrajectory(const Manipulation::JointPose& start, const Manipulation::JointPose& goal, Manipulation::JointTrajectory_out trajectory);

   void passPlanningMethod(int m){method=m;}
   void setMesh(Manipulation::MultiMesh* robotsMesh, Manipulation::Node* envMesh);

   void setComp(RTC::DataFlowComponentBase* ptr){jSampler->setComp(ptr);}

};



#endif // TRAJECTORYPLANNERSVC_IMPL_H


