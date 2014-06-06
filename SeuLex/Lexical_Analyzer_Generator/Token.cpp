#include "stdafx.h"
#include "Token.h"


Token::Token(void)
{
}

Token::Token(string _name,int _value,bool _vn,bool _w,bool _a)
{
	name=_name;
	value=_value;
	tokenvn=_vn;
	whitevn=_w;
	addressvn=_a;
}

Token::~Token(void)
{
}
