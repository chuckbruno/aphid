/*
 *  dynamicsSolver.cpp
 *  qtbullet
 *
 *  Created by jian zhang on 7/13/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "dynamicsSolver.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletSoftBody/btSoftBody.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "Muscle.h"
#include "Skin.h"

DynamicsSolver::DynamicsSolver()
{
    _drawer = new ShapeDrawer();
    m_activeBody = 0;
    m_testJoint = 0;

	m_interactMode = TranslateBone;
}

DynamicsSolver::~DynamicsSolver()
{
    killPhysics();
    m_activeBody = 0;
}
	
void DynamicsSolver::initPhysics()
{
    
    m_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	btSoftBodyWorldInfo worldInfo;
	worldInfo.m_dispatcher = m_dispatcher;

	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	
	m_broadphase = new btAxisSweep3(worldMin,worldMax, 1000);

	worldInfo.m_broadphase = m_broadphase;
	worldInfo.m_sparsesdf.Initialize();
    worldInfo.m_gravity.setValue(0,0.0,0);
	m_constraintSolver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btSoftRigidDynamicsWorld(m_dispatcher, m_broadphase, m_constraintSolver, m_collisionConfiguration);

	m_dynamicsWorld->getDispatchInfo().m_enableSPU = true;
	

/*	_dynamicsWorld->setGravity(btVector3(0,0,0));

	btCollisionShape* groundShape = new btBoxShape(btVector3(75,1,75));
	_collisionShapes.push_back(groundShape);
	
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(0,-5,0));
	btRigidBody* body = localCreateRigidBody(0.f,tr,groundShape);


	//_dynamicsWorld->addRigidBody(body);
	
	btCollisionShape* cubeShape = new btBoxShape(btVector3(1.f,1.f,1.f));
	_collisionShapes.push_back(cubeShape);
	
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(0.0, 5.0, 3.0));
	
	btCollisionShape* cubeShape1 = new btBoxShape(btVector3(1.f,4.f,.2f));
	_collisionShapes.push_back(cubeShape1);
	
	btRigidBody* body0 = localCreateRigidBody(0.f, trans, cubeShape1);
	_dynamicsWorld->addRigidBody(body0);
	
	btCollisionShape* cubeShape2 = new btBoxShape(btVector3(2.f,.2f,.5f));
	_collisionShapes.push_back(cubeShape2);
	
	trans.setOrigin(btVector3(5.0, 9.0, 3.0));
	btRigidBody* clavicle = localCreateRigidBody(1.f, trans, cubeShape2);
	_dynamicsWorld->addRigidBody(clavicle);
	
	btCollisionShape* cubeShape3 = new btBoxShape(btVector3(4.f,.2f,.5f));
	_collisionShapes.push_back(cubeShape3);
	
	trans.setOrigin(btVector3(20.0, 9.0, 4.0));
	btRigidBody* body2 = localCreateRigidBody(1.f, trans, cubeShape3);
	_dynamicsWorld->addRigidBody(body2);
	
	trans.setOrigin(btVector3(25.0, 9.0, 6.0));
	btRigidBody* body3 = localCreateRigidBody(1.f, trans, cubeShape3);
	_dynamicsWorld->addRigidBody(body3);
	
	btCollisionShape* cubeShape4 = new btBoxShape(btVector3(.5f,.5f,.5f));
	_collisionShapes.push_back(cubeShape4);
	
	trans.setOrigin(btVector3(28.0, 9.0, 12.0));
	btRigidBody* body4 = localCreateRigidBody(1.f, trans, cubeShape4);
	_dynamicsWorld->addRigidBody(body4);
	
	btMatrix3x3 flip(1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f);
	btTransform frameInA(flip), frameInB(flip);
    
    frameInA.setOrigin(btVector3(2., 4., 0.));
    frameInB.setOrigin(btVector3(-2.5, 0., 0.));
	btGeneric6DofConstraint* d6f = new btGeneric6DofConstraint(*body0, *clavicle, frameInA, frameInB, true);
	d6f->setAngularLowerLimit(btVector3(0., -SIMD_PI/4., -SIMD_PI/4.));
    d6f->setAngularUpperLimit(btVector3(0., SIMD_PI/4., SIMD_PI/4.));	
	_dynamicsWorld->addConstraint(d6f);
	
	frameInA.setOrigin(btVector3(2.5, 0., 0.));
    frameInB.setOrigin(btVector3(-6., 0., 0.));
	
	btGeneric6DofConstraint* d6f1 = new btGeneric6DofConstraint(*body2, *clavicle, frameInB, frameInA, true);
	d6f1->setAngularLowerLimit(btVector3(-SIMD_PI/2.3, -SIMD_PI/2.1, -SIMD_PI/22.3));
    d6f1->setAngularUpperLimit(btVector3(SIMD_PI/2.3, SIMD_PI/12.3, SIMD_PI/1.8));	
	_dynamicsWorld->addConstraint(d6f1);
	
	frameInA.setOrigin(btVector3(6., 0., 0.));
    frameInB.setOrigin(btVector3(0., 0., 0.));
	
	btGeneric6DofConstraint* d6f2 = new btGeneric6DofConstraint(*body2, *body4, frameInA, frameInB, true);
	//d6f2->setAngularLowerLimit(btVector3(0., 0., -SIMD_PI* .75));
    //d6f2->setAngularUpperLimit(btVector3(0., 0., 0.));
    //d6f2->setAngularLowerLimit(btVector3(0., 0., 0.));
    //d6f2->setAngularUpperLimit(btVector3(0., 0., 0.));
    d6f2->setLinearLowerLimit(btVector3(-33.3, 0., 0.));
    d6f2->setLinearUpperLimit(btVector3(33.3, 0., 0.));	
	_dynamicsWorld->addConstraint(d6f2);
	
	frameInA.setOrigin(btVector3(6., 0., 0.));
    frameInB.setOrigin(btVector3(-2., 0., 0.));
	
	btGeneric6DofConstraint* d6f3 = new btGeneric6DofConstraint(*body3, *body4, frameInA, frameInB, true);	
	_dynamicsWorld->addConstraint(d6f3);
	
	body0->setDamping(.99f, .99f);
	clavicle->setDamping(.99f, .99f);
	body2->setDamping(.99f, .99f);
	body3->setDamping(.99f, .99f);
	body4->setDamping(.99f, .99f);
	
	btCollisionShape* scapulaShape = new btBoxShape(btVector3(1.f,1.5f,.25f));
	_collisionShapes.push_back(scapulaShape);
	
	trans.setOrigin(btVector3(6.0, 7.0, 4.0));
	btRigidBody* scapula = localCreateRigidBody(1.f, trans, scapulaShape);
	_dynamicsWorld->addRigidBody(scapula);
	scapula->setDamping(.99f, .99f);
	
	frameInA.setOrigin(btVector3(2.5, 0., 0.));
    frameInB.setOrigin(btVector3(1.2, 1.99, 0.5));
	
	btGeneric6DofConstraint* c2s = new btGeneric6DofConstraint(*clavicle, *scapula, frameInA, frameInB, true);
	c2s->setAngularLowerLimit(btVector3(-SIMD_PI/2.3, -SIMD_PI/2.1, -SIMD_PI/22.3));
    c2s->setAngularUpperLimit(btVector3(SIMD_PI/2.3, SIMD_PI/12.3, SIMD_PI/1.8));	
	_dynamicsWorld->addConstraint(c2s);
	
	frameInA.setOrigin(btVector3(-1.2, 1.99, 0.));
	frameInB.setOrigin(btVector3(3., 2., -3.));
    btGeneric6DofConstraint* r2s = new btGeneric6DofConstraint(*scapula, *body0, frameInA, frameInB, true);
	r2s->setLinearLowerLimit(btVector3(-3.3, -3.1, -3.3));
    r2s->setLinearUpperLimit(btVector3(3.3, 3.3, 3.8));	
	
    
    _dynamicsWorld->addConstraint(r2s);
    
    frameInA.setOrigin(btVector3(-1.2, -1.99, 0.));
	frameInB.setOrigin(btVector3(3., -2., -3.));
    btGeneric6DofConstraint* r2s2 = new btGeneric6DofConstraint(*scapula, *body0, frameInA, frameInB, true);
	r2s2->setLinearLowerLimit(btVector3(-3.3, -3.1, -3.3));
    r2s2->setLinearUpperLimit(btVector3(3.3, 3.3, 3.8));	
	
    
    _dynamicsWorld->addConstraint(r2s2);
    */
	
	
	
	initRope();
}

