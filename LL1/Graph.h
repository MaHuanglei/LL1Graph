#include"stdafx.h"

#ifndef GRAPH_H
#define GRAPH_H

class Graph{
public:
	string graphName;

	vector<Node> endNodeSet;
	vector<Node> noEndNodeSet;
	Node beginNode;
	vector<vector<Rule>> rules;

	Node epsilon;
	Node superEndNode;
	vector<Set> firstSets;
	vector<Set> followSets;
	vector<vector<Point>> analyseTable;
public:
	Graph(){ };
	~Graph();

	bool SetGFromFile(string fileName);
	bool CheckLeftRecursion();
	void InitFirstSet();
	bool CheckProduceRules();
	void InitFollowSet();
	bool CheckFFSet();
	void InitAnalyseTable();
	void CalculateAnalyseTable();
	void PrintFFT();
	void WriteFFTToFile(string fileName);


	void MergeProduceRules();
	Node GetNodeInfo(string nodeName);
	bool FindRepeatNode(vector<vector<Node>> ve);
	bool FindNode(vector<Node> ve, Node obj);
	vector<Node> GetNodesFromStr(string nodeStr, bool &success);
	vector<Node> GetFirstSetOfRule(vector<Node> ve);

private:
	void _PrintFirstSets(ostream *obj);
	void _PrintFollowSets(ostream *obj);
	void _PrintAnalyseTable(ostream *obj);
};

Graph::~Graph()
{
	endNodeSet.clear();
	noEndNodeSet.clear();
	rules.clear();
}

bool Graph::SetGFromFile(string fileName)
{
	ifstream infile(fileName);
	if (!infile){
		cout << "open " << fileName << " fail !" << endl;
		return false;
	}

	string tmpStr;
	char tmpLine[200];
	vector<string> words;

	//1. read graph name
	infile.getline(tmpLine, 200, '\n');
	graphName = split(tmpLine, "£º")[1];

	//2. read end nodes
	epsilon.num = -1;
	infile.getline(tmpLine, 200, '\n');
	tmpStr = split(tmpLine, "£º")[1];
	words = split(tmpStr, " ");
	for (int i = 0; i < words.size(); i++)
	{
		Node tmpNode;
		tmpNode.num = i;
		tmpNode.name = words[i];
		tmpNode.isEndNode = true;
		endNodeSet.push_back(tmpNode);
		if (epsilon.num == -1 && tmpNode.name == "¦Å")
			epsilon = tmpNode;
	}
	if (epsilon.num == -1) //if G has no ¦Å then add ¦Å weself;
	{
		epsilon.num = endNodeSet.size();
		epsilon.name = "¦Å";
		epsilon.isEndNode = true;
		endNodeSet.push_back(epsilon);
	} //add # as superEndNode weself;
	superEndNode.num = epsilon.num;
	superEndNode.name = "#";
	superEndNode.isEndNode = true;

	//3. read noEnd nodes
	infile.getline(tmpLine, 200, '\n');
	tmpStr = split(tmpLine, "£º")[1];
	words = split(tmpStr, " ");
	for (int i = 0; i < words.size(); i++)
	{
		Node tmpNode;
		tmpNode.num = i;
		tmpNode.name = words[i];
		tmpNode.isEndNode = false;
		noEndNodeSet.push_back(tmpNode);
	}

	//4. read beginNode
	infile.getline(tmpLine, 200, '\n');
	tmpStr = split(tmpLine, "£º")[1];
	beginNode = GetNodeInfo(tmpStr);

	//5. read produce rules
	infile.getline(tmpLine, 200, '\n');
	infile.getline(tmpLine, 200, '\n');
	while (strcmp(tmpLine, "endp"))
	{
		vector<Rule> tmpVes;
		words = split(tmpLine, "->");
		Node left = GetNodeInfo(words[0]);
		words = split(words[1], "|");
		for (int i = 0; i < words.size(); i++)
		{
			bool success = true;
			Rule tmpRule;
			tmpRule.left = left;
			tmpRule.right = GetNodesFromStr(words[i], success);
			tmpRule.name = left.name + "->" + words[i];

			if (!success)
			{
				cout << "produce-rules error : " << tmpRule.name << endl;
				exit(0);
			}
			tmpVes.push_back(tmpRule);
		}
		rules.push_back(tmpVes);
		tmpVes.clear();

		infile.getline(tmpLine, 200, '\n');
	}
	MergeProduceRules();

	infile.close();
	return true;
}

