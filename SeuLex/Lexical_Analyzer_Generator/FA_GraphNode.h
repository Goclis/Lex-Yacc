#pragma once

#define NON_ACCEPTING -99
#define DEFAULT_END -1

//DEFAULT_END ��Ϊ�հ׷�������ֹ̬����

//-99 �ǽ����㣬default_end:��token�Ľ�����

class FA_GraphNode :
	public GraphNode
{
protected:

	//12-5 ������bool ����int �Դ���token���
	//-1:��ͨ >0��Ӧ��token��ţ�
	//Ǳ�����⣺�����ķ����ܴ���һ���ڵ��Ƕ��token��accepting state
	//			����������Ƕ����ķ���������ͬtoken֮��϶��з�epsilon�߼��
	//�����������RD�е�Token����˳���������Ŵ��Token���ȼ���

	//bool accepting_node;		//12-4 �������Ҫ����������int (�� Token ���)
	int accepting_node;			//token
public:
	FA_GraphNode(void);
	FA_GraphNode(int idx,int acpt);


	void SetAcceptingNode(int tkn){accepting_node=tkn;}
	int GetAcceptingNode(){return accepting_node;}
	virtual ~FA_GraphNode(void);
};

