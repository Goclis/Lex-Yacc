#pragma once

//singleton
#define pRDReader RDReader::Instance()


//define for special char
#define RD_ESC '\\'
#define RD_SPLIT '%'
#define RD_SPLIT_STR "%"
#define RD_TOKENVN '?'
#define RD_TOKENVN_STR "?"
#define RD_WHITEVN '~'
#define RD_WHITEVN_STR "~"
#define RD_VARIABLE '$'
#define RD_VARIABLE_STR "$"
/*
#define RD_VNFLAG VT_VNFLAG
#define RD_CONCATENATION VT_CONCATENATION
#define RD_ALTERNATION VT_ALTERNATION
#define RD_CLOSURE VT_CLOSURE
*/
#define RD_VNFLAG '@'
#define RD_CONCATENATION '.'
#define RD_ALTERNATION '|'
#define RD_CLOSURE '*'
#define RD_LEFT_BRACKET '('
#define RD_RIGHT_BRACKET ')'

class RDReader
{
protected:
	RDReader(void);
	~RDReader(void);

	FiniteAutomata global_dfa;

public:
	FiniteAutomata& GetDFA(){return global_dfa;}
	static RDReader* Instance();

	char EscMeaning(char c);
	char SpecialMeaning(char c);
	bool IsWhiteSpace(char c){return c==' '||c=='\n'||c=='\t';}

	void ReadRDFile(string& filename);
};

