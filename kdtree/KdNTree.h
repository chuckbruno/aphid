#pragma once
#include <Geometry.h>
#include <Boundary.h>
#include "KdNNode.h"
#include "KdSah.h"
#include <sstream>

template <typename T, typename Tn>
class KdNTree : public Geometry, public Boundary
{
	/// i --> tree_leaf[i] --> prim_start
	///                    \-> rope_ind   --> leaf_neighbors[rope_ind]
	///
	struct TreeLeaf {
		unsigned _primStart;
		unsigned _ropeInd;
	};
	
	VectorArray<T> * m_source;
    Tn * m_nodePool;
	TreeLeaf * m_leafNodes;
    int * m_leafDataIndices;
    int m_maxNumNodes;
	int m_maxNumData;
	int m_numNodes;
	int m_numLeafNodes;
	int m_numLeafData;
	
public:
    KdNTree(VectorArray<T> * source);
	virtual ~KdNTree();

    Tn * root();
    Tn * nodes();
	
    int maxNumNodes() const;
	int maxNumData() const;
	
	int numNodes() const;
	int addNode();
	
	void addDataIndex(int x);
	int numData() const;
	T * dataAt(unsigned idx) const;
	
	int numLeafNodes() const;
	void addLeafNode(unsigned primStart);
	unsigned leafPrimStart(unsigned idx) const;
	unsigned leafRopeInd(unsigned idx) const;
	
	VectorArray<T> * source();
	
	virtual std::string verbosestr() const;
protected:

private:

};

template <typename T, typename Tn>
KdNTree<T, Tn>::KdNTree(VectorArray<T> * source) 
{
	m_source = source;
	int numPrims = source->size();
    m_maxNumNodes = numPrims>>Tn::BranchingFactor-1;
	m_maxNumData = numPrims<<1;
    m_nodePool = new Tn[m_maxNumNodes];
	m_leafNodes = new TreeLeaf[m_maxNumNodes<<2];
    m_leafDataIndices = new int[m_maxNumData];
	m_numNodes = 1;
	m_numLeafNodes = 0;
	m_numLeafData = 0;
}

template <typename T, typename Tn>
KdNTree<T, Tn>::~KdNTree() 
{
    delete[] m_nodePool;
	delete[] m_leafNodes;
    delete[] m_leafDataIndices;
}

template <typename T, typename Tn>
VectorArray<T> * KdNTree<T, Tn>::source()
{ return m_source; }

template <typename T, typename Tn>
Tn * KdNTree<T, Tn>::root()
{ return &m_nodePool[0]; }

template <typename T, typename Tn>
Tn * KdNTree<T, Tn>::nodes()
{ return m_nodePool; }

template <typename T, typename Tn>
int KdNTree<T, Tn>::maxNumNodes() const
{ return m_maxNumNodes; }

template <typename T, typename Tn>
int KdNTree<T, Tn>::maxNumData() const
{ return m_maxNumData; }

template <typename T, typename Tn>
int KdNTree<T, Tn>::numNodes() const
{ return m_numNodes; }

template <typename T, typename Tn>
int KdNTree<T, Tn>::numData() const
{ return m_numLeafData; }

template <typename T, typename Tn>
void KdNTree<T, Tn>::addDataIndex(int x)
{
	m_leafDataIndices[m_numLeafData] = x;
	m_numLeafData++;
}

template <typename T, typename Tn>
int KdNTree<T, Tn>::addNode()
{ 
	m_numNodes++; 
	return m_numNodes - 1;
}

template <typename T, typename Tn>
T * KdNTree<T, Tn>::dataAt(unsigned idx) const
{ return m_source->get(m_leafDataIndices[idx]); }

template <typename T, typename Tn>
int KdNTree<T, Tn>::numLeafNodes() const
{ return m_numLeafNodes; }

template <typename T, typename Tn>
void KdNTree<T, Tn>::addLeafNode(unsigned primStart)
{ 
	m_leafNodes[m_numLeafNodes]._primStart = primStart;
	m_numLeafNodes++; 
}

template <typename T, typename Tn>
unsigned KdNTree<T, Tn>::leafPrimStart(unsigned idx) const
{ return m_leafNodes[idx]._primStart; }

template <typename T, typename Tn>
unsigned KdNTree<T, Tn>::leafRopeInd(unsigned idx) const
{ return m_leafNodes[idx]._ropeInd; }

template <typename T, typename Tn>
std::string KdNTree<T, Tn>::verbosestr() const
{ 
	std::stringstream sst;
	sst<<"\n KdNTree: "
	<<"\n treelet level "<<Tn::BranchingFactor
	<<"\n n input "<<m_source->size()
	<<"\n n nodes(max) "<<numNodes()<<"("<<maxNumNodes()<<")"
	<<"\n n leaf(max) "<<m_numLeafNodes<<"("<<(maxNumNodes()<<2)<<")"
	<<"\n n data(max) "<<numData()<<"("<<maxNumData()<<")"
	<<"\n";
	return sst.str();
}
//:~
