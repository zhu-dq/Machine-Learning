//
// Created by zhudanqi on 16-4-5.
//

#include "CorrelationAnalysis.h"
void CorrelationAnalysis::initCA(std::string attr_path,std::string data_path,unsigned int pos1, unsigned int pos2,bool abIsData)
{
    this->abisdata = abIsData;
    if(this->abisdata)
    {
        this->ab_r =  CA_Utils::initandcalculate_ab_r(data_path,pos1,pos2);
        return;
    }
    else
    {
        this->allattr = CA_Utils::loadAttr(attr_path,&this->pos_attr);
        AbMatrix abm;
        CA_Utils::loadData(data_path,&abm,this->allattr,this->pos_attr,pos1,pos2);
        this->all_ab.push_back(abm);
    }

}

void CorrelationAnalysis::calculateX2()
{
    if(this->abisdata)
        return;
    for(auto & abm : this->all_ab)
        CA_Utils::calculatexx(&abm);

}
void CorrelationAnalysis::result()
{
    if(this->abisdata)
    {
        std::cout<<std::endl;
        std::cout<<"相关系数简介："<<std::endl;
        std::cout<<"\t"<<"相关系数rab 范围：[-1,1],如果rab大于0,则A和B是正相关，该值越大相关性越强"<<std::endl;
        std::cout<<"两个属性的相关系数是 ： "<<this->ab_r<<std::endl;
        return;
    }
    for(auto & abm : this->all_ab)
    {
        std::cout<<std::endl;
        std::cout<<"卡方检验查表方法简介："<<std::endl;
        std::cout<<"\t"<<"1.找到自由度所在行"<<std::endl;
        std::cout<<"\t"<<"2.找到与卡方值最接近的数值所在列"<<std::endl;
        std::cout<<"\t"<<"3.此列最上面的p值就是这两个属性的无关性p,1-p就是其大概的相关性"<<std::endl;
        std::cout<<" ["<<abm.a_attr<<","<<abm.b_attr<<"] 的卡方 ：  "<<abm.x2<<std::endl;
        std::cout<<" ["<<abm.a_attr<<","<<abm.b_attr<<"] 的自由度 ：  "<<(abm.count_a.size()-1)*(abm.count_b.size()-1)<<std::endl;
    }
    /*
    for(auto & abm : this->all_ab)
    {
        for(auto & ai : this->allattr[abm.a_attr])
            for(auto & bi : this->allattr[abm.b_attr])
                std::cout<<" ["<<ai<<","<<bi<<"]"<<" : "<<abm.abmatrix[ai][bi].first<<std::endl;
       for(auto & item : abm.count_a)
           std::cout<<item.first<<" : "<<item.second<<std::endl;



        for(auto & item : abm.count_b)
            std::cout<<item.first<<" : "<<item.second<<std::endl;

        std::cout<<" : "<<abm.n<<std::endl;
    }
    */
}