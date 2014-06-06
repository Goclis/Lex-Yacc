#pragma once

#define PostfixConvertor_SIGN 20	//起始终止符
//#define PostfixConvertor_SIGN '#'	//起始终止符

#define pPostfixConvertor PostfixConvertor::Instance()

//12-6 todo: 引入@13@的处理
//		char a=1 用笑脸符号来？这个应该用不着。。

class PostfixConvertor
{
	//used for converting infix to postfix string
	//试着写一个可以重用的代码
protected:
	vector<char> operators;		//操作符组
	vector<int> icp;			//栈内优先级
	vector<int> isp;			//栈外优先级

	bool IsOP(char ch);
	int Index(char ch);

	PostfixConvertor(void);
	~PostfixConvertor(void);

public:
	

	void InfixToPostFix(const string & infix, string & postfix);

	static PostfixConvertor* Instance();

};

