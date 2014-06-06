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
	case 's':return ' ';	//�Լ��ӵĿո�ת��
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
	//����global dfa��֮ǰ����Ϸ�ʽ����ֹ״̬���б�������ֹ״̬�Ƕ��

	ifstream in;
	in.open(filename);
	assert(!in.fail() && "open RD file failed!");



	int part=0;		//�ڵڼ�����
					//0-programtype
					//1-all VT
					//2-RD

	char c;

	cout<<"��ȡԴ�����ʽ..\n\n";
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
	cout<<"��ȡ�Ϸ��ռ���..\n\n";
	in>>c;
	while(c!=RD_SPLIT)
	{
		//1-all valid VT
		//�˲���û�����⺬��Ķ���
		if(c==RD_ESC)
		{
			//ת��
			in>>c;
			c=EscMeaning(c);
		}
		pVTManager->Push(c);
		in>>c;
	}
	//c==%


	//2-rd
	//2014-5-6 ���Լ���[0-9]���������壬��ʵ�ܼ�

	cout<<"��ȡ������..\n\n";
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
															//�������
			in>>vn_name;
		}
		else if(vn_name==RD_VARIABLE_STR)
		{
			//��Ҫ�������token
			tokenvn=true;
			whitevn=false;
			addressvn=true;
			tokenvn_id.push_back(pVNManager->GetSize());

			in>>vn_name;

		}
		else if(vn_name==RD_WHITEVN_STR)
		{
			//�հײ����token
			tokenvn=true;
			whitevn=true;
			addressvn=false;
			tokenvn_id.push_back(pVNManager->GetSize());	//the id that this vn will take in the future
															//�������
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

		//����re��@mmm@���vn_idx
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
		pPostfixConvertor->InfixToPostFix(reo,reop);	//12-7 ©����SpecialMeaning()
		
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


	cout<<"����ȫ��״̬��..\n\n";

	FiniteAutomata tmp_nfa;
	tmp_nfa.AddOrignalStartNode();
	int tmp_start,tmp_end;
	//����token ��vn���global dfa
	for(i=0;i<tokenvn_id.size();i++)
	{
		//pVNManager->GetFAAt(tokenvn_id.at(i)).Print();	//test

		//12-8 ��ʵ������Լ�DFA��Ч�ʻ����
		//�о�����tmp_nfa������û���⣿���תDFA������

		//12-8 ����@3@ (int) nfa ���������⣬��������epsilon,
		//������ʼ״̬����ͨ��epsilon��ֱ����ֹ״̬
		//�ѽ��


		//12-8 ����ֱ�Ӽ�DFA
		//�ֲ�һ�º���Ҳ���ԡ��������ٶ�ʲô�Ĳ���ô��
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