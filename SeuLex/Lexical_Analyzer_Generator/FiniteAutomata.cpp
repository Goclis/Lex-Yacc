#include "stdafx.h"
//#include <cassert>
//#include "FiniteAutomata.h"


FiniteAutomata::FiniteAutomata(void):SparseGraph<FA_GraphNode,FA_GraphEdge>(true)
{
	//default setting
	fa_type=NFA;	//���������Ⱑ
	//AddOrignalStartNode();	//12-5 �ָ���NFA��һ��Ĭ�Ͽ�ʼ�ڵ�
}


FiniteAutomata::FiniteAutomata(string postfix_re,int vn_idx):SparseGraph<FA_GraphNode,FA_GraphEdge>(true)
{
	//����Сnfa�Զ���

	//construct a NFA according to the given regular expression
	//(postfix,directly include only terminal symbol)
	fa_type=NFA;
	//AddOrignalStartNode();	
	ConstructNFA(postfix_re,vn_idx);
}

/*
FiniteAutomata::FiniteAutomata(FiniteAutomata& nfa):SparseGraph<FA_GraphNode,FA_GraphEdge>(true)
{
	//construct a NFA according to the given regular expression
	//(postfix,directly include only terminal symbol)
	fa_type=DFA;
	//AddOrignalStartNode();
	ConstructDFA(nfa);
}
*/

FiniteAutomata::~FiniteAutomata(void)
{
}


void FiniteAutomata::AddOrignalStartNode()
{
	//add an orginal start state
	start_state=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),NON_ACCEPTING));//12-5
}


void FiniteAutomata::AddEdge(EdgeType edge)
{
	 //first make sure the from and to nodes exist within the graph 
  assert( (edge.From() < m_iNextNodeIndex) && (edge.To() < m_iNextNodeIndex) &&
          "<SparseGraph::AddEdge>: invalid node index");

  //make sure both nodes are active before adding the edge
  if ( (m_Nodes[edge.To()].Index() != invalid_node_index) && 
       (m_Nodes[edge.From()].Index() != invalid_node_index))
  {
	  
    //add the edge, first making sure it is unique
    //if (UniqueEdge(edge.From(), edge.To()))
    //{
      m_Edges[edge.From()].push_back(edge);
    //}
	

    //if the graph is undirected we must add another connection in the opposite
    //direction
    if (!m_bDigraph)
    {
      //check to make sure the edge is unique before adding
      if (UniqueEdge(edge.To(), edge.From()))
      {
        EdgeType NewEdge = edge;

        NewEdge.SetTo(edge.From());
        NewEdge.SetFrom(edge.To());

        m_Edges[edge.To()].push_back(NewEdge);
      }
    }
  }
}




