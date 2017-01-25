#include "MotionPlanner.h"

#include <boost/bind.hpp>
using namespace std;

JointStateSampler::JointStateSampler()
{
	m_collision = new Manipulation::CollisionPairSeq();
}
JointStateSampler::~JointStateSampler(){
}

void JointStateSampler::initSampler(const Manipulation::RobotIdentifier& robotID, Manipulation::RobotJointInfo* joints)
{
	m_robotID = robotID;
	m_robotJointInfo = joints;
	setAngleLimits();
}

void JointStateSampler::setAngleLimits(){
	m_jointNum = m_robotJointInfo->jointParameterSeq.length();

	for (size_t  i=0; i < m_jointNum-1; i++){
		JointLimit limit;
		limit.max = m_robotJointInfo->jointParameterSeq[i].limit.upper;
		limit.min = m_robotJointInfo->jointParameterSeq[i].limit.lower;
		m_jointLimits.push_back(limit);
		//std::cout << "max:" << m_jointLimits[i].max << " min:" << m_jointLimits[i].min << std::endl;
	}
}

bool JointStateSampler::isStateValid(const ob::State *state)
{
    //Cast state=>state_vec=>jointAngle
    const ob::RealVectorStateSpace::StateType *state_vec= state->as<ob::RealVectorStateSpace::StateType>();
    Manipulation::JointAngleSeq jointAngleSeq;
    jointAngleSeq.length(m_jointNum);

	for (size_t  i = 0; i < m_jointNum-1; i++){
		jointAngleSeq[i].data= (*state_vec)[i];
	}
	//Gripper
	jointAngleSeq[m_jointNum-1].data = 0.090;

	m_rtcomp->callIsCollide(m_robotID, jointAngleSeq, m_collision);
	bool result;
    if (m_collision->length() > 0) {
    	result = false;
    } else {
    	result = true;
    }
	return result;
}


bool JointStateSampler::planWithSimpleSetup(const Manipulation::JointAngleSeq& startJointAngleSeq, const Manipulation::JointAngleSeq& goalJointAngleSeq,
											Manipulation::ManipulationPlan_out manipPlan)
{
	ob::StateSpacePtr space(new ob::RealVectorStateSpace(m_jointNum-1));

	//Set joint limits
	ob::RealVectorBounds bounds(m_jointNum-1);
	for (int i=0; i < m_jointNum-1; ++i){
		bounds.setLow(i, m_jointLimits[i].min);
		bounds.setHigh(i, m_jointLimits[i].max);
	}
	space->as<ob::RealVectorStateSpace>()->setBounds(bounds);
	og::SimpleSetupPtr sampler(new og::SimpleSetup(space));

	sampler->setStateValidityChecker(boost::bind(&JointStateSampler::isStateValid, this, _1));
	//Set start and goal states
	assert(startJointAngleSeq.length()==goalJointAngleSeq.length());
	m_jointNum =7;
	std::cout << m_jointNum <<std::endl;
	ob::ScopedState<ob::RealVectorStateSpace> start(space);
	for (int i = 0; i < m_jointNum-1; ++i){
	  std::cout << i<<std::endl;
		start->as<ob::RealVectorStateSpace::StateType>()->values[i] = startJointAngleSeq[i].data;
	}
	ob::ScopedState<ob::RealVectorStateSpace> goal(space);
	for (int i = 0; i < m_jointNum-1; ++i){
	  	  std::cout << i<<std::endl;
		goal->as<ob::RealVectorStateSpace::StateType>()->values[i] = goalJointAngleSeq[i].data;
	}
	sampler->setStartAndGoalStates(start, goal);

	std::cout << "Start state: ( ";
	for(int i=0;i<m_jointNum-1;i++){
		std::cout << start->as<ob::RealVectorStateSpace::StateType>()->values[i] << " ";
	}std::cout<< ")" <<std::endl;
	std::cout << "Goal state: ( ";
	for(int i=0;i<m_jointNum-1;i++){
		std::cout << goal->as<ob::RealVectorStateSpace::StateType>()->values[i] << " ";
	}std::cout<< ")" <<std::endl;

	sampler->getSpaceInformation()->setStateValidityCheckingResolution(0.01);

	if (selector == 1) {
		ob::PlannerPtr planner(new og::PRM(sampler->getSpaceInformation()));
		sampler->setPlanner(planner);
	} else if (selector == 2) {
		ob::PlannerPtr planner(new og::RRT(sampler->getSpaceInformation()));
		sampler->setPlanner(planner);
	} else if (selector == 3) {
		ob::PlannerPtr planner(new og::RRTConnect(sampler->getSpaceInformation()));
		sampler->setPlanner(planner);
	} else if (selector == 4) {
		ob::PlannerPtr planner(new og::RRTstar(sampler->getSpaceInformation()));
		sampler->setPlanner(planner);
	} else if (selector == 5) {
		ob::PlannerPtr planner(new og::LBTRRT(sampler->getSpaceInformation()));
		sampler->setPlanner(planner);
	} else if (selector == 6) {
		ob::PlannerPtr planner(new og::LazyRRT(sampler->getSpaceInformation()));
		sampler->setPlanner(planner);
	} else if (selector == 7) {
		ob::PlannerPtr planner(new og::TRRT(sampler->getSpaceInformation()));
		sampler->setPlanner(planner);
	} else if (selector == 8) {
		ob::PlannerPtr planner(new og::pRRT(sampler->getSpaceInformation()));
		sampler->setPlanner(planner);
	} else if (selector == 9) {
		ob::PlannerPtr planner(new og::EST(sampler->getSpaceInformation()));
		sampler->setPlanner(planner);
	}

    Manipulation::ManipulationPlan_var mp(new Manipulation::ManipulationPlan());

    bool result = sampler->solve(30);
	if (result) {
		cout << "Found solution:" << endl;
		sampler->simplifySolution();
		
        og::PathGeometric path(sampler->getSolutionPath());
        path.print(cout);
		//std::ofstream ofs("../plot/path.dat");
		//path.printAsMatrix(ofs);
        mp->manipPath.length(path.getStates().size());
	std::cout <<path.getStates().size()<< std::endl;

        for(int i=0; i<path.getStates().size(); i++){
            mp->manipPath[i].length(m_jointNum-1);
            for(int j =0; j<m_jointNum-1; j++){
       		mp->manipPath[i][j].data = path.getState(i)->as<ob::RealVectorStateSpace::StateType>()->values[j];
		//   std::cout << mp->manipPath[i][j].data<<std::endl;			
            }
	    // std::cout<<std::endl;
        }

	} else {
		cout << "No solution found" << endl;
	}

    manipPlan = mp._retn();
	return result;

}
