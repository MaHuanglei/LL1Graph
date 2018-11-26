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

private:
	bool _ConverteStrToNodes();
};

void Analyser::InitAnalyser()
{
	//清空待分析的字符串
	if (!strToAnalyse.empty())
		strToAnalyse.clear();

	//清空待分析的有序符号集
	if (!strNodes.empty())
		strNodes.clear();

	//清空符号栈
	while (!nodeStack.empty())
		nodeStack.pop();

	//设置当前分析步骤次序为0
	printNum = 0;
}

bool Analyser::AnalyseStr(string str)
{
	//初始化分析器
	InitAnalyser();

	//设置要分析的字符串
	strToAnalyse = str;

	//调用_ConverteStrToNodes()将字符串转化为有序符号集
	if (!_ConverteStrToNodes())
	{
		//提示：含有非法字符
		cout << "the string \"" << strToAnalyse << "\" is illegal !\n\n";
		return false;
	}

	//检查待分析的字符串中是否含有非终结符
	for (int i = 0; i < strNodes.size(); i++)
	{
		if (!strNodes[i].isEndNode)
		{
			//提示：含有非终结符
			cout << "the string \"" << strToAnalyse << "\" has noEndNodes !\n\n";
			return false;
		}
	}
	
	//临时变量
	Point pRule;
	pRule.x = pRule.y = -1;
	Node nStack, nStr;

	//将#压栈
	nodeStack.push(graph.superEndNode);
	//将文法graph的开始符压栈
	nodeStack.push(graph.beginNode);

	//循环条件：符号栈不为空 && 输入串不为空
	while (!nodeStack.empty() && !strNodes.empty())
	{
		//打印当前符号栈、输入串信息
		PrintCurCondition(pRule);

		//弹出符号栈栈顶元素
		nStack = nodeStack.top();
		nodeStack.pop();

		//指向输入串首元素
		nStr = strNodes[0];

		//如果符号栈栈顶元素与输入串首元素相同，消去
		if (nStack.name == nStr.name)
		{
			strNodes.erase(strNodes.begin());
			pRule.x = pRule.y = -1;
			continue;
		}

		//如果符号栈栈顶元素是非终结符且在上一步中没有消去，报错
		if (nStack.isEndNode)
		{
			cout << "error : the stack.top is endNode "
				<< "and it doesn't meet the left node of current-string !\n\n";
			return false;
		}

		//查分析表
		pRule = graph.analyseTable[nStack.num][nStr.num];

		//若分析表对应位置为空，报错
		if (pRule.x == -1)
		{
			cout << "error : when the stack.top is " << nStack.name << endl
				<< "      and the left node of current-string is " << nStr.name << endl
				<< "      there is no matching produce-rule\n\n";
			return false;
		}

		//将分析表对应位置产生式的右部逆序压栈
		for (int i = graph.rules[pRule.x][pRule.y].right.size() - 1; i >= 0; i--)
		{
			//若产生式右部为ε，跳过
			if (graph.rules[pRule.x][pRule.y].right[i].name == "ε")
				continue;
			nodeStack.push(graph.rules[pRule.x][pRule.y].right[i]);
		}
	}

	//循环已结束，若符号栈和输入串不同时为空，报错
	if (!nodeStack.empty() || !strNodes.empty())
	{
		cout << "Unknown error !\n\n";
		return false;
	}

	//打印提示信息：分析成功
	cout << "Analyse Correctly !\n\n";
	return true;
}

void Analyser::PrintCurCondition(Point pRule)
{
	//设置输出标志：左对齐
	cout << setiosflags(ios::left);

	//计算输入串项宽度
	int len = strToAnalyse.size() > 6 ? strToAnalyse.size() : 6;
	len += 3;

	//若第一次打印，则打印标题栏信息
	if (printNum == 0)
	{
		cout << setw(7) << "步骤"
			<< setw(25) << "符号栈"
			<< setw(len) << "输入串"
			<< "所用产生式" << endl;
	}


	//输出当前步骤次序
	cout << setw(7) << printNum;

	//输出符号栈
	string tmpStr = "";
	stack<Node> tmpStack = nodeStack;
	while (!tmpStack.empty())
	{
		tmpStr = tmpStack.top().name + tmpStr;
		tmpStack.pop();
	}
	cout << setw(25) << tmpStr;
	tmpStr.clear();

	//输出输入串
	for (int i = 0; i < strNodes.size(); i++)
		tmpStr += strNodes[i].name;
	cout << setw(len) << tmpStr;

	//若产生式坐标有效，输出产生式
	if (pRule.x != -1)
		cout << graph.rules[pRule.x][pRule.y].name;

	cout << endl;

	//分析步骤次序自增
	++printNum;
}

bool Analyser::_ConverteStrToNodes()
{
	bool success = true;
	strNodes = graph.GetNodesFromStr(strToAnalyse, success);
	if (!success)
	{
		return false;
	}

	//在输入的有序符号集尾部增加 #
	strNodes.push_back(graph.superEndNode);
	
	return true;
}
#endif