bool FiniteAutomata::ConstructNFA(string postfix_re,int vn_idx)
{
	//12-5 postfix_re������ռ����� Ҫ����
	//������ռ���@12@�Ժ�postfix���ô����ˡ�����

	//construct a NFA according to one given regular expression
	//(postfix,directly include only terminal symbol)
	//return whether the RE is valid
	//assert(vn_idx>=0);

	if(postfix_re=="")
	{
		//an empty string
		return false;
	}

	bool origin_empty=(NumActiveNodes()<=0);

	//I need a stack here
	//Is deque superior to vector?
	stack<NFA_BLOCK> NFAs;
	int i=0;	//reader of the RE
	int size=postfix_re.size();
	int this_start_state;	//the start state of the NFA of this RE

	

	char concatenation=pVTManager->GetVTAt(-2);
	char alternation=pVTManager->GetVTAt(-3);
	char closure=pVTManager->GetVTAt(-4);
	char epsilon=pVTManager->GetEpsilon();
	char vn_flag=pVTManager->GetVTAt(-5);//@vn_idx@

	//any other char will be treated as VT

	int tmp_start;
	int tmp_end;
	int tmp1,tmp2;
	char tmp_char;
	


	for(i=0;i<size;i++)
	{
		tmp_char=postfix_re.at(i);
		if(tmp_char==concatenation)
		{
			tmp_end=NFAs.top().start_node;
			tmp2=NFAs.top().accepting_node;
			NFAs.pop();
			tmp_start=NFAs.top().accepting_node;
			tmp1=NFAs.top().start_node;
			NFAs.pop();
			AddEdge(FA_GraphEdge(tmp_start,tmp_end,epsilon));
			GetNode(tmp_start).SetAcceptingNode(NON_ACCEPTING);

			//Register the new small NFA
			NFAs.push(NFA_BLOCK(NFA_BLOCK::NOT_TERMINAL,tmp1,tmp2));
			this_start_state=tmp1;
		}
		else if(tmp_char==alternation)
		{
			tmp_start=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),NON_ACCEPTING));
			tmp_end=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),vn_idx));//12-5 temp default end
			
			GetNode(NFAs.top().accepting_node).SetAcceptingNode(NON_ACCEPTING);
			AddEdge(FA_GraphEdge(tmp_start,NFAs.top().start_node,epsilon));
			AddEdge(FA_GraphEdge(NFAs.top().accepting_node,tmp_end,epsilon));
			NFAs.pop();

			GetNode(NFAs.top().accepting_node).SetAcceptingNode(NON_ACCEPTING);
			AddEdge(FA_GraphEdge(tmp_start,NFAs.top().start_node,epsilon));
			AddEdge(FA_GraphEdge(NFAs.top().accepting_node,tmp_end,epsilon));
			NFAs.pop();

			//Register the new small NFA
			NFAs.push(NFA_BLOCK(NFA_BLOCK::NOT_TERMINAL,tmp_start,tmp_end));
			this_start_state=tmp_start;
		}
		else if(tmp_char==closure)
		{
			tmp_start=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),NON_ACCEPTING));
			tmp_end=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),vn_idx));
			AddEdge(FA_GraphEdge(NFAs.top().accepting_node,NFAs.top().start_node,epsilon));
			AddEdge(FA_GraphEdge(tmp_start,NFAs.top().start_node,epsilon));
			AddEdge(FA_GraphEdge(NFAs.top().accepting_node,tmp_end,epsilon));
			AddEdge(FA_GraphEdge(tmp_start,tmp_end,epsilon));
			GetNode(NFAs.top().accepting_node).SetAcceptingNode(NON_ACCEPTING);
			NFAs.pop();

			//Register the new small NFA
			NFAs.push(NFA_BLOCK(NFA_BLOCK::NOT_TERMINAL,tmp_start,tmp_end));
			this_start_state=tmp_start;
		}
		else if(tmp_char==vn_flag)
		{
			//@VN_idx@

			//12-6 ɶʱ���п���string,itoa��дһ��
			queue<char> vn_string;
			int nlength=-1;	//@..@�����ֵ�λ��

			do
			{
				i++;
				tmp_char=postfix_re.at(i);
				vn_string.push(tmp_char);
				nlength++;

			}while(tmp_char!=vn_flag);


			int link_vn_idx=0;
			
			while(nlength>0)
			{
				nlength--;
				//�����Ȳ�����������
				link_vn_idx+=pow(10,nlength)*((int)vn_string.front()-48);
				vn_string.pop();
			}


			//12-8 tmp start �ӵ�״̬��Щ����
			AddNFA(pVNManager->GetFAAt(link_vn_idx),tmp_start,tmp_end);

			//tmp_start,tmp_end���¼ӵ�СNFA����ֹ��idx
			NFAs.push(NFA_BLOCK(NFA_BLOCK::NOT_TERMINAL,tmp_start,tmp_end));

		}
		else
		{
			//VT
			tmp_start=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),NON_ACCEPTING));//temp start
			tmp_end=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),vn_idx));//temp end
			AddEdge(FA_GraphEdge(tmp_start,tmp_end,tmp_char));
			
			NFAs.push(NFA_BLOCK(tmp_char,tmp_start,tmp_end));
			this_start_state=tmp_start;
		}

		
	}


	if( NFAs.size()==1 && i==size )	//ջֻʣ�����ӵ��Ǹ�nfa�����ң����պö��꣬�����ɹ�
	{	
		//12-7 ���s->@a@,ֻ��һ�����ռ�����accepting node �������Ⱑ����
		GetNode(NFAs.top().accepting_node).SetAcceptingNode(vn_idx);

		if(origin_empty)
		{
			//12-4���Ӧ���ж�ԭ����û�е㣬��ʵ��һ��original������õ�
			start_state=this_start_state;
		}
		else
		{
			//ԭ�����գ�����ֻ��original state��Ҳ������ԭ��ĳ��nfa��
			//valid RE
			//connect the NFA generated by this RE to the global start state
			AddEdge(FA_GraphEdge(start_state,this_start_state,epsilon));
			//12-5ע����������䣬��Ϊoriginal startnode���ٱ����״̬��
			//GetNode(start_state).SetAcceptingNode(NON_ACCEPTING);
		}
		return true;
	}

	return false;


}


