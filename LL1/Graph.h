#include"stdafx.h"

#ifndef GRAPH_H
#define GRAPH_H

class Graph{
public:
	string graphName;

	vector<Node> endNodeSet;	//终结符集
	vector<Node> noEndNodeSet;	//非终结符集
	Node beginNode;				//开始符号
	vector<vector<Rule>> rules;	//产生式(同一非终结符的所有产生式占一行)

	Node epsilon;			//特殊节点标记 ε
	Node superEndNode;		//特殊节点标记 # 
	vector<Set> firstSets;		//First集数组
	vector<Set> followSets;		//Follow集数组
	vector<vector<Point>> analyseTable;		//分析表
public:
	Graph(){ }
	~Graph();;
	bool SetGFromFile(string fileName);		//从文件读取文法G
	
	void InitFirstSet();	//初始化firstSets
	void InitFollowSet();	//初始化followSets
	void InitAnalyseTable();	//初始化分析表
	void CalculateAnalyseTable();	//计算分析表

	bool CheckLeftRecursion(Node &leftRecurNode);	//检查文法G是否满足LL1文法的条件一
	bool CheckProduceRules(Node &errorNode);	//检查文法G是否满足LL1文法的条件二
	bool CheckFFSet(Node &errorNode);		//检查文法G是否满足LL1文法的条件三

	void PrintFFT();	//打印FFT信息到屏幕, FFT = First + Follow + analyseTable
	void WriteFFTToFile(string fileName);	//打印FFT信息到文件

	//计算有序符号集ve的First集并返回，ve代表一个字符串
	vector<Node> GetFirstSetOfRule(vector<Node> ve);

	//1. 分析字符串nodeStr，将其转化终结符与非终结符的有序符号集并返回
	//2. 若含有非法字符则置success为false，否则置success为true
	vector<Node> GetNodesFromStr(string nodeStr, bool &success);

//私有函数
private:
	void _MergeAndSortProduceRules();	//将同一非终结符的所有产生式合并到一个vector<Rule>中
	Node _GetNodeInfo(string nodeName);		//根据nodeName返回符号信息
	void _PrintFirstSets(ostream *obj);		//使用obj打印First集信息
	void _PrintFollowSets(ostream *obj);	//使用obj打印Follow集信息
	void _PrintAnalyseTable(ostream *obj);	//使用obj打印分析表信息
	bool _FindNode(vector<Node> ve, Node obj);		//检查容器ve是否包含obj
	bool _FindRepeatNode(vector<vector<Node>> ve);	//检查二维容器ve是否有重复元素
	vector<Node> _MergeTwoVector(vector<Node> &ve1, vector<Node> &ve2);	//return ve1+ve2
};

