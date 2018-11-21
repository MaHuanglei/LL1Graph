#include "stdafx.h"
#include "Graph.h"
#include "FFSetCalculator.h"
#include "Analyser.h"

int main()
{
	Graph graph;
	graph.SetGFromFile("explameG.txt");

	//1. check that the graph has left-recursion or not
	if (!graph.CheckLeftRecursion())
	{
		cout << "This graph doesn't meet the second condition that:\n"
			<<"can't has left-recursion in its produce-rules\n"
			<< "so it's not a LL(1)-graph. \n"
			<< "Calculate stop !" << endl;
		exit(0);
	}

	//2. check that if (A->α1|α2|...|αn) then (First(αi) ∩ First(αj) = ∅)
	graph.InitFirstSet();
	FFSetCalculator calculator;
	calculator.CalculateFirstSetsForGraph(graph);
	if (!graph.CheckProduceRules())
	{
		cout << "This graph doesn't meet the second condition that:\n"
			<< "if (A->α1|α2|...|αn) then (First(αi) ∩ First(αj) = ∅)\n"
			<< "so it's not a LL(1)-graph. \n"
			<< "Calculate stop !" << endl;
		exit(0);
	}

	//3. check that if (ε ∈ First(A)) then (First(A) ∩ Follow(A) = ∅)
	graph.InitFollowSet();
	calculator.CalculateFollowSetsForGraph(graph);
	if (!graph.CheckFFSet())
	{
		cout << "This graph doesn't meet the third condition that:\n"
			<< "if (ε ∈ First(A)) then (First(A) ∩ Follow(A) = ∅)\n"
			<< "so it's not a LL(1)-graph. \n"
			<< "Calculate stop !" << endl;
		exit(0);
	}

	cout << "This is a LL(1) graph !\n\n";

	//4. now we can calculate the graph's analyseTable
	graph.InitAnalyseTable();
	graph.CalculateAnalyseTable();

	//5. write some informations into file
	//graph.PrintFFT();
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