void FiniteAutomata::AddNFA(FiniteAutomata& nfa, int & start,int & end)
{
	//add the nfa to this nfa
	//used for VN

	//�������������ͱ�
	//�¼ӵĶ����Ľڵ�ı��Ҫͳһƫ��
	
	//12-8 Ϊ�˼���һ�¼�dfa
	//assert(nfa.fa_type==NFA);	//�����protected�ܷ��ʣ�

	//����ȫ����û��ɾ���ڵ�Ĳ��������ýڵ���Ӧ���ǳ��ܵ�

	int offset=GetNextFreeNodeIndex();

	//12-6ͻȻ���ֺ���protected����ʵ��ͬһ���಻ͬʵ����ķ��ʣ�
	//NodeVector::iterator ni;
	int ni;		//node,EdgeListVector�������±�
	//EdgeListVector::iterator evi;
	EdgeList::iterator ei;

	start=nfa.GetStartNode()+offset;	//12-8 ����+offset

	int end_node_idx=nfa.m_Nodes.size();
	int current,tmp_acpt;
	for(ni=0;ni<end_node_idx;ni++)
	{
		//һ��Ҳ����׳..
		
		tmp_acpt=nfa.m_Nodes.at(ni).GetAcceptingNode();
		current=AddNode(FA_GraphNode(nfa.m_Nodes.at(ni).Index()+offset
					,tmp_acpt));
		
		if(tmp_acpt!=NON_ACCEPTING)
		{
			end=current;
		}
		/*
		//12-6 �˴�Ҫ�Ȱ�Nodeȫ������С�SparseGraph�н�ǿ�ı�������
		for(ei=nfa.m_Edges.at(ni).begin();ei!=nfa.m_Edges.at(ni).end();++ei)
		{
			AddEdge(FA_GraphEdge(current,ei->To()+offset,ei->getVT()));
		}
		*/
	}

	for(ni=0;ni<end_node_idx;ni++)
	{
		//12-6 �˴�Ҫ�Ȱ�Nodeȫ������С�SparseGraph�н�ǿ�ı�������

		for(ei=nfa.m_Edges.at(ni).begin();ei!=nfa.m_Edges.at(ni).end();++ei)
		{
			AddEdge(FA_GraphEdge(ei->From()+offset,ei->To()+offset,ei->getVT()));
		}
	}

}




