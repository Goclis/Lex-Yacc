#pragma once


class FA_GraphEdge :
	public GraphEdge
{
protected:

	char vt;

public:
	FA_GraphEdge(int from,int to,char c);

	char getVT(){return vt;}

	virtual ~FA_GraphEdge(void);
};

