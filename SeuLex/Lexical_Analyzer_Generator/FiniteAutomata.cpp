#include "stdafx.h"
//#include <cassert>
//#include "FiniteAutomata.h"


FiniteAutomata::FiniteAutomata(void):SparseGraph<FA_GraphNode,FA_GraphEdge>(true)
{
	//default setting
	fa_type=NFA;	//这里有问题啊
	//AddOrignalStartNode();	//12-5 恢复，NFA用一个默认开始节点
}


FiniteAutomata::FiniteAutomata(string postfix_re,int vn_idx):SparseGraph<FA_GraphNode,FA_GraphEdge>(true)
{
	//构造小nfa自动机

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
	//12-5 postfix_re引入非终极符后 要处理
	//引入非终极符@12@以后postfix不好处理了。。。

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

			//12-6 啥时候有空拿string,itoa重写一下
			queue<char> vn_string;
			int nlength=-1;	//@..@中数字的位数

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
				//这里先不做错误处理了
				link_vn_idx+=pow(10,nlength)*((int)vn_string.front()-48);
				vn_string.pop();
			}


			//12-8 tmp start 接的状态有些问题
			AddNFA(pVNManager->GetFAAt(link_vn_idx),tmp_start,tmp_end);

			//tmp_start,tmp_end是新加的小NFA的起止点idx
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


	if( NFAs.size()==1 && i==size )	//栈只剩最后添加的那个nfa，并且，串刚好读完，构建成功
	{	
		//12-7 如果s->@a@,只有一个非终极符，accepting node 会有问题啊。。
		GetNode(NFAs.top().accepting_node).SetAcceptingNode(vn_idx);

		if(origin_empty)
		{
			//12-4这个应该判断原来有没有点，其实用一个original还是最好的
			start_state=this_start_state;
		}
		else
		{
			//原来不空（可能只是original state，也可能是原有某个nfa）
			//valid RE
			//connect the NFA generated by this RE to the global start state
			AddEdge(FA_GraphEdge(start_state,this_start_state,epsilon));
			//12-5注释了下面这句，因为original startnode不再标结束状态了
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

	//遍历方法加入点和边
	//新加的东西的节点的标号要统一偏移
	
	//12-8 为了兼容一下加dfa
	//assert(nfa.fa_type==NFA);	//这里的protected能访问？

	//鉴于全过程没有删除节点的操作，可用节点编号应该是稠密的

	int offset=GetNextFreeNodeIndex();

	//12-6突然发现好像protected可以实现同一个类不同实例间的访问？
	//NodeVector::iterator ni;
	int ni;		//node,EdgeListVector遍历的下标
	//EdgeListVector::iterator evi;
	EdgeList::iterator ei;

	start=nfa.GetStartNode()+offset;	//12-8 修正+offset

	int end_node_idx=nfa.m_Nodes.size();
	int current,tmp_acpt;
	for(ni=0;ni<end_node_idx;ni++)
	{
		//一点也不健壮..
		
		tmp_acpt=nfa.m_Nodes.at(ni).GetAcceptingNode();
		current=AddNode(FA_GraphNode(nfa.m_Nodes.at(ni).Index()+offset
					,tmp_acpt));
		
		if(tmp_acpt!=NON_ACCEPTING)
		{
			end=current;
		}
		/*
		//12-6 此处要先把Node全加完才行。SparseGraph有较强的保护机制
		for(ei=nfa.m_Edges.at(ni).begin();ei!=nfa.m_Edges.at(ni).end();++ei)
		{
			AddEdge(FA_GraphEdge(current,ei->To()+offset,ei->getVT()));
		}
		*/
	}

	for(ni=0;ni<end_node_idx;ni++)
	{
		//12-6 此处要先把Node全加完才行。SparseGraph有较强的保护机制

		for(ei=nfa.m_Edges.at(ni).begin();ei!=nfa.m_Edges.at(ni).end();++ei)
		{
			AddEdge(FA_GraphEdge(ei->From()+offset,ei->To()+offset,ei->getVT()));
		}
	}

}




//void FiniteAutomata::ConstructDFA(FiniteAutomata& nfa,bool global_accpeting_state)
void FiniteAutomata::ConstructDFA(FiniteAutomata& nfa)
{
	//12-5不行啊，dfa里不能有epsilon边啊。。。
	//12-5 bool global_accpeting_state,是否添加一个global 终止状态
	//只在分立小dfa才可以设为true

	//12-5 考虑所有终止状态在引一条边到一个公共终止状态？ 方便NFA_BLOCK
	//小dfa只是返回一种token,只有global dfa有多个不同终止状态

	//generate DFA according to the given NFA
	fa_type=DFA;


	//##########  preperation  #######################

	//set are be represented by set vector<bool> simply
	int num_states=nfa.NumNodes();	//num of nfa states
	int num_vt=pVTManager->GetSize();

	vector<SetOfState> core(0,SetOfState());	//e-closure( core )

	SetOfState tmp_corestart(0);
	tmp_corestart.AddState(nfa.GetStartNode());		//12-4修正
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
	//dfa_state_i同时起一个行的作用
	//bool new_dfa_state=true;


	bool exist=false;	
	//bool *bvt;	//比较集合是否相等用
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
				//是空集
				//这一格肯定是空的，直接看下一格
				//12-4 空集在edge_table里怎么表示？（哑边）
				core.pop_back();
				core_i--;
				edge_table.at(column_vt).push_back(-1);
				continue;
			}



			dfa_state.push_back(SetOfState());
			e_closure(dfa_state.back(),core.at(core_i),nfa);

			//TODO:	这时判断新的dfa_state与之前的有没有重复
			//	(这个判断怎么降低时间复杂性?)
			//		重复：pop(); new_dfa_state=false;
			//		新的: 链上edge_table; new_dfa_state=true;
			

			//check whether this
			exist=false;
			for(i=0;i<dfa_state.size()-1;i++)
			{
				if(  SetOfState::Equal(dfa_state.at(i),dfa_state.back(),num_states)    )
				//if(  dfa_state.at(i).Equal(dfa_state.at(i),dfa_state.back())    )
				{
					//有相等的dfa_state，不是新的state
					exist=true;
					dfa_state.pop_back();

					//链上对应的state
					//edge_table.at(column_vt).at(i)=i;
					edge_table.at(column_vt).push_back(i);
					break;
				}
			}

			if(!exist)
			{
				//生成了新的state
				//12-4 此处溢出鸟？做出了一个修正
				//edge_table.at(column_vt).at(dfa_state_i)=dfa_state.size()-1;	
				edge_table.at(column_vt).push_back(dfa_state.size()-1);
			}


		}

		//下一行
		dfa_state_i++;
	}

	//delete bvt;
	//**************************************************************



	//表格构建好了，接下来是构建DFA了
	//初始的开始节点是有的，作为I0

	//i:当前正在操作的dfa_state (指向table) , j:当前操作的VT
	dfa_state_i=0;	//当前已添加的节点的状态的最后一个(指向vector)
	vector<int> id_node(dfa_state.size(),0);	//状态对应在图中的节点编号
	

	//12-4 下面这段新加（没有original_state的情形)
	if(dfa_state.size()>0)
	{
		i=0;
		start_state=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),dfa_state.at(i).accepting));	//temp
	}
	//////////////////////////////

	id_node.at(0)=GetStartNode();

	/*
	//这个想法不行。。
	//12-5 global_accpeting_state的处理
	int global_end_idx=-1;	//全局终止节点index
	if(global_accpeting_state)
	{
		global_end_idx=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),NON_ACCEPTING));
	}
	*/

	for(i=0;i<dfa_state.size();i++)
	{
		//遍历所有dfa状态

		for(j=0;j<num_vt;j++)
		{
			//j指示vt的编号
			if(edge_table.at(j).at(i)==-1)
			{
				//空集，哑边
				continue;
			}

			if(edge_table.at(j).at(i)>dfa_state_i)		//此处讨个巧，状态的生成顺序与dfa_state是线性一致的
			{
				//该I状态还未出现过
				//先在图中添加该状态对应的节点
				id_node.at(++dfa_state_i)=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),dfa_state.at(edge_table.at(j).at(i)).accepting));//12-4 修正accepting错误
			}
			//from: 当前操作的状态（i指示）
			//to: 这一单元格所指状态（内容为下标）
			AddEdge(FA_GraphEdge(id_node.at(i) ,  id_node.at(edge_table.at(j).at(i)) ,  pVTManager->GetVTAt(j) )  );
		}

	}





}







