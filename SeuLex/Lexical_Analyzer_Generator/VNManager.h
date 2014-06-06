#pragma once

//singleton
#define pVNManager VNManager::Instance()


//store all the VN that is valid
//according to the input RDs

//非终极符管理器
class VNManager
{
	//12-6 VN还分为token和不是token的，不是token的（比如letter_)的accepting 最好是负数。。
	//还得再处理一步
protected:
	vector<Token> VN;	//12-5 this should be token
	vector<string> RD;	//regular definition
	
	//vector<FiniteAutomata*> vn_nfa;
	vector<FiniteAutomata> vn_nfa;


	int num_var_tokens;		//需要内码的token数量

	VNManager(void);
	~VNManager(void);
public:
	int GetSize(){return VN.size();}
	const Token& GetVNAt(int idx)
	{
		return VN.at(idx);
	}

	string& GetTokenNameAt(int idx)
	{
		return VN.at(idx).name;
	}

	FiniteAutomata& GetFAAt(int idx)
	{
		return vn_nfa.at(idx);
	}

	int GetVarTokenNum()
	{
		return num_var_tokens;
	}

	//temp
	void Push(Token & vt,string & rd)
	{
		if(vt.addressvn)
		{
			vt.address_token_id=num_var_tokens;
			num_var_tokens++;
		}

		VN.push_back(vt);


		RD.push_back(rd);
		
		//vn_nfa.push_back(new FiniteAutomata());
		//12-6 warning 这里pushback 用了复制构造函数。。把你那个蹩脚的nfa构造dfa删了。。。

		//12-9 空白符的加入
		int temp=vt.value;
		if(vt.whitevn)
		{
			temp=DEFAULT_END;
		}

		vn_nfa.push_back(FiniteAutomata(rd,temp));//12-7 此处改为了Token.value?
														//原为VN.size()-1
	}

	//为下标为idx的Token，根据RD构建NFA
	bool ConstructNFA_at(int idx)
	{
		return vn_nfa.at(idx).ConstructNFA(RD.at(idx),VN.at(idx).value);
	}


	//输出终结符表，供syntax analyzer使用
	void OutputTokenList(string filename);



	static VNManager* Instance();
};