void DynamicsSolver::killPhysics()
{
	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{

			while (body->getNumConstraintRefs())
			{
				btTypedConstraint* constraint = body->getConstraintRef(0);
				m_dynamicsWorld->removeConstraint(constraint);
				delete constraint;
			}
			delete body->getMotionState();
			m_dynamicsWorld->removeRigidBody(body);
		} 
		else
		{
			m_dynamicsWorld->removeCollisionObject( obj );
		}
		delete obj;
	}
	
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}

	delete m_constraintSolver;

	//delete _overlappingPairCache;

	delete m_dispatcher;

	delete m_collisionConfiguration;
}

void DynamicsSolver::renderWorld()
{
	const int	numObjects= m_dynamicsWorld->getNumCollisionObjects();
	btVector3 wireColor(1,0,0);
	for(int i=0;i<numObjects;i++)
	{
		btCollisionObject*	colObj= m_dynamicsWorld->getCollisionObjectArray()[i];
		_drawer->drawObject(colObj);
	}
	
	for (  int i=0;i<m_dynamicsWorld->getSoftBodyArray().size();i++) {
		//btSoftBody*	psb=(btSoftBody*)m_dynamicsWorld->getSoftBodyArray()[i];

			//btSoftBodyHelpers::DrawFrame(psb,m_dynamicsWorld->getDebugDrawer());
			//btSoftBodyHelpers::Draw(psb,m_dynamicsWorld->getDebugDrawer(),m_dynamicsWorld->getDrawFlags());
	}
	
	const int numConstraints = m_dynamicsWorld->getNumConstraints();
	for(int i=0;i< numConstraints;i++) {
	    btTypedConstraint* constraint = m_dynamicsWorld->getConstraint(i);
	    _drawer->drawConstraint(constraint);
	}
	
	if(!m_activeBody) return;
	
	if(m_interactMode == TranslateBone) {
	    _drawer->drawTranslateHandle(m_activeBody);
	}
	//else if(m_interactMode == RotateJoint) {
	    
	//}
}

