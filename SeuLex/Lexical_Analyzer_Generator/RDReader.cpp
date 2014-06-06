#include "stdafx.h"
#include "RDReader.h"

RDReader::RDReader(void)
{
	global_dfa.SetFAType(DFA);
}


RDReader::~RDReader(void)
{
}

RDReader* RDReader::Instance()
{
	static RDReader instance;
	return &instance;
}

char RDReader::EscMeaning(char c)
{
	//temp
	//after \.
	switch(c)
	{
	case 't':return '\t';
	case 'n':return '\n';
	case 's':return ' ';	//自己加的空格转义
	case 'r':return '\r';
	}
	return c;
}

char RDReader::SpecialMeaning(char c)
{
	//temp
	//after \.
	switch(c)
	{
	case RD_CONCATENATION:return VT_CONCATENATION;
	case RD_ALTERNATION:return VT_ALTERNATION;
	case RD_CLOSURE:return VT_CLOSURE;
	case RD_VNFLAG:return VT_VNFLAG;
	case RD_LEFT_BRACKET:return VT_LEFT_BRACKET;
	case RD_RIGHT_BRACKET:return VT_RIGHT_BRACKET;
	}
	return c;
}



void RDReader::ReadRDFile(string& filename)
{
	//最后的global dfa和之前的组合方式在终止状态有有别。它的终止状态是多个

	ifstream in;
	in.open(filename);
	assert(!in.fail() && "open RD file failed!");



	int part=0;		//在第几部分
					//0-programtype
					//1-all VT
					//2-RD

	char c;

	cout<<"读取源程序格式..\n\n";
	{
		//0-programtype
		string programtype;
		in>>programtype;
		pCodeGenerator->SetProgramFileType(programtype);
		//part=1;
	}

	in>>c;
	assert(c==RD_SPLIT);


	//1-all valid VT
	cout<<"读取合法终极符..\n\n";
	in>>c;
	while(c!=RD_SPLIT)
	{
		//1-all valid VT
		//此部分没有特殊含义的东西
		if(c==RD_ESC)
		{
			//转义
			in>>c;
			c=EscMeaning(c);
		}
		pVTManager->Push(c);
		in>>c;
	}
	//c==%


	//2-rd
	//2014-5-6 可以加入[0-9]这样的语义，其实很简单

	cout<<"读取正则定义..\n\n";
	vector<int> tokenvn_id;
	char tmp_char=VT_VNFLAG;
	bool tokenvn,whitevn,addressvn;
	int i,j,tmp_size;
	string vn_name,re,reo,reop,temp;
	in>>vn_name;
	while(vn_name!=RD_SPLIT_STR)
	{
		if(vn_name==RD_TOKENVN_STR)
		{
			//token
			tokenvn=true;
			whitevn=false;
			addressvn=false;
			tokenvn_id.push_back(pVNManager->GetSize());	//the id that this vn will take in the future
															//代码巨渣
			in>>vn_name;
		}
		else if(vn_name==RD_VARIABLE_STR)
		{
			//需要有内码的token
			tokenvn=true;
			whitevn=false;
			addressvn=true;
			tokenvn_id.push_back(pVNManager->GetSize());

			in>>vn_name;

		}
		else if(vn_name==RD_WHITEVN_STR)
		{
			//空白不输出token
			tokenvn=true;
			whitevn=true;
			addressvn=false;
			tokenvn_id.push_back(pVNManager->GetSize());	//the id that this vn will take in the future
															//代码巨渣
			in>>vn_name;
		}
		else
		{
			tokenvn=false;
			whitevn=false;
			addressvn=false;
		}

		re.clear();
		reo.clear();
		in>>re;

		//处理re的@mmm@变成vn_idx
		i=0;
		tmp_size=re.size();
		while(i<tmp_size)
		{
			if(re.at(i)==RD_VNFLAG)
			{
				i++;
				temp.clear();
				while(re.at(i)!=RD_VNFLAG)
				{
					temp.push_back(re.at(i));
					i++;
				}

				//search for the vn named temp
				for(j=0;j<pVNManager->GetSize();j++)
				{
					if(pVNManager->GetTokenNameAt(j)==temp)
					{
						//convert the name to index
						break;
					}
				}

				assert(j<pVNManager->GetSize());	
				//if not, there's no VN whose name is temp

				{
					char buf[10];
					sprintf(buf, "%d",j);
					temp=buf;
				}
				reo+=tmp_char+temp+tmp_char;
				
			}
			else
			{
				if(re.at(i)==RD_ESC)
				{
					i++;
					reo.push_back(EscMeaning(re.at(i)));
				}
				else
				{
					reo.push_back(SpecialMeaning(re.at(i)));
				}
			}
			i++;
		}
		reop.clear();
		pPostfixConvertor->InfixToPostFix(reo,reop);	//12-7 漏了用SpecialMeaning()
		
		pVNManager->Push(Token(vn_name,pVNManager->GetSize(),tokenvn,whitevn,addressvn),reop);
		


		vn_name.clear();
		in>>vn_name;
	}

	//%has been eaten

	//it is over

	/*
	//12-8 test
	for(i=0;i<pVNManager->GetSize();i++)
	{
		pVNManager->GetFAAt(i).Print();
	}
	*/


	cout<<"构建全局状态机..\n\n";

	FiniteAutomata tmp_nfa;
	tmp_nfa.AddOrignalStartNode();
	int tmp_start,tmp_end;
	//由是token 的vn组成global dfa
	for(i=0;i<tokenvn_id.size();i++)
	{
		//pVNManager->GetFAAt(tokenvn_id.at(i)).Print();	//test

		//12-8 其实这里可以加DFA，效率会高吗？
		//感觉好像tmp_nfa的生成没问题？最后转DFA有问题

		//12-8 发现@3@ (int) nfa 生成有问题，不可能是epsilon,
		//但从起始状态可以通过epsilon边直达终止状态
		//已解决


		//12-8 尝试直接加DFA
		//粗测一下好像也可以。。，但速度什么的不怎么快
		/*
		FiniteAutomata tmp_small_dfa;
		tmp_small_dfa.ConstructDFA(pVNManager->GetFAAt(tokenvn_id.at(i)));
		tmp_nfa.AddNFA(tmp_small_dfa,tmp_start,tmp_end);
		*/
		
		tmp_nfa.AddNFA(pVNManager->GetFAAt(tokenvn_id.at(i)),tmp_start,tmp_end);
		tmp_nfa.AddEdge(FA_GraphEdge(tmp_nfa.GetStartNode(),tmp_start,VT_EPSILON));
	}
	//tmp_nfa.Print();	//test
	
	//global_dfa.ConstructDFA(tmp_nfa);
	global_dfa.ConstructOptimizedDFA(tmp_nfa);



	in.close();

}