//void FiniteAutomata::ConstructDFA(FiniteAutomata& nfa,bool global_accpeting_state)
void FiniteAutomata::ConstructDFA(FiniteAutomata& nfa)
{
	//12-5���а���dfa�ﲻ����epsilon�߰�������
	//12-5 bool global_accpeting_state,�Ƿ����һ��global ��ֹ״̬
	//ֻ�ڷ���Сdfa�ſ�����Ϊtrue

	//12-5 ����������ֹ״̬����һ���ߵ�һ��������ֹ״̬�� ����NFA_BLOCK
	//Сdfaֻ�Ƿ���һ��token,ֻ��global dfa�ж����ͬ��ֹ״̬

	//generate DFA according to the given NFA
	fa_type=DFA;


	//##########  preperation  #######################

	//set are be represented by set vector<bool> simply
	int num_states=nfa.NumNodes();	//num of nfa states
	int num_vt=pVTManager->GetSize();

	vector<SetOfState> core(0,SetOfState());	//e-closure( core )

	SetOfState tmp_corestart(0);
	tmp_corestart.AddState(nfa.GetStartNode());		//12-4����
	core.push_back(tmp_corestart);

	//generated by e-closure( core[same idx] )
	//first is e-closure( {s} )
	

	vector<SetOfState> dfa_state(0,SetOfState());
	dfa_state.push_back(SetOfState(0));
	e_closure(dfa_state.at(0),core.at(0),nfa);

	//2d table
	//each edge point to which dfa state
	//11-6: this should be modified to index:int only(point to dfa_state)
	//vector< vector<SetOfState> >    edge_table( num_vt  ,   vector<SetOfState>(0)   );	//to be 
	vector<vector<int>> edge_table(num_vt,vector<int>());

	//##############################################








	//***********construct the e-closure table***********************
	int i,j;

	int core_i=0,dfa_state_i=0,column_vt=0;		//reader
	//dfa_state_iͬʱ��һ���е�����
	//bool new_dfa_state=true;


	bool exist=false;	
	//bool *bvt;	//�Ƚϼ����Ƿ������
	//bvt=new bool[num_vt];


	while( dfa_state_i<dfa_state.size() )
	{
		//there's new Ii generated
		//core_i++;
		//dfa_state_i++;	//point the the current operating one


		for(column_vt=0;column_vt<num_vt;column_vt++)
		{
			
			core.push_back(SetOfState());
			Move(core.at(++core_i),dfa_state.at(dfa_state_i),column_vt,nfa);	//generate the core

			if(core.at(core_i).set.size()==0)
			{
				//�ǿռ�
				//��һ��϶��ǿյģ�ֱ�ӿ���һ��
				//12-4 �ռ���edge_table����ô��ʾ�����Ʊߣ�
				core.pop_back();
				core_i--;
				edge_table.at(column_vt).push_back(-1);
				continue;
			}



			dfa_state.push_back(SetOfState());
			e_closure(dfa_state.back(),core.at(core_i),nfa);

			//TODO:	��ʱ�ж��µ�dfa_state��֮ǰ����û���ظ�
			//	(����ж���ô����ʱ�临����?)
			//		�ظ���pop(); new_dfa_state=false;
			//		�µ�: ����edge_table; new_dfa_state=true;
			

			//check whether this
			exist=false;
			for(i=0;i<dfa_state.size()-1;i++)
			{
				if(  SetOfState::Equal(dfa_state.at(i),dfa_state.back(),num_states)    )
				//if(  dfa_state.at(i).Equal(dfa_state.at(i),dfa_state.back())    )
				{
					//����ȵ�dfa_state�������µ�state
					exist=true;
					dfa_state.pop_back();

					//���϶�Ӧ��state
					//edge_table.at(column_vt).at(i)=i;
					edge_table.at(column_vt).push_back(i);
					break;
				}
			}

			if(!exist)
			{
				//�������µ�state
				//12-4 �˴������������һ������
				//edge_table.at(column_vt).at(dfa_state_i)=dfa_state.size()-1;	
				edge_table.at(column_vt).push_back(dfa_state.size()-1);
			}


		}

		//��һ��
		dfa_state_i++;
	}

	//delete bvt;
	//**************************************************************



	//��񹹽����ˣ��������ǹ���DFA��
	//��ʼ�Ŀ�ʼ�ڵ����еģ���ΪI0

	//i:��ǰ���ڲ�����dfa_state (ָ��table) , j:��ǰ������VT
	dfa_state_i=0;	//��ǰ����ӵĽڵ��״̬�����һ��(ָ��vector)
	vector<int> id_node(dfa_state.size(),0);	//״̬��Ӧ��ͼ�еĽڵ���
	

	//12-4 ��������¼ӣ�û��original_state������)
	if(dfa_state.size()>0)
	{
		i=0;
		start_state=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),dfa_state.at(i).accepting));	//temp
	}
	//////////////////////////////

	id_node.at(0)=GetStartNode();

	/*
	//����뷨���С���
	//12-5 global_accpeting_state�Ĵ���
	int global_end_idx=-1;	//ȫ����ֹ�ڵ�index
	if(global_accpeting_state)
	{
		global_end_idx=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),NON_ACCEPTING));
	}
	*/

	for(i=0;i<dfa_state.size();i++)
	{
		//��������dfa״̬

		for(j=0;j<num_vt;j++)
		{
			//jָʾvt�ı��
			if(edge_table.at(j).at(i)==-1)
			{
				//�ռ����Ʊ�
				continue;
			}

			if(edge_table.at(j).at(i)>dfa_state_i)		//�˴��ָ��ɣ�״̬������˳����dfa_state������һ�µ�
			{
				//��I״̬��δ���ֹ�
				//����ͼ����Ӹ�״̬��Ӧ�Ľڵ�
				id_node.at(++dfa_state_i)=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),dfa_state.at(edge_table.at(j).at(i)).accepting));//12-4 ����accepting����
			}
			//from: ��ǰ������״̬��iָʾ��
			//to: ��һ��Ԫ����ָ״̬������Ϊ�±꣩
			AddEdge(FA_GraphEdge(id_node.at(i) ,  id_node.at(edge_table.at(j).at(i)) ,  pVTManager->GetVTAt(j) )  );
		}

	}





}







