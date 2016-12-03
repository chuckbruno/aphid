/*
 *  KdBuilder.h
 *  aphid
 *
 *  Created by jian zhang on 10/29/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "KdRope.h"

namespace aphid {

template<int NumLevels, typename T, typename Tn>
class KdTreeletBuilder : public Treelet<NumLevels > {

	static int NumSubSplits;
    SahSplit<T> * m_splits[(1<<NumLevels+1) - 2];
    KdNTree<T, Tn> * m_tree;
	
public:
	KdTreeletBuilder(int index, KdNTree<T, Tn> * tree);
	virtual ~KdTreeletBuilder();
	
	void setRootToLeaf(SahSplit<T> * parent, Tn * root, int iRoot);

	void build(int parentIdx, SahSplit<T> * parent, Tn * node, Tn * root, int iRoot);
	
	SahSplit<T> * split(int idx);
    
	static int MaxNumPrimsInLeaf;

protected:
	bool subdivideRoot(int parentIdx, SahSplit<T> * parent, Tn * root, int iRoot);
	bool subdivideInterial(Tn * interial, int level);
	void setNodeInternal(Tn * node, int idx, int axis, float pos, int offset);
	void setNodeLeaf(SahSplit<T> * parent, Tn * node, int idx);
private:
	void clearSplit(int idx);
	void costNotice(SahSplit<T> * parent, SplitEvent * plane) const;
};

template<int NumLevels, typename T, typename Tn>
int KdTreeletBuilder<NumLevels, T, Tn>::MaxNumPrimsInLeaf = 8;

template<int NumLevels, typename T, typename Tn>
int KdTreeletBuilder<NumLevels, T, Tn>::NumSubSplits = (1<<NumLevels+1) - 2;

template<int NumLevels, typename T, typename Tn>
KdTreeletBuilder<NumLevels, T, Tn>::KdTreeletBuilder(int index, KdNTree<T, Tn> * tree) :
Treelet<NumLevels>(index)
{
	int i;
	for(i=0;i<NumSubSplits;i++) {
		m_splits[i] = NULL;
	}
    m_tree = tree;
}

template<int NumLevels, typename T, typename Tn>
KdTreeletBuilder<NumLevels, T, Tn>::~KdTreeletBuilder()
{
	int i;
	for(i=0;i<NumSubSplits;i++) {
		if(m_splits[i]) delete m_splits[i];
	}
}

template<int NumLevels, typename T, typename Tn>
void KdTreeletBuilder<NumLevels, T, Tn>::setRootToLeaf(SahSplit<T> * parent, Tn * root, int iRoot)
{
	setNodeLeaf(parent, root, iRoot);
}

template<int NumLevels, typename T, typename Tn>
void KdTreeletBuilder<NumLevels, T, Tn>::build(int parentIdx, SahSplit<T> * parent, Tn * node, Tn * root, int iRoot)
{
	if(!subdivideRoot(parentIdx, parent, root, iRoot))
		return;
	
	int level = 1;
    for(;level <= NumLevels; level++) {
        if(!subdivideInterial(node, level)) break;
	}
}

template<int NumLevels, typename T, typename Tn>
bool KdTreeletBuilder<NumLevels, T, Tn>::subdivideRoot(int parentIdx, SahSplit<T> * parent, Tn * root, int iRoot)
{
	if(parent->numPrims() < MaxNumPrimsInLeaf) {
		// std::cout<<"\n root count low";
		setNodeLeaf(parent, root, iRoot);
		return false;
	}

	SplitEvent * plane = parent->bestSplit();
	
	if(parent->canEndSubdivide(plane ) ) {
		// std::cout<<"\n root split";
		//costNotice(parent, plane);
		setNodeLeaf(parent, root, iRoot);
		return false;
	}
	
	SahSplit<T>  * lftChild = new SahSplit<T>(parent->source() );
	SahSplit<T>  * rgtChild = new SahSplit<T>(parent->source() );
	parent->partition(lftChild, rgtChild);
	
	m_splits[0] = lftChild;
	m_splits[1] = rgtChild;
	
    const int offsetRoot = Treelet<NumLevels>::index() - parentIdx;
	// std::cout<<"\n root offset "<<offsetRoot;
	setNodeInternal(root, iRoot, plane->getAxis(), plane->getPos(), offsetRoot | Tn::TreeletOffsetMask);
	
	return true;
}

template<int NumLevels, typename T, typename Tn>
bool KdTreeletBuilder<NumLevels, T, Tn>::subdivideInterial(Tn * interial, int level)
{
	bool needNextLevel = false;
    // std::cout<<"\n\n subdiv level "<<level;
    const int nSplitAtLevel = 1<<level;
    int i;
    for(i=0; i<nSplitAtLevel; i++) {
        const int iNode = Treelet<NumLevels>::OffsetByLevel(level) + i;
        const int iLftChild = iNode + Treelet<NumLevels>::ChildOffset(iNode);
        
		// std::cout<<"\n  node "<<iNode;
		
        SahSplit<T>  * parent = m_splits[iNode];
		if(!parent) {
			// std::cout<<"\n no parent ";
			continue;
		}
        
		if(parent->numPrims() < MaxNumPrimsInLeaf) {
			setNodeLeaf(parent, interial, iNode);
			clearSplit(iNode);
			continue;
		}
	
        SplitEvent * plane = parent->bestSplit();
		
		if(parent->canEndSubdivide(plane ) ) {
			// std::cout<<"\n internal split";
			//costNotice(parent, plane);
			setNodeLeaf(parent, interial, iNode);
			clearSplit(iNode);
			continue;
		}
		
		if(level < NumLevels) {
			SahSplit<T>  * lftChild = new SahSplit<T>(parent->source() );
			SahSplit<T>  * rgtChild = new SahSplit<T>(parent->source() );
			parent->partition(lftChild, rgtChild);
			
			m_splits[iLftChild] = lftChild;
			m_splits[iLftChild + 1] = rgtChild;
			
			// std::cout<<"\n spawn "
			//	<<" lft "<<iLftChild
			//	<<" rgt "<<iLftChild + 1;
			
			setNodeInternal(interial, iNode, plane->getAxis(), plane->getPos(), Treelet<NumLevels>::ChildOffset(iNode) );
			
			clearSplit(iNode);
		}
		else {
			// std::cout<<"\n end of treelet "<<parent->numPrims();
			setNodeInternal(interial, iNode, 0, 0.f, 1);
		}

		needNextLevel = true;
    }
	return needNextLevel;
}

template<int NumLevels, typename T, typename Tn>
void KdTreeletBuilder<NumLevels, T, Tn>::clearSplit(int idx)
{
	delete m_splits[idx];
	m_splits[idx] = NULL;
}

template<int NumLevels, typename T, typename Tn>
void KdTreeletBuilder<NumLevels, T, Tn>::setNodeInternal(Tn * node, int idx, int axis, float pos, int offset)
{ 
	m_tree->addNInternal();
	node->setInternal(idx, axis, pos, offset); 
}

template<int NumLevels, typename T, typename Tn>
void KdTreeletBuilder<NumLevels, T, Tn>::setNodeLeaf(SahSplit<T> * parent, Tn * node, int idx)
{
	parent->decompressPrimitives(true);
	int iLeaf = m_tree->numLeafNodes();
	int primStart = 0, primLen = 0;
	if(parent->isEmpty()) {
		m_tree->addEmptyVolume(parent->getBBox().volume() );
	}
	else {
		primStart = m_tree->numPrimIndirection();
		primLen = parent->numPrims();
		int i = 0;
		for(;i<parent->numPrims();i++)
			m_tree->addDataIndex( *parent->indices()[i] );
			
		m_tree->addNLeaf();
		m_tree->updateNPrim(primLen);
	}
	// std::cout<<"\n leaf["<<iLeaf<<"] ("<<primStart<<","<<primLen<<")";
	node->setLeaf(idx, iLeaf, primLen);
	m_tree->addLeafNode(primStart, primLen);
}

template<int NumLevels, typename T, typename Tn>
SahSplit<T> * KdTreeletBuilder<NumLevels, T, Tn>::split(int idx)
{ return m_splits[idx]; }

template<int NumLevels, typename T, typename Tn>
void KdTreeletBuilder<NumLevels, T, Tn>::costNotice(SahSplit<T> * parent, SplitEvent * plane) const
{
	std::cout<<"\n visit cost "
			<<parent->visitCost()
			<<" < split cost "
			<<plane->getCost()
			<<" stop subdivide";
	plane->verbose();
}

template<int NumLevels, typename T, typename Tn>
class KdNBuilder {
	
public:
	KdNBuilder();
	virtual ~KdNBuilder();
	
	void build(SahSplit<T> * parent, KdNTree<T, Tn> * tree);
	
	static void SetNumPrimsInLeaf(int x);
	static int MaxTreeletLevel;
	
protected:
	void subdivide(KdTreeletBuilder<NumLevels, T, Tn> * treelet, KdNTree<T, Tn> * tree, int level);
	void process(const KdRope<NumLevels, T, Tn> * treelet, KdNTree<T, Tn> * tree);
	
private:

};

template<int NumLevels, typename T, typename Tn>
int KdNBuilder<NumLevels, T, Tn>::MaxTreeletLevel = 8;

template<int NumLevels, typename T, typename Tn>
KdNBuilder<NumLevels, T, Tn>::KdNBuilder() {}

template<int NumLevels, typename T, typename Tn>
KdNBuilder<NumLevels, T, Tn>::~KdNBuilder() {}

template<int NumLevels, typename T, typename Tn>
void KdNBuilder<NumLevels, T, Tn>::SetNumPrimsInLeaf(int x)
{ KdTreeletBuilder<NumLevels, T, Tn>::MaxNumPrimsInLeaf = x; }

template<int NumLevels, typename T, typename Tn>
void KdNBuilder<NumLevels, T, Tn>::build(SahSplit<T> * parent, KdNTree<T, Tn> * tree)
{
	/// node[1]
	tree->addBranch();
	KdTreeletBuilder<NumLevels, T, Tn> treelet(1, tree);
	Tn * root = tree->root();
	sdb::VectorArray<Tn> & nodes = tree->branches();
	/// only first node in first treelet is useful
	/// spawn into second treelet
	treelet.build(0, parent, nodes[1], root, 0);
	subdivide(&treelet, tree, 0);
	
	KdRope<NumLevels, T, Tn> rope(1, tree);
	rope.beginMap();
	BoxNeighbors ns;
	ns.reset();
	rope.build(0, 0, parent->getBBox(), ns);
	process(&rope, tree);
	rope.endMap();
}

template<int NumLevels, typename T, typename Tn>
void KdNBuilder<NumLevels, T, Tn>::subdivide(KdTreeletBuilder<NumLevels, T, Tn> * treelet, KdNTree<T, Tn> * tree, int level)
{	
	tree->addMaxLevel(level);
    const int parentIdx = treelet->index();
	sdb::VectorArray<Tn> & nodes = tree->branches();
	Tn * parentNode = nodes[parentIdx];
	const int n = treelet->numNodes();
	int i = treelet->LastLevelOffset();
	for(;i<n;i++) {
		SahSplit<T> * parent = treelet->split(i);
		if(!parent) continue;
		
		const int branchIdx = tree->addBranch();

        KdTreeletBuilder<NumLevels, T, Tn> subTreelet(branchIdx, tree);
        if(level+1 == MaxTreeletLevel) {
			subTreelet.setRootToLeaf(parent, parentNode, i);
		}
		else {
			subTreelet.build(parentIdx, parent, nodes[branchIdx], parentNode, i);
			subdivide(&subTreelet, tree, level+1);
		}
	}
}

template<int NumLevels, typename T, typename Tn>
void KdNBuilder<NumLevels, T, Tn>::process(const KdRope<NumLevels, T, Tn> * treelet, KdNTree<T, Tn> * tree)
{
	const int parentIdx = treelet->index();
	sdb::VectorArray<Tn> & nodes = tree->branches();
	Tn * parentNode = nodes[parentIdx];
	const int n = treelet->numNodes();
	int i = treelet->LastLevelOffset();
	for(;i<n;i++) {
		KdTreeNode * k = parentNode->node(i);
		if(k->isLeaf()) continue;
		
		int offset = k->getOffset();
		if(offset < KdNode4::TreeletOffsetMask) continue; /// empty
		offset &= ~KdNode4::TreeletOffsetMask;
			
		const int branchIdx = parentIdx + offset;

        KdRope<NumLevels, T, Tn> subTreelet(branchIdx, tree);
		// std::cout<<"\n process "<<parentIdx<<" "<<i<<" "<<treelet->box(i);
		// treelet->neighbor(i).verbose();
		subTreelet.build(parentIdx, i, treelet->box(i), treelet->neighbor(i) );
		process(&subTreelet, tree);
	}
}

}
//:~