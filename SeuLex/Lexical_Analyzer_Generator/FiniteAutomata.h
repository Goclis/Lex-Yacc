#pragma once
#include "Graph\SparseGraph.h"

enum FA_TYPE{NFA,DFA};

//С��NFA��
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



//״̬����
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
		//���Խṹ��ɵ��
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

	
	//12-4������static���µ�������

	//�ж����������Ƿ����
	static bool Equal(SetOfState & a,SetOfState & b,int num_state)
	//bool Equal(SetOfState & a,SetOfState & b)
	{
		if(a.set.size()!=b.set.size())
		{
			return false;
		}


		//a.set.size()==b.set.size()

		//һ�ְ취�Ǵ���һ����ʱbool[]��size=VT
		//����list_a�����true�������list_b,��Ӧ����

		//12-4 �޸�һ��
		//int num_vt=pVTManager->GetSize();	//��ô��num_vt?
		
		//bool *tempvt=new bool[num_vt];
		vector<bool> tempvt(num_state,false);//����Ӧ����nfa��num of states
		//int i;

		/*
		for(i=0;i<num_state;i++)
		{
			tempvt[i]=false;
		}
		*/

		//����a:list
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
		

		//Ϊʲôdelete tempvt �������⣿
		//delete tempvt;

		return e;

	}
	

};










class FiniteAutomata :
	public SparseGraph<FA_GraphNode,FA_GraphEdge>
{
protected:
	FA_TYPE fa_type;	//�ò������ƻ����ٸ�DFA,NFA�������ࣿ

	int start_state;	

	//vector<NFA_BLOCK> vn_nfa;	//nfa���ã������ռ�����Сnfa
								//12-5���ְ취���²��а���letter.letter
								//�㵱LR(1)�ǳ԰׷��İ�

	//FA_GraphNode* accepting_state;

public:
	FiniteAutomata();
	FiniteAutomata(string postfix_re,int vn_idx);
	//FiniteAutomata(FiniteAutomata& nfa);	��ô�㸲���˸��ƹ��캯��
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

	//12-10�¸�
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