bool Graph::CheckLeftRecursion()
{
	for (int i = 0; i < rules.size(); i++)
	{
		int ruleLeftNodeNum = rules[i][0].left.num;
		for (int j = 0; j < rules[i].size(); j++)
		{
			if (rules[i][j].right[0].num == ruleLeftNodeNum)
			{
				return false;
			}
		}
	}

	return true;
}

void Graph::InitFirstSet()
{
	Set firstP;
	for (int i = 0; i < noEndNodeSet.size(); i++)
	{
		firstP.owner = noEndNodeSet[i];
		firstSets.push_back(firstP);
	}
}

bool Graph::CheckProduceRules()
{
	for (int i = 0; i < rules.size(); i++)
	{
		if (rules[i].size() == 1) 
			continue;

		vector<vector<Node>> sonSets;
		for (int j = 0; j < rules[i].size(); j++)
			sonSets.push_back(GetFirstSetOfRule(rules[i][j].right));
		
		if (FindRepeatNode(sonSets))
			return false;
	}

	return true;
}

void Graph::InitFollowSet()
{
	Set followP;
	for (int i = 0; i < noEndNodeSet.size(); i++)
	{
		followP.owner = noEndNodeSet[i];
		followSets.push_back(followP);
	}
}

bool Graph::CheckFFSet()
{
	for (int i = 0; i < firstSets.size(); i++)
	{
		if (FindNode(firstSets[i].items, epsilon))
		{
			for (int j = 0; j < followSets[i].items.size(); j++)
			{
				if (FindNode(firstSets[i].items, followSets[i].items[j]))
					return false;
			}
		}
	}
	return true;
}

void Graph::InitAnalyseTable()
{
	Point p;
	p.x = p.y = -1;
	vector<Point> tmpRow;
	for (int j = 0; j < endNodeSet.size(); j++)
		tmpRow.push_back(p);
	for (int i = 0; i < noEndNodeSet.size(); i++)
		analyseTable.push_back(tmpRow);
}

void Graph::CalculateAnalyseTable()
{
	Point pRule;
	vector<Node> tmpFirstSet;
	for (int i = 0; i < rules.size(); i++)
	for (int j = 0; j < rules[i].size(); j++)
	{
		pRule.x = i;
		pRule.y = j;
		int index = rules[i][0].left.num;
		tmpFirstSet = GetFirstSetOfRule(rules[i][j].right);
		for (int t = 0; t < tmpFirstSet.size(); t++)
		{
			if (tmpFirstSet[t].num == epsilon.num)
			{
				for (int z = 0; z < followSets[index].items.size(); z++)
				{
					analyseTable[index][followSets[index].items[z].num] = pRule;
				}
			}
			analyseTable[index][tmpFirstSet[t].num] = pRule;
		}
	}
}

void Graph::PrintFFT()
{
	_PrintFirstSets(&cout);
	_PrintFollowSets(&cout);
	_PrintAnalyseTable(&cout);
}

void Graph::WriteFFTToFile(string fileName)
{
	ofstream ofile(fileName);
	if (!ofile)
	{
		cout << "open " << fileName << " fail !\n";
		return;
	}

	_PrintFirstSets(&ofile);
	_PrintFollowSets(&ofile);
	_PrintAnalyseTable(&ofile);
	
	ofile.close();
	cout << "Write FFT to " << fileName << " successfully !\n\n";
}







void Graph::MergeProduceRules()
{
	for (int i = 0; i < rules.size(); i++)
	{
		Node left = rules[i][0].left;
		for (int j = i+1;j<rules.size();j++)
		{
			if (left.num == rules[j][0].left.num)
			{
				for (int z = 0; z < rules[j].size(); ++z)
					rules[i].push_back(rules[j][z]);
				vector<vector<Rule>>::iterator it = rules.begin();
				for (int z = 0; z < j; ++z, ++it);
				rules.erase(it);
				--j;
			}
		}
	}
}

Node Graph::GetNodeInfo(string nodeName)
{
	Node node;
	node.num = -1;
	for (int i = 0; i < endNodeSet.size(); i++)
	{
		if (endNodeSet[i].name == nodeName)
		{
			node.num = i;
			node.name = nodeName;
			node.isEndNode = true;
			return node;
		}
	}

	for (int i = 0; i < noEndNodeSet.size(); i++)
	{
		if (noEndNodeSet[i].name == nodeName)
		{
			node.num = i;
			node.name = nodeName;
			node.isEndNode = false;
			return node;
		}
	}

	return node;
}

