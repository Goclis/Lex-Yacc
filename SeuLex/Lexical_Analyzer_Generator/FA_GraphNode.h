#pragma once

#define NON_ACCEPTING -99
#define DEFAULT_END -1

//DEFAULT_END 作为空白符的终终止态。。

//-99 非结束点，default_end:非token的结束符

class FA_GraphNode :
	public GraphNode
{
protected:

	//12-5 将这里bool 换成int 以代表token序号
	//-1:普通 >0对应的token序号？
	//潜在问题：二义文法可能存在一个节点是多个token的accepting state
	//			不过如果不是二义文法，两个不同token之间肯定有非epsilon边间隔
	//这里的问题由RD中的Token定义顺序决定，序号大的Token优先级高

	//bool accepting_node;		//12-4 这里可能要换成至少是int (与 Token 相关)
	int accepting_node;			//token
public:
	FA_GraphNode(void);
	FA_GraphNode(int idx,int acpt);


	void SetAcceptingNode(int tkn){accepting_node=tkn;}
	int GetAcceptingNode(){return accepting_node;}
	virtual ~FA_GraphNode(void);
};

