//
// Created by zhudanqi on 16-4-13.
//

#include "utils.h"
void utils::loadTarinData(string  tarin_path, MATRIX * U_I_rating,IUR * I_U_rating ,set<index_t > * itemset,set<index_t > * userset)
{
    fstream fin(tarin_path);
    if(!fin)
    {
        cout<<" tarin_data path is error !"<<endl;
    }
    string line;
    index_t uid = 0;
    index_t iid = 0;
    float  rating = 0;
    index_t  date = 0;
    while (getline(fin,line))
    {
        stringstream ss_line(line);
        ss_line>>uid;
        ss_line>>iid;
        ss_line>>rating;
        ss_line>>date;
        (*U_I_rating)[uid][iid] = rating;
        (*I_U_rating)[iid].insert(uid);
        (*itemset).insert(iid);
        (*userset).insert(uid);
    }
    cout<<"tarin data load finish "<<endl;
    cout<<"user count : "<<(*U_I_rating).size()<<endl;
    cout<<"item count : "<<(*I_U_rating).size()<<endl;
}
void utils::loadTestData(string  test_path, MATRIX * U_I_test)
{
    fstream fin(test_path);
    if(!fin)
    {
        cout<<" tarin_data path is error !"<<endl;
    }
    string line;
    index_t uid = 0;
    index_t iid = 0;
    float  rating = 0;
    index_t  date = 0;
    while (getline(fin,line))
    {
        stringstream ss_line(line);
        ss_line>>uid;
        ss_line>>iid;
        ss_line>>rating;
        ss_line>>date;
        (*U_I_test)[uid][iid] = rating;
    }
    cout<<"test data load finish "<<endl;
    cout<<"user count : "<<(*U_I_test).size()<<endl;
}
bool utils::sort_func(const pair<index_t, float> &a, const pair<index_t, float> &b)
{
    return a.second>b.second;
}
void utils::buildUserGroup(set<index_t > * users, ULIST * th_ulist)
{
    vector<index_t> u(users->begin(), users->end());
    for (index_t i = 0; i < u.size(); ++i)
    {
        (*th_ulist)[i%THREADNUM].push_back(u[i]);
    }
}
void utils::Group2GroupRecommendComputing(vector<index_t> ulist, SIMILAR *I_I_Similar, set<index_t> *itemset,
                                          MATRIX *U_I_Rating, REALRECOM *U_RealRcom_I,index_t k)
{
    for(auto uid : ulist)
    {
        for(auto & iid : *itemset)
        {
            if((*U_I_Rating)[uid].count(iid))// mai guo
                continue;
            float puj = 0.0;
            for(auto & similar_iid : (*I_I_Similar)[iid])
            {
                if((*U_I_Rating)[uid].count(similar_iid.first))
                    puj += similar_iid.second * (*U_I_Rating)[uid][similar_iid.first];//wji * rui
            }
            (*U_RealRcom_I)[uid].push_back(pair<index_t ,float >(iid,puj));
        }
    }
    //qu qian k ge
    for(auto & recommendItem :(*U_RealRcom_I))
    {
        if(recommendItem.second.size()<=k)
            continue;
        vector<pair<index_t,float>> v = recommendItem.second;
        sort(v.begin(),v.end(),utils::sort_func);
        //recommendItem.second.clear();
        //copy(v.begin(),v.begin()+k,back_inserter(recommendItem.second));
        (*U_RealRcom_I)[recommendItem.first] =vector<pair<index_t,float>>(v.begin(),v.begin()+k);
    }
}
void utils::countRealAll(MATRIX * U_I_test, index_t * cnt)
{
    *cnt = 0;
    for (auto & userref : *U_I_test)
    {
        for (auto & item : userref.second)
            (*cnt)++;
    }
}
void utils::countRecomAll(MATRIX * U_I_test,ALLRECOMMEND * ALL_U_Recommend_I, index_t * cnt,index_t * test_cnt)
{
    *cnt = 0;
    *test_cnt=0;
    for (auto & threads : *ALL_U_Recommend_I)
    {
        for (auto & userref : *threads.second)
        {
            for (auto & item : userref.second)
                (*cnt)++;
            if((*U_I_test).count(userref.first))
            {
                for (auto & item : userref.second)
                    (*test_cnt)++;
            }
        }
    }
}

void utils::countHitAll(MATRIX * U_I_test, ALLRECOMMEND * ALL_U_Recommend_I, index_t * cnt)
{
    *cnt = 0;
    for (auto & threads : *ALL_U_Recommend_I)
    {
        for (auto & userref : *threads.second)
        {
            for (auto & item : userref.second)
            {
                if ((*U_I_test)[userref.first].count(item.first))
                    (*cnt)++;
            }
        }
    }
}