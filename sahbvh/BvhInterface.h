#ifndef BVHINTERFACE_H
#define BVHINTERFACE_H

#include "bvh_common.h"
#include "radixsort_implement.h"

namespace bvhhash {
void computePrimitiveHash(KeyValuePair * dst, 
            Aabb * leafBoxes, 
            uint numLeaves, 
            uint buffSize, 
			Aabb * bigBox);
}

namespace bvhcost {
void computeTraverseCost(float * costs,
        int2 * nodes,
        int * nodeNumPrimitives,
	    Aabb * nodeAabbs,
        uint n);

void countPrimitviesInNodeAtLevel(int * nodeNumPrimitives,
        int * nodeLevels,
        int2 * nodes,
        int level,
	    uint n);
}

namespace bvhlazy {
void updateNodeAabbAtLevel(Aabb * nodeAabbs,
        int * nodeLevels,
        int2 * nodes,
        KeyValuePair * primitiveIndirections,
        Aabb * primitiveAabbs,
        int level,
	    uint n);
}

#endif        //  #ifndef BVHINTERFACE_H

