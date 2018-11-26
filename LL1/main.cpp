/***************************************************************************
	项目名称：LL1
	项目描述：实现判断一个文法G是否是LL1的，若是则构造分析表，并据此分析所给的字符串
	项目配置：VS2013
	作   者：马黄雷 2018-11-20 V1.0
	上次修改：2018-11-21
	备   注：更多详细信息参见 ../README.md
---------------------------------------------------------------------------
	文件名称：main.cpp
	文件描述：实现本项目的主函数
	作   者：马黄雷 2018-11-21
	备注：
***************************************************************************/
#include "stdafx.h"
#include "Graph.h"
#include "FFSetCalculator.h"
#include "Analyser.h"

int main()
{
	Graph graph;
	if (!graph.SetGFromFile("egG.txt"))
	{
		cout << "create graph from file fail !\n";
		system("pause");
		exit(0);
	}
		
	Node errorNode;
	//1. check that the graph has left-recursion or not
	if (!graph.CheckLeftRecursion(errorNode))
	{
		cout << "This graph doesn't meet the first condition that:\n"
			<<"\"can't has left-recursion in its produce-rules\"\n"
			<< "because of the noEndNode \"" << errorNode.name << "\"\n"
			<< "So it's not a LL(1)-graph. \n"
			<< "Calculate stop !" << endl;
		system("pause");
		exit(0);
	}

	//2. check that if (A->α1|α2|...|αn) then (First(αi) ∩ First(αj) = ∅)
	graph.InitFirstSet();
	FFSetCalculator calculator;
	calculator.CalculateFirstSetsForGraph(graph);
	if (!graph.CheckProduceRules(errorNode))
	{
		cout << "This graph doesn't meet the second condition that:\n"
			<< "\"if (A->α1|α2|...|αn) then (First(αi) ∩ First(αj) = ∅)\"\n"
			<< "because of the noEndNode \"" << errorNode.name << "\"\n"
			<< "So it's not a LL(1)-graph. \n"
			<< "Calculate stop !" << endl;
		system("pause");
		exit(0);
	}

	//3. check that if (ε ∈ First(A)) then (First(A) ∩ Follow(A) = ∅)
	graph.InitFollowSet();
	calculator.CalculateFollowSetsForGraph(graph);
	if (!graph.CheckFFSet(errorNode))
	{
		cout << "This graph doesn't meet the third condition that:\n"
			<< "\"if (ε ∈ First(A)) then (First(A) ∩ Follow(A) = ∅)\"\n"
			<< "because of the noEndNode \"" << errorNode.name << "\"\n"
			<< "So it's not a LL(1)-graph. \n"
			<< "Calculate stop !" << endl;
		system("pause");
		exit(0);
	}

	cout << "This is a LL(1) graph !\n\n";

	//4. now we can calculate the graph's analyseTable
	graph.InitAnalyseTable();
	graph.CalculateAnalyseTable();

	//5. write some informations into file
	graph.PrintFFT();
	graph.WriteFFTToFile("FFTof" + graph.graphName + ".txt");

	//6. loop deal strings with analyseTable
	string str;
	Analyser analyser;
	analyser.SetGraph(graph);
	cout << "input a string(\"#\" mean stop):";
	cin >> str;
	while (str != "#")
	{
		analyser.AnalyseStr(str);
		cout << "input a string(\"#\" mean stop):";
		cin >> str;
	}

	system("pause");
	return 0;
}