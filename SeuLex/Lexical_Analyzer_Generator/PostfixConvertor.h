#pragma once

#define PostfixConvertor_SIGN 20	//��ʼ��ֹ��
//#define PostfixConvertor_SIGN '#'	//��ʼ��ֹ��

#define pPostfixConvertor PostfixConvertor::Instance()

//12-6 todo: ����@13@�Ĵ���
//		char a=1 ��Ц�������������Ӧ���ò��š���

class PostfixConvertor
{
	//used for converting infix to postfix string
	//����дһ���������õĴ���
protected:
	vector<char> operators;		//��������
	vector<int> icp;			//ջ�����ȼ�
	vector<int> isp;			//ջ�����ȼ�

	bool IsOP(char ch);
	int Index(char ch);

	PostfixConvertor(void);
	~PostfixConvertor(void);

public:
	

	void InfixToPostFix(const string & infix, string & postfix);

	static PostfixConvertor* Instance();

};

