#include "user_cfengine.h"

UCFEngine::UCFEngine(){
	
	/*
	this->U_I_Rating = shared_ptr<MATRIX>(new MATRIX);
	this->I_U_Rating = shared_ptr<IUSET>(new IUSET);
	this->I_U_Table = shared_ptr<IUInvertTable>(new IUInvertTable);
	this->users = shared_ptr<USER>(new USER);
	this->items = shared_ptr<USER>(new USER);
	this->U_U_Simiar = shared_ptr<SIMILAR>(new SIMILAR);
	this->ALL_U_Recommend_I = shared_ptr<ALLRECOMMEND>(new ALLRECOMMEND);
	this->th_ulist = shared_ptr<ULIST>(new ULIST);
	*/

	this->U_I_Rating = new MATRIX();
	this->I_U_Rating = new IUSET();
	this->U_I_test = new MATRIX();
	this->I_U_Table = new IUInvertTable();
	this->users = new USER();
	this->items =new USER();
	this->U_U_Simiar = new SIMILAR();
	this->ALL_U_Recommend_I =new ALLRECOMMEND();
	this->th_ulist = new ULIST();
}
UCFEngine::~UCFEngine()
{
	/*
		delete this->U_I_Rating;
		delete this->I_U_Rating;
		delete this->U_I_test;
		delete this->I_U_Table;
		delete this->users;
		delete this->items;
		delete this->U_U_Simiar;
		for (int i = 0; i < THREADNUM; ++i)
			delete (*ALL_U_Recommend_I)[i];
		delete this->ALL_U_Recommend_I;
		delete this->th_ulist;
	*/
}
void UCFEngine::init()const
{
	utils::loadFile("u1.base", "u1.test", U_I_Rating,I_U_Rating,users,items,U_I_test);
	utils::buildUserGroup(users, th_ulist);
	vector<index_t> u(users->begin(),users->end());
	index_t length = u.size();
	for (int i = 0; i < length; ++i)
	{
		(*I_U_Table)[u[i]][u[i]].assign(THREADNUM+1,0);

		for (int j = i + 1; j < length; ++j)
		{
			(*I_U_Table)[u[i]][u[j]].assign(THREADNUM+1,0);
			(*I_U_Table)[u[j]][u[i]].assign(THREADNUM+1,0);
		}
	}
	cout << "初始化成功。。。" << endl;
	cout << "商品个数：" << I_U_Rating->size() << endl;
	cout << "用户个数: " << U_I_Rating->size() <<endl;
}

void UCFEngine::constructItemUserInvertList()const
{
	
	map<index_t, vector<index_t>> itemList;
	vector<index_t> itemGroup(items->begin(),items->end());

	for (index_t i = 0; i < itemGroup.size(); ++i)
	{
		itemList[i%THREADNUM].push_back(itemGroup[i]);
	}

	vector<thread> thread_list;
	for (index_t i = 0; i < THREADNUM; ++i)
	{
		thread t(utils::builOneGroupItemUserInvert, itemList[i],i+1, I_U_Table, I_U_Rating);
		thread_list.push_back(move(t));
	}

	for (auto & elem : thread_list)
	{
		elem.join();
	}
	
	vector<thread> thread_list1;
	for (index_t i = 0; i < THREADNUM; ++i)
	{
		thread t(utils::count_vec, (*th_ulist)[i], I_U_Table);
		thread_list1.push_back(move(t));
	}

	for (auto & elem : thread_list1)
	{
		elem.join();
	}

	cout << "构建 物品-用户倒排表成功" << endl;
}

void UCFEngine::dumpItemUserInvertList()const
{
	ofstream fout("ItemUserInvertList.txt");
	vector<index_t> u(users->begin(), users->end());
	index_t length = u.size();
	fout << "\t";
	for (auto & elem : *users)
	{
		fout << elem << "\t";
	}
	fout << "\n";
	for (auto & elem : *users)
	{
		fout << elem << "\t";
		for (auto & item : *users)
		{
			fout << (*I_U_Table)[item][elem][0] << "\t";
		}
		fout << "\n";
	}
	fout.close();

	cout << "打印 物品-用户倒排表成功" << endl;
}

void UCFEngine::constructSimilarUserList()const
{
	vector<thread> thread_list;
	for (index_t i = 0; i < THREADNUM; ++i)
	{
		thread t(utils::constructOneGroupSimilarUserList, (*th_ulist)[i], users, U_I_Rating, I_U_Table, U_U_Simiar);
		thread_list.push_back(move(t));
	}
	for (auto & elem : thread_list)
	{
		elem.join();
	} 
	cout << "构建兴趣相投的用户集合成功..." << endl;
}

void UCFEngine::dumpSimilarUserList()const
{
	ofstream fout("SimilarUserList.txt");

	for (auto & desuser : *U_U_Simiar)
	{
		fout << desuser.first << "\t:\t";
		for (auto & similaruser : desuser.second)
		{
			fout << similaruser.first << "\t";
		}
		fout << "\n";
	}
	fout.close();

	cout << "打印 兴趣相投的用户集合成功..." << endl;
}

void UCFEngine::BuildRecommendItemList()const
{
	vector<thread> thread_list;
	for (index_t i = 0; i < THREADNUM; ++i)
	{
		REALRECOM* U_RealRcom_I = new REALRECOM();
		thread t(utils::Group2GroupRecommendComputing, (*th_ulist)[i], U_U_Simiar, U_I_Rating, U_RealRcom_I);
		(*ALL_U_Recommend_I)[i] = U_RealRcom_I;
		thread_list.push_back(move(t));
	}
	for (auto & elem : thread_list)
	{
		elem.join();
	}
	cout << "构建用户推荐列表成功..." << endl;
}

void UCFEngine::dumpRecommendItemList()const
{
	ofstream fout("RecommendItemList");

	for (auto & thread : *ALL_U_Recommend_I)
	{
		for (auto & user : *thread.second)
		{
			fout << user.first << "\t:\t";
			for (auto & item : user.second)
			{
				fout << item.first << "\t";
			}
			fout << "\n";
			
		}
	}
	fout.close();
	cout << "打印推荐商品列表" << endl;
}

void UCFEngine::testResult()const
{
	index_t * realAllCount = new index_t();
	index_t * recommendAllCount = new index_t();
	index_t	* hidAllcount = new index_t();
	thread t1(utils::countRealAll, this->U_I_test, realAllCount);
	thread t2(utils::countRecomAll, this->ALL_U_Recommend_I, recommendAllCount);
	thread t3(utils::countHitAll, U_I_test, ALL_U_Recommend_I, hidAllcount);
	t1.join();
	t2.join();
	t3.join();

	float precision = (float)(*hidAllcount) / (float)(*recommendAllCount);
	float recall = (float)(*hidAllcount) / (float)(*realAllCount);
	cout << "正确率 ： " << precision << endl;
	cout << "召回率 : " << recall << endl;

	delete realAllCount;
	delete recommendAllCount;
	delete hidAllcount;
}