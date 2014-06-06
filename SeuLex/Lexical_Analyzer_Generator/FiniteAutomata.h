#pragma once
#include "Graph\SparseGraph.h"

enum FA_TYPE{NFA,DFA};

//小的NFA项
struct NFA_BLOCK
{
	//NFA building block
	//used with stack
	//to assist constructing the NFA with many basic NFAs
	//has only one accepting_node
	//VT    .   |   *
	static const char NOT_TERMINAL;

	char vt;
	int start_node;
	int accepting_node;

	NFA_BLOCK(char c,int start,int accepting)
	{
		vt=c;
		start_node=start;
		accepting_node=accepting;
	}


};

const char NFA_BLOCK::NOT_TERMINAL=' ';	//since ' ' cannot be a terminal char in lexical token (white space is for syntax)



//状态集合
class SetOfState
{
public:
	//vector<bool> set;
	list<int> set;	

	//12-5 modify
	//bool accepting;
	int accepting;	//accepting_token index
	
	SetOfState(int num=0):set(num,0)
	{
		accepting=NON_ACCEPTING;
	}

	~SetOfState()
	{
	}


	void AddState(int node)
	{
		set.push_back(node);
	}

	void Clear()
	{
		set.clear();
	}
	/*
	void SetValue(int idx,bool b=true)
	{
		set.at(idx)=b;
	}
	*/
	bool Exist(int node)
	{
		//线性结构真傻逼
		//return whether the node exists in the current list
		list<int>::iterator iter;
		for(iter=set.begin();iter!=set.end();++iter)
		{
			if(node==*iter)
			{
				//already exist
				return true;
			}
		}
		return false;
	}

	bool AddState_New(int node)
	{
		//only add the state when it didn't exist in the list
		if(Exist(node))
		{
			AddState(node);
			return true;
		}
		return false;
	}

	
	//12-4是这里static导致的问题吗

	//判断两个集合是否相等
	static bool Equal(SetOfState & a,SetOfState & b,int num_state)
	//bool Equal(SetOfState & a,SetOfState & b)
	{
		if(a.set.size()!=b.set.size())
		{
			return false;
		}


		//a.set.size()==b.set.size()

		//一种办法是创建一个零时bool[]，size=VT
		//遍历list_a，标出true的项，遍历list_b,对应查找

		//12-4 修改一处
		//int num_vt=pVTManager->GetSize();	//怎么是num_vt?
		
		//bool *tempvt=new bool[num_vt];
		vector<bool> tempvt(num_state,false);//这里应该是nfa的num of states
		//int i;

		/*
		for(i=0;i<num_state;i++)
		{
			tempvt[i]=false;
		}
		*/

		//遍历a:list
		list<int>::iterator aiter;
		for(aiter=a.set.begin();aiter!=a.set.end();++aiter)
		{
			//tempvt[(*aiter)]=true;
			//i=*aiter;
			tempvt.at((*aiter))=true;
		}

		bool e=true;
		list<int>::iterator biter;
		for(biter=b.set.begin();biter!=b.set.end();++biter)
		{
			if(!tempvt.at((*biter)))
			{
				//noticed that sizeA=sizeB
				e=false;
				break;
			}
		}
		

		//为什么delete tempvt 会有问题？
		//delete tempvt;

		return e;

	}
	

};










class FiniteAutomata :
	public SparseGraph<FA_GraphNode,FA_GraphEdge>
{
protected:
	FA_TYPE fa_type;	//用参数控制还是再搞DFA,NFA两个子类？

	int start_state;	

	//vector<NFA_BLOCK> vn_nfa;	//nfa适用，各非终极符的小nfa
								//12-5这种办法恐怕不行啊，letter.letter
								//你当LR(1)是吃白饭的啊

	//FA_GraphNode* accepting_state;

public:
	FiniteAutomata();
	FiniteAutomata(string postfix_re,int vn_idx);
	//FiniteAutomata(FiniteAutomata& nfa);	这么搞覆盖了复制构造函数
	virtual ~FiniteAutomata(void);

	
	
	//12-11

	void AddEdge(EdgeType edge);




	//construct a new NFA according to the postfix regular expression
	//connect it to the global NFA
	//return whether the re is valid

	bool ConstructNFA(string postfix_re,int vn_idx);
	
	//add the nfa to this big nfa
	//used for VN
	void AddNFA(FiniteAutomata& nfa,int & start,int & end);

	//construct a new DFA according to the NFA
	void ConstructDFA(FiniteAutomata& nfa);

	//12-10新改
	//construct a DFAo according to the nfa
	void ConstructOptimizedDFA(FiniteAutomata& nfa);

	// Lex algorithm
	void ConstructDFA(string re);


	//print the graph (node->edgelist)
	void Print();

	//generate Lexical Analyzer source code by DFA
	//string GenerateSourceCode();


	int GetStartNode();

	void SetFAType(FA_TYPE t){fa_type=t;}
	//add an orginal start state
	void AddOrignalStartNode();

	int GetEdgeNumOfNode(int node_id){return m_Edges.at(node_id).size();}

protected:
	

	void e_closure(SetOfState& closure,SetOfState& core,FiniteAutomata& nfa);
	void Move(SetOfState &r,SetOfState& T,int vt_idx,FiniteAutomata& nfa);

	
	int GetMoveVT(SetOfState& block,int from,char vt,FiniteAutomata& nfa);

	
};

