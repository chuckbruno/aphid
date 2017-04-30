/*
// how to use
// createNode -n zik2Bsolver zik2Bsolver;
// ikHandle -sol zik2Bsolver -sj joint1 -ee joint3;
//
// reference:
// https://www.toolchefs.com/?portfolio=soft-ik-solver
// http://www.softimageblog.com/archives/108
//
// midJointSlide float -1 - 1
// midJointLockWeight float 0 - 1
// midJointLockPosition point
// use pole vector as lock position bool
// midJointRestPose point
// endJointRestPose point
//
*/

#include <math.h>
#include <maya/MIOStream.h>

#include <maya/MObject.h>
#include <maya/MDagPath.h>

#include <maya/MString.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>

#include <maya/MPxIkSolverNode.h>
#include <maya/MIkHandleGroup.h>
#include <maya/MFnIkHandle.h>
#include <maya/MFnIkEffector.h>
#include <maya/MFnIkJoint.h>

#include <maya/MArgList.h>

#include <maya/MGlobal.h>
#include <HermiteCurve.h>

#define kSolverType "ik2Bsolver"
#define kPi 3.14159265358979323846264338327950
#define kEpsilon 1.0e-5
#define absoluteValue(x) ((x) < 0 ? (-(x)) : (x))

class ik2Bsolver : public MPxIkSolverNode {

public:
                                        ik2Bsolver();
    virtual                     ~ik2Bsolver();
        void                    postConstructor();

        virtual MStatus doSolve();
        virtual MString solverTypeName() const;

        static  void*   creator();
        static  MStatus initialize();
        static MObject asoftDistance;
		static MObject arestLength1;
		static MObject arestLength2;
		static MObject amaxStretching;

        static  MTypeId id;

private:
        MVector poleVectorFromHandle(const MDagPath &handlePath);
        double  twistFromHandle(const MDagPath &handlePath);
        bool findFirstJointChild(const MDagPath & root, MDagPath & result);
        void solveIK(const MPoint &startJointPos,
                         const MPoint &midJointPos,
                         const MPoint &effectorPos,
                         const MPoint &handlePos,
                         const MVector &poleVector,
                         double twistValue,
                         MQuaternion &qStart,
                         MQuaternion &qMid,
                         const double & softDistance,
						 const double & restLength1,
						 const double & restLength2,
						 double & stretching);
private:
    HermiteCurve m_herm;
};