vector<Node> Graph::GetNodesFromStr(string nodeStr, bool &success)
{
	success = true;
	vector<Node> nodes;
	int cur_len;
	while (!nodeStr.empty() && success)
	{
		cur_len = 1;
		string nodeName = "";
		Node node, tmpNode;
		node.num = -1;
		while (cur_len <= nodeStr.size())
		{
			nodeName += nodeStr[cur_len-1];
			tmpNode = GetNodeInfo(nodeName);
			if (tmpNode.num != -1)
				node = tmpNode;
			++cur_len;
		}

		if (node.num == -1)
		{
			success = false;
			continue;
		}

		nodes.push_back(node);
		nodeStr.erase(0, node.name.size());
	}
	return nodes;
}

vector<Node> Graph::GetFirstSetOfRule(vector<Node> ve)
{
	vector<Node> tempSet;

	for (int i = 0; i < ve.size(); i++)
	{
		if (ve[i].isEndNode)
		{
			tempSet.push_back(ve[i]);
			break;
		}

		for (int j = 0; j < firstSets[ve[i].num].items.size(); j++)
			tempSet.push_back(firstSets[ve[i].num].items[j]);

		if (!FindNode(firstSets[ve[i].num].items, epsilon))
			break;
	}

	return tempSet;
}

bool Graph::FindNode(vector<Node> ve, Node obj)
{
	for (int j = 0; j < ve.size(); j++)
	{
		if (ve[j].num == obj.num && ve[j].name == obj.name)
			return true;
	}

	return false;
}

bool Graph::FindRepeatNode(vector<vector<Node>> ve)
{
	vector<bool> flags;
	for (int i = 0; i < endNodeSet.size(); i++)
		flags.push_back(false);

	for (int i = 0; i < ve.size();i++)
	for (int j = 0; j < ve[i].size(); j++)
	{
		if (flags[ve[i][j].num])
			return true;
		else flags[ve[i][j].num] = true;
	}
	
	return false;
}

void Graph::_PrintFirstSets(ostream *obj)
{
	for (int i = 0; i < firstSets.size(); i++)
	{
		*obj << "First(" << firstSets[i].owner.name << ") = { ";
		*obj << firstSets[i].items[0].name;
		for (int j = 1; j < firstSets[i].items.size(); j++)
		{
			*obj << ", " << firstSets[i].items[j].name;
		}
		*obj << " }\n";
	}
	*obj << endl << endl;
}

void Graph::_PrintFollowSets(ostream *obj)
{
	for (int i = 0; i < followSets.size(); i++)
	{
		*obj << "Follow(" << followSets[i].owner.name << ") = { ";
		*obj << followSets[i].items[0].name;
		for (int j = 1; j < followSets[i].items.size(); j++)
		{
			*obj << ", " << followSets[i].items[j].name;
		}
		*obj << " }\n";
	}
	*obj << endl << endl;
}

void Graph::_PrintAnalyseTable(ostream *obj)
{
	int maxLenOfRule = 0;
	for (int i = 0; i < rules.size(); i++)
	{
		for (int j = 0; j < rules[i].size(); j++)
		{
			if (maxLenOfRule < rules[i][j].name.size())
				maxLenOfRule = rules[i][j].name.size();
		}
	}
	maxLenOfRule += 3;

	int maxLenOfNoEndNode = 0;
	for (int i = 0; i < noEndNodeSet.size(); i++)
	if (maxLenOfNoEndNode < noEndNodeSet[i].name.size())
		maxLenOfNoEndNode = noEndNodeSet[i].name.size();
	maxLenOfNoEndNode += 3;

	(*obj).setf(ios::left);
	*obj << setw(maxLenOfNoEndNode) << " ";
	for (int i = 0; i < endNodeSet.size() - 1; i++)
	{
		*obj << setw(maxLenOfRule) << endNodeSet[i].name;
	}
	*obj << setw(maxLenOfRule) << superEndNode.name << endl;


	Point pRule;
	for (int i = 0; i < noEndNodeSet.size(); i++)
	{
		*obj << setw(maxLenOfNoEndNode) << noEndNodeSet[i].name;
		for (int j = 0; j < endNodeSet.size(); j++)
		{
			pRule = analyseTable[i][j];
			if (pRule.x == -1)
				*obj << setw(maxLenOfRule) << "error";
			else *obj << setw(maxLenOfRule) << rules[pRule.x][pRule.y].name;
		}
		*obj << endl;
	}
	*obj << endl;
	(*obj).unsetf(ios::left);
}


#endif