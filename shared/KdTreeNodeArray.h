/*
 *  KdTreeNodeArray.h
 *  kdtree
 *
 *  Created by jian zhang on 10/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <BaseArray.h>
#include <KdTreeNode.h>

class KdTreeNodeArray : public BaseArray<KdTreeNode> {
public:
	KdTreeNodeArray();
	virtual ~KdTreeNodeArray();
	
	KdTreeNode * asKdTreeNode(unsigned index);
	KdTreeNode * asKdTreeNode();
protected:
	virtual unsigned elementSize() const;
	virtual unsigned numElementPerBlock() const;
};