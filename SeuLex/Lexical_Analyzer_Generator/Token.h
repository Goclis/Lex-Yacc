#pragma once
class Token
{
public:
	string name;
	int value;	//?
	bool tokenvn;		//是否可归约的终结符
	bool whitevn;	//是否是空白符型
	bool addressvn;	//是否要内码(变量，值型)

	int address_token_id;	//内码型token idx

	Token(void);
	Token(string,int,bool,bool,bool);
	~Token(void);
};

