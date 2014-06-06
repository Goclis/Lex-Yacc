#include "stdafx.h"
#include "PostfixConvertor.h"


PostfixConvertor::PostfixConvertor(void):operators(1,PostfixConvertor_SIGN),icp(1,0),isp(1,0)
{
	//static init
	/*
		操作符ch		#	(	.	|	*	)
		栈内优先级isp	0	1	7	5	9	99
		栈外优先级icp	0	99	6	4	8	1
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
	int op;		//弹出操作符idx
	stack<int> s;		//符号栈（在vector中的idx）

	//string p;
	//Formula f;
	
	temp=0;
	s.push(temp);             //压入起止符
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
				//是VT
				postfix=postfix+now;
				
			}
			else
			{
				//是操作符
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
					//优先级相等#,()  (实际上没有#)
					s.pop();
					
				}
			}
			i++;
		}
		else
		{
			//字符已经读完，但栈还没空
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