void DynamicsSolver::simulate()
{
	btScalar dt = (btScalar)_clock.getTimeMicroseconds();
	_clock.reset();
	m_dynamicsWorld->stepSimulation(dt / 100.f, 30);
}

btRigidBody* DynamicsSolver::localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);
	
	printf("inertial %f %f %f \n", localInertia.getX(), localInertia.getY(), localInertia.getZ());

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);
	

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(_defaultContactProcessingThreshold);

	return body;
}

char DynamicsSolver::selectByRayHit(const Vector3F & origin, const Vector3F & ray, Vector3F & hitP)
{
    m_activeBody = 0;
    btVector3 fromP(origin.x, origin.y, origin.z);
    btVector3 toP(origin.x + ray.x, origin.y + ray.y, origin.z + ray.z);
    btCollisionWorld::ClosestRayResultCallback rayCallback(fromP, toP);
    m_dynamicsWorld->rayTest(fromP , toP, rayCallback);
    if(rayCallback.hasHit()) {
        btRigidBody * body = (btRigidBody *)btRigidBody::upcast(rayCallback.m_collisionObject);
        if(body) {
            body->setActivationState(DISABLE_DEACTIVATION);
            btVector3 pickPos = rayCallback.m_hitPointWorld;
            hitP.x = pickPos.getX();
            hitP.y = pickPos.getY();
            hitP.z = pickPos.getZ();
            m_activeBody = body;
            
            
            return 1;
        }
    }
    return 0;
}

