/*
 *  Suspension.cpp
 *  wheeled
 *
 *  Created by jian zhang on 5/31/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "Suspension.h"
#include <DynamicsSolver.h>
#include <PhysicsState.h>
#include <Common.h>
namespace caterpillar {
#define SPEEDLIMIT 6.2f
#define BRAKEFORCE 50.f
#define POWERFORCE 30.f

Suspension::Profile::Profile() 
{
	_upperWishboneAngle[0] = -.354f;
	_upperWishboneAngle[1] = .35f;
	_lowerWishboneAngle[0] = -.354f;
	_lowerWishboneAngle[1] = .35f;
	_wheelHubX = .6f;
	_wheelHubR = 1.41f;
	_upperJointY = 2.02f; 
	_lowerJointY = -1.f;
	_steerArmJointZ = 2.f;
	_upperWishboneLength = 3.4f;
	_lowerWishboneLength = 5.7f;
	_upperWishboneTilt = .01f;
	_lowerWishboneTilt = -0.19f;
	_damperY = 4.f;
	_steerable = true;
	_powered = false;
}

float Suspension::RodRadius = .3f;
btRigidBody * Suspension::ChassisBody;
Vector3F Suspension::ChassisOrigin;
int Suspension::Gear = 0;

static float gearRatio[] = { 1.f, .9f, .83f, .79f, .7f, .61f, 1.f };
static float gearForce[] = { 27.f, 29.f, 29.f, 29.f, 29.f, 29.f, 27.f };

Suspension::Suspension() 
{
	m_differential[0] = m_differential[1] = 1.f;
	m_wheelForce[0] = m_wheelForce[1] = 0.f;
}

Suspension::~Suspension() {}
void Suspension::setProfile(const Profile & info)
{
	m_profile = info;
}

const float Suspension::width() const
{
	float wishboneU = m_profile._upperWishboneLength * cos(m_profile._upperWishboneTilt);
	const float wishboneL = m_profile._lowerWishboneLength * cos(m_profile._lowerWishboneTilt);
	if(wishboneL > wishboneU) wishboneU = wishboneL;
	return wishboneU + m_profile._wheelHubX;
}

btRigidBody* Suspension::create(const Vector3F & pos, const float & scaling, bool isLeft) 
{
	Matrix44F tm;
	if(!isLeft) tm.rotateY(PI);
	
	if(isLeft)
		tm.setTranslation(-m_profile._wheelHubX, 0.f, 0.f);
	else
		tm.setTranslation(m_profile._wheelHubX, 0.f, 0.f);
	
	tm.translate(pos);

	btRigidBody* carrier = createCarrier(tm, scaling, isLeft);
	createWishbone(carrier, tm, scaling, true, isLeft);
	btRigidBody* lowerArm = createWishbone(carrier, tm, scaling, false, isLeft);
	createDamper(lowerArm, tm, isLeft);
	
	createSteeringArm(carrier, tm, scaling, isLeft);
	
	btRigidBody * bar = createSwayBar(tm, lowerArm, isLeft);
	
	if(isLeft) m_swayBarLeft = bar;
	else connectSwayBar(tm, bar);
	
	return carrier;
}

btRigidBody* Suspension::createCarrier(const Matrix44F & tm, const float & scaling, bool isLeft)
{
	btCollisionShape* armShape = PhysicsState::engine->createCylinderShape(RodRadius, (m_profile._upperJointY - m_profile._lowerJointY) * .5f, RodRadius);
	btCollisionShape* hubShape = PhysicsState::engine->createCylinderShape(m_profile._wheelHubR, m_profile._wheelHubX * .5f, m_profile._wheelHubR);
	
	btCompoundShape* carrierShape = new btCompoundShape();
	btTransform childT; childT.setIdentity();
	childT.getOrigin()[1] = (m_profile._upperJointY + m_profile._lowerJointY) * .5f * scaling;
	carrierShape->addChildShape(childT, armShape);
	
	Matrix44F ctm; 
	if(isLeft) {
		ctm.rotateZ(PI * -.5f);
	}
	else {
		ctm.rotateZ(PI * .5f);
	}
	ctm.translate(Vector3F(m_profile._wheelHubX * .5f * scaling, 0.f, 0.f));
	
	childT = Common::CopyFromMatrix44F(ctm);
	carrierShape->addChildShape(childT, hubShape);
	
	Matrix44F scaledTm = tm;
	scaledTm.setTranslation(tm.getTranslation() * scaling);
	
	btRigidBody* carrierBody = PhysicsState::engine->createRigidBody(carrierShape, scaledTm, 4.f);
	carrierBody->setDamping(0.f, 0.f);
	
	if(isLeft) m_wheelHub[0] = carrierBody;
	else m_wheelHub[1] = carrierBody;
	
	return carrierBody;
}

btRigidBody* Suspension::createWishbone(btRigidBody* carrier, const Matrix44F & tm, const float & scaling, bool isUpper, bool isLeft)
{
	Matrix44F btm;
	
	if(isUpper) btm.rotateZ(m_profile._upperWishboneTilt);
	else btm.rotateZ(m_profile._lowerWishboneTilt);
	if(!isLeft) btm.rotateY(PI);
	
	if(isUpper) btm.translate(Vector3F(0.f, m_profile._upperJointY, 0.f) * scaling);
	else btm.translate(Vector3F(0.f, m_profile._lowerJointY, 0.f) * scaling);
	
	btm.translate(tm.getTranslation() * scaling);
	
	btCompoundShape* wishboneShape = createWishboneShape(scaling, isUpper, isLeft);
	
	btRigidBody* wishboneBody = PhysicsState::engine->createRigidBody(wishboneShape, btm, 1.f);
	wishboneBody->setDamping(0.f, 0.f);
	
	connectArm(wishboneBody, btm, scaling, isUpper, isLeft, true);
	connectArm(wishboneBody, btm, scaling, isUpper, isLeft, false);
	
	Matrix44F frmCarrier;
	if(isUpper) frmCarrier.translate(Vector3F(0.f, m_profile._upperJointY, 0.f) * scaling);
	else frmCarrier.translate(Vector3F(0.f, m_profile._lowerJointY, 0.f) * scaling);
	
	Matrix33F rot; 
	if(isUpper) rot.rotateZ(-m_profile._upperWishboneTilt);
	else rot.rotateZ(-m_profile._lowerWishboneTilt);
	
	Matrix44F frmArm; 
	frmArm.setRotation(rot); 
	
	const float fra = 1.5f;
	btGeneric6DofConstraint* ball = PhysicsState::engine->constrainBy6Dof(*carrier, *wishboneBody, frmCarrier, frmArm, true);
	ball->setLinearLowerLimit(btVector3(0.0f, 0.0f,0.0f));
	ball->setLinearUpperLimit(btVector3(0.0f, 0.0f,0.0f));
	ball->setAngularLowerLimit(btVector3(-fra, -fra, -fra));
	ball->setAngularUpperLimit(btVector3(fra, fra, fra));
	
	return wishboneBody;
}

void Suspension::connectArm(btRigidBody* arm, const Matrix44F & tm, const float & scaling, bool isUpper, bool isLeft, bool isFront)
{
	Matrix44F localTM = wishboneHingTMLocal(1.f, isUpper, isLeft, isFront);
	
	Matrix33F rot; 
	if(isUpper) rot.rotateZ(-m_profile._upperWishboneTilt);
	else rot.rotateZ(-m_profile._lowerWishboneTilt);
	localTM.setRotation(rot);
	
	Matrix44F scaledBack = tm;
	scaledBack.setTranslation(tm.getTranslation() / scaling);
	
	Matrix44F hingeTM = wishboneHingTMLocal(1.f, isUpper, isLeft, isFront);
	hingeTM *= scaledBack;
	
	rot.setIdentity();
	if(!isLeft) rot.rotateY(PI);
	hingeTM.setTranslation(hingeTM.getTranslation() - ChassisOrigin);
	hingeTM.setRotation(rot);
	
	btGeneric6DofConstraint* hinge = PhysicsState::engine->constrainBy6Dof(*ChassisBody, *arm, hingeTM, localTM, true);
	hinge->setAngularLowerLimit(btVector3(0.0, 0.0, -.2f));
	hinge->setAngularUpperLimit(btVector3(0.0, 0.0, .2f));
	hinge->setLinearLowerLimit(btVector3(0.0, 0.0, 0.0));
	hinge->setLinearUpperLimit(btVector3(0.0, 0.0, 0.0));
}

btCompoundShape* Suspension::createWishboneShape(const float & scaling, bool isUpper, bool isLeft)
{
	btCompoundShape* shape = new btCompoundShape();
	
	float l, ang;
	wishboneLA(isUpper, isLeft, true, l, ang);
		
	Matrix44F tm;
	tm.rotateZ(PI * .5f);
	tm.rotateY(-ang);
	
	tm.setTranslation(Vector3F(-l * .5f * cos(ang), 0.f, -l * .5f * sin(ang)) * scaling);
	
	btTransform childT = Common::CopyFromMatrix44F(tm);
	
	btCollisionShape* armShape = PhysicsState::engine->createCylinderShape(RodRadius, l * .5f, RodRadius);
	
	shape->addChildShape(childT, armShape);
	
	wishboneLA(isUpper, isLeft, false, l, ang);
	
	tm.setIdentity();
	tm.rotateZ(PI * .5f);
	tm.rotateY(-ang);
	
	tm.setTranslation(Vector3F(-l * .5f * cos(ang), 0.f, -l * .5f * sin(ang)) * scaling);
	
	childT = Common::CopyFromMatrix44F(tm);
	
	armShape = PhysicsState::engine->createCylinderShape(RodRadius, l * .5f, RodRadius);
	
	shape->addChildShape(childT, armShape);
	
	return shape;
}

const Matrix44F Suspension::wishboneHingTMLocal(const float & scaling, bool isUpper, bool isLeft, bool isFront) const
{	
	float l, ang;
	wishboneLA(isUpper, isLeft, isFront, l, ang);
	
	Matrix44F local;
	local.setTranslation(Vector3F(-l * cos(ang), 0.f, -l  * sin(ang)) * scaling);
	
	return local;
}

void Suspension::wishboneLA(bool isUpper, bool isLeft, bool isFront, float & l, float & a) const
{
	float lA, lB, angA, angB;
	if(isUpper) {
		angA = m_profile._upperWishboneAngle[0];
		lA = m_profile._upperWishboneLength / cos(angA);
		
		angB = m_profile._upperWishboneAngle[1];
		lB = m_profile._upperWishboneLength / cos(angB);
	}
	else {
		angA = m_profile._lowerWishboneAngle[0];
		lA = m_profile._lowerWishboneLength / cos(angA);
		
		angB = m_profile._lowerWishboneAngle[1];
		lB = m_profile._lowerWishboneLength / cos(angB);
	}
	
	if(!isLeft) {
		angA *= -1.f;
		angB *= -1.f;
	}
	
	l = lA;
	a = angA;
	if(!isFront) {
		l = lB;
		a = angB;
	}
}

btRigidBody* Suspension::createSteeringArm(btRigidBody* carrier, const Matrix44F & tm, const float & scaling, bool isLeft)
{
	Matrix44F btm;
	
	btm.rotateZ(m_profile._lowerWishboneTilt);
	if(!isLeft) btm.rotateY(PI);
	
	btm.translate(Vector3F(0.f, 0.f, m_profile._steerArmJointZ));
	
	btm.translate(tm.getTranslation());
	
	float l, ang;
	wishboneLA(false, isLeft, true, l, ang);
	
	Matrix44F atm;
	atm.rotateZ(PI * .5f);

	atm.setTranslation(Vector3F(-l * .5f * cos(ang), 0.f, 0.f));
	
	atm *= btm;
	
	btCollisionShape* armShape = PhysicsState::engine->createCylinderShape(RodRadius, l * .5f * cos(ang), RodRadius);
	
	btRigidBody* armBody = PhysicsState::engine->createRigidBody(armShape, atm, 1.f);
	armBody->setDamping(0.f, 0.f);
	
	Matrix44F tmB; 
	
	if(isLeft) tmB.rotateZ(-m_profile._lowerWishboneTilt - PI* .5f);
	else tmB.rotateZ(- PI* .5f - m_profile._lowerWishboneTilt);
	
	tmB.translate(0.f, l * .5f * cos(ang) * scaling, 0.f);
	
	Matrix44F tmA;
	if(!isLeft) tmA.rotateY(PI);
	
	Vector3F pobj(0.f, l * .5f * cos(ang) * scaling, 0.f);
	pobj = atm.transform(pobj);
	pobj -= ChassisOrigin;
	tmA.translate(pobj);
	
	btGeneric6DofConstraint* hinge = PhysicsState::engine->constrainBy6Dof(*ChassisBody, *armBody, tmA, tmB, true);
	
	hinge->setAngularLowerLimit(btVector3(0.f, 0.f, -.2f));
	hinge->setAngularUpperLimit(btVector3(0.f, 0.f, .2f));
	hinge->setLinearLowerLimit(btVector3(0.0, 0.0, 0.0));
	hinge->setLinearUpperLimit(btVector3(0.0, 0.0, 0.0));
	
	tmA.setIdentity();
	if(isLeft) tmA.translate(0.f, 0.f, m_profile._steerArmJointZ * scaling);
	else tmA.translate(0.f, 0.f, -m_profile._steerArmJointZ * scaling);
	
	tmB.translate(0.f, l * -1.f * cos(ang) * scaling, 0.f);
	
	hinge = PhysicsState::engine->constrainBy6Dof(*carrier, *armBody, tmA, tmB, true);
	
	hinge->setAngularLowerLimit(btVector3(-.2f, -.2f, -.2f));
	hinge->setAngularUpperLimit(btVector3(.2f, .2f, .2f));
	hinge->setLinearLowerLimit(btVector3(0.0, 0.0, 0.0));
	hinge->setLinearUpperLimit(btVector3(0.0, 0.0, 0.0));
	
	if(isLeft) m_steerJoint[0] = hinge;
	else m_steerJoint[1] = hinge;
	
	return armBody;
}

btRigidBody* Suspension::createDamper(btRigidBody * lowerArm, const Matrix44F & tm, bool isLeft)
{
	Matrix44F lowerJntTm;
	
	const float tilt = atan((m_profile._upperWishboneLength - RodRadius * 5.f) / (m_profile._damperY - m_profile._lowerJointY));
	const float l = (m_profile._damperY - m_profile._lowerJointY) / cos(tilt);
	
	lowerJntTm.rotateZ(tilt);
	
	lowerJntTm.translate(Vector3F(RodRadius * -5.f, m_profile._lowerJointY, 0.f));
	
	lowerJntTm *= tm;
	
	Matrix44F damperTm; damperTm.translate(0.f, l * .25f, 0.f);
	damperTm *= lowerJntTm;
	
	btCollisionShape* damperShape = PhysicsState::engine->createCylinderShape(RodRadius, l * .25f, RodRadius);

	btRigidBody* damperLowBody = PhysicsState::engine->createRigidBody(damperShape, damperTm, 1.f);
	damperLowBody->setDamping(0.f, 0.f);
	
	damperTm.setIdentity(); 
	damperTm.translate(0.f, l * .75f, 0.f);
	damperTm *= lowerJntTm;

	btRigidBody* damperHighBody = PhysicsState::engine->createRigidBody(damperShape, damperTm, 1.f);
	damperHighBody->setDamping(0.f, 0.f);
	
	damperTm.setIdentity();
	damperTm.rotateZ(- tilt);
	damperTm.translate(0.f, -l * .25f, 0.f);
	
	Matrix44F armTm; armTm.translate(RodRadius * -5.f, 0.f, 0.f);
	armTm.rotateZ(-m_profile._lowerWishboneTilt);
	
	btGeneric6DofConstraint* hinge = PhysicsState::engine->constrainBy6Dof(*damperLowBody, *lowerArm, damperTm, armTm, true);
	hinge->setAngularLowerLimit(btVector3(0.f, 0.f, -.5f));
	hinge->setAngularUpperLimit(btVector3(0.f, 0.f, .5f));
	hinge->setLinearLowerLimit(btVector3(0.0, 0.0, 0.0));
	hinge->setLinearUpperLimit(btVector3(0.0, 0.0, 0.0));
	
	damperTm.translate(0.f, l * .5f, 0.f);
	
	Matrix44F chassisTm; chassisTm.translate(RodRadius * -5.f, m_profile._lowerJointY, 0.f);
	chassisTm.translate(-l * sin(tilt), l * cos(tilt), 0.f);
	chassisTm *= tm;
	chassisTm.setTranslation(chassisTm.getTranslation() - ChassisOrigin);
	
	hinge = PhysicsState::engine->constrainBy6Dof(*damperHighBody, *ChassisBody, damperTm, chassisTm, true);
	hinge->setAngularLowerLimit(btVector3(0.f, 0.f, -.5f));
	hinge->setAngularUpperLimit(btVector3(0.f, 0.f, .5f));
	hinge->setLinearLowerLimit(btVector3(0.0, 0.0, 0.0));
	hinge->setLinearUpperLimit(btVector3(0.0, 0.0, 0.0));
	
	Damper * shockAbsorber = new Damper(damperLowBody, damperHighBody, l * .25f);
	
	if(isLeft) m_damper[0] = shockAbsorber;
	else m_damper[1] = shockAbsorber;
	
	return damperHighBody;
}

btRigidBody* Suspension::createSwayBar(const Matrix44F & tm, btRigidBody * arm, bool isLeft)
{
	const float l = tm.getTranslation().x - ChassisOrigin.x;
	
	float bl = l;
	if(bl < 0.f) bl = -bl;
	
	btCollisionShape* barShape = PhysicsState::engine->createCylinderShape(RodRadius, bl* .5f - 5.f * RodRadius, RodRadius);
	
	Matrix44F barTm; 
	barTm.rotateZ(PI * .5f);
	barTm *= tm;
	
	float xoff = -l * .5f - 5.f * RodRadius;
	if(!isLeft) xoff = -l * .5f + 5.f * RodRadius;
	
	const float zoff = m_profile._lowerWishboneLength * sin(m_profile._lowerWishboneAngle[1]);
	
	barTm.translate(xoff, m_profile._lowerJointY, -zoff);
	
	const Vector3F jnt = barTm.getTranslation();

	btRigidBody* barBody = PhysicsState::engine->createRigidBody(barShape, barTm, 1.f);
	barBody->setDamping(0.f, 0.f);
	
	barTm.setIdentity();
	barTm.translate(0.f, bl* .5f - 5.f * RodRadius -bl, 0.f);
	if(isLeft) barTm.translate(0.f, 0.f, zoff);
	else barTm.translate(0.f, 0.f, -zoff);
	
	Matrix44F frmArm;
	
	btGeneric6DofConstraint* ball = PhysicsState::engine->constrainBy6Dof(*barBody, *arm, barTm, frmArm, true);
	ball->setLinearLowerLimit(btVector3(0.f, 0.f, 0.f));
	ball->setLinearUpperLimit(btVector3(0.f, 0.f, 0.f));
	ball->setAngularLowerLimit(btVector3(-PI, -PI, -PI));
	ball->setAngularUpperLimit(btVector3(PI, PI, PI));
	
	barTm.setIdentity();
	
	Matrix44F chassisTm;
	chassisTm.rotateZ(PI * .5f);
	if(!isLeft) {
		Matrix44F flipX; flipX.rotateY(PI);
		chassisTm *= flipX;
	}
	
	chassisTm.setTranslation(jnt - ChassisOrigin);
	
	ball = PhysicsState::engine->constrainBy6Dof(*barBody, *ChassisBody, barTm, chassisTm, true);
	ball->setLinearLowerLimit(btVector3(0.f, 0.f, 0.f));
	ball->setLinearUpperLimit(btVector3(0.f, 0.f, 0.f));
	ball->setAngularLowerLimit(btVector3(0.f, -PI, 0.f));
	ball->setAngularUpperLimit(btVector3(0.f, PI, 0.f));
	
	return barBody;
}

void Suspension::connectSwayBar(const Matrix44F & tm, btRigidBody * bar)
{
	const float l = tm.getTranslation().x - ChassisOrigin.x;
	
	float bl = l;
	if(bl < 0.f) bl = -bl;
	bl *= .5f;
	bl -= 5.f * RodRadius;
	
	Matrix44F frmA;
	frmA.translate(Vector3F(0.f, bl, 0.f));
	
	Matrix44F frmB; frmB.rotateX(PI);
	frmB.translate(0.f, bl, 0.f);
	
	btGeneric6DofSpringConstraint* ball = PhysicsState::engine->constrainBySpring(*m_swayBarLeft, *bar, frmA, frmB, true);
	ball->setLinearLowerLimit(btVector3(0.f, -1.f, 0.f));
	ball->setLinearUpperLimit(btVector3(0.f, 1.f, 0.f));
	ball->setAngularLowerLimit(btVector3(0.f, -PI, 0.f));
	ball->setAngularUpperLimit(btVector3(0.f, PI, 0.f));
	
	ball->enableSpring(4, true);
	ball->setStiffness(4, 600.f);
	ball->setDamping(4, 0.05f);
	ball->setEquilibriumPoint(4, 0.f);
}

const float Suspension::wheelHubX() const { return m_profile._wheelHubX; }

void Suspension::connectWheel(Wheel* wheel, bool isLeft)
{
	Matrix44F frmA;
	frmA.translate(Vector3F(wheelHubX(), 0.f, 0.f));
	
	Matrix44F frmB;
	
	btRigidBody * hub = m_wheelHub[1];
	if(isLeft) hub = m_wheelHub[0];
	btGeneric6DofConstraint* drv = PhysicsState::engine->constrainBy6Dof(*hub, *wheel->body(), frmA, frmB, true);
	drv->setLinearLowerLimit(btVector3(0.0, 0.0, 0.0));
	drv->setLinearUpperLimit(btVector3(0.0, 0.0, 0.0));
	drv->setAngularLowerLimit(btVector3(-SIMD_PI, 0.0, 0.0));
	drv->setAngularUpperLimit(btVector3(SIMD_PI, 0.0, 0.0));
	
	if(isLeft) m_driveJoint[0] = drv;
	else m_driveJoint[1] = drv;
	
	if(isLeft) m_wheel[0] = wheel;
	else m_wheel[1] = wheel;
}

const bool Suspension::isPowered() const { return m_profile._powered; }
const bool Suspension::isSteerable() const { return m_profile._steerable; }

float Suspension::limitDrive(const int & i, const float & targetSpeed, bool goForward)
{
	float wheelSpeed = wheelVelocity(i).length();
	float diff = targetSpeed - wheelSpeed;
	
	float force = 33.f;
	if(diff < 0.f) {
		std::cout<<"decelerating ";
		// force = 43.f;
	}
	
	const float r = m_wheel[0]->radius();
	const float lmt = r * SPEEDLIMIT;
	if(diff > lmt) diff = lmt;
	else if(diff < -lmt) diff = -lmt;
	
	wheelSpeed += diff * m_differential[i];
	// std::cout<<"drive ["<<i<<"] "<<wheelSpeed;
	float rps = wheelSpeed / r;
	if(!goForward) rps = -rps;
	applyMotor(rps, i, force);
	return rps;
}

void Suspension::releaseBrake()
{
	if(!isPowered()) {
		m_driveJoint[0]->getRotationalLimitMotor(0)->m_enableMotor = false;
		m_driveJoint[1]->getRotationalLimitMotor(0)->m_enableMotor = false;
	}
}

void Suspension::applyMotor(float rps, const int & i, float force)
{
	m_driveJoint[i]->getRotationalLimitMotor(0)->m_enableMotor = true;
	if(i==0) m_driveJoint[i]->getRotationalLimitMotor(0)->m_targetVelocity = -rps;
	else m_driveJoint[i]->getRotationalLimitMotor(0)->m_targetVelocity = rps;
	m_driveJoint[i]->getRotationalLimitMotor(0)->m_maxMotorForce = force;
	m_driveJoint[i]->getRotationalLimitMotor(0)->m_damping = 0.5f;
}

void Suspension::steerWheel(const float & ang, int i)
{
	btTransform & frmA = m_steerJoint[i]->getFrameOffsetA();
	if(i == 0) {
		frmA.getOrigin()[0] = -sin(ang) * m_profile._steerArmJointZ;
		frmA.getOrigin()[2] = cos(ang) * m_profile._steerArmJointZ;
	}
	else {
		frmA.getOrigin()[0] = sin(ang) * m_profile._steerArmJointZ;
		frmA.getOrigin()[2] = -cos(ang) * m_profile._steerArmJointZ;
	}
}

const Matrix44F Suspension::wheelHubTM(const int & i) const
{
	btTransform tm = m_wheelHub[i]->getWorldTransform();
	return Common::CopyFromBtTransform(tm);
}

const Vector3F Suspension::wheelVelocity(const int & i) const
{
	Vector3F vel = m_wheel[i]->velocity();
	Matrix44F tm = wheelHubTM(i); 
	
	Vector3F hubx(tm.M(0,0), tm.M(0,1),tm.M(0,2));

	Vector3F front = Vector3F::ZAxis * 100.f;
	if(i > 0) front *= -1.f;
	front = tm.transformAsNormal(front);
	vel *= vel.normal().dot(front.normal());
	return vel;
}

void Suspension::update()
{
	m_damper[0]->update();
	m_damper[1]->update();
	
	const float slip0 = computeWheelSlip(0);
	const float slip1 = computeWheelSlip(1);
	m_wheel[0]->computeFriction(slip0);
	m_wheel[1]->computeFriction(slip1);
	
	m_wheelSkid[0] = computeWheelSkid(0);
	m_wheelSkid[1] = computeWheelSkid(1);
	m_wheelSlip[0] = slip0;
	m_wheelSlip[1] = slip1;
	// std::cout<<"friction l/r "<<m_wheel[0]->friction()<<" "<<m_wheel[1]->friction()<<"\n";
}

void Suspension::brake(const float & strength, bool goForward)
{
	if(strength < .001f) return;
	brake(0, strength, goForward);
	brake(1, strength, goForward);
}

void Suspension::brake(const int & i, const float & strength, bool goForward)
{
	float wheelSpeed = wheelVelocity(i).length();
	
	m_wheelForce[i] = -wheelSpeed * strength * m_differential[i];
	if(m_wheelForce[i] < -SPEEDLIMIT * m_differential[i]) m_wheelForce[i] = -SPEEDLIMIT * m_differential[i];
	// m_wheelForce[i] = -SPEEDLIMIT * strength * m_differential[i];
		
	float diff = m_wheel[0]->radius() * m_wheelForce[i];
	
	wheelSpeed += diff;
	if(wheelSpeed < 0.f) wheelSpeed = 0.f;

	float rps = wheelSpeed / m_wheel[0]->radius();
	if(!goForward) rps = -rps;
	applyMotor(rps, i, BRAKEFORCE);
}

void Suspension::power(const int & i, const float & strength, bool goForward)
{
	float wheelSpeed = wheelVelocity(i).length();
	
	m_wheelForce[i] = SPEEDLIMIT * gearRatio[Gear] * strength * m_differential[i];

	float diff = m_wheel[0]->radius() * m_wheelForce[i];
	
	wheelSpeed += diff;
	if(wheelSpeed < 0.f) wheelSpeed = 0.f;
	
	float rps = wheelSpeed / m_wheel[0]->radius();
	if(!goForward) rps = -rps;
	applyMotor(rps, i, gearForce[Gear]);
}

void Suspension::power(const float & strength, bool goForward)
{
	power(0, strength, goForward);
	power(1, strength, goForward);
}

void Suspension::drive(const float & gasStrength, const float & brakeStrength, bool goForward)
{
	m_wheelForce[0] = 0.f;
	m_wheelForce[1] = 0.f;
	
	releaseBrake();
	
	if(!isPowered()) {
		brake(brakeStrength, goForward);
		return;
	}
	
	const float k = gasStrength - brakeStrength;
	if(k >= 0.f) power(gasStrength, goForward);
	else brake(brakeStrength, goForward);
}

void Suspension::computeDifferential(const Vector3F & turnAround, const float & z, const float & wheelSpan)
{
	m_differential[0] = 1.f;
	m_differential[1] = 1.f;
	const float r = turnAround.x;
	
	const float h = wheelSpan * .5f;
	if(r > h || r < -h) {
		const float turnArm = z - turnAround.z;
	
		float lL = sqrt((r - h) * (r - h) + turnArm * turnArm);
		float rL = sqrt((r + h) * (r + h) + turnArm * turnArm);
		float cL = sqrt(r * r + turnArm * turnArm);
	
		m_differential[0] = lL / cL;
		m_differential[1] = rL / cL;
	}
}

void Suspension::steer(const Vector3F & turnAround, const float & z, const float & wheelSpan)
{
	if(!isSteerable()) return;
	const float r = turnAround.x;
	const float h = wheelSpan * .5f - wheelHubX();
	if(r > h || r < -h) {
		const float turnArm = z - turnAround.z;
	
		float lL = (r - h);
		float rL = (r + h);
		
		float lA = atan(turnArm / lL);
		float rA = atan(turnArm / rL);
		
		//std::cout<<"angle lft/rgt "<<lA<<" / "<<rA<<"\n";
		
		steerWheel(lA, 0);
		steerWheel(rA, 1);
	}
	else {
		steerWheel(0.f, 0);
		steerWheel(0.f, 1);
	}
}

void Suspension::differential(float * dst) const
{
	dst[0] = m_differential[0];
	dst[1] = m_differential[1];
}

void Suspension::wheelForce(float * dst) const
{
	dst[0] = m_wheelForce[0];
	dst[1] = m_wheelForce[1];
}

void Suspension::wheelSlip(float * dst) const
{
	dst[0] = m_wheelSlip[0];
	dst[1] = m_wheelSlip[1];
}

void Suspension::wheelFriction(float * dst) const
{
    dst[0] = m_wheel[0]->friction();
	dst[1] = m_wheel[1]->friction();
}

const float Suspension::computeWheelSlip(const int & i) const
{
	if(!PhysicsState::engine->isPhysicsEnabled()) return 0.f;
	
	Vector3F vel = m_wheel[i]->velocity();
	if(vel.length() < 0.01f) return 0.f;
	Matrix44F tm = wheelHubTM(i);
	tm.inverse();
	vel = tm.transformAsNormal(vel);
	// vel.normalize();
	return atan(vel.x / vel.z);
}

void Suspension::wheelSkid(float * dst) const
{
	dst[0] = m_wheelSkid[0];
	dst[1] = m_wheelSkid[1];
}

const float Suspension::computeWheelSkid(const int & i) const
{
	if(!PhysicsState::engine->isPhysicsEnabled()) return 0.f;
	
	float wheelSpeed = wheelVelocity(i).length();
	
	if(wheelSpeed < 0.01f) return 0.f;
	
	float a = m_wheel[i]->angularVelocity().length();
	
	return (a - wheelSpeed / m_wheel[i]->radius());
}

void Suspension::parkingBrake()
{
	applyMotor(0.f, 0, BRAKEFORCE);
	applyMotor(0.f, 1, BRAKEFORCE);
}

}