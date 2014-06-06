// Lexical_Analyzer_Generator.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	/*
	//���Դ���
	
	int i;

	ifstream in;
	//1.infix to postfix
	PostfixConvertor pc;
	string infix,postfix;

	//infix="((a|b)*.(a|b).(a|b))*.(a|b)";
	//infix="x*";
	in.open("zTestInfixInput.txt");

	//VT:temp
	//char a='a',b='b',c='c';
	pVTManager->Push('a');
	pVTManager->Push('b');
	pVTManager->Push('c');



	in>>infix;

	pc.InfixToPostFix(infix,postfix);

	cout<<postfix<<'\n';

	//VN:temp
	pVNManager->Push(Token("letter",DEFAULT_END),postfix);
	i=0;
	pVNManager->GetFAAt(i).Print();


	infix.clear();
	postfix.clear();
	
	
	in>>infix;
	pc.InfixToPostFix(infix,postfix);
	
	cout<<postfix<<'\n';
	pVNManager->Push(Token("variable",1),postfix);
	i=1;
	pVNManager->GetFAAt(i).Print();



	


	

	in.close();


	
	
	i=1;
	FiniteAutomata global_dfa;
	global_dfa.ConstructDFA(pVNManager->GetFAAt(i));
	global_dfa.Print();
	


	pCodeGenerator->GenerateSourceCode(global_dfa);
	*/

	string filename="lexical_definition.l";
	pRDReader->ReadRDFile(filename);

	//pRDReader->GetDFA().Print();	//12-8 state 28 ��Ӧ����ֹ̬6���������ɵ�NON_ACCEPT
									//nfa ����6��ֹ̬��dfa�ﱻ����

	//����Token��
	//pVNManager->OutputTokenList("z"+(pCodeGenerator->GetProgramFileType())+".txt");
	pVNManager->OutputTokenList("token_list.lst");

	pCodeGenerator->GenerateSourceCode(pRDReader->GetDFA());



	system("pause");
	return 0;
}

