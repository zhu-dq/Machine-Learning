#include "utils.h"
mutex g_lock;

utils::utils()
{
}

bool utils::sort_func(const pair<index_t, float> & u1,const  pair<index_t, float> & u2)
{
	return u1.second > u2.second;
}

void utils::countRealAll(MATRIX * U_I_test, index_t * cnt)
{
	*cnt = 0;
	for (auto & items : *U_I_test)
	{
		for (auto & item : items.second)
			(*cnt)++;
	}
}

void utils::countRecomAll(ALLRECOMMEND * ALL_U_Recommend_I, index_t * cnt)
{
	*cnt = 0;
	for (auto & threads : *ALL_U_Recommend_I)
	{
		for (auto & items : *threads.second)
		{
			for (auto & item : items.second)
				(*cnt)++;
		}
	}
}

void utils::countHitAll(MATRIX * U_I_test, ALLRECOMMEND * ALL_U_Recommend_I, index_t * cnt)
{
	*cnt = 0;
	for (auto & threads : *ALL_U_Recommend_I)
	{
		for (auto & items : *threads.second)
		{
			for (auto & item : items.second)
			{
				if ((*U_I_test)[items.first].count(item.first))
					(*cnt)++;
			}
		}

	}
}

void utils::buildUserGroup(USER* users, ULIST* th_ulist)
{
	vector<index_t> u(users->begin(), users->end());
	for (index_t i = 0; i < u.size(); ++i)
	{
		(*th_ulist)[i%THREADNUM].push_back(u[i]);
	}
}

void utils::loadSrcFile(const char * src_file,
	MATRIX * U_I_rating, IUSET* I_U_rating, USER* users, USER* items)
{
	ifstream fin(src_file);
	if (!fin)
		cout << "Source file address is incorrect" << endl;
	string line;
	index_t uid = 0, iid = 0, timestamp = 0;
	float rating = 0.0;
	while (getline(fin, line))
	{
		sscanf_s(line.c_str(), "%u\t%u\t%f\t%u", &uid, &iid, &rating, &timestamp);
		(*U_I_rating)[uid][iid] = rating;
		(*I_U_rating)[iid].insert(uid);
		(*users).insert(uid);
		(*items).insert(iid);
	}

}

void utils::loadTestFile(const char * test_file, MATRIX * U_I_test)
{
	ifstream fin(test_file);
	if (!fin)
		cout << "test file address is incorrect" << endl;
	string line;
	index_t uid = 0, iid = 0, timestamp = 0;
	float rating = 0.0;
	while (getline(fin, line))
	{
		sscanf_s(line.c_str(), "%u\t%u\t%f\t%u", &uid, &iid, &rating, &timestamp);
		(*U_I_test)[uid][iid] = rating;
	}
}

void utils::loadFile(const char * src_file, const char * test_src_file,
	MATRIX * U_I_rating, IUSET* I_U_rating, USER* users, USER* items, MATRIX * U_I_test)
{
	thread t1(loadSrcFile, src_file, U_I_rating, I_U_rating, users, items);
	thread t2(loadTestFile, test_src_file, U_I_test);
	t1.join();
	t2.join();
}

void utils::count_vec(vector<index_t> & group, IUInvertTable* I_U_Table)
{
	for (auto & elem : group)
	{
		for (auto & item : (*I_U_Table)[elem])//map
		{
			for (index_t i = 0; i < item.second.size(); ++i)
			{
				item.second[0] += item.second[i];
			}
		}
	}
}

void utils::builOneGroupItemUserInvert(vector<index_t> & group, index_t th_num,
	IUInvertTable* I_U_Table, IUSET* I_U_Rating)
{
	for (auto & elem : group)
	{
		vector<index_t> users((*I_U_Rating)[elem].begin(), (*I_U_Rating)[elem].end());
		index_t length = users.size();
		for (int i = 0; i < length; ++i)
		{
			for (int j = i + 1; j < length; ++j)
			{
				(*I_U_Table)[users[i]][users[j]][th_num] += 1;
				(*I_U_Table)[users[j]][users[i]][th_num] += 1;
			}
		}
	}
}

float  utils::computingUserSimiar(index_t uid1, index_t uid2, MATRIX* U_I_rating, IUInvertTable* I_U_Table)
{

	index_t divisor = (*I_U_Table)[uid1][uid2][0];
	if (divisor == 0)
	{
		return 0.0;
	}
	index_t u1_length = (*U_I_rating)[uid1].size();
	index_t u2_length = (*U_I_rating)[uid2].size();
	float dividend = sqrt(u1_length*u2_length);
	return ((float)divisor) / dividend;
}

void utils::constructOneSimilarUserList(index_t uid, 
	USER* users, MATRIX* U_I_Rating, IUInvertTable* I_U_Table, SIMILAR* U_U_Simiar)
{
	vector<pair<index_t, float>> similarUser;
	for (auto & item : *users)
	{
		similarUser.push_back(make_pair(item, utils::computingUserSimiar(uid, item, U_I_Rating, I_U_Table)));
	}
	sort(similarUser.begin(), similarUser.end(), sort_func);
	lock_guard<mutex> lck(g_lock);
	if (similarUser.size() < K)
		(*U_U_Simiar)[uid] = similarUser;
	else
		copy(similarUser.begin(), similarUser.begin() + K, back_inserter((*U_U_Simiar)[uid]));
}


void utils::constructOneGroupSimilarUserList(vector<index_t>  userlist,
	USER* users, MATRIX* U_I_Rating, IUInvertTable* I_U_Table, SIMILAR* U_U_Simiar)
{
	for (auto & elem : userlist)
	{
		constructOneSimilarUserList(elem, users, U_I_Rating, I_U_Table, U_U_Simiar);
	}

}

void utils::user2UserRecommendComputing(index_t uid1, index_t uid2, float simial, 
	MATRIX* U_I_Rating, RECOMMEND* U_Recommend_I)
{
	for (auto & elem : (*U_I_Rating)[uid2])
	{
		if ((*U_I_Rating)[uid1].count(elem.first))
			continue;
		float Pui = simial*elem.second;
		if ((*U_Recommend_I)[uid1].count(elem.first))
		{
			(*U_Recommend_I)[uid1][elem.first] += Pui;
		}
		else
		{
			(*U_Recommend_I)[uid1][elem.first] = Pui;
		}
	}
}

void utils::user2GroupRecommendComputing(index_t iid, 
	SIMILAR* U_U_Simiar, MATRIX* U_I_Rating, RECOMMEND* U_Recommend_I)
{
	for (auto & elem : (*U_U_Simiar)[iid])
	{
		user2UserRecommendComputing(iid, elem.first, elem.second, U_I_Rating, U_Recommend_I);

	}
}

void utils::Group2GroupRecommendComputing(UGROUP & usergroup,
	SIMILAR* U_U_Simiar, MATRIX* U_I_Rating, REALRECOM* U_Recommend_I)
{
	for (auto & elem : usergroup)
	{
		RECOMMEND * recommend = new RECOMMEND();
		user2GroupRecommendComputing(elem, U_U_Simiar, U_I_Rating, recommend);
		vector<pair<index_t, float>> v((*recommend)[elem].begin(), (*recommend)[elem].end());
		sort(v.begin(), v.end(), sort_func);
		if (v.size() < K)
			copy(v.begin(), v.end(), back_inserter((*U_Recommend_I)[elem]));
		else
			copy(v.begin(), v.begin() + K, back_inserter((*U_Recommend_I)[elem]));
		delete recommend;
	}
}

utils::~utils()
{
}
