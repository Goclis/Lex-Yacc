#pragma once
class Token
{
public:
	string name;
	int value;	//?
	bool tokenvn;		//�Ƿ�ɹ�Լ���ս��
	bool whitevn;	//�Ƿ��ǿհ׷���
	bool addressvn;	//�Ƿ�Ҫ����(������ֵ��)

	int address_token_id;	//������token idx

	Token(void);
	Token(string,int,bool,bool,bool);
	~Token(void);
};

