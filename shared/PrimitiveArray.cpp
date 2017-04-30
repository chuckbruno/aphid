/*
 *  PrimitiveArray.cpp
 *  kdtree
 *
 *  Created by jian zhang on 10/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "PrimitiveArray.h"
namespace aphid {

PrimitiveArray::PrimitiveArray() 
{
}

PrimitiveArray::~PrimitiveArray() 
{
}

Primitive *PrimitiveArray::asPrimitive(unsigned index)
{
	return (Primitive *)at(index);
}
/*
Primitive *PrimitiveArray::asPrimitive(unsigned index) const
{
	return (Primitive *)at(index);
}
*/
Primitive *PrimitiveArray::asPrimitive()
{
	return (Primitive *)current();
}

unsigned PrimitiveArray::elementSize() const
{ return sizeof(Primitive); }

}
