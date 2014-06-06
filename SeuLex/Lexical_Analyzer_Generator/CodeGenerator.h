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
	string program_file_type;//生成的LA读入的sourcecode文件类型(后缀名)
	string token_file_type;//输出的token输出的文件类型(后缀名)


	//相对固定的部分程序
	string s1,s2,s3,s4,s5;	//在源码的开头部分
	string e1,e2,e3;		//在源码的后面部分

	//需要由DFA信息确定的一些部分
	string def;

public:
	static CodeGenerator* Instance();

	string GetProgramFileType();
	
	void SetFileName(const string&);	//设置输出源码的名称
	void SetTokenFileType(const string&);	//设置LA输出Token序列的文件后缀名(tkn)
	void SetProgramFileType(const string&);	//设置LA读取的源程序的后缀名(cmm)
	void GenerateSourceCode(FiniteAutomata & dfa);	//根据DFAo生成LA源码
};

