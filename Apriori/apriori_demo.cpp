#include "bits/stdc++.h"
using namespace std;
#define  MAP map<vector<char>,int>
#define ITEMS vector<char>
#define  FOR_MAP(iter,M) for(MAP::iterator (iter) = (M).begin();(iter) != (M).end();(iter)++)
#define MIN_SUP  2

MAP candidate;
MAP frequent;

void candidate1()
{
	ifstream fin("input1.txt");
	if(!fin){
		std::cout<<"open error !"<<std::endl;
	}
	char in_char;
	ITEMS items;
	while(fin>>in_char) 
	{
		if(in_char=='0')
			continue;
        	items.push_back(in_char);
        if(candidate.count(items))
            candidate[items]++;
        else
            candidate[items] = 1;
        items.clear();
	}
    fin.close();
}

void print_F(MAP m)
{
	FOR_MAP(iter, m)
	{
		ITEMS item = iter->first;
		std::cout<<"{  ";
		for(ITEMS::iterator i = item.begin(); i != item.end(); i++)
			std::cout<<(*i)<<"  ";
		std::cout<<"  }   -----support------------------>   "<< iter->second<<endl;
	}
}
void frequent1()
{
    FOR_MAP(iter, candidate)
    {
        if(iter->second >= MIN_SUP)
            frequent[iter->first] = iter->second;
    }
}

bool compareab(ITEMS a,ITEMS b)
{
	bool flag=true;
	for (int i = 0; i < a.size()-1; ++i)
	{
		if (a[i]!=b[i])
		{
			flag=false;
			break;
		}
	}

	return flag;
}
void generate_C()
{
	candidate.clear();
	FOR_MAP(iter, frequent)
	{
		for(MAP::iterator i= iter;i != frequent.end();i++)
		{
			if(i == iter)
				continue;
			ITEMS item1,item2;
			item1 = iter->first;
			item2 = i->first;
			if(compareab(item1,item2))
			{
				item1.push_back(item2.back());
				sort(item1.begin(),item1.end());
				candidate[item1] = 0;
			}
			item1.clear();
			item2.clear();
		}
	}
}
void prune()
{
	ITEMS item1,item2;
	FOR_MAP(iter, candidate)
	{
		item1 = iter->first;
		for(int i = 0;i<item1.size();i++)
		{
			for(int j =0; j<item1.size();j++)
			{
				if(i==j)
					continue;
				item2.push_back(item1[j]);
			}
			if(!frequent.count(item2))
			{
				iter->second = -1;
				item2.clear();
				break;
			}
			item2.clear();
		}
		item1.clear();
		item2.clear();
	}

	MAP after_prune;
	FOR_MAP(iter, candidate)
	{
		if(iter->second  != -1)
			after_prune[iter->first] = iter->second;
	}
	candidate.clear();
	candidate = after_prune;
}

void count_after_prune(ITEMS item2)
{
	FOR_MAP(iter, candidate)
	{
		ITEMS item1 = iter->first;
		int cnt = 0;
		if(item1.size()>item2.size())
		{
			item1.clear();
			continue;
		}
		for(int i = 0; i < item1.size();i++)
		{
			for(int j = 0; j < item2.size(); j++)
			{
				if(item1[i] == item2[j])
				{
					cnt++;
					break;
				}
			}
		}
		if(cnt == item1.size())
			iter->second++;
		item1.clear();
	}
}
void scan_DB_sup()
{
	ifstream fin("input1.txt");
	if(!fin)
		std::cout<<"open error !"<<std::endl;
	char in_char;
	ITEMS  item;
	while(fin>>in_char) {
	    if(in_char == '0'  &&  item.size()>0)
	    {
	    	count_after_prune(item);
	    	item.clear();
	    }else if (in_char != '0')
	    {
	    	item.push_back(in_char);
	    }
	}
	fin.close();
}
void generate_F()
{
	frequent.clear();
	FOR_MAP(iter, candidate)
	{
		if(iter->second>=MIN_SUP)
			frequent[iter->first] = iter->second;
	}
}

int main()
{
	candidate1();
	frequent1();
	if(frequent.size() == 0)
		return 0;
	print_F(frequent);
	while(true) {
		generate_C();
		if(candidate.size() == 0)
			break;
		prune();
		if(candidate.size() == 0)
			break;
		scan_DB_sup();
		generate_F();
		if(frequent.size() == 0)
			break;
		print_F(frequent);
	}
	return 0;
}
