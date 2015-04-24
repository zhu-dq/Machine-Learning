#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
using namespace std;
#define MAXLEN 18//����ÿ�е����ݸ���
vector <vector <string> > state;//ʵ����
vector <string> item(MAXLEN);//��Ӧһ��ʵ����
vector <string> attribute_row;//�������м�����������
vector<float> v_value;
string yes("r");
string no("d");
string blank("");
map<string, vector < string > > map_attribute_values;//�洢���Զ�Ӧ�����е�ֵ
ofstream fout("out.txt");
struct Node{//�������ڵ�
	string attribute;//����ֵ
	string arrived_value;//���������ֵ
	vector<Node *> childs;//���еĺ���
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
//��������ʵ������������ֵ��ɵ�map
/*
	��ȡ������Ϣ��
	outlook��Sunny  Overcast  Rainy
	Temperature��Hot   Mild    Cool
	Humidity�� High  Normal
	Wind��  Weak  Strong
	������������
*/
void ComputeMapFrom2DVector(){
	unsigned int i, j, k;
	bool exited = false;
	vector<string> values;
	for (i = 1; i < MAXLEN - 1; i++){//�����б���
		for (j = 1; j < state.size(); j++)
		{
			for (k = 0; k < values.size(); k++)//һ��ʼvalues.size()��0
			{
				if (!values[k].compare(state[j][i]))//compare  ��ͬ��0  ������������Ѿ����� �� exited = true;
				{
					exited = true;
				}
			}//k---���������ľ���ֵ
			if (!exited){//���������  û�������
				values.push_back(state[j][i]);//ע��Vector�Ĳ��붼�Ǵ�ǰ�����ģ�ע�����it��ʼ��ָ��vectorͷ
			}
			exited = false;
		}//j  --�������ľ���ֵ
		map_attribute_values[state[0][i]] = values;//�� i=1ʱ����ô����Ӧ��������outlook,��values�е�ֵ�ǣ�Sunny  Overcast  Rainy
		values.erase(values.begin(), values.end());
	}//i  ---������
}
//���ݾ������Ժ�ֵ��������
double ComputeEntropy(vector <vector <string> > remain_state, string attribute, string value, bool ifparent){
	vector<int> count(2, 0);
	unsigned int i, j;
	bool done_flag = false;//�ڱ�ֵ �Ƿ��ҵ�����������
	for (j = 1; j < MAXLEN; j++)
	{
		if (done_flag) break;
		if (!attribute_row[j].compare(attribute)){//compare  ��ͬ��0  �ҵ�����Ӧ��������
			for (i = 1; i < remain_state.size(); i++){
				if ((!ifparent&&!remain_state[i][j].compare(value)) || ifparent){//ifparent��¼�Ƿ��㸸�ڵ� ���Ƿ���㲻���ֵ���� 
					if (!remain_state[i][MAXLEN - 1].compare(yes)){
						count[0]++;
					}
					else count[1]++;
				}
			}
			done_flag = true;
		}
	}//for
	if (count[0] == 0 || count[1] == 0) return 0;//ȫ������ʵ�����߸�ʵ��
	//��������� ����[+count[0],-count[1]],log2Ϊ��ͨ�����׹�ʽ������Ȼ������
	double sum = count[0] + count[1];
	double entropy = -count[0] / sum*log(count[0] / sum) / log(2.0) - count[1] / sum*log(count[1] / sum) / log(2.0);
	return entropy;
}
//���㰴������attribute���ֵ�ǰʣ��ʵ������Ϣ����
double ComputeGain(vector <vector <string> > remain_state, string attribute){
	unsigned int j, k, m;
	//������������ʱ����
	double parent_entropy = ComputeEntropy(remain_state, attribute, blank, true);//�ؼ���true  ������
	double children_entropy = 0;
	//Ȼ���������ֺ����ֵ����
	vector<string> values = map_attribute_values[attribute];
	vector<double> ratio;
	vector<int> count_values;
	int tempint;
	//----------������A��ÿ����������ֵ���ڵ�ǰ���ݼ��еĸ���
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
	//   �����������ֵ����Ӧ���� 
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
//�ҳ�������ռ��������
string MostCommonLabel(vector <vector <string> > remain_state){
	int p = 0, n = 0;
	for (unsigned i = 0; i < remain_state.size(); i++){
		if (!remain_state[i][MAXLEN - 1].compare(yes)) p++;
		else n++;
	}
	if (p >= n) return yes;
	else return no;
}
//�ж������Ƿ������Զ�Ϊlabel
bool AllTheSameLabel(vector <vector <string> > remain_state, string label){
	int count = 0;
	for (unsigned int i = 0; i < remain_state.size(); i++){
		if (!remain_state[i][MAXLEN - 1].compare(label)) count++;
	}
	if (count == remain_state.size() - 1) return true;
	else return false;
}
//������Ϣ���棬DFS����������
//current_nodeΪ��ǰ�Ľڵ�
//remain_stateΪʣ������������
//remian_attributeΪʣ�໹û�п��ǵ�����
//���ظ����ָ��
Node * BulidDecisionTreeDFS(Node * p, vector <vector <string> > remain_state, vector <string> remain_attribute){
	if (p == NULL)
		p = new Node();
	//�ȿ���������Ҷ�����
	//  ��������1
	if (AllTheSameLabel(remain_state, yes)){
		p->attribute = yes;
		return p;
	}
	//��������2
	if (AllTheSameLabel(remain_state, no)){
		p->attribute = no;
		return p;
	}
	//  ��������3
	if (remain_attribute.size() == 0){//���е����Ծ��Ѿ���������,��û�з־�
		string label = MostCommonLabel(remain_state);
		p->attribute = label;
		return p;
	}
	//�ҵ���������
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
	//�������max_itָ������������ֵ�ǰ���������������������Լ�
	vector <string> new_attribute;
	vector <vector <string> > new_state;
	//  ��ȡʣ�������ֵ
	for (vector <string>::iterator it2 = remain_attribute.begin(); it2 < remain_attribute.end(); it2++){
		if ((*it2).compare(*max_it)) new_attribute.push_back(*it2);
	}
	//ȷ������ѻ������ԣ�ע�Ᵽ��
	p->attribute = *max_it;
	vector <string> values = map_attribute_values[*max_it];//������������ֵ
	int attribue_num = FindAttriNumByName(*max_it);//�к�
	new_state.push_back(attribute_row);
	for (vector <string>::iterator it3 = values.begin(); it3 < values.end(); it3++)//ÿ�����������µ��������Ϻ����Լ���.
	{
		for (unsigned int i = 1; i < remain_state.size(); i++)
		{
			if (!remain_state[i][attribue_num].compare(*it3))
			{//��ȡ�����µ���������
				new_state.push_back(remain_state[i]);
			}
		}
		Node * new_node = new Node();
		new_node->arrived_value = *it3;
		if (new_state.size() == 0)
		{//��ʾ��ǰû�������֧����������ǰ��new_nodeΪҶ�ӽڵ�
			new_node->attribute = MostCommonLabel(remain_state);
		}
		else
			BulidDecisionTreeDFS(new_node, new_state, new_attribute);
		//�ݹ麯������ʱ������ʱ��Ҫ1 ���½����븸�ڵ㺢������ 2���new_state����
		p->childs.push_back(new_node);
		new_state.erase(new_state.begin() + 1, new_state.end());//ע�������new_state�е�ǰһ��ȡֵ��������׼��������һ��ȡֵ����
	}
	return p;
}
void Input(){
	ifstream fin("data.txt");
	if (!fin)
	{
		fout << "û�ҵ������ļ�" << std::endl;
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
		state.push_back(item);           //����һ������
	}
	//��¼��������ֵ
	for (int j = 0; j < MAXLEN; j++){
		attribute_row.push_back(state[0][j]);
	}
}
//----------------------------------�������ֵ
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
//--------------------------��������ֵ
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
//------------------------------------ͳ��Ҷ�ӽڵ����
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
// ---------------------------�����������
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
//---------��ӡ��
void PrintTree(Node *p, int depth,float c,float d){
	for (int i = 0; i < depth; i++) fout << '\t';//����������������tab
	if (!p->arrived_value.empty()){//��ͷ������
		fout <<""<<"\t"<< p->arrived_value ;
		for (int i = 0; i < depth + 1; i++) fout << '\t';//����������������tab
	}
	fout << -(float)(p->x) << "\t" << -(p->y) << "\t" << -c << "\t" << -d << "\t" << p->attribute << endl;//�ڵ������
	for (vector<Node*>::iterator it = p->childs.begin(); it != p->childs.end(); it++){
		PrintTree(*it, depth + 1,p->x,p->y);
	}
}
//  ------------��������
bool testdata(Node *p, vector<string> item)
{
	if (p == NULL) return false;
	if ((p != NULL) && (p->childs.empty()))//Ҷ�ڵ�
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
		fout << "û�ҵ������ļ�" << std::endl;
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
//-----------�ͷ���
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
	//--------------------------����
	vector <string> remain_attribute;//������ߵ����� A�ļ���
	for (vector<string>::iterator i = attribute_row.begin() + 1; i != attribute_row.end() - 1; i++)
		remain_attribute.push_back(*i);
	//-----------------------���ݼ�
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
	cout << "��ȷ�ʣ�\t"<<test(root) *100<<"%"<< endl;
	FreeTree(root);
	fout.close();
	system("pause");
	return 0;
}