void DynamicsSolver::addImpulse(const Vector3F & impulse)
{
    if(!m_activeBody) return;
/*
    m_activeBody->setCollisionFlags(m_activeBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    m_activeBody->setActivationState(DISABLE_DEACTIVATION);
    
    btTransform tm = m_activeBody->getWorldTransform();
    btVector3 t = tm.getOrigin() + btVector3(impulse.x/10.f, impulse.y/10.f, impulse.z/80.f);
    tm.setOrigin(t);
    
    m_activeBody->setWorldTransform(tm);
    m_activeBody->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
    m_activeBody->setAngularVelocity(btVector3(0.f, 0.f, 0.f));
    m_activeBody->setCollisionFlags(m_activeBody->getCollisionFlags() & ~(btCollisionObject::CF_KINEMATIC_OBJECT));
    m_activeBody->forceActivationState(ACTIVE_TAG);
    */
    if(m_activeBody->getInvMass() > 0.001f) {
        btVector3 impulseV(impulse.x, impulse.y, impulse.z);
        m_activeBody->setActivationState(ACTIVE_TAG);
        m_activeBody->applyForce(impulseV, btVector3(0,0,0));
    }
    else {
        btTransform tm = m_activeBody->getWorldTransform();
        btVector3 t = tm.getOrigin() + btVector3(impulse.x/10.f, impulse.y/10.f, impulse.z/80.f);
        tm.setOrigin(t);
    
        m_activeBody->setWorldTransform(tm);
    }
    //relaxRope();
    m_skin->m_nodes[10].m_im = 0.f;
    m_skin->m_nodes[10].m_x += btVector3(impulse.x, impulse.y, impulse.z);
    
    //m_skin->m_nodes[67].m_q = m_skin->m_nodes[67].m_x;
    //m_skin->m_nodes[10].m_x += btVector3(impulse.x/2.f, impulse.y/2.f, impulse.z/2.f);
    //m_skin->m_nodes[5].m_x += btVector3(impulse.x / 1.3f, impulse.y / 1.3f, impulse.z / 1.3f);
    //m_skin->m_nodes[5].m_im = 0.f;
    //m_skin->m_nodes[135].m_im = 0.f;
    //m_skin->m_nodes[135].m_x += btVector3(impulse.x/20.f, impulse.y/20.f, impulse.z/10.f);
    //m_skin->m_nodes[136].m_im = 0.f;
    //m_skin->m_nodes[136].m_x += btVector3(impulse.x/20.f, impulse.y/20.f, impulse.z/10.f);
    m_skin->m_nodes[0].m_im = 0.f;
    m_skin->m_nodes[0].m_x -= btVector3(impulse.x, impulse.y, impulse.z);
    //m_skin->m_nodes[0].m_x += btVector3(impulse.x/20.f, impulse.y/20.f, impulse.z/20.f);
    m_skin->m_nodes[120].m_im = 0.f;
    
    m_skin->m_nodes[120].m_x += btVector3(impulse.x, impulse.y, impulse.z);
    m_skin->m_nodes[110].m_im = 0.f;
    m_skin->m_nodes[110].m_x -= btVector3(impulse.x/1.1f, impulse.y/1.1f, impulse.z/1.1f);
    //m_skin->m_nodes[65].m_im = 0.f;
    //m_skin->m_nodes[65].m_x += btVector3(impulse.x/3.f, impulse.y/8.f, impulse.z/8.f);
}

