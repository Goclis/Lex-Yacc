#include "stdafx.h"
#include "PostfixConvertor.h"


PostfixConvertor::PostfixConvertor(void):operators(1,PostfixConvertor_SIGN),icp(1,0),isp(1,0)
{
	//static init
	/*
		������ch		#	(	.	|	*	)
		ջ�����ȼ�isp	0	1	7	5	9	99
		ջ�����ȼ�icp	0	99	6	4	8	1
	*/
	operators.push_back(VT_LEFT_BRACKET);
	operators.push_back(VT_CONCATENATION);
	operators.push_back(VT_ALTERNATION);
	operators.push_back(VT_EPSILON);
	operators.push_back(VT_RIGHT_BRACKET);

	isp.push_back(1);
	isp.push_back(7);
	isp.push_back(5);
	isp.push_back(9);
	isp.push_back(99);

	icp.push_back(99);
	icp.push_back(6);
	icp.push_back(4);
	icp.push_back(8);
	icp.push_back(1);
}


PostfixConvertor::~PostfixConvertor(void)
{
}


bool PostfixConvertor::IsOP(char ch)
{
	vector<char>::iterator iter;
	for(iter=operators.begin();iter!=operators.end();++iter)
	{
		if( ch==(*iter) )
		{
			return true;
		}
	}
	return false;
}

int PostfixConvertor::Index(char ch)
{
	int i;
	for(i=0;i<operators.size();i++)
	{
		if( ch==operators.at(i) )
		{
			return i;
		}
	}
	return -1;	//ch is not a valid operator
}



void PostfixConvertor::InfixToPostFix(const string & infix,string & postfix)
{
	int i;
	char now;
	int current;
	int temp;
	int op;		//����������idx
	stack<int> s;		//����ջ����vector�е�idx��

	//string p;
	//Formula f;
	
	temp=0;
	s.push(temp);             //ѹ����ֹ��
	i=0;

	int infix_length=(int)infix.length();
	
	while(!s.empty())
	{
		if(i<infix_length)
		{
			now=infix.at(i);
			current=Index(now);
			if(current==-1)
			{
				//��VT
				postfix=postfix+now;
				
			}
			else
			{
				//�ǲ�����
				temp=s.top();
				if(isp.at(temp)<icp.at(current))
				{
					s.push(current);
				}
				else if(isp.at(temp)>icp.at(current))
				{
					op=s.top();
					postfix=postfix+operators.at(op);
					s.pop();
					continue;
				}
				else
				{
					//���ȼ����#,()  (ʵ����û��#)
					s.pop();
					
				}
			}
			i++;
		}
		else
		{
			//�ַ��Ѿ����꣬��ջ��û��
			temp=s.top();
			if(temp!=0)
			{
				postfix=postfix+operators.at(temp);
			}
			s.pop();
		}


	}

}


PostfixConvertor* PostfixConvertor::Instance()
{
	static PostfixConvertor instance;
	return &instance;
}