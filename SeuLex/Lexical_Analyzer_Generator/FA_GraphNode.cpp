#include "stdafx.h"
//#include "FA_GraphNode.h"


FA_GraphNode::FA_GraphNode(void):GraphNode()
{
	accepting_node=-1;
}

FA_GraphNode::FA_GraphNode(int idx,int acpt=-1):GraphNode(idx)
{
	accepting_node=acpt;
}

FA_GraphNode::~FA_GraphNode(void)
{
}
