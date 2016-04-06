//
// Created by zhudanqi on 16-4-5.
//

#include "CA_Utils.h"
ALLATTR CA_Utils::loadAttr(std::string & attr_path,std::vector<std::string> * pos_attr)
{
    ALLATTR allattr;
    std::fstream fin(attr_path);
    if(!fin)
    {
        std::cout<<"attr_path does not exist !"<<std::endl;
        return allattr;
    }
    std::string attr_str;
    while (std::getline(fin,attr_str))
    {
        std::stringstream attrref(attr_str);
        std::string attr_key;
        std::string attr_value;
        attrref>>attr_key;
        (*pos_attr).push_back(attr_key);
        std::vector<std::string> v;
        allattr.insert(std::pair<std::string,std::vector<std::string>>(attr_key,v));
        while (attrref>>attr_value)
        {
            allattr[attr_key].push_back(attr_value);
        }
    }
    fin.close();
    return allattr;

}
void CA_Utils::loadData(std::string &data_path,AbMatrix * abm,ALLATTR & all_attr,
                        std::vector<std::string> & pos_attr, unsigned int pos_a, unsigned int pos_b)
{
    std::fstream fin(data_path);
    if(!fin)
    {
        std::cout<<"data_path does not exist !"<<std::endl;
        return;
    }

    /*
     *  init abm
     */
    if(pos_a>=pos_attr.size())
    {
        std::cout<<"pos_a  value is error !"<<std::endl;
        return ;
    }
    if(pos_b>=pos_attr.size())
    {
        std::cout<<"pos_b  value is error !"<<std::endl;
        return ;
    }
    abm->a_attr = pos_attr[pos_a];
    abm->b_attr = pos_attr[pos_b];

    for(auto & ai : all_attr[abm->a_attr])
        for(auto & bi : all_attr[abm->b_attr])
            abm->abmatrix[ai][bi] = std::pair<unsigned  int,double >(0,0.0);

    for(auto & ai : all_attr[abm->a_attr])
        abm->count_a.insert(std::pair<std::string,unsigned int>(ai,0));

    for(auto & bi : all_attr[abm->b_attr])
        abm->count_b.insert(std::pair<std::string,unsigned int>(bi,0));

    abm->n = 0;
    abm->x2 = 0.0;

    std::string data_str;

    while (std::getline(fin,data_str))
    {
        std::stringstream dataref(data_str);
        std::string data_value;
        std::vector<std::string> v;
        while(dataref>>data_value)
            v.push_back(data_value);
        abm->abmatrix[v[pos_a]][v[pos_b]].first+=1;
    }

    for(auto & ai : all_attr[abm->a_attr])
    {
        for(auto & elem : abm->abmatrix[ai])
            abm->count_a[ai]+=elem.second.first;
    }
    for(auto & bi : all_attr[abm->b_attr])
    {
        for(auto & ai : all_attr[abm->a_attr])
            abm->count_b[bi]+=abm->abmatrix[ai][bi].first;
    }

    for(auto & ai : abm->count_a)
        abm->n+=ai.second;

    for(auto & ai : all_attr[abm->a_attr])
        for(auto & bj : all_attr[abm->b_attr])
            abm->abmatrix[ai][bj].second=(abm->count_a[ai]*abm->count_b[bj])/ static_cast<double>(abm->n);
}

void CA_Utils::calculatexx(AbMatrix *abm)
{
    for(auto & ai : abm->abmatrix)
        for(auto & bi : ai.second)
        {
            unsigned  int  Oij = bi.second.first;
            double  Eij = bi.second.second;
            abm->x2 += (Oij-Eij)*(Oij*Eij)/Eij;
        }
}
double CA_Utils::initandcalculate_ab_r(std::string data_path, unsigned int pos1, unsigned int pos2)
{
    std::fstream fin(data_path);
    if(!fin)
    {
        std::cout<<"data_path error !"<<std::endl;
        return 0.0;
    }
    std::vector<std::pair<double ,double >> v_attr;
    double  A = 0.0;
    double  B = 0.0;
    double  AB = 0.0;
    unsigned  int n = 0;
    std::string data_str;
    while (std::getline(fin,data_str))
    {
        std::stringstream attr_str(data_str);
        std::string attr_value1;
        std::string attr_value2;
        size_t  pos = 0;
        while (attr_str>>attr_value1 && pos!=pos1)pos++;
        if(pos!=pos1)
        {
            std::cout<<"pos 1 is error"<<std::endl;
            return 0.0;
        }

        size_t  pos_ = 0;
        while (attr_str>>attr_value2 && pos_!=pos2)pos_++;
        if(pos_!=pos2)
        {
            std::cout<<"pos 2 is error"<<std::endl;
            return 0.0;
        }
        double  ai =  atof(attr_value1.c_str());
        double  bi =  atof(attr_value2.c_str());

        v_attr.push_back(std::pair<double ,double >(ai, bi));

        A += ai;
        B += bi;
        AB+= ai*bi;
        n++;
    }

    A = A/n;
    B = B/n;
    AB = AB/n;
    double A_A_ = 0.0;
    double B_B_ = 0.0;
    for(auto & ab : v_attr)
    {
        A_A_ += (ab.first-A)*(ab.first-A);
        B_B_ += (ab.second-B)*(ab.second-B);
    }

    double  qa = sqrt(A_A_/n);
    double  qb = sqrt(B_B_/n);

    double  rab = (AB - A*B)/(qa*qb);
    return rab;

}