//
// Created by zhudanqi on 16-4-13.
//

#include "item_cfengine.h"

ICFEngine::ICFEngine()
{
    this->U_I_Rating = new MATRIX();
    this->I_U_Rating = new IUR();
    this->U_I_test = new MATRIX();
    this->itemset = new set<index_t >();
    this->userset = new set<index_t >();
    this->U_I_InvertTable = new UIRT();
    this->I_I_Similar = new SIMILAR();
    this->th_ulist = new ULIST();
    this->ALL_U_Recommend_I = new ALLRECOMMEND();
}
void ICFEngine::init(string tarin_path, string test_path, index_t k)
{
    this->p.k = k;
    this->p.tarin_path = tarin_path;
    this->p.test_path = test_path;
    thread t1(utils::loadTarinData,tarin_path,this->U_I_Rating,this->I_U_Rating,this->itemset,this->userset);
    thread t2(utils::loadTestData,test_path,this->U_I_test);
    t1.join();
    t2.join();
}
ICFEngine::~ICFEngine()
{
    delete(this->U_I_Rating);
    delete(this->I_U_Rating);
    delete(this->U_I_test);
    delete(this->itemset);
    delete(this->userset);
    delete(this->U_I_InvertTable);
    delete(this->I_I_Similar);
    delete(this->th_ulist);
    for(size_t i = 0;i<THREADNUM;++i)
        delete((*ALL_U_Recommend_I)[i]);
    delete(this->ALL_U_Recommend_I);
}
void ICFEngine::constructUserItemTable() const
{
    vector<index_t > v(itemset->begin(),itemset->end());
    for(size_t i = 0;i<v.size()-1;++i)
    {
       // (*this->U_I_InvertTable)[v[i]][v[i]]=(*this->I_U_Rating)[v[i]].size();
        for(size_t j=i+1;j<v.size();++j)
        {
            vector<index_t>  temp ;
            temp.resize(((*this->I_U_Rating)[v[i]]).size()+((*this->I_U_Rating)[v[j]]).size());
            set_intersection((*this->I_U_Rating)[v[i]].begin(),(*this->I_U_Rating)[v[i]].end(),
                             (*this->I_U_Rating)[v[j]].begin(),(*this->I_U_Rating)[v[j]].end(),
                              temp.begin());
            index_t cnt =0;
            for(auto & e : temp)
            {
                if(e>0)
                    cnt++;
            }
            (*this->U_I_InvertTable)[v[i]][v[j]]=cnt;
            (*this->U_I_InvertTable)[v[j]][v[i]]=cnt;
        }
    }
    cout<<"construct UserItemTable complete "<<endl;

}
void ICFEngine::constructSimilarItemList() const
{
    //ji suan wij
    vector<index_t> v(itemset->begin(),itemset->end());
    for(size_t i =0;i<v.size();++i)
    {
        //(*this->I_I_Similar)[v[i]][v[i]] = 1;
      //  (*this->I_I_Similar)[v[i]].push_back(pair<index_t ,float >(v[i],1));
        for(size_t j= 0;j<v.size();++j)
        {
            if(i == j)
                continue;
            if((*U_I_InvertTable)[v[i]][v[j]]==0)
            {
                //(*this->I_I_Similar)[v[i]][v[j]]=0;
                //(*this->I_I_Similar)[v[i]].push_back(pair<index_t ,float >(v[j],0));
                continue;
            }
            double  numerator = (*U_I_InvertTable)[v[i]][v[j]];
            double  denomomator = sqrt((*I_U_Rating)[v[i]].size()*(*I_U_Rating)[v[j]].size());
            //double  denomomator1 = (*I_U_Rating)[v[i]].size();
            float wij = static_cast<float >(numerator/denomomator);
           // float wij  = static_cast<float >(numerator/denomomator1);
            //(*this->I_I_Similar)[v[i]][v[j]] = wij;
            (*this->I_I_Similar)[v[i]].push_back(pair<index_t ,float >(v[j],wij));
        }
    }
    //qu qian k ge
    for(auto & similarItem : *this->I_I_Similar)
    {
        if(similarItem.second.size()<=this->p.k)
            continue;
        //vector<pair<index_t,float>> v(similarItem.second.begin(),similarItem.second.end());
        vector<pair<index_t,float>> v = similarItem.second;
        sort(v.begin(),v.end(),utils::sort_func);
        //similarItem.second.clear();
        (*I_I_Similar)[similarItem.first] = vector<pair<index_t,float>>(v.begin(),v.begin()+p.k);
        //copy(v.begin(),v.begin()+p.k,back_inserter(similarItem.second));
    }
}
void ICFEngine::BuildRecommendItemList() const
{
    utils::buildUserGroup(this->userset,this->th_ulist);
    vector<thread> th_list;
    for(index_t i = 0;i<THREADNUM;++i)
    {
        REALRECOM * U_RealRcom_I = new REALRECOM();
        thread t(utils::Group2GroupRecommendComputing,
                 (*this->th_ulist)[i],
                 I_I_Similar,
                 this->itemset,
                 this->U_I_Rating,
                 U_RealRcom_I,
                 p.k );
        (*ALL_U_Recommend_I)[i] = U_RealRcom_I;
        th_list.push_back(move(t));
    }
    for(auto & elem : th_list)
    {
        elem.join();
    }
    cout<<"build recommenditemlist complete "<<endl;
}
void ICFEngine::showRecommmendItem() const
{
    for(auto & thread : * ALL_U_Recommend_I )
    {
        for(auto & userref : *thread.second)
        {
            cout<<" uid : "<<userref.first;
            cout<<" iid : ";
            for(auto & iid : userref.second)
            {
                cout<<iid.first<<" ";
            }
            cout<<endl;
        }
    }
}
void ICFEngine::testResult() const
{
    index_t * realAllCount = new index_t();
    index_t * recommendAllCount = new index_t();
    index_t * test_recommendAllCount = new index_t();
    index_t	* hidAllcount = new index_t();
    thread t1(utils::countRealAll,this->U_I_test,realAllCount);
    thread t2(utils::countRecomAll, this->U_I_test,this->ALL_U_Recommend_I, recommendAllCount,test_recommendAllCount);
    thread t3(utils::countHitAll, this->U_I_test, this->ALL_U_Recommend_I, hidAllcount);
    t1.join();
    t2.join();
    t3.join();
    cout<<" realcount : "<<*realAllCount<<endl;
    cout<<" recommendallcount : "<<*recommendAllCount<<endl;
    cout<<" test_recommendallcount : "<<*test_recommendAllCount<<endl;
    cout<<" hidallcount : "<<*hidAllcount<<endl;
    //float precision = (float)(*hidAllcount) / (float)(*recommendAllCount);
    float precision = (float)(*hidAllcount) / (float)(*test_recommendAllCount);
    float recall = (float)(*hidAllcount) / (float)(*realAllCount);
    cout << "正确率 ： " << precision << endl;
    cout << "召回率 : " << recall << endl;
    delete realAllCount;
    delete recommendAllCount;
    delete hidAllcount;
}