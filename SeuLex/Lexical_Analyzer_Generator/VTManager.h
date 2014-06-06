#pragma once

//singleton
#define pVTManager VTManager::Instance()


//某些保留字符的宏定义
//字符值定义在了ASCII码较小部分
#define VT_EPSILON 1
#define VT_CONCATENATION 2
#define VT_ALTERNATION 3
#define VT_CLOSURE 4
#define VT_VNFLAG 5

#define VT_LEFT_BRACKET 11
#define VT_RIGHT_BRACKET 12






//store all the VT that is valid
//according to the input REs/RDs


//终极符管理器
class VTManager
{
protected:
	vector<char> VT;

	VTManager(void);
	~VTManager(void);
public:
	int GetSize(){return VT.size();}
	char GetVTAt(int idx)
	{
			switch(idx)
			{
			case -1: return GetEpsilon();	//epsilon
			case -2: return VT_CONCATENATION;	//concatenation
			case -3: return VT_ALTERNATION;	//alternation
			case -4: return VT_CLOSURE;	//closure
			case -5: return VT_VNFLAG;	//@13@ 包围着非终极符index
			}
		return VT.at(idx);
	}
	char GetEpsilon()
	{
		return VT_EPSILON;
	}

	void Push(char vt){VT.push_back(vt);}

	static VTManager* Instance();
};