void FiniteAutomata::ConstructOptimizedDFA(FiniteAutomata& nfa)
{
	//TODO
	//O(nlogn)
	//���ϻ��ֵİ취
	
	
	//�ӹ���DFA����ճ���ġ���
	//�ؼ�Ҫedgetable
	//����һ������ľ���nfa����ʵ
	fa_type=DFA;


	//##########  preperation  #######################

	//set are be represented by set vector<bool> simply
	int num_states=nfa.NumNodes();	//num of nfa states
	int num_vt=pVTManager->GetSize();

	vector<SetOfState> core(0,SetOfState());	//e-closure( core )

	SetOfState tmp_corestart(0);
	tmp_corestart.AddState(nfa.GetStartNode());		//12-4����
	core.push_back(tmp_corestart);

	//generated by e-closure( core[same idx] )
	//first is e-closure( {s} )
	

	vector<SetOfState> dfa_state(0,SetOfState());
	dfa_state.push_back(SetOfState(0));
	e_closure(dfa_state.at(0),core.at(0),nfa);

	//2d table
	//each edge point to which dfa state
	//11-6: this should be modified to index:int only(point to dfa_state)
	//vector< vector<SetOfState> >    edge_table( num_vt  ,   vector<SetOfState>(0)   );	//to be 
	vector<vector<int>> edge_table(num_vt,vector<int>());

	//##############################################








	//***********construct the e-closure table***********************
	int i,j,k;

	int core_i=0,dfa_state_i=0,column_vt=0;		//reader
	//dfa_state_iͬʱ��һ���е�����
	//bool new_dfa_state=true;


	bool exist=false;	
	//bool *bvt;	//�Ƚϼ����Ƿ������
	//bvt=new bool[num_vt];


	while( dfa_state_i<dfa_state.size() )
	{
		//there's new Ii generated
		//core_i++;
		//dfa_state_i++;	//point the the current operating one


		for(column_vt=0;column_vt<num_vt;column_vt++)
		{
			
			core.push_back(SetOfState());
			Move(core.at(++core_i),dfa_state.at(dfa_state_i),column_vt,nfa);	//generate the core

			if(core.at(core_i).set.size()==0)
			{
				//�ǿռ�
				//��һ��϶��ǿյģ�ֱ�ӿ���һ��
				//12-4 �ռ���edge_table����ô��ʾ�����Ʊߣ�
				core.pop_back();
				core_i--;
				edge_table.at(column_vt).push_back(-1);
				continue;
			}



			dfa_state.push_back(SetOfState());
			e_closure(dfa_state.back(),core.at(core_i),nfa);

			//TODO:	��ʱ�ж��µ�dfa_state��֮ǰ����û���ظ�
			//	(����ж���ô����ʱ�临����?)
			//		�ظ���pop(); new_dfa_state=false;
			//		�µ�: ����edge_table; new_dfa_state=true;
			

			//check whether this
			exist=false;
			for(i=0;i<dfa_state.size()-1;i++)
			{
				if(  SetOfState::Equal(dfa_state.at(i),dfa_state.back(),num_states)    )
				//if(  dfa_state.at(i).Equal(dfa_state.at(i),dfa_state.back())    )
				{
					//����ȵ�dfa_state�������µ�state
					exist=true;
					dfa_state.pop_back();

					//���϶�Ӧ��state
					//edge_table.at(column_vt).at(i)=i;
					edge_table.at(column_vt).push_back(i);
					break;
				}
			}

			if(!exist)
			{
				//�������µ�state
				//12-4 �˴������������һ������
				//edge_table.at(column_vt).at(dfa_state_i)=dfa_state.size()-1;	
				edge_table.at(column_vt).push_back(dfa_state.size()-1);
			}


		}

		//��һ��
		dfa_state_i++;
	}

	//delete bvt;
	//**************************************************************



	




	//test
	cout<<"��С��״̬��״̬��..\n\n";







	//����edge_table����DFAo

	//12-10 �ȼ��ಢ�鼯��
	//�ƺ�����ҪDFA���edge_table
	int num_nodes=dfa_state.size();

	int OLD_SPLIT=-num_nodes-10;	//���ѷ���ĵȼ�����


	UFSets ufset(num_nodes);		//���ķ�ʽ�洢�Ĳ��鼯



	
	vector<int> root_id;	//�����ϵĸ��ڵ�index
	vector<int> assist_from;		//�����ã�һһ��Ӧroot_id
							//��ǰvt�ִ洢֮ǰ��Find()
							//������һvt֮��ĳ�-1

	if(num_nodes>0)
	{
		root_id.push_back(0);
		assist_from.push_back(OLD_SPLIT);
	}
	

	
	//init
	//��accepting_nodeΪ���ݹ���һ���ȼ��໮��
	int tmp_acpt;
	bool isnew;//�Ƿ���acpt
	for(i=1;i<num_nodes;i++)
	{
		isnew=true;
		tmp_acpt=dfa_state.at(i).accepting;
		
		//������ǰ�ȼ���
		for(j=0;j<root_id.size();j++)
		{
			if(tmp_acpt==dfa_state.at(root_id.at(j)).accepting)
			{
				//��acpt״̬���и��ڵ㣬ָ����
				ufset.SimpleSimpleUnion(root_id.at(j),i);
				isnew=false;
				break;
			}
		}

		if(isnew)
		{
			root_id.push_back(i);
			assist_from.push_back(OLD_SPLIT);
		}
	}
	//��ʱȷ��������ȶ�ֻ��2
	//**********************************************


	




	//������Ҫѭ�����ϲ�״̬
	//for vt a, if from and to both equal,union them

	//?partition, not union

	int temp;
	bool change=true;	//�Ƿ����µĻ���
	int new_from;


	while(change)//temp
	{
		change=false;
		for(i=0;i<num_vt;i++)
		{
			//for each vt

			new_from=root_id.size();	//����vt�·ֳ����Ĵ��⿪ʼ

			for(j=0;j<num_nodes;j++)
			{
				//for each dfa_state

				




				//��to����
				temp=ufset.Find(edge_table.at(i).at(j));
				if(temp!=ufset.Find(edge_table.at(i).at(ufset.Find(j))))
				{
					//ע��-1�Ʊߵ����

					//��ǰvt�ı�To���಻ͬ
					//���������ȼ���
					change=true;
					




					//������û�п��Թ����ĵȼ���
					//12-10 ???�˴����ش���

					//12-10 �����ǣ���һ�֣���vt���ԣ��·ֳ�����Ҫ����ǣ�
					//�������Ƿֳ���֮ǰ��Find()

					//����һ�㣬�������ȷ����ֻ��2�㣬һ����������ȫ���������ϵ�Ҷ��

					
					isnew=true;
					for(k=new_from;k<root_id.size();k++)	
					{

						if(temp==edge_table.at(i).at(ufset.Find(root_id.at(k)))		//to������ͬ
							&& ufset.Find(j)==assist_from.at(k)		//from������ͬ
							&& ufset.Find(j)!=root_id.at(k) )		//����֮ǰsplit�������ࣨ��������ˣ�
						{
							//�ҵ��˹���
							isnew=false;
							ufset.Split(j);
							ufset.SimpleSimpleUnion(root_id.at(k),j);

							break;
						}
					}


					if(isnew)
					{
						//�µĻ��ֳ����ĵȼ���
						root_id.push_back(j);
						assist_from.push_back(ufset.Find(j));
						ufset.Split(j);	//һֱҪ�õ�����Find�����Ե����ڲŷ���
					}

					


					

				}

			}

			//����vt�·ֳ������assist_from��Ϊ-1
			assert(root_id.size()==assist_from.size());

			for(k=new_from;k<root_id.size();k++)
			{
				assist_from.at(k)=OLD_SPLIT;
			}



		}
	}
	//.........................................



	





	//״̬�ϲ����,
	//������edge_table,root_id����DFAo״̬��

	


	vector<int> has_been_added(root_id.size(),-5);	//DFAo�еĽڵ���



	//12-4 ��������¼ӣ�û��original_state������)
	if(root_id.size()>0)
	{
		i=0;
		start_state=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),dfa_state.at(root_id.at(i)).accepting));	//temp
		has_been_added.at(i)=start_state;
	}
	//////////////////////////////


	int tmp_from,tmp_to;
	for(i=0;i<root_id.size();i++)
	{
		//for each DFAo node
		if(has_been_added.at(i)>=0)
		{
			tmp_from=has_been_added.at(i);
		}
		else
		{
			tmp_from=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),dfa_state.at(root_id.at(i)).accepting));
			has_been_added.at(i)=tmp_from;
		}
		for(j=0;j<num_vt;j++)
		{
			//for each vt(Edge)
			if(edge_table.at(j).at(root_id.at(i))==-1)
			{
				//�ռ����Ʊ�
				continue;
			}
			else
			{
				//12-10 edge_table.at(j).at(root_id.at(i)) ���ص���optimizeǰ��DFA�ڵ���
				//temp=DFAo�Ľڵ���
				//12-10temp��root_id �� ֵ����edge_table.at(j).at(root_id.at(i))������±꣡������
				temp=ufset.Find(edge_table.at(j).at(root_id.at(i)));
				for(k=0;k<root_id.size();k++)
				{
					if(root_id.at(k)==temp)
					{
						break;
					}
				}
				assert(k<root_id.size());
				temp=k;
				//temp=
				if(has_been_added.at(temp)<0)
				{
					//��I״̬��δ���ֹ�
					//����ͼ����Ӹ�״̬��Ӧ�Ľڵ�
					tmp_to=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),dfa_state.at(root_id.at(temp)).accepting));//12-4 ����accepting����
					has_been_added.at(temp)=tmp_to;
				}
				else
				{
					tmp_to=has_been_added.at(temp);
				}
				//from: ��ǰ������״̬��iָʾ��
				//to: ��һ��Ԫ����ָ״̬������Ϊ�±꣩

				//12-11:���⣺SparseGraph��AddEdge��֤��ͼ��from��to��ֻһ����
				//�������FiniteAutomata��дAddEdge��
				AddEdge(FA_GraphEdge(tmp_from ,  tmp_to ,  pVTManager->GetVTAt(j) )  );



				//AddEdge(FA_GraphEdge(root_id.at(i),edge_table.at(j).at(root_id.at(i)),pVTManager->GetVTAt(j)));
			}

			


		}

	}




}





