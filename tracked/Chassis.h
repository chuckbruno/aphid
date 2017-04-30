/*
 *  Chassis.h
 *  tracked
 *
 *  Created by jian zhang on 5/18/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <AllMath.h>

class Chassis {
public:
	Chassis();
	virtual ~Chassis();
	void setDim(const float & x, const float & y, const float & z);
	void setOrigin(const Vector3F & p);
	void setSpan(const float & x);
	void setWidth(const float & x);
	void setHeight(const float & x);
	void setTrackWidth(const float & x);
	void setDriveSprocketRadius(const float & x);
	void setTensionerRadius(const float & x);
	void setRoadWheelRadius(const float & x);
	void setSupportRollerRadius(const float & x);
	void setNumRoadWheels(const int & x);
	void setRoadWheelZ(const int & i, const float & x);
	void setNumSupportRollers(const int & x);
	void setSupportRollerZ(const int & i, const float & x);
	void setBogieArmLength(const float & x);
	void setBogieArmWidth(const float & x);
	void setDriveSprocketY(const float & x);
	void setDriveSprocketZ(const float & x);
	void setTensionerY(const float & x);
	void setTensionerZ(const float & x);
	void setRoadWheelY(const float & x);
	void setSupportRollerY(const float & x);
	void setTorsionBarRestAngle(const float & x);
	void setTorsionBarTargetAngle(const float & x);
	void setToothWidth(const float & x);

	const float bogieArmLength() const;
	const float bogieArmWidth() const;
	const float trackWidth() const;
	const float tensionerWidth() const;
	const float roadWheelWidth() const;
	const float supportRollerWidth() const;
	const float span() const;
	const float driveSprocketRadius() const;
	const float tensionerRadius() const;
	const float roadWheelRadius() const;
	const float supportRollerRadius() const;
	const int numRoadWheels() const;
	const int numSupportRollers() const;
	const Vector3F center() const;
	const Vector3F extends() const;
	const Vector3F driveSprocketOrigin(bool isLeft = true) const;
	const Vector3F driveSprocketOriginObject(bool isLeft = true) const;
	const Vector3F tensionerOrigin(bool isLeft = true) const;
	const Vector3F tensionerOriginObject(bool isLeft = true) const;
	const Vector3F roadWheelOrigin(const int & i, bool isLeft = true) const;
	const Vector3F roadWheelOriginObject(const int & i, bool isLeft = true) const;
	const Vector3F supportRollerOrigin(const int & i, bool isLeft = true) const;
	const Vector3F supportRollerOriginObject(const int & i, bool isLeft = true) const;
	const Vector3F torsionBarHingeObject(const int & i, bool isLeft = true) const;
	const Vector3F torsionBarHinge(const int & i, bool isLeft = true) const;
	const Matrix44F bogieArmOrigin(const int & i, bool isLeft = true) const;
	const Matrix44F computeBogieArmOrigin(const float & chassisWidth, const Vector3F & wheelP, const float & l, const float & s, const float & ang) const;
	const float torsionBarRestAngle() const;
	const float torsionBarTargetAngle() const;
	const float toothWidth() const;
	const Vector3F computeWheelOrigin(const float & chassisWidth, const float & trackWidth, const float & y, const float & z, bool isLeft = true) const;
	const Vector3F roadWheelOriginToBogie(bool isLeft = true) const;
	const bool isBackdrive() const;
	const bool aroundFirstSupportRoller(Vector3F & p, float & r, bool isLeft = true) const;
	const bool aroundLastSupportRoller(Vector3F & p, float & r, bool isLeft = true) const;
	void getBackWheel(Vector3F & p, float & r, bool isLeft = true) const;
	void getFrontWheel(Vector3F & p, float & r, bool isLeft = true) const;
private:
	Vector3F m_origin;
	float m_span, m_width, m_height, m_trackWidth, m_toothWidth;
	float m_driveSprocketRadius, m_tensionerRadius, m_roadWheelRadius, m_supportRollerRadius;
	float m_driveSprocketY, m_driveSprocketZ, m_tensionerY, m_tensionerZ, m_roadWheelY, m_supportRollerY;
	float m_bogieArmLength, m_bogieArmWidth, m_torsionBarRestAngle, m_torsionBarTargetAngle;
	float * m_roadWheelZ;
	float * m_supportRollerZ;
	int m_numRoadWheels, m_numSupportRollers;
};