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
	//��մ��������ַ���
	if (!strToAnalyse.empty())
		strToAnalyse.clear();

	//��մ�������������ż�
	if (!strNodes.empty())
		strNodes.clear();

	//��շ���ջ
	while (!nodeStack.empty())
		nodeStack.pop();

	//���õ�ǰ�����������Ϊ0
	printNum = 0;
}

bool Analyser::AnalyseStr(string str)
{
	//��ʼ��������
	InitAnalyser();

	//����Ҫ�������ַ���
	strToAnalyse = str;

	//����_ConverteStrToNodes()���ַ���ת��Ϊ������ż�
	if (!_ConverteStrToNodes())
	{
		//��ʾ�����зǷ��ַ�
		cout << "the string \"" << strToAnalyse << "\" is illegal !\n\n";
		return false;
	}

	//�����������ַ������Ƿ��з��ս��
	for (int i = 0; i < strNodes.size(); i++)
	{
		if (!strNodes[i].isEndNode)
		{
			//��ʾ�����з��ս��
			cout << "the string \"" << strToAnalyse << "\" has noEndNodes !\n\n";
			return false;
		}
	}
	
	//��ʱ����
	Point pRule;
	pRule.x = pRule.y = -1;
	Node nStack, nStr;

	//��#ѹջ
	nodeStack.push(graph.superEndNode);
	//���ķ�graph�Ŀ�ʼ��ѹջ
	nodeStack.push(graph.beginNode);

	//ѭ������������ջ��Ϊ�� && ���봮��Ϊ��
	while (!nodeStack.empty() && !strNodes.empty())
	{
		//��ӡ��ǰ����ջ�����봮��Ϣ
		PrintCurCondition(pRule);

		//��������ջջ��Ԫ��
		nStack = nodeStack.top();
		nodeStack.pop();

		//ָ�����봮��Ԫ��
		nStr = strNodes[0];

		//�������ջջ��Ԫ�������봮��Ԫ����ͬ����ȥ
		if (nStack.name == nStr.name)
		{
			strNodes.erase(strNodes.begin());
			pRule.x = pRule.y = -1;
			continue;
		}

		//�������ջջ��Ԫ���Ƿ��ս��������һ����û����ȥ������
		if (nStack.isEndNode)
		{
			cout << "error : the stack.top is endNode "
				<< "and it doesn't meet the left node of current-string !\n\n";
			return false;
		}

		//�������
		pRule = graph.analyseTable[nStack.num][nStr.num];

		//���������Ӧλ��Ϊ�գ�����
		if (pRule.x == -1)
		{
			cout << "error : when the stack.top is " << nStack.name << endl
				<< "      and the left node of current-string is " << nStr.name << endl
				<< "      there is no matching produce-rule\n\n";
			return false;
		}

		//���������Ӧλ�ò���ʽ���Ҳ�����ѹջ
		for (int i = graph.rules[pRule.x][pRule.y].right.size() - 1; i >= 0; i--)
		{
			//������ʽ�Ҳ�Ϊ�ţ�����
			if (graph.rules[pRule.x][pRule.y].right[i].name == "��")
				continue;
			nodeStack.push(graph.rules[pRule.x][pRule.y].right[i]);
		}
	}

	//ѭ���ѽ�����������ջ�����봮��ͬʱΪ�գ�����
	if (!nodeStack.empty() || !strNodes.empty())
	{
		cout << "Unknown error !\n\n";
		return false;
	}

	//��ӡ��ʾ��Ϣ�������ɹ�
	cout << "Analyse Correctly !\n\n";
	return true;
}

void Analyser::PrintCurCondition(Point pRule)
{
	//���������־�������
	cout << setiosflags(ios::left);

	//�������봮����
	int len = strToAnalyse.size() > 6 ? strToAnalyse.size() : 6;
	len += 3;

	//����һ�δ�ӡ�����ӡ��������Ϣ
	if (printNum == 0)
	{
		cout << setw(7) << "����"
			<< setw(25) << "����ջ"
			<< setw(len) << "���봮"
			<< "���ò���ʽ" << endl;
	}


	//�����ǰ�������
	cout << setw(7) << printNum;

	//�������ջ
	string tmpStr = "";
	stack<Node> tmpStack = nodeStack;
	while (!tmpStack.empty())
	{
		tmpStr = tmpStack.top().name + tmpStr;
		tmpStack.pop();
	}
	cout << setw(25) << tmpStr;
	tmpStr.clear();

	//������봮
	for (int i = 0; i < strNodes.size(); i++)
		tmpStr += strNodes[i].name;
	cout << setw(len) << tmpStr;

	//������ʽ������Ч���������ʽ
	if (pRule.x != -1)
		cout << graph.rules[pRule.x][pRule.y].name;

	cout << endl;

	//���������������
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

	//�������������ż�β������ #
	strNodes.push_back(graph.superEndNode);
	
	return true;
}
#endif