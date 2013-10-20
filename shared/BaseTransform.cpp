/*
 *  BaseTransform.cpp
 *  eulerRot
 *
 *  Created by jian zhang on 10/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "BaseTransform.h"
#include <Plane.h>
BaseTransform::BaseTransform(BaseTransform * parent)
{
	m_parent = parent;
	setEntityType(TTransform);
}

BaseTransform::~BaseTransform() {}

void BaseTransform::setParent(BaseTransform * parent)
{
	m_parent = parent;
}

BaseTransform * BaseTransform::parent() const
{
	return m_parent;
}

void BaseTransform::translate(const Vector3F & v)
{
	m_translation += v;
}

void BaseTransform::setTranslation(const Vector3F & v)
{
	m_translation = v;
}

Vector3F BaseTransform::translation() const
{
	return m_translation;
}

void BaseTransform::rotate(const Vector3F & v)
{
	Vector3F v1 = m_angles + v;
	setRotationAngles(v1);
}

void BaseTransform::setRotationAngles(const Vector3F & v)
{
	m_angles = v;
}

Vector3F BaseTransform::rotationBaseAngles() const
{
	return Vector3F();
}

Matrix33F BaseTransform::rotation() const
{
	Matrix33F r;
	Vector3F angs = rotationBaseAngles() + rotationAngles();
	r.rotateEuler(angs.x, angs.y, angs.z);
	return r;
}

Vector3F BaseTransform::rotationAngles() const
{
	return m_angles;
}

void BaseTransform::addChild(BaseTransform * child)
{
	m_children.push_back(child);
}

unsigned BaseTransform::numChildren() const
{
	return m_children.size();
}

BaseTransform * BaseTransform::child(unsigned idx) const
{
	return m_children[idx];
}

void BaseTransform::parentSpace(Matrix44F & dst) const
{
	if(!parent()) return;
	
	BaseTransform * p = parent();
	while(p) {
		dst.multiply(p->space());
		p = p->parent();
	}
}

Matrix44F BaseTransform::space() const
{
	Matrix44F s;
	s.setTranslation(m_translation);
	s.setRotation(rotation());
	return s;
}

Matrix44F BaseTransform::worldSpace() const
{
	Matrix44F s = space();
	parentSpace(s);
	return s;
}

bool BaseTransform::intersect(const Ray & ray) const
{
	Matrix44F s = worldSpace();
	Plane pl(ray.m_dir.reversed(), s.getTranslation());
	Vector3F hit, d;
    float t;
	if(pl.rayIntersect(ray, hit, t)) {
		d = hit - s.getTranslation();
		if(d.length() < 8.f)
			return true;
	}
	return false;
}

Vector3F BaseTransform::translatePlane(RotateAxis a) const
{
	if(a == AX) return Vector3F::XAxis;
	if(a == AY) return Vector3F::YAxis;
	return Vector3F::ZAxis;
}

Vector3F BaseTransform::rotatePlane(RotateAxis a) const
{
	
	if(a == AZ) return Vector3F::ZAxis;
	
	if(a == AY) {
		Matrix33F m;
		m.rotateEuler(0.f, 0.f, rotationAngles().z + rotationBaseAngles().z);
		return m.transform(Vector3F::YAxis);
	}
	
	Matrix33F r = rotation();
	return r.transform(Vector3F::XAxis);
}