void FiniteAutomata::ConstructOptimizedDFA(FiniteAutomata& nfa)
{
	//TODO
	//O(nlogn)
	//集合划分的办法
	
	
	//从构建DFA那里粘来的。。
	//关键要edgetable
	//这样一来输入的就是nfa了其实
	fa_type=DFA;


	//##########  preperation  #######################

	//set are be represented by set vector<bool> simply
	int num_states=nfa.NumNodes();	//num of nfa states
	int num_vt=pVTManager->GetSize();

	vector<SetOfState> core(0,SetOfState());	//e-closure( core )

	SetOfState tmp_corestart(0);
	tmp_corestart.AddState(nfa.GetStartNode());		//12-4修正
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
	//dfa_state_i同时起一个行的作用
	//bool new_dfa_state=true;


	bool exist=false;	
	//bool *bvt;	//比较集合是否相等用
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
				//是空集
				//这一格肯定是空的，直接看下一格
				//12-4 空集在edge_table里怎么表示？（哑边）
				core.pop_back();
				core_i--;
				edge_table.at(column_vt).push_back(-1);
				continue;
			}



			dfa_state.push_back(SetOfState());
			e_closure(dfa_state.back(),core.at(core_i),nfa);

			//TODO:	这时判断新的dfa_state与之前的有没有重复
			//	(这个判断怎么降低时间复杂性?)
			//		重复：pop(); new_dfa_state=false;
			//		新的: 链上edge_table; new_dfa_state=true;
			

			//check whether this
			exist=false;
			for(i=0;i<dfa_state.size()-1;i++)
			{
				if(  SetOfState::Equal(dfa_state.at(i),dfa_state.back(),num_states)    )
				//if(  dfa_state.at(i).Equal(dfa_state.at(i),dfa_state.back())    )
				{
					//有相等的dfa_state，不是新的state
					exist=true;
					dfa_state.pop_back();

					//链上对应的state
					//edge_table.at(column_vt).at(i)=i;
					edge_table.at(column_vt).push_back(i);
					break;
				}
			}

			if(!exist)
			{
				//生成了新的state
				//12-4 此处溢出鸟？做出了一个修正
				//edge_table.at(column_vt).at(dfa_state_i)=dfa_state.size()-1;	
				edge_table.at(column_vt).push_back(dfa_state.size()-1);
			}


		}

		//下一行
		dfa_state_i++;
	}

	//delete bvt;
	//**************************************************************



	




	//test
	cout<<"最小化状态机状态数..\n\n";







	//根据edge_table构建DFAo

	//12-10 等价类并查集啊
	//似乎我需要DFA里的edge_table
	int num_nodes=dfa_state.size();

	int OLD_SPLIT=-num_nodes-10;	//早已分离的等价类标记


	UFSets ufset(num_nodes);		//树的方式存储的并查集



	
	vector<int> root_id;	//各集合的根节点index
	vector<int> assist_from;		//辅助用，一一对应root_id
							//当前vt轮存储之前的Find()
							//进入下一vt之后改成-1

	if(num_nodes>0)
	{
		root_id.push_back(0);
		assist_from.push_back(OLD_SPLIT);
	}
	

	
	//init
	//以accepting_node为依据构建一个等价类划分
	int tmp_acpt;
	bool isnew;//是否新acpt
	for(i=1;i<num_nodes;i++)
	{
		isnew=true;
		tmp_acpt=dfa_state.at(i).accepting;
		
		//遍历当前等价类
		for(j=0;j<root_id.size();j++)
		{
			if(tmp_acpt==dfa_state.at(root_id.at(j)).accepting)
			{
				//此acpt状态已有根节点，指向它
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
	//此时确保树的深度都只有2
	//**********************************************


	




	//下面主要循环，合并状态
	//for vt a, if from and to both equal,union them

	//?partition, not union

	int temp;
	bool change=true;	//是否有新的划分
	int new_from;


	while(change)//temp
	{
		change=false;
		for(i=0;i<num_vt;i++)
		{
			//for each vt

			new_from=root_id.size();	//本轮vt新分出来的从这开始

			for(j=0;j<num_nodes;j++)
			{
				//for each dfa_state

				




				//边to的类
				temp=ufset.Find(edge_table.at(i).at(j));
				if(temp!=ufset.Find(edge_table.at(i).at(ufset.Find(j))))
				{
					//注意-1哑边的情况

					//当前vt的边To的类不同
					//分离出这个等价类
					change=true;
					




					//看看有没有可以归属的等价类
					//12-10 ???此处严重存疑

					//12-10 问题是：这一轮（对vt而言）新分出来的要做标记，
					//标明他们分出来之前的Find()

					//还有一点，这样搞得确保树只有2层，一个根，其他全是连到根上的叶子

					
					isnew=true;
					for(k=new_from;k<root_id.size();k++)	
					{

						if(temp==edge_table.at(i).at(ufset.Find(root_id.at(k)))		//to的类相同
							&& ufset.Find(j)==assist_from.at(k)		//from的类相同
							&& ufset.Find(j)!=root_id.at(k) )		//不是之前split出来的类（这个冗余了）
						{
							//找到了归属
							isnew=false;
							ufset.Split(j);
							ufset.SimpleSimpleUnion(root_id.at(k),j);

							break;
						}
					}


					if(isnew)
					{
						//新的划分出来的等价类
						root_id.push_back(j);
						assist_from.push_back(ufset.Find(j));
						ufset.Split(j);	//一直要用到它的Find，所以到现在才分离
					}

					


					

				}

			}

			//本轮vt新分出的类的assist_from归为-1
			assert(root_id.size()==assist_from.size());

			for(k=new_from;k<root_id.size();k++)
			{
				assist_from.at(k)=OLD_SPLIT;
			}



		}
	}
	//.........................................



	





	//状态合并完成,
	//下面由edge_table,root_id构建DFAo状态机

	


	vector<int> has_been_added(root_id.size(),-5);	//DFAo中的节点编号



	//12-4 下面这段新加（没有original_state的情形)
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
				//空集，哑边
				continue;
			}
			else
			{
				//12-10 edge_table.at(j).at(root_id.at(i)) 返回的是optimize前的DFA节点编号
				//temp=DFAo的节点编号
				//12-10temp是root_id 里 值等于edge_table.at(j).at(root_id.at(i))的项的下标！！！！
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
					//该I状态还未出现过
					//先在图中添加该状态对应的节点
					tmp_to=AddNode(FA_GraphNode(GetNextFreeNodeIndex(),dfa_state.at(root_id.at(temp)).accepting));//12-4 修正accepting错误
					has_been_added.at(temp)=tmp_to;
				}
				else
				{
					tmp_to=has_been_added.at(temp);
				}
				//from: 当前操作的状态（i指示）
				//to: 这一单元格所指状态（内容为下标）

				//12-11:问题：SparseGraph的AddEdge保证简单图（from，to边只一条）
				//解决方案FiniteAutomata重写AddEdge类
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
		//iter好像只能对一个list使用吧。。
		//这样似乎会出错
		for(iter=m_Edges.at(i).begin();iter!=m_Edges.at(i).end();++iter)
		{
			cout<<" -- ["<<iter->From()<<","<<iter->To()<<","<<iter->getVT()<<"]";
		}
	}

	cout<<"\n\n#end\n";

}











