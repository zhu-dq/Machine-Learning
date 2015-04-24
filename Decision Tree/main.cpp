#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
using namespace std;
#define MAXLEN 18//输入每行的数据个数
vector <vector <string> > state;//实例集
vector <string> item(MAXLEN);//对应一行实例集
vector <string> attribute_row;//保存首行即属性行数据
vector<float> v_value;
string yes("r");
string no("d");
string blank("");
map<string, vector < string > > map_attribute_values;//存储属性对应的所有的值
ofstream fout("out.txt");
struct Node{//决策树节点
	string attribute;//属性值
	string arrived_value;//到达的属性值
	vector<Node *> childs;//所有的孩子
	float x;
	float y;
	Node(){
		attribute = blank;
		arrived_value = blank;
		x = 0.0;
		y = 0.0;
	}
};
float x_node = 0.0;
float y_node = 0.0;
Node * root;
//根据数据实例计算属性与值组成的map
/*
	获取如下信息：
	outlook：Sunny  Overcast  Rainy
	Temperature：Hot   Mild    Cool
	Humidity： High  Normal
	Wind：  Weak  Strong
	。。。。。。
*/
void ComputeMapFrom2DVector(){
	unsigned int i, j, k;
	bool exited = false;
	vector<string> values;
	for (i = 1; i < MAXLEN - 1; i++){//按照列遍历
		for (j = 1; j < state.size(); j++)
		{
			for (k = 0; k < values.size(); k++)//一开始values.size()是0
			{
				if (!values[k].compare(state[j][i]))//compare  相同是0  即如果此属性已经有了 则 exited = true;
				{
					exited = true;
				}
			}//k---》存特征的具体值
			if (!exited){//如果此属性  没有则添加
				values.push_back(state[j][i]);//注意Vector的插入都是从前面插入的，注意更新it，始终指向vector头
			}
			exited = false;
		}//j  --》特征的具体值
		map_attribute_values[state[0][i]] = values;//当 i=1时，那么所对应的特征是outlook,则values中的值是：Sunny  Overcast  Rainy
		values.erase(values.begin(), values.end());
	}//i  ---》特征
}
//根据具体属性和值来计算熵
double ComputeEntropy(vector <vector <string> > remain_state, string attribute, string value, bool ifparent){
	vector<int> count(2, 0);
	unsigned int i, j;
	bool done_flag = false;//哨兵值 是否找到特征所在列
	for (j = 1; j < MAXLEN; j++)
	{
		if (done_flag) break;
		if (!attribute_row[j].compare(attribute)){//compare  相同是0  找到含相应特征的列
			for (i = 1; i < remain_state.size(); i++){
				if ((!ifparent&&!remain_state[i][j].compare(value)) || ifparent){//ifparent记录是否算父节点 即是否计算不划分的情况 
					if (!remain_state[i][MAXLEN - 1].compare(yes)){
						count[0]++;
					}
					else count[1]++;
				}
			}
			done_flag = true;
		}
	}//for
	if (count[0] == 0 || count[1] == 0) return 0;//全部是正实例或者负实例
	//具体计算熵 根据[+count[0],-count[1]],log2为底通过换底公式换成自然数底数
	double sum = count[0] + count[1];
	double entropy = -count[0] / sum*log(count[0] / sum) / log(2.0) - count[1] / sum*log(count[1] / sum) / log(2.0);
	return entropy;
}
//计算按照属性attribute划分当前剩余实例的信息增益
double ComputeGain(vector <vector <string> > remain_state, string attribute){
	unsigned int j, k, m;
	//首先求不做划分时的熵
	double parent_entropy = ComputeEntropy(remain_state, attribute, blank, true);//关键在true  不划分
	double children_entropy = 0;
	//然后求做划分后各个值的熵
	vector<string> values = map_attribute_values[attribute];
	vector<double> ratio;
	vector<int> count_values;
	int tempint;
	//----------求特征A的每个具体属性值，在当前数据集中的个数
	for (m = 0; m < values.size(); m++){
		tempint = 0;
		for (k = 1; k < MAXLEN - 1; k++){
			if (!attribute_row[k].compare(attribute)){
				for (j = 1; j < remain_state.size(); j++){
					if (!remain_state[j][k].compare(values[m])){
						tempint++;
					}
				}
			}
		}
		count_values.push_back(tempint);
	}
	//   计算具体特征值的相应概率 
	for (j = 0; j < values.size(); j++){
		ratio.push_back((double)count_values[j] / (double)(remain_state.size() - 1));
	}
	double temp_entropy;
	for (j = 0; j < values.size(); j++){
		temp_entropy = ComputeEntropy(remain_state, attribute, values[j], false);
		children_entropy += ratio[j] * temp_entropy;
	}
	return (parent_entropy - children_entropy);
}
int FindAttriNumByName(string attri){
	for (int i = 0; i < MAXLEN; i++){
		if (!state[0][i].compare(attri)) return i;
	}
	cerr << "can't find the numth of attribute" << endl;
	return 0;
}
//找出样例中占多数的类
string MostCommonLabel(vector <vector <string> > remain_state){
	int p = 0, n = 0;
	for (unsigned i = 0; i < remain_state.size(); i++){
		if (!remain_state[i][MAXLEN - 1].compare(yes)) p++;
		else n++;
	}
	if (p >= n) return yes;
	else return no;
}
//判断样例是否正负性都为label
bool AllTheSameLabel(vector <vector <string> > remain_state, string label){
	int count = 0;
	for (unsigned int i = 0; i < remain_state.size(); i++){
		if (!remain_state[i][MAXLEN - 1].compare(label)) count++;
	}
	if (count == remain_state.size() - 1) return true;
	else return false;
}
//计算信息增益，DFS构建决策树
//current_node为当前的节点
//remain_state为剩余待分类的样例
//remian_attribute为剩余还没有考虑的属性
//返回根结点指针
Node * BulidDecisionTreeDFS(Node * p, vector <vector <string> > remain_state, vector <string> remain_attribute){
	if (p == NULL)
		p = new Node();
	//先看搜索到树叶的情况
	//  结束条件1
	if (AllTheSameLabel(remain_state, yes)){
		p->attribute = yes;
		return p;
	}
	//结束条件2
	if (AllTheSameLabel(remain_state, no)){
		p->attribute = no;
		return p;
	}
	//  结束条件3
	if (remain_attribute.size() == 0){//所有的属性均已经考虑完了,还没有分尽
		string label = MostCommonLabel(remain_state);
		p->attribute = label;
		return p;
	}
	//找到最优特征
	double max_gain = 0, temp_gain;
	vector <string>::iterator max_it = remain_attribute.begin();
	vector <string>::iterator it1;
	for (it1 = remain_attribute.begin(); it1 < remain_attribute.end(); it1++){
		temp_gain = ComputeGain(remain_state, (*it1));
		if (temp_gain > max_gain) {
			max_gain = temp_gain;
			max_it = it1;
		}
	}
	//下面根据max_it指向的属性来划分当前样例，更新样例集和属性集
	vector <string> new_attribute;
	vector <vector <string> > new_state;
	//  获取剩余的特征值
	for (vector <string>::iterator it2 = remain_attribute.begin(); it2 < remain_attribute.end(); it2++){
		if ((*it2).compare(*max_it)) new_attribute.push_back(*it2);
	}
	//确定了最佳划分属性，注意保存
	p->attribute = *max_it;
	vector <string> values = map_attribute_values[*max_it];//最优特征特征值
	int attribue_num = FindAttriNumByName(*max_it);//行号
	new_state.push_back(attribute_row);
	for (vector <string>::iterator it3 = values.begin(); it3 < values.end(); it3++)//每个具体特征下的样本集合和属性集合.
	{
		for (unsigned int i = 1; i < remain_state.size(); i++)
		{
			if (!remain_state[i][attribue_num].compare(*it3))
			{//获取特征下的样本集合
				new_state.push_back(remain_state[i]);
			}
		}
		Node * new_node = new Node();
		new_node->arrived_value = *it3;
		if (new_state.size() == 0)
		{//表示当前没有这个分支的样例，当前的new_node为叶子节点
			new_node->attribute = MostCommonLabel(remain_state);
		}
		else
			BulidDecisionTreeDFS(new_node, new_state, new_attribute);
		//递归函数返回时即回溯时需要1 将新结点加入父节点孩子容器 2清除new_state容器
		p->childs.push_back(new_node);
		new_state.erase(new_state.begin() + 1, new_state.end());//注意先清空new_state中的前一个取值的样例，准备遍历下一个取值样例
	}
	return p;
}
void Input(){
	ifstream fin("data.txt");
	if (!fin)
	{
		fout << "没找到数据文件" << std::endl;
	}
	string str;
	string s;
	while (getline(fin, str))
	{	
		int i = 0;
		istringstream stream(str);
		while ((stream >> s)&&(s.compare("end")!=0))
		{
			item[i++] = s;
		}
		state.push_back(item);           //存入一条数据
	}
	//记录所有特征值
	for (int j = 0; j < MAXLEN; j++){
		attribute_row.push_back(state[0][j]);
	}
}
//----------------------------------添加坐标值
void addnodexy(Node *p,float x,float y,int ceng)
{
	if (p == NULL)
		return;
	p->x = x;
	p->y = y;
	if (v_value.size()<ceng)
		v_value.push_back(0.0);
	float x_value = (float)v_value[ceng-1];
	if ((p != NULL) && (p->childs.empty()))
		return ;
	for (vector<Node*>::iterator it = p->childs.begin(); it != p->childs.end(); it++)
	{
			v_value[ceng - 1] = (float)(v_value[ceng - 1] + 20);
			addnodexy(*it, (float)(v_value[ceng - 1]+20), y + 50, ceng+1);
	}
}
	/*float getmaxX()
	{
		float max = 0.0;
		for (vector<float>::iterator i = v_value.begin(); i != v_value.end(); i++)
		{
			if (*i > max)
				max = *i;
		}
		return max;
	}*/