Graph::~Graph()
{
	endNodeSet.clear();
	noEndNodeSet.clear();
	rules.clear();
	firstSets.clear();
	followSets.clear();
	analyseTable.clear();
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
	graphName = split(tmpLine, "：")[1];

	//2. read end nodes
	epsilon.num = -1;
	infile.getline(tmpLine, 200, '\n');
	tmpStr = split(tmpLine, "：")[1];
	words = split(tmpStr, " ");
	for (int i = 0; i < words.size(); i++)
	{
		Node tmpNode;
		tmpNode.num = i;
		tmpNode.name = words[i];
		tmpNode.isEndNode = true;
		endNodeSet.push_back(tmpNode);
		if (epsilon.num == -1 && tmpNode.name == "ε")
			epsilon = tmpNode;
	}
	if (epsilon.num == -1) //if G has no ε then add ε weself;
	{
		epsilon.num = endNodeSet.size();
		epsilon.name = "ε";
		epsilon.isEndNode = true;
		endNodeSet.push_back(epsilon);
	} //add # as superEndNode weself;
	superEndNode.num = epsilon.num;
	superEndNode.name = "#";
	superEndNode.isEndNode = true;

	//3. read noEnd nodes
	infile.getline(tmpLine, 200, '\n');
	tmpStr = split(tmpLine, "：")[1];
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
	tmpStr = split(tmpLine, "：")[1];
	beginNode = _GetNodeInfo(tmpStr);

	//5. read produce rules
	infile.getline(tmpLine, 200, '\n');
	infile.getline(tmpLine, 200, '\n');
	while (strcmp(tmpLine, "endp"))
	{
		vector<Rule> tmpVes;
		words = split(tmpLine, "->");
		Node left = _GetNodeInfo(words[0]);
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
	_MergeAndSortProduceRules();

	infile.close();
	return true;
}

bool Graph::CheckLeftRecursion(Node &leftRecurNode)
{
	int maxProduceNum = noEndNodeSet.size() + 1;

	stack<vector<Node>> myStack;
	stack<int> produceNumStack;
	for (int i = 0; i < noEndNodeSet.size(); i++)
	{
		Node left = noEndNodeSet[i];
		for (int j = 0; j < rules[i].size(); j++)
		{
			myStack.push(rules[i][j].right);
			produceNumStack.push(1);
		}

		int curProduceNum;
		vector<Node> sentence;
		while (!myStack.empty())
		{
			sentence = myStack.top();
			myStack.pop();
			curProduceNum = produceNumStack.top();
			produceNumStack.pop();

			if (curProduceNum > maxProduceNum)
				continue;
			if (sentence[0].isEndNode)
				continue;
			if (sentence[0].num == left.num)
			{
				leftRecurNode = left;
				return false;
			}

			Node curFirstNodeOfSentence = sentence[0];
			sentence.erase(sentence.begin());
			vector<Node> newSentence;
			for (int t = 0; t < rules[curFirstNodeOfSentence.num].size(); t++)
			{
				if (rules[curFirstNodeOfSentence.num][t].right[0].name != epsilon.name)
					newSentence = _MergeTwoVector(rules[curFirstNodeOfSentence.num][t].right, sentence);
				else newSentence = sentence;
				if (!newSentence.empty())
				{
					myStack.push(newSentence);
					produceNumStack.push(curProduceNum + 1);
				}
					
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

bool Graph::CheckProduceRules(Node &errorNode)
{
	for (int i = 0; i < rules.size(); i++)
	{
		if (rules[i].size() == 1) 
			continue;

		vector<vector<Node>> sonSets;
		for (int j = 0; j < rules[i].size(); j++)
			sonSets.push_back(GetFirstSetOfRule(rules[i][j].right));
		
		if (_FindRepeatNode(sonSets))
		{
			errorNode = rules[i][0].left;
			return false;
		}
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

bool Graph::CheckFFSet(Node &errorNode)
{
	for (int i = 0; i < firstSets.size(); i++)
	{
		if (_FindNode(firstSets[i].items, epsilon))
		{
			for (int j = 0; j < followSets[i].items.size(); j++)
			{
				if (_FindNode(firstSets[i].items, followSets[i].items[j]))
				{
					errorNode = firstSets[i].owner;
					return false;
				}
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
			tmpNode = _GetNodeInfo(nodeName);
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

		if (!_FindNode(firstSets[ve[i].num].items, epsilon))
			break;
	}

	return tempSet;
}

void Graph::_MergeAndSortProduceRules()
{
	/*vector<Rule> tmpRuleRow;
	for (vector<Node>::iterator t = noEndNodeSet.begin(); t < noEndNodeSet.end(); ++t)
	{
		for (vector<vector<Rule>>::iterator i = rules.begin(); i < rules.end(); ++i)
		{
			if (*t.num == (*(*i).begin()).left.num)
			{
				for (vector<Rule>::iterator j = (*i).begin(); j < (*i).end(); ++j)
				{
					tmpRuleRow.push_back(*j);
				}
				i = rules.erase(i);
			}
		}
		rules.insert()
	}*/

	
	for (int i = 0; i < rules.size(); i++)
	{
		Node left = rules[i][0].left;
		for (int j = i + 1; j<rules.size(); j++)
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

void Graph::_PrintFirstSets(ostream *obj)
{
	for (int i = 0; i < firstSets.size(); i++)
	{
		*obj << "First(" << firstSets[i].owner.name << ") = ";
		if (firstSets[i].items.size() == 0) {
			*obj << "Φ" << endl;
			continue;
		}

		*obj << "{ " << firstSets[i].items[0].name;
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
		*obj << "Follow(" << followSets[i].owner.name << ") = ";
		if (followSets[i].items.size() == 0) {
			*obj << "Φ" << endl;
			continue;
		}

		*obj << "{ " << followSets[i].items[0].name;
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

Node Graph::_GetNodeInfo(string nodeName)
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

bool Graph::_FindNode(vector<Node> ve, Node obj)
{
	for (int j = 0; j < ve.size(); j++)
	{
		if (ve[j].num == obj.num && ve[j].name == obj.name)
			return true;
	}

	return false;
}

bool Graph::_FindRepeatNode(vector<vector<Node>> ve)
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

vector<Node> Graph::_MergeTwoVector(vector<Node> &ve1, vector<Node> &ve2)
{
	vector<Node> newVector(ve1);
	for (int i = 0; i < ve2.size(); i++)
		newVector.push_back(ve2[i]);
	return newVector;
}

#endif