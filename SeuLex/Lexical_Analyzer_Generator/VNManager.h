#pragma once

//singleton
#define pVNManager VNManager::Instance()


//store all the VN that is valid
//according to the input RDs

//���ռ���������
class VNManager
{
	//12-6 VN����Ϊtoken�Ͳ���token�ģ�����token�ģ�����letter_)��accepting ����Ǹ�������
	//�����ٴ���һ��
protected:
	vector<Token> VN;	//12-5 this should be token
	vector<string> RD;	//regular definition
	
	//vector<FiniteAutomata*> vn_nfa;
	vector<FiniteAutomata> vn_nfa;


	int num_var_tokens;		//��Ҫ�����token����

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
		//12-6 warning ����pushback ���˸��ƹ��캯�����������Ǹ����ŵ�nfa����dfaɾ�ˡ�����

		//12-9 �հ׷��ļ���
		int temp=vt.value;
		if(vt.whitevn)
		{
			temp=DEFAULT_END;
		}

		vn_nfa.push_back(FiniteAutomata(rd,temp));//12-7 �˴���Ϊ��Token.value?
														//ԭΪVN.size()-1
	}

	//Ϊ�±�Ϊidx��Token������RD����NFA
	bool ConstructNFA_at(int idx)
	{
		return vn_nfa.at(idx).ConstructNFA(RD.at(idx),VN.at(idx).value);
	}


	//����ս������syntax analyzerʹ��
	void OutputTokenList(string filename);



	static VNManager* Instance();
};