void FiniteAutomata::ConstructDFA(string re)
{
	//Algorithm 3.36 p179
	//Directly convert a RE into a DFA
}



void FiniteAutomata::Print()
{
	//node->edgelist

	/*
	ConstNodeIterator ni(*this);
	ConstEdgeIterator *ei;

	for(;!ni.end();ni.next())
	{
		ni.
	}
	*/
	cout<<"\nFiniteAutomata:\n";

	cout<<"start_state:"<<start_state<<'\n';

	EdgeList::iterator iter;
	//string endstr;

	int i;
	int temp;
	for(i=0;i<m_Nodes.size();i++)
	{
		cout<<'\n'<<m_Nodes.at(i).Index();
		temp=m_Nodes.at(i).GetAcceptingNode();
		if(temp!=NON_ACCEPTING)
		{
			//temp
			cout<<" "<<temp<<'\t';
		}
		else
		{
			cout<<'\t';
		}
		//cout<<'\n'<<m_Nodes.at(i).Index()<<endstr<<'\t';
		//iter����ֻ�ܶ�һ��listʹ�ðɡ���
		//�����ƺ������
		for(iter=m_Edges.at(i).begin();iter!=m_Edges.at(i).end();++iter)
		{
			cout<<" -- ["<<iter->From()<<","<<iter->To()<<","<<iter->getVT()<<"]";
		}
	}

	cout<<"\n\n#end\n";

}











