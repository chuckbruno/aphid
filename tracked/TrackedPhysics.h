/*
 *  TrackedPhysics.h
 *  tracked
 *
 *  Created by jian zhang on 5/18/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "Tread.h"
#include "Chassis.h"
#include "GroupId.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
namespace caterpillar {
class TrackedPhysics : public GroupId, public Chassis
{
public:
	TrackedPhysics();
	virtual ~TrackedPhysics();
	void setTrackThickness(const float & x);
	const float trackTension() const;
	void setTrackTension(const float & x);
	void addTension(const float & x);
	void addPower(const float & x);
	void addBrake(bool leftSide);
	void create();
	void setTargetVelocity(const float & x);
	void setTargetSpeed(const float & lft, const float & rgt);
	void displayStatistics() const;
	const float vehicleSpeed() const;
	void createObstacles();

private:
	class CreateWheelProfile {
	public:
		CreateWheelProfile() { isSpringConstraint = false; }
		Vector3F worldP, objectP;
		btRigidBody * connectTo;
		btRigidBody * dstBody;
		btGeneric6DofConstraint* dstHinge;
		btGeneric6DofSpringConstraint* dstSpringHinge;
		float radius, width, mass, gap, scaling;
		bool isLeft;
		bool isSpringConstraint;
	};
	
	void createChassis(Chassis & c);
	void addTreadSections(Tread & t, bool isLeft = true);
	void createTread(Tread & t, bool isLeft = true);
	void createDriveSprocket(Chassis & c, btRigidBody * chassisBody, const float & scaling, bool isLeft = true);
	void createTensioner(Chassis & c, btRigidBody * chassisBody, const float & scaling, bool isLeft = true);
	void createRoadWheels(Chassis & c, btRigidBody * chassisBodyy, const float & scaling, bool isLeft = true);
	void createSupportRollers(Chassis & c, btRigidBody * chassisBody, const float & scaling, bool isLeft = true);
	void createWheel(CreateWheelProfile & profile);
	void createCompoundWheel(CreateWheelProfile & profile);
	btCollisionShape* simpleWheelShape(CreateWheelProfile & profile);
	btCollisionShape* compoundWheelShape(CreateWheelProfile & profile);
	void createWheel(btCollisionShape* wheelShape, CreateWheelProfile & profile);
	btCollisionShape* createShoeShape(Tread & tread, const float & scale);
	btCollisionShape* createPinShape(Tread & tread, const float & scale);
	btCollisionShape* createSprocketShape(CreateWheelProfile & profile, const float & scaling);
	void threePointHinge(btTransform & frameInA, btTransform & frameInB, const float & side, btRigidBody* bodyA, btRigidBody* bodyB);
	btRigidBody * createTorsionBar(btRigidBody * chassisBody, const int & i, const float & scaling, bool isLeft = true);
    const float scaling() const;
    
private:
	Tread m_leftTread, m_rightTread;
	btGeneric6DofSpringConstraint* m_tension[2];
	btGeneric6DofConstraint* m_drive[2];
	float m_targeVelocity, m_trackTension;
	bool m_firstMotion;
};
}
