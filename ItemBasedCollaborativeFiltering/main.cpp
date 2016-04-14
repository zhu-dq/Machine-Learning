#include <iostream>
#include "item_cfengine.h"
using namespace std;

int main() {
    ICFEngine cf;
    time_t start , stop,allstart,allstop;
    start = time(NULL);
    allstart = time(NULL);
    cf.init("/home/zhudanqi/ClionProjects/ItemBasedCollaborativeFiltering/u1.base",
            "/home/zhudanqi/ClionProjects/ItemBasedCollaborativeFiltering/u1.test",11);
    stop = time(NULL);
    cout<<"init time : "<<stop-start<<endl;
    cf.constructUserItemTable();
    start = time(NULL);
    cout<<"construct UserItemTable cost time : "<<start-stop<<endl;
    cf.constructSimilarItemList();
    stop = time(NULL);
    cout<<"construct SimilarItemList cost time : "<<stop - start<<endl;
    cf.BuildRecommendItemList();
    start = time(NULL);
    cout<<"build Recommenditemlist : "<<start-stop<<endl;
    //cf.showRecommmendItem();
    //stop = time(NULL);
   // cout<<"show recommend time : "<<stop-start<<endl;
    cf.testResult();
    start = time(NULL);
    cout<<"test result time : "<<start - stop<<endl;

    allstop = time(NULL);
    cout<<"all cost : "<<allstop - allstart<<endl;
/*
    //test 1
    vector<pair<index_t ,float>> v;
    v.push_back(pair<index_t ,float >(1,55));
    v.push_back(pair<index_t ,float >(2,99));
    v.push_back(pair<index_t ,float >(3,44));
    sort(v.begin(),v.end(),utils::sort_func);
    for(auto & e : v)
    {
        cout<<e.first<<" : "<<e.second<<endl;
    }
    //test 2
    vector<pair<index_t ,float>> m;
    m.push_back(pair<index_t ,float >(1,15));
    m.push_back(pair<index_t ,float >(2,19));
    m.push_back(pair<index_t ,float >(3,14));
    m.clear();
    copy(v.begin(),v.begin()+2,back_inserter(m));
    cout<<"----------------"<<endl;
    for(auto & e : m)
    {
        cout<<e.first<<" : "<<e.second<<endl;
    }
    cout<<"---------------------"<<endl;
    //test3
    set<pair<index_t ,float>> s1;
    s1.insert(pair<index_t ,float >(1,2.0));
    s1.insert(pair<index_t ,float >(1,3.0));
    s1.insert(pair<index_t ,float >(3,1.0));

    set<pair<index_t ,float>> s2;
    s2.insert(pair<index_t ,float >(1,2.0));
    s2.insert(pair<index_t ,float >(1,4.0));
    s2.insert(pair<index_t ,float >(5,5.0));
    vector<pair<index_t ,float >> v1;
    v1.resize(s1.size()+s2.size());
    set_intersection(s1.begin(),s1.end(),s2.begin(),s2.end(),v1.begin());
    for(auto & e : v1)
    {
        cout<<e.first<<" : "<<e.second<<endl;
    }
    cout<<v1.size()<<endl;
*/
    return 0;
}