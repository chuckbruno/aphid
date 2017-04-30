/*
 *  FemGlobal.h
 *  testfem
 *
 *  Created by jian zhang on 4/23/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <string>
#include <ATetrahedronMesh.h>
#include <HesperisFile.h>
#include <HTetrahedronMesh.h>
#include <BaseBuffer.h>
#include <tetrahedron_math.h>
#define DISABLE_FEM 0
class FemGlobal {
public:
	static std::string FileName;
	static int CGSolverMaxNumIterations;
};