void FiniteAutomata::e_closure(SetOfState& closure, SetOfState& core,FiniteAutomata& nfa)
{
	//12-4 ����������
	//ֻ����һ��epsilon�߾�ֹͣ�ˣ��ұհ���Ӧ���Ǹ�������
	//��һ��ջ��ʾ��δ��ʼ̽���Ľڵ�

	//e-closure( core )

	/*12-4 ע�� annotate
	closure=core;	//�������
	Move(closure,core,-1,nfa);	//epsilon
	*/
	char epsilon=pVTManager->GetEpsilon();

	queue<int> nodes;

	vector<bool> assist(nfa.NumNodes(),false);		//�����ж�״̬�ߵ���û


	list<int>::iterator iter0;
	for(iter0=core.set.begin();iter0!=core.set.end();++iter0)
	{
		assist.at(*iter0)=true;
		nodes.push(*iter0);
	}

	
	EdgeList* from_edge_list;//=nfa.m_Edges.at(from);

	EdgeList::iterator iter;

	//12-5
	int accepting_token=core.accepting;	//12-8 ����
	int temp;
	//bool accepting=false;		//�ж�I�Ƿ�Ϊ����״̬
	
	
	int to;

	//int j;
	//bool assist_exist;

	while(!nodes.empty())
	{
		from_edge_list=&nfa.m_Edges.at(nodes.front());
		for(iter=from_edge_list->begin();iter!=from_edge_list->end();++iter)
		{
			if( iter->getVT()==epsilon )
			{
				to=iter->To();

				//12-4 Ӧ����closure �� nodes �ﶼ������
				//��assist�ռ任ʱ���޸�,���޸�
				//��nodes ���񲻺÷��ʰ�
				//�����رߣ��˴�Ҳ���ظ���
				
				//if(!closure.Exist(to))
				if(!assist.at(to))
				{
					assist.at(to)=true;
					nodes.push(to);
					//12-5 accepting problem todo temp
					temp=nfa.GetNode(to).GetAcceptingNode();
					if(temp!=NON_ACCEPTING)
					{
						
						
						

						if(accepting_token==NON_ACCEPTING || accepting_token==temp)
						{
							accepting_token=temp;
						}
						else
						{
							//ͬһ״̬��Ӧ��������TOKEN
							//�ж����� ��if ���п�����if,Ҳ�����Ǳ���
							//��һ������acceptingֵ�����Ϊ���ȼ���
							//assert(0);
							accepting_token=max(temp,accepting_token);
						}
					}
					//accepting=accepting || nfa.GetNode(to).GetAcceptingNode();	//ֻҪ����accepting state�ͽ�accepting��Ϊtrue
				}
				

				
				

				//��ʵ��֮ǰ�ṩ�Ĺ���NFA�ķ������ƺ�ֻ��epsilon���ж����ߡ�����
			}
		}
		closure.AddState(nodes.front());
		nodes.pop();
	}
	from_edge_list=NULL;
	closure.accepting=accepting_token;
}