//--------------------------修正坐标值
/*void xiuzhengnodexy(Node *p,  int ceng,float max)
{
	if (p == NULL)
		return;
	p->x = ((max - v_value[ceng - 1]) / 2) + p->x;
	if ((p != NULL) && (p->childs.empty()))
		return;
	for (vector<Node*>::iterator it = p->childs.begin(); it != p->childs.end(); it++)
	{
		xiuzhengnodexy(*it,ceng+1,max);
	}
}*/
//------------------------------------统计叶子节点个数
/*int   getNumLeafs(Node *p){
	int maxDepth = 0;
	if ((p != NULL) && (p->childs.empty()))
	{
		return 1;
	}
	for (vector<Node*>::iterator it = p->childs.begin(); it != p->childs.end(); it++)
	{
		maxDepth += getNumLeafs(*it);
	}
	return  maxDepth;
}*/
// ---------------------------计算树的深度
/*int   getTreeDepth(Node *p){
	int maxDepth = 0;
	if (p == NULL)
	{
		return maxDepth;
	}
	if (p->childs.empty())
	{
		if (1>maxDepth)
			maxDepth = 1;
		return maxDepth;
	}
	for (vector<Node*>::iterator it = p->childs.begin(); it != p->childs.end(); it++)
	{
		if ((getTreeDepth(*it) + 1)>maxDepth)
		{
			maxDepth = getTreeDepth(*it) + 1;
		}
	}
	return  maxDepth;
}*/
//---------打印树
void PrintTree(Node *p, int depth,float c,float d){
	for (int i = 0; i < depth; i++) fout << '\t';//按照树的深度先输出tab
	if (!p->arrived_value.empty()){//箭头的内容
		fout <<""<<"\t"<< p->arrived_value ;
		for (int i = 0; i < depth + 1; i++) fout << '\t';//按照树的深度先输出tab
	}
	fout << -(float)(p->x) << "\t" << -(p->y) << "\t" << -c << "\t" << -d << "\t" << p->attribute << endl;//节点的内容
	for (vector<Node*>::iterator it = p->childs.begin(); it != p->childs.end(); it++){
		PrintTree(*it, depth + 1,p->x,p->y);
	}
}
//  ------------测试数据
bool testdata(Node *p, vector<string> item)
{
	if (p == NULL) return false;
	if ((p != NULL) && (p->childs.empty()))//叶节点
	{
		if (!p->attribute.compare(item[17]))
		{
			return  true;
		}
		else
		{
			return false;
		}
	}
	for (vector<Node*>::iterator it = p->childs.begin(); it != p->childs.end(); it++)
	{
		int i = FindAttriNumByName(p->attribute);
		if (!((*it)->arrived_value.compare(item[i])))
		{
			return testdata(*it, item);
		}
	}
}
float test(Node *p)
{
	ifstream fin("test.txt");
	if (!fin)
	{
		fout << "没找到数据文件" << std::endl;
	}
	string str;
	string s;
	int cunt1 = 0;
	int cunt2 = 0;
	while (getline(fin, str))
	{
		int i = 0;
		istringstream stream(str);
		while ((stream >> s) && (s.compare("end") != 0))
		{
			item[i++] = s;
		}
		if (testdata(p, item)){cunt1++;
		}else{cunt2++;}
		//item.clear();
	}
	if ((cunt1 + cunt2) == 0){return 0.0;}
	return (float)((float)cunt1 /(float) (cunt1 + cunt2));
}
//-----------释放树
void FreeTree(Node *p){
	if (p == NULL)
		return;
	for (vector<Node*>::iterator it = p->childs.begin(); it != p->childs.end(); it++){
		FreeTree(*it);
	}
	delete p;
}

int main(){
	Input();
	//--------------------------特征
	vector <string> remain_attribute;//参与决策的特征 A的集合
	for (vector<string>::iterator i = attribute_row.begin() + 1; i != attribute_row.end() - 1; i++)
		remain_attribute.push_back(*i);
	//-----------------------数据集
	vector <vector <string> > remain_state;
	for (unsigned int i = 0; i < state.size(); i++){
		remain_state.push_back(state[i]);
	}
	ComputeMapFrom2DVector();
	root = BulidDecisionTreeDFS(root, remain_state, remain_attribute);
	addnodexy(root,0,0,1);
	//xiuzhengnodexy(root, 1,getmaxX());
	fout << ".\t" ;
	PrintTree(root, 0, 0, 1.0);
	cout << "正确率：\t"<<test(root) *100<<"%"<< endl;
	FreeTree(root);
	fout.close();
	system("pause");
	return 0;
}