void DynamicsSolver::addTorque(const Vector3F & torque)
{
    if(!m_activeBody) return;
    
    m_activeBody->setCollisionFlags(m_activeBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    m_activeBody->setActivationState(DISABLE_DEACTIVATION);
    
    btTransform tm = m_activeBody->getWorldTransform();
    btQuaternion q = tm.getRotation();
    
    btQuaternion a;
    a.setRotation(btVector3(0.f, 0.f, 1.f), btScalar(1.57f));
    
    q = a;
    tm.setRotation(q);
    
    m_activeBody->setWorldTransform(tm);
    m_activeBody->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
    m_activeBody->setAngularVelocity(btVector3(0.f, 0.f, 0.f));
    m_activeBody->setCollisionFlags(m_activeBody->getCollisionFlags() & ~(btCollisionObject::CF_KINEMATIC_OBJECT));
    //m_activeBody->forceActivationState(ACTIVE_TAG);
    
    //m_testJoint->getRotationalLimitMotor(0)->m_enableMotor = true;
    //m_testJoint->getRotationalLimitMotor(0)->m_targetVelocity = 4.0f;
    //m_testJoint->getRotationalLimitMotor(0)->m_maxMotorForce = 100.f;
}

void DynamicsSolver::removeTorque()
{
    if(!m_testJoint) return;
    m_testJoint->getRotationalLimitMotor(0)->m_enableMotor = false;
}

void DynamicsSolver::removeSelection()
{
    m_activeBody = 0;
}

char DynamicsSolver::hasActive() const
{
    return m_activeBody != 0;
}

void DynamicsSolver::toggleMassProp()
{
    if(!m_activeBody) return;
    
    if(m_activeBody->getInvMass() < 0.001f)
        m_activeBody->setMassProps(1.f, btVector3(0.666667, 0.666667, 0.666667));
    else
        m_activeBody->setMassProps(0.f, btVector3(0,0,0));
}

void DynamicsSolver::setInteractMode(InteractMode mode)
{
    m_interactMode = mode;
}

DynamicsSolver::InteractMode DynamicsSolver::getInteractMode() const
{
    return m_interactMode;
}

void DynamicsSolver::initRope()
{
    m_dynamicsWorld->setGravity(btVector3(0,0,0));
    m_dynamicsWorld->getWorldInfo().m_gravity.setValue(0,0,0);

	btCollisionShape* groundShape = new btBoxShape(btVector3(75,1,75));
	m_collisionShapes.push_back(groundShape);
	
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(0,-5,0));
	btRigidBody* body = localCreateRigidBody(0.f,tr,groundShape);


	m_dynamicsWorld->addRigidBody(body);
	
	btCollisionShape* cubeShape = new btBoxShape(btVector3(1.f,1.f,1.f));
	m_collisionShapes.push_back(cubeShape);
	
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(0.0, 5.0, 1.0));
	
	btCollisionShape* cubeShape1 = new btBoxShape(btVector3(1.f,2.f,1.f));
	m_collisionShapes.push_back(cubeShape1);
	
	btRigidBody* body4 = localCreateRigidBody(2.f, trans, cubeShape1);
	m_dynamicsWorld->addRigidBody(body4);
	body4->setDamping(.99f, .99f);
	
	btVector3 start(-12.0, 1.0, 1.0), end(-2.0, 3., 1.0);
	btSoftBody*	psb = btSoftBodyHelpers::CreateRope(m_dynamicsWorld->getWorldInfo(), start, end,8,1);
    psb->setTotalMass(1.1);
    psb->m_materials[0]->m_kLST	= 0.426945;
    m_dynamicsWorld->addSoftBody(psb);
    
    start = btVector3(15.0, 12.0, 1.0);
    end = btVector3(0.0, 8., 1.0);
	btSoftBody*	psb1 = btSoftBodyHelpers::CreateRope(m_dynamicsWorld->getWorldInfo(), start, end,8,1);
    psb1->setTotalMass(1.1);
    psb1->m_materials[0]->m_kLST	= 0.426945;
    m_dynamicsWorld->addSoftBody(psb1);
    
	psb->appendAnchor(psb->m_nodes.size()-1,body4);
	psb1->appendAnchor(psb->m_nodes.size()-1,body4);
	
	MuscleFascicle fascicle;
	fascicle.addVertex(0.f, 6.f, -1.f);
	fascicle.addVertex(1.f, 7.f, -2.f);
	fascicle.addVertex(2.f, 8.f, -3.f);
	fascicle.addVertex(3.f, 9.f, -4.f);
	fascicle.addVertex(4.f, 10.f, -5.f);
	fascicle.addVertex(5.f, 11.f, -6.f);
	fascicle.addVertex(6.f, 12.f, -7.f);
	fascicle.addVertex(7.f, 13.f, -8.f);
	
	MuscleFascicle fascicle1;
	fascicle1.addVertex(1.f, 8.f, -1.f);
	fascicle1.addVertex(2.1f, 9.f, -2.f);
	fascicle1.addVertex(3.2f, 10.f, -3.f);
	fascicle1.addVertex(4.3f, 11.f, -4.f);
	fascicle1.addVertex(5.4f, 12.f, -5.f);
	fascicle1.addVertex(6.5f, 13.f, -6.f);
	fascicle1.addVertex(7.6f, 14.f, -7.f);
	fascicle1.addVertex(8.7f, 15.f, -8.f);
	
	MuscleFascicle fascicle2;
	fascicle2.addVertex(3.1f, 5.5f, -2.f);
	fascicle2.addVertex(4.2f, 6.5f, -3.f);
	fascicle2.addVertex(5.3f, 7.5f, -4.f);
	fascicle2.addVertex(6.4f, 8.5f, -5.f);
	fascicle2.addVertex(7.5f, 9.5f, -6.f);
	fascicle2.addVertex(8.6f, 10.5f, -7.f);
	fascicle2.addVertex(9.7f, 11.5f, -8.f);
	fascicle2.addVertex(10.8f, 12.5f, -9.f);
	
	Muscle msc;
	msc.addFacicle(fascicle);
	msc.addFacicle(fascicle1);
	msc.addFacicle(fascicle2);
	
	msc.create(m_dynamicsWorld->getWorldInfo());
	m_dynamicsWorld->addSoftBody(msc.getSoftBody());
	
	msc.addAnchor(body4, 0, 0);
	msc.addAnchor(body4, 1, 0);
	msc.addAnchor(body4, 2, 0);
	msc.addAnchor(0, 0, 1);
	msc.addAnchor(0, 1, 1);
	msc.addAnchor(0, 2, 1);
	
	msc.connectFascicles(0, 1, 1);
	msc.connectFascicles(0, 1, 2);
	msc.connectFascicles(0, 1, 3);
	msc.connectFascicles(0, 1, 4);
	msc.connectFascicles(0, 1, 5);
	
	msc.connectFascicles(1, 2, 1);
	msc.connectFascicles(1, 2, 2);
	msc.connectFascicles(1, 2, 3);
	msc.connectFascicles(1, 2, 4);
	msc.connectFascicles(1, 2, 5);
	
	msc.connectFascicles(2, 0, 1);
	msc.connectFascicles(2, 0, 2);
	msc.connectFascicles(2, 0, 3);
	msc.connectFascicles(2, 0, 4);
	msc.connectFascicles(2, 0, 5);
	
	Skin skn;
	skn.create(m_dynamicsWorld->getWorldInfo(), "/Users/jianzhang/aphid/qtbullet/nose.m");
	m_dynamicsWorld->addSoftBody(skn.getSoftBody());
	
	btCollisionShape* cubeShape2 = new btBoxShape(btVector3(.36f,.36f,.36f));
	m_collisionShapes.push_back(cubeShape2);
	
	trans.setOrigin(btVector3(0.0, 12.325, 22.0));
	btRigidBody* body5 = localCreateRigidBody(1.f, trans, cubeShape2);
	m_dynamicsWorld->addRigidBody(body5);
	body5->setDamping(.99f, .99f);
	
	//skn.addAnchor(body5, 30);
	skn.addAnchor(0, 131);
	//skn.addAnchor(0, 61);
	skn.addAnchor(0, 142);
	
	skn.addAnchor(0, 143);
	//skn.addAnchor(0, 75);
	//skn.addAnchor(0, 8);
	//skn.addAnchor(0, 86);
	
	trans.setOrigin(btVector3(3.742343, 9.339369, 19.463814));
	btRigidBody* body6 = localCreateRigidBody(1.f, trans, cubeShape2);
	m_dynamicsWorld->addRigidBody(body6);
	body6->setDamping(.99f, .99f);
	
	skn.addAnchor(body6, 142);
	
	trans.setOrigin(btVector3(-3.742343, 9.339369, 19.463814));
	btRigidBody* body7 = localCreateRigidBody(1.f, trans, cubeShape2);
	m_dynamicsWorld->addRigidBody(body7);
	body7->setDamping(.99f, .99f);
	
	skn.addAnchor(body7, 74);
	
	m_skin = skn.getSoftBody();
	
	//skn.addAnchor(0, 67);
	
	Skin skn1;
	skn1.create(m_dynamicsWorld->getWorldInfo(), "/Users/jianzhang/aphid/qtbullet/plane.m");
	m_dynamicsWorld->addSoftBody(skn1.getSoftBody());
	
	m_skin = skn1.getSoftBody();
	
	skn1.addAnchor(0, 110);
	skn1.addAnchor(0, 120);
	skn1.addAnchor(0, 0);
}

void DynamicsSolver::relaxRope()
{
    for (  int i=0;i<m_dynamicsWorld->getSoftBodyArray().size();i++) {
		btSoftBody*	body =(btSoftBody*)m_dynamicsWorld->getSoftBodyArray()[i];
		btSoftBody::Node & n0 = body->m_nodes[0];
		btSoftBody::Node & n1 = body->m_nodes[body->m_nodes.size() - 1];
		
		btVector3 seg = n0.m_x - n1.m_x;
        float fulllen = seg.length();

        float divl = fulllen / body->m_links.size() * 0.998f;
        for(int j=0; j < body->m_links.size(); ++j) {
		    btSoftBody::Link&	l = body->m_links[j];
            l.m_rl = divl;
            l.m_c1 = divl * divl;
        }
	}
}
