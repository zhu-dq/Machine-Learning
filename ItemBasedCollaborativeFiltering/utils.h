//
// Created by zhudanqi on 16-4-13.
//

#ifndef ITEMBASEDCOLLABORATIVEFILTERING_UTILS_H
#define ITEMBASEDCOLLABORATIVEFILTERING_UTILS_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <set>
#include <algorithm>
#include <map>
#include <cmath>
using namespace std;
typedef unsigned long index_t;
#define MATRIX unordered_map<index_t,unordered_map<index_t,float>>
#define UIRT unordered_map<index_t,unordered_map<index_t,index_t>>
#define IUR unordered_map<index_t ,set<index_t>>
#define SIMILAR unordered_map<index_t,vector<pair<index_t,float>>>
#define REALRECOM unordered_map<index_t,vector<pair<index_t,float>>>
#define ALLRECOMMEND map<index_t,REALRECOM*>
#define UGROUP vector<index_t>
#define ULIST map<index_t, UGROUP>
#define THREADNUM 4
class utils {
public:
    static void loadTarinData( string  tarin_path,MATRIX * U_I_rating,IUR * I_U_rating, set<index_t > * itemset,set<index_t > * userset);
    static void loadTestData(string  test_path,MATRIX * U_I_test);
    static bool sort_func(const pair<index_t,float> & a,const pair<index_t ,float > & b);
    /*
     * 功能：用户分组
     */
    static void buildUserGroup(set<index_t > * users, ULIST * th_ulist);
    /*
     * 功能：计算用户对物品的兴趣并构建推荐列表
     */
    static void Group2GroupRecommendComputing(vector<index_t > ulist,
                                              SIMILAR  * I_I_Similar,
                                              set<index_t > * itemset,
                                              MATRIX  * U_I_Rating,
                                              REALRECOM * U_RealRcom_I,
                                              index_t k);

    /*
     *功能：统计所有测试数据的item数量
     */
    static void countRealAll(MATRIX * U_I_test, index_t * cnt);
    /*
     *功能：统计所有推荐数据的item数量
     */
    static void countRecomAll(MATRIX * U_I_test,ALLRECOMMEND * ALL_U_Recommend_I, index_t * cnt,index_t * test_cnt);
    /*
     *功能：统计hit数量
     */
    static void countHitAll(MATRIX * U_I_test, ALLRECOMMEND * ALL_U_Recommend_I,index_t * cnt);
};
#endif //ITEMBASEDCOLLABORATIVEFILTERING_UTILS_H