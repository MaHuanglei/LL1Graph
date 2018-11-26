#include"stdafx.h"
#include"Graph.h"

#ifndef FFSETCALCULATOR_H
#define FFSETCALCULATOR_H

class FFSetCalculator{
private:

public:
	FFSetCalculator(){ };
	~FFSetCalculator(){ };

	void CalculateFirstSetsForGraph(Graph &graph);
	void CalculateFollowSetsForGraph(Graph &graph);
};


//此函数计算非终结符的FirstSet
void FFSetCalculator::CalculateFirstSetsForGraph(Graph &graph)
{
	vector<vector<Rule>> exRules = graph.rules;
	stack<Node> myStack;
	for (int i = 0; i < exRules.size(); i++)
	for (int j = 0; j < exRules[i].size(); j++)
	{
		if (split(exRules[i][j].name, "->")[1] == "ε")
			myStack.push(exRules[i][j].left);
	}
	
	Node left;
	while (!myStack.empty())
	{
		left = myStack.top();
		myStack.pop();

		for (int i = 0; i < exRules.size(); i++)
		for (int j = 0; j < exRules[i].size(); j++)
		{
			if (!exRules[i][j].right[0].isEndNode && exRules[i][j].right[0].num == left.num)
			{
				Rule newRule = exRules[i][j];
				newRule.right.erase(newRule.right.begin());
				if (newRule.right.empty())
				{
					newRule.right.push_back(graph.epsilon);
					myStack.push(newRule.left);
				}
				exRules[i].push_back(newRule);
			}
		}
	}

	vector<bool> tmp;
	for (int j = 0; j < graph.endNodeSet.size(); j++)
		tmp.push_back(false);
	vector<vector<bool>> belongF;
	for (int i = 0; i < graph.noEndNodeSet.size(); i++)
		belongF.push_back(tmp);

	Point point;
	stack<Point> pStack;
	for (int i = 0; i < exRules.size(); i++)
	for (int j = 0; j < exRules[i].size(); j++)
	{
		if (exRules[i][j].right[0].isEndNode)
		{
			
			point.x = exRules[i][0].left.num;
			point.y = exRules[i][j].right[0].num;
			belongF[point.x][point.y] = true;
			pStack.push(point);
		}
	}

	while (!pStack.empty())
	{
		point = pStack.top();
		pStack.pop();

		if (point.y == graph.epsilon.num)
			continue;

		for (int i = 0; i < exRules.size(); i++)
		for (int j = 0; j < exRules[i].size(); j++)
		{
			if (!exRules[i][j].right[0].isEndNode && exRules[i][j].right[0].num == point.x)
			{
				Point newp;
				newp.x = exRules[i][j].left.num;
				newp.y = point.y;
				belongF[newp.x][newp.y] = true;
				pStack.push(newp);
			}
		}
	}

	for (int i = 0; i < belongF.size(); i++)
	{
		for (int j = 0; j < belongF[i].size(); j++)
		{
			if (belongF[i][j])
			{
				graph.firstSets[i].items.push_back(graph.endNodeSet[j]);
			}
		}
	}
}

//此函数计算非终结符的FollowSet
void FFSetCalculator::CalculateFollowSetsForGraph(Graph &graph)
{
	vector<bool> tmp;
	vector<vector<bool>> belongF;
	for (int i = 0; i < graph.noEndNodeSet.size(); i++)
	{
		tmp.clear();
		for (int j = 0; j < graph.endNodeSet.size(); j++)
			tmp.push_back(false);
		belongF.push_back(tmp);
	}

	
	Point p;
	p.x = graph.beginNode.num;
	p.y = graph.superEndNode.num;
	belongF[p.x][p.y] = true;
	stack<Point> pStack;
	pStack.push(p);
	vector<Node> tmpNodeSet, tmpFirstSet;
	vector<vector<Rule>> exRules = graph.rules;
	for (int i = 0; i < exRules.size(); i++)
	for (int j = 0; j < exRules[i].size(); j++)
	{
		tmpNodeSet = exRules[i][j].right;
		for (int z = 0; z < exRules[i][j].right.size()-1; z++)
		{
			tmpNodeSet.erase(tmpNodeSet.begin());
			if (exRules[i][j].right[z].isEndNode)
				continue;
			
			tmpFirstSet = graph.GetFirstSetOfRule(tmpNodeSet);
			for (int t = 0; t < tmpFirstSet.size(); t++)
			{
				if (tmpFirstSet[t].num == graph.epsilon.num)
				{
					Rule newRule = exRules[i][j];
					vector<Node>::iterator it = newRule.right.begin();
					for (int s = 0; s <= z; ++s, ++it);
					newRule.right.erase(it, newRule.right.end());
					exRules[i].push_back(newRule);
					continue;
				}
				p.x = exRules[i][j].right[z].num;
				p.y = tmpFirstSet[t].num;
				belongF[p.x][p.y] = true;
				pStack.push(p);
			}
		}
	}

	Point tmpP;
	while (!pStack.empty())
	{
		tmpP = pStack.top();
		pStack.pop();

		for (int i = 0; i < exRules.size(); i++)
		{
			if (exRules[i][0].left.num == tmpP.x)
			{
				for (int j = 0; j < exRules[i].size(); j++)
				{ 
					if (!exRules[i][j].right[exRules[i][j].right.size() - 1].isEndNode
						&& !belongF[exRules[i][j].right[exRules[i][j].right.size() - 1].num][tmpP.y])
					{
						Point p;
						p.x = exRules[i][j].right[exRules[i][j].right.size() - 1].num;
						p.y = tmpP.y;
						belongF[p.x][p.y] = true;
						pStack.push(p);
					}
				}
				break;
			}
		}
	}

	for (int i = 0; i < belongF.size(); i++)
	{
		for (int j = 0; j < belongF[i].size()-1; j++)
		{
			if (belongF[i][j])
			{
				graph.followSets[i].items.push_back(graph.endNodeSet[j]);
			}
		}
		if (belongF[i][graph.superEndNode.num])
		{
			graph.followSets[i].items.push_back(graph.superEndNode);
		}
	}
}


#endif