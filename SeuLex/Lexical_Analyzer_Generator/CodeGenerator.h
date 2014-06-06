#pragma once

//singleton
#define pCodeGenerator CodeGenerator::Instance()

//generating source code for lexical analyzer
class CodeGenerator
{
protected:
	CodeGenerator(void);
	~CodeGenerator(void);

	string source_file_name;
	string program_file_type;//���ɵ�LA�����sourcecode�ļ�����(��׺��)
	string token_file_type;//�����token������ļ�����(��׺��)


	//��Թ̶��Ĳ��ֳ���
	string s1,s2,s3,s4,s5;	//��Դ��Ŀ�ͷ����
	string e1,e2,e3;		//��Դ��ĺ��沿��

	//��Ҫ��DFA��Ϣȷ����һЩ����
	string def;

public:
	static CodeGenerator* Instance();

	string GetProgramFileType();
	
	void SetFileName(const string&);	//�������Դ�������
	void SetTokenFileType(const string&);	//����LA���Token���е��ļ���׺��(tkn)
	void SetProgramFileType(const string&);	//����LA��ȡ��Դ����ĺ�׺��(cmm)
	void GenerateSourceCode(FiniteAutomata & dfa);	//����DFAo����LAԴ��
};

