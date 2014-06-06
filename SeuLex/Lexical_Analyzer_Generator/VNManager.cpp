#include "stdafx.h"
#include "FiniteAutomata.h"

VNManager::VNManager(void):num_var_tokens(0)
{
}


VNManager::~VNManager(void)
{
	/*
	int i;
	for(i=0;i<vn_nfa.size();i++)
	{
		if(vn_nfa.at(i)!=NULL)
		{
			delete vn_nfa.at(i);
			vn_nfa.at(i)=NULL;
		}
	}
	*/
}


VNManager* VNManager::Instance()
{
	static VNManager instance;
	return &instance;
}


void VNManager::OutputTokenList(string filename)
{
	ofstream out;
	out.open(filename);

	vector<Token>::iterator iter;

	for(iter=VN.begin();iter!=VN.end();iter++)
	{
		out<<iter->name<<'\t';
		out<<iter->tokenvn<<'\t';
		out<<iter->whitevn<<'\t';
		out<<iter->addressvn<<'\n';
	}

	out.close();
}