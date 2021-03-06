#include "stdafx.h"
#include "UFSets.h"
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

//构造函数
UFSets::UFSets(int sz){
	size = sz;
	parent = new int[size];  //创建双亲数组
	for (int i = 0; i < size; i++){
		parent[i] = -1;
	}
}

UFSets::UFSets(UFSets &R){
	size = R.size;
	parent = new int[size];
	for (int i = 0; i < size; i++){
		parent[i] = R.parent[i];
	}
}

UFSets& UFSets::operator = (UFSets &R){
	size = R.size;
	parent = new int[size];
	for (int i = 0; i < size; i++){
		parent[i] = R.parent[i];
	}
	return *this;
}


void UFSets::Split(int id)
{
	if(parent[id]>=0)
	{
		parent[parent[id]]+=1;
		parent[id]=-1;
	}
	else
	{
		assert(0);
	}
}

void UFSets::Print()
{
	int i;
	for(i=0;i<size;i++)
	{
		cout<<i<<'\t'<<parent[i]<<'\n';
	}
}




//函数搜索并返回包含元素x的树的根。
int UFSets::Find(int x){
	if(x<0)
	{
		return x;
	}
	

	while (parent[x] >= 0){
		x = parent[x];
	}
	return x;
}

//求两个不相交集合Root1与Root2的并(将Root2连接到Root1下面)
void UFSets::SimpleSimpleUnion(int Root1, int Root2){
	//确保Root1,Root2是根节点
	
	if (Root1 != Root2){
		parent[Root1] += parent[Root2];
		parent[Root2] = Root1;
	}
}



//求两个不相交集合Root1与Root2的并(将Root2连接到Root1下面)
int UFSets::SimpleUnion(int Root1, int Root2){
	int r1 = Find(Root1), r2 = Find(Root2);
	if (r1 != r2){
		parent[r1] += parent[r2];
		parent[r2] = r1;
	}
	return r1;
}

//按Union的加权规则改进的算法
int UFSets::WeightedUnion(int Root1, int Root2){
	int r1 = Find(Root1), r2 = Find(Root2), temp;
	if (r1 != r2){
		temp = parent[r1] + parent[r2];
		if (parent[r2] < parent[r1]){
			parent[r1] = r2;
			parent[r2] = temp;
			return r2;
		}
		else{
			parent[r2] = r1;
			parent[r1] = temp;
			return r1;
		}
	}
	return r1;	//本来根就相同不用合并
}

//使用折叠规则的搜索算法
int UFSets::CollapsingFind(int i){
	int j = i;
	for (; parent[j] >= 0; j = parent[j]);
	while (i != j){
		//把一路上所有非直接子节点的点变成直接子节点
		int temp = parent[i];  
		parent[i] = j; 
		i = temp; 
	}
    return j;
}

void UFSets::output(ostream &out){
	int i;
	out << "\n" << string(70, '=');
	for (int j = 0; j <= (size-1)/10; j++){
		out << "\nNode:\t";
		for(i = 10*j; i < 10*(j+1) && i < size; i++){
			out << setw(6) << i;
		}
		out << "\nParent:\t";
		for(i = 10*j; i < 10*(j+1) && i < size; i++){
			out << setw(6) << parent[i];
		}
		if (i < size){
			out << "\n" << string(70, '-');
		}
	}
	cout << "\n" << string(70, '=') << "\n";
}