void FiniteAutomata::Move(SetOfState &r,SetOfState& T,int vt_idx,FiniteAutomata& nfa)
{
	//complete Move(T,vt) of a block of the table
	//all the vt
	list<int>::iterator iter;
	//int num_vt=pVTManager->GetSize();

	//12-5
	//bool accepting=false;
	int accepting_token=NON_ACCEPTING;

	int temp;

	char vt=pVTManager->GetVTAt(vt_idx);
	//int i;
	for(iter=T.set.begin();iter!=T.set.end();++iter)
	{
		//for each node in the set T get --vt--> {}
		//r is accumulating
		temp=GetMoveVT(r,(*iter),vt,nfa);
		if(temp!=NON_ACCEPTING)
		{
			/*
			//12-8 test
			if(temp==6)
			{
				cout<<nfa.GetNode(*iter).GetAcceptingNode()<<'\n';
			}
			*/

			if(accepting_token==NON_ACCEPTING || accepting_token==temp)
			{
				accepting_token=temp;
			}
			else
			{
				//ͬһ״̬��Ӧ��������TOKEN
				//�ж����� ��if ���п�����if,Ҳ�����Ǳ���
				//��һ������acceptingֵ�����Ϊ���ȼ���
				//assert(0);
				accepting_token=max(temp,accepting_token);
			}
		}
		
		//accepting=temp || accepting;
	}
	
	r.accepting=accepting_token;
	//r.accepting=accepting;

}






int FiniteAutomata::GetStartNode()
{
	return start_state;
}

int FiniteAutomata::GetMoveVT(SetOfState& r,int from,char vt,FiniteAutomata& nfa)
{
	//12-5
	//complete nodes through one vt in the block
	//return the accepting_token index


	/*
	//complete nodes through one vt in the block
	//return whether this new added nodes includes accepting one
	*/

	/*
	assert( (from < m_Nodes.size()) &&
          (from >=0)              &&
           m_Nodes[from].Index() != invalid_node_index &&
          "<SparseGraph::GetEdge>: invalid 'from' index");
	*/
	EdgeList& from_edge_list=nfa.m_Edges.at(from);

	EdgeList::iterator iter;

	//bool accepting=false;		//�ж�I�Ƿ�Ϊ����״̬
	int accepting_token=NON_ACCEPTING;	//����token���
	int temp;
	int to;

	for(iter=from_edge_list.begin();iter!=from_edge_list.end();++iter)
	{
		if( iter->getVT()==vt )
		{
			to=iter->To();
			//12-5 accepting problem temp
			temp=nfa.GetNode(to).GetAcceptingNode();
			if(temp!=NON_ACCEPTING)
			{
				/*
				//12-8 test
				if(temp==6)
				{
					cout<<nfa.GetNode(to).GetAcceptingNode()<<'\n';
				}
				*/

				if(accepting_token==NON_ACCEPTING || accepting_token==temp)
				{
					accepting_token=temp;
				}
				else
				{
					//ͬһ״̬��Ӧ��������TOKEN
					//�ж����� ��if ���п�����if,Ҳ�����Ǳ���
					//��һ������acceptingֵ�����Ϊ���ȼ���
					//assert(0);
					accepting_token=max(temp,accepting_token);
				}
			}
			//accepting=accepting || nfa.GetNode(to).GetAcceptingNode();	//ֻҪ����accepting state�ͽ�accepting��Ϊtrue
			r.set.push_back(to);

			//��ʵ��֮ǰ�ṩ�Ĺ���NFA�ķ������ƺ�ֻ��epsilon���ж����ߡ�����
		}
	}

	return accepting_token;
}