void FiniteAutomata::e_closure(SetOfState& closure, SetOfState& core,FiniteAutomata& nfa)
{
	//12-4 这里有问题
	//只走了一条epsilon边就停止了，找闭包，应该是个遍历？
	//用一个栈表示还未起始探索的节点

	//e-closure( core )

	/*12-4 注释 annotate
	closure=core;	//是深复制吗
	Move(closure,core,-1,nfa);	//epsilon
	*/
	char epsilon=pVTManager->GetEpsilon();

	queue<int> nodes;

	vector<bool> assist(nfa.NumNodes(),false);		//辅助判断状态走到过没


	list<int>::iterator iter0;
	for(iter0=core.set.begin();iter0!=core.set.end();++iter0)
	{
		assist.at(*iter0)=true;
		nodes.push(*iter0);
	}

	
	EdgeList* from_edge_list;//=nfa.m_Edges.at(from);

	EdgeList::iterator iter;

	//12-5
	int accepting_token=core.accepting;	//12-8 修正
	int temp;
	//bool accepting=false;		//判断I是否为结束状态
	
	
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

				//12-4 应该是closure 和 nodes 里都不存在
				//用assist空间换时间修改,已修复
				//可nodes 好像不好访问啊
				//若有重边，此处也会重复加
				
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
							//同一状态对应至少两个TOKEN
							//有二义性 如if 既有可能是if,也可能是变量
							//做一个处理，accepting值大的视为优先级高
							//assert(0);
							accepting_token=max(temp,accepting_token);
						}
					}
					//accepting=accepting || nfa.GetNode(to).GetAcceptingNode();	//只要包含accepting state就将accepting设为true
				}
				

				
				

				//其实按之前提供的构建NFA的方法，似乎只有epsilon会有多条边。。。
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
				//同一状态对应至少两个TOKEN
				//有二义性 如if 既有可能是if,也可能是变量
				//做一个处理，accepting值大的视为优先级高
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

	//bool accepting=false;		//判断I是否为结束状态
	int accepting_token=NON_ACCEPTING;	//结束token序号
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
					//同一状态对应至少两个TOKEN
					//有二义性 如if 既有可能是if,也可能是变量
					//做一个处理，accepting值大的视为优先级高
					//assert(0);
					accepting_token=max(temp,accepting_token);
				}
			}
			//accepting=accepting || nfa.GetNode(to).GetAcceptingNode();	//只要包含accepting state就将accepting设为true
			r.set.push_back(to);

			//其实按之前提供的构建NFA的方法，似乎只有epsilon会有多条边。。。
		}
	}

	return accepting_token;
}



