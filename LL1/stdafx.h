#ifndef STDAFX_H
#define STDAFX_H

//head file
#include<iostream>
using namespace std;

#include<fstream>
#include<vector>
#include<stack>
#include<string>
#include<iomanip>


//Data Structs
typedef struct Node{
	int num;
	string name;
	bool isEndNode;
}Node;

typedef struct Rule{
	Node left;
	vector<Node> right;
	string name;
}Rule;

typedef struct Set{
	Node owner;
	vector<Node> items;
}Set;

typedef struct Point
{
	int x;
	int y;
}Point;


//****some common functions****

//根据分隔符sc将字符串source分隔为一个字符串数组并返回
vector<string> split(string source, string sc)
{
	vector<string> words;
	string temp;
	int index_sc;
	while (source.size() > 0)
	{
		index_sc = source.find(sc, 0);
		if (index_sc != string::npos){
			temp = source.substr(0, index_sc);
			if (temp != "")
				words.push_back(temp);
			source.erase(0, index_sc + sc.size());
		}
		else{
			temp = source;
			words.push_back(temp);
			break;
		}
	}
	return words;
}

#endif