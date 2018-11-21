#include"stdafx.h"
#include"Graph.h"

#ifndef ANALYSER_H
#define ANALYSER_H

class Analyser{
private:
	Graph graph;
	string strToAnalyse;
	vector<Node> strNodes;
	stack<Node> nodeStack;
	int printNum = 0;

public:
	Analyser(){ };
	~Analyser(){ };

	void InitAnalyser();
	void SetGraph(Graph g){ graph = g; }
	bool AnalyseStr(string strToAnalyse);
	
	void PrintCurCondition(Point pRule);
	bool ConverteStrToNodes();
};

bool Analyser::AnalyseStr(string str)
{
	InitAnalyser();
	strToAnalyse = str;
	if (!ConverteStrToNodes())
	{
		cout << "the string " << strToAnalyse << " is illegal !\n\n";
		return false;
	}

	for (int i = 0; i < strNodes.size(); i++)
	{
		if (!strNodes[i].isEndNode)
		{
			cout << "the string \"" << strToAnalyse << "\" has noEndNodes !\n\n";
			return false;
		}
	}
	

	Point pRule;
	pRule.x = pRule.y = -1;
	Node nStack, nStr;
	nodeStack.push(graph.superEndNode);
	nodeStack.push(graph.beginNode);
	while (!nodeStack.empty() && !strNodes.empty())
	{
		PrintCurCondition(pRule);

		nStack = nodeStack.top();
		nodeStack.pop();
		nStr = strNodes[0];

		if (nStack.name == nStr.name)
		{
			strNodes.erase(strNodes.begin());
			pRule.x = pRule.y = -1;
			continue;
		}

		if (nStack.isEndNode)
		{
			cout << "error : the stack.top is endNode "
				<< "and it doesn't meet the left node of current-string !\n\n";
			return false;
		}

		pRule = graph.analyseTable[nStack.num][nStr.num];
		if (pRule.x == -1)
		{
			cout << "error : when the stack.top is " << nStack.name << endl
				<< "      and the left node of current-string is " << nStr.name << endl
				<< "      there is no matching produce-rule\n\n";
			return false;
		}

		for (int i = graph.rules[pRule.x][pRule.y].right.size() - 1; i >= 0; i--)
		{
			if (graph.rules[pRule.x][pRule.y].right[i].name == "ε")
				continue;
			nodeStack.push(graph.rules[pRule.x][pRule.y].right[i]);
		}
	}

	if (!nodeStack.empty() || !strNodes.empty())
	{
		cout << "unknown error !\n\n";
		return false;
	}

	cout << "Analyse Correctly !\n\n";
	return true;
}





void Analyser::InitAnalyser()
{
	if (!strToAnalyse.empty())
		strToAnalyse.clear();
	if (!strNodes.empty())
		strNodes.clear();
	while (!nodeStack.empty())
		nodeStack.pop();

	printNum = 0;
}

bool Analyser::ConverteStrToNodes()
{
	bool success = true;
	strNodes = graph.GetNodesFromStr(strToAnalyse, success);
	if (!success)
	{
		return false;
	}

	if (strNodes[strNodes.size()-1].num != graph.superEndNode.num)
	{
		strNodes.push_back(graph.superEndNode);
	}
	return true;
}

void Analyser::PrintCurCondition(Point pRule)
{
	cout << setiosflags(ios::left);
	int len = strToAnalyse.size() > 6 ? strToAnalyse.size() : 6;
	len += 3;

	if (printNum == 0)
	{
		cout << setw(7) << "步骤"
			<< setw(25) << "符号栈"
			<< setw(len) << "输入串"
			<< "所用产生式" << endl;
	}

	cout << setw(7) << printNum;

	string tmpStr = "";
	stack<Node> tmpStack = nodeStack;
	while (!tmpStack.empty())
	{
		tmpStr = tmpStack.top().name + tmpStr;
		tmpStack.pop();
	}
	cout << setw(25) << tmpStr;

	tmpStr.clear();
	for (int i = 0; i < strNodes.size(); i++)
		tmpStr += strNodes[i].name;
	cout << setw(len) << tmpStr;

	if (pRule.x != -1)
		cout << graph.rules[pRule.x][pRule.y].name;

	cout << endl;

	++printNum;
}

#endif