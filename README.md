项目名称：LL1
项目描述：实现判断一个文法G是否是LL1的，若是则构造分析表，并据此分析所给的字符串
项目配置：VS2013
作   者：马黄雷 2018-11-20 V1.0

【简介】
	一个上下文无关文法是一个四元式： G = (Vt, Vn, S, P)
		Vt：终结符的非空有限集；
		Vn：非终结符的非空有限集；
		S：开始符号，属于非终结符；
		P：产生式的集合，产生式形如 A->α ，其中A∈Vn，α∈(Vt∪Vn)*.
	应编译原理课程要求，创建本项目。意在判断一个上下文无关文法G是否是LL1的，若是
则构造自上而下的语法分析表，并根据此表对输入的字符串进行分析，判断该字符串是否可以
由文法G产生，打印分析过程。

【项目输入】
	现阶段本程序采用读文件的方式来输入文法G，未来有可能会结合MFC框架做出图形界面
以丰富输入方式。注：文法G必须以一定的格式存在文件中，具体格式如下：
		文法名称：grapgName
		终结符VT：a b ^ ( ) + * ε	//以空格分隔，若有ε则ε必须放在最后
		非终结符VN：E E' T T' F F' P		//以空格分隔
		开始符号S：E
		产生式P：
		E->TE'		//产生式中不得有多余空格或其他字符
		E'->+E|ε
		T->FT'
		T'->T|ε
		F->PF'
		F'->*F'|ε
		P->(E)|a|b|^
		endp		//产生式结束标志

【项目输出】
	本项目有以下几种输出内容：
		1. 相关操作提示信息；
		2. 文法G的FFT(First Follow analyseTable)信息；
		3. 包含文法G的FFT(First Follow analyseTable)信息的.txt文件；
		4. 输入字符串的分析过程信息。

【计算过程】
	1. 从文件读取一个文法G并保持；
	2. 检查G是否满足LL1文法的条件一：
			产生式不含左递归
		若不满足则终止程序；
	3. 计算每个非终结符的First集，然后检查文法G是否满足LL1文法的条件二：
			if (A->α1|α2|...|αn) then (First(αi) ∩ First(αj) = ∅)
		若不满足则终止程序；
	4. 计算每个非终结符的Follow集，然后检查文法G是否满足LL1文法的条件三：
			if (ε ∈ First(A)) then (First(A) ∩ Follow(A) = ∅)
		若不满足则终止程序；
	5. 根据以求得的First集和Follow集构造分析表AnalyseTable；
	6. 打印FFT信息到屏幕(可选)，打印FFT信息到文件(可选)；
	7. 循环输入字符串并根据AnalyseTable进行分析，判断其是否是文法G的句型。若输入串为"#"则结束循环；
	8. 结束。

【数据结构】
	1. 保存终结符和非终结符
		typedef struct Node{
			int num;
			string name;
			bool isEndNode;
		}Node;

	2. 保存产生式
		typedef struct Rule{
			Node left;
			vector<Node> right;
			string name;
		}Rule;

	3. 保存First集和Follow集
		typedef struct Set{
			Node owner;
			vector<Node> items;
		}Set;

	4. 保存坐标
		typedef struct Point{
			int x;
			int y;
		}Point;

	5. 保存文法G

【主要算法】
	1. 求First集：

	2. 求Follow集：

	3. 求AnalyseTable：

	4. 分析字符串：
