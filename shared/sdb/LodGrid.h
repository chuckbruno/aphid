/*
 *  LodGrid.h
 *  
 *  adaptive grid with aggregated property per cell
 *
 *  Created by jian zhang on 11/28/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifndef APH_SDB_LOD_GRID_H
#define APH_SDB_LOD_GRID_H

#include <sdb/AdaptiveGrid3.h>
#include <sdb/Array.h>
#include <sdb/GridSampler.h>
#include "boost/date_time/posix_time/posix_time.hpp"

namespace aphid {

namespace sdb {

/// splat with aggregated position, normal, radius
class LodNode {

public:
	LodNode();
	~LodNode();
	
	Vector3F pos, nml;
	float radius;
	int index;
	
private:

};

class LodCell : public Array<int, LodNode >, public AdaptiveGridCell {

typedef Array<int, LodNode > TParent;
	
public:
	LodCell(Entity * parent = NULL);
	virtual ~LodCell();
	
	virtual void clear();
	
	void countNodesInCell(int & it);
	void dumpNodesInCell(LodNode * dst);
	
	template<typename T>
	void closestToPoint(T * result);
	
private:

};

template<typename T>
void LodCell::closestToPoint(T * result)
{
	begin();
	while(!end() ) {
		LodNode * nd = value();
		float d = nd->pos.distanceTo(result->_toPoint);
		if(d < result->_distance) {
			result->_distance = d;
			result->_hasResult = true;
			result->_hitPoint = nd->pos;
			result->_hitNormal = nd->nml;
		}
		
		if(result->closeEnough() ) {
			return;
		}
		
		next();
	}
}


class LodGrid : public AdaptiveGrid3<LodCell, LodNode, 10 > {

typedef AdaptiveGrid3<LodCell, LodNode, 10 > TParent;

	int m_finestNodeLevel;
	
public:
	LodGrid(Entity * parent = NULL);
	virtual ~LodGrid();
	
/// reset level0 cell size and bound
	void resetBox(const BoundingBox & b,
				const float & h);
	void fillBox(const BoundingBox & b,
				const float & h);
		
	template<typename Tf>
	void subdivideToLevel(Tf & fintersect,
						int minLevel, int maxLevel)
	{
		std::cout<<"\n LodGrid::subdivide ";
		BoundingBox cb;
		int level = minLevel;
		while(level < maxLevel) {
			std::vector<Coord4> dirty;
			begin();
			while(!end() ) {
				if(key().w == level) {
					getCellBBox(cb, key() );
					
					if(fintersect.intersect(cb) ) {
						dirty.push_back(key() );
					}
					
				}
				next();
			}
			
			if(dirty.size() < 1) {
				break;
			}
			
			std::cout<<"\n level"<<level;
			
			std::vector<Coord4>::const_iterator it = dirty.begin();
			for(;it!=dirty.end();++it) {
				subdivideCell(fintersect, *it);
			}
			level++;
		}
		storeCellNeighbors();
		
		const int nlc = numCellsAtLevel(maxLevel);
		std::cout<<"\n level"<<maxLevel<<" n cell "<<nlc;
		std::cout.flush();
		
	}
	
	template<typename Tf, int Ndiv>
	void insertNodeAtLevel(int level,
							Tf & fintersect)
	{
		const int nlc = numCellsAtLevel(level);
		std::cout<<"\n LodGrid::insertNodeAtLevel "<<level
				<<"\n n cells "<<nlc;
		std::cout.flush();
				
		boost::posix_time::ptime t0(boost::posix_time::second_clock::local_time());
		boost::posix_time::ptime t1;
		boost::posix_time::time_duration t3;
		
		GridSampler<Tf, LodNode, Ndiv > sampler;
		
		BoundingBox cellBx;
		
		int nc = 0;
		int c = 0;
		begin();
		while(!end() ) {
			if(key().w == level) {
			
				getCellBBox(cellBx, key() );
				sampler.sampleInBox(fintersect, cellBx );
				
				const int & ns = sampler.numValidSamples();
				for(int i=0;i<ns;++i) {

					LodNode * par = new LodNode;
					
					const LodNode & src = sampler.sample(i);
					*par = src;
					par->index = -1;
					
					value()->insert(i, par);
					
				}
				c += ns;
				
				nc++;
				
				if((nc & 1023) == 0) {
					t1 = boost::posix_time::ptime (boost::posix_time::second_clock::local_time());
					t3 = t1 - t0;
					std::cout<<"\n processed "<<nc<<" cells in "<<t3;
					std::cout.flush();
				}
				
			}
			
			if(key().w > level) {
				break;
			}
			
			next();
		}
		
		t1 = boost::posix_time::ptime (boost::posix_time::second_clock::local_time());
		t3 = t1 - t0;
		
		std::cout<<"\n done. "
				<<"\n n samples "<<c
				<<"\n cost time "<<t3;
		std::cout.flush();
		m_finestNodeLevel = level;
	}
	
	void aggregateAtLevel(int level);
	
	virtual void clear(); 
	
	int countLevelNodes(int level);
	void dumpLevelNodes(LodNode * dst, int level);
	
private:
	template<typename Tf>
	void subdivideCell(Tf & fintersect,
						const Coord4 & cellCoord)
	{
		LodCell * cell = findCell(cellCoord);
		if(!cell) {
			std::cout<<"\n [ERROR] LodGrid cannot find cell to subdivide "<<cellCoord;
			return;
		}
		
		if(cell->hasChild() ) {
			return;
		}
			
		BoundingBox cb;
		for(int i=0; i< 8; ++i) { 
			getCellChildBox(cb, i, cellCoord );
			
			if(fintersect.intersect(cb) ) {
				subdivide(cell, cellCoord, i);
			}
			
		}
	}
	
	void aggregateInCell(LodCell * cell, 
						const Coord4 & cellCoord);
	void processKmean(int & n, 
					LodNode * samples);
	
};

}

}
#endif