#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <fstream>
#include <cstdio>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <set>
#include <ctime>
#include <cmath>
#include <thread>
#include <algorithm>
#include <mutex>

using namespace std;
typedef unsigned long index_t;
#define MATRIX unordered_map<index_t,unordered_map<index_t,float>>	
#define RECOMMEND unordered_map<index_t,unordered_map<index_t,float>>
#define REALRECOM unordered_map<index_t,vector<pair<index_t,float>>>
#define ALLRECOMMEND map<index_t,REALRECOM*>
#define SIMILAR unordered_map<index_t,vector<pair<index_t,float>>>
#define IUInvertTable unordered_map<index_t,unordered_map<index_t,vector<index_t>>>
#define UGROUP vector<index_t>
#define ULIST map<index_t, UGROUP>
#define USER set<index_t>
#define IUSET unordered_map<index_t,USER>
#define THREADNUM 4
#define K 10

class utils
{
public:
	utils();
	/*
		功能：统计所有测试数据的item数量
	*/
	static void countRealAll(MATRIX * U_I_test, index_t * cnt);
	/*
		功能：统计所有推荐数据的item数量
	*/
	static void countRecomAll(ALLRECOMMEND * ALL_U_Recommend_I, index_t * cnt);
	/*
		功能：统计hit数量
	*/
	static void countHitAll(MATRIX * U_I_test, ALLRECOMMEND * ALL_U_Recommend_I,index_t * cnt);
	/*
		功能：排序方式
	*/
	static bool sort_func(const pair<index_t, float> & u1,const pair<index_t, float> & u2);
	/*
		功能：求和
	*/
	static void count_vec(vector<index_t> & group,IUInvertTable* I_U_Table);
	/*
		功能：用户分组
	*/
	static void buildUserGroup(USER* users, ULIST* th_ulist);
	/*
		功能：载入数据
		数据格式： user id | item id | rating | timestamp.  
	*/
	static void loadSrcFile(const char * src_file,
		MATRIX * U_I_rating, IUSET* I_U_rating, USER* users, USER* items);
	static void loadTestFile(const char * test_file,MATRIX * U_I_test);
	static void loadFile(const char * src_file, const char * test_src_file,
		MATRIX * U_I_rating, IUSET* I_U_rating, USER* users, USER* items, MATRIX * U_I_test);
	/*
		功能：构建一组item的物品用户倒排表
	*/
	static void builOneGroupItemUserInvert(vector<index_t> & group, index_t th_num,
		IUInvertTable* I_U_Table, IUSET* I_U_Rating);
	/*
		功能：计算用户相似度
	*/
	static float  computingUserSimiar(index_t uid1, index_t uid2, MATRIX* U_I_rating,IUInvertTable* I_U_Table);
	/*
		功能：计算单个用户的相似度用户列表
	*/
	static void constructOneSimilarUserList(index_t uid, 
		USER* users,MATRIX* U_I_Rating, IUInvertTable* I_U_Table, SIMILAR* U_U_Simiar);
	/*
		功能：计算一组用户的相似度用户列表
	*/
	static 	void constructOneGroupSimilarUserList(vector<index_t>  userlist,
		USER* users, MATRIX* U_I_Rating, IUInvertTable* I_U_Table, SIMILAR* U_U_Simiar);
	/*
		功能：单个相似用户的推荐计算
	*/
	static void user2UserRecommendComputing(index_t uid1, index_t uid2, float simial, 
		MATRIX* U_I_Rating, RECOMMEND* U_Recommend_I);
	/*
		功能：一组相似用户的推荐计算
	*/
	static void user2GroupRecommendComputing(index_t iid, 
		SIMILAR* U_U_Simiar, MATRIX* U_I_Rating, RECOMMEND* U_Recommend_I);

	/*
		功能:一组目标用户的推荐计算
	*/
	static void Group2GroupRecommendComputing(UGROUP & usergroup,
		SIMILAR* U_U_Simiar, MATRIX* U_I_Rating, REALRECOM* U_Recommend_I);

	~utils();
};
#endif

