//
// Created by zhudanqi on 16-4-5.
//

#ifndef CORRELATIONANALYSIS_CA_UTILS_H
#define CORRELATIONANALYSIS_CA_UTILS_H
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cmath>
#define  MAP std::unordered_map
#define ALLATTR MAP<std::string,std::vector<std::string>>
#define UNIONMATRIX std::unordered_map<std::string,std::unordered_map<std::string,std::pair<unsigned int,double>>>//      UNIONMATRIX[ai][bj]->{ count(ai,bj) , eij }
#define COUNTATTR std::unordered_map<std::string,unsigned int>
class AbMatrix{
public:
    UNIONMATRIX abmatrix;//A B  联合矩阵
    COUNTATTR count_a;//属性A的值的个数
    COUNTATTR count_b;//属性B的值的个数
    std::string a_attr;//属性A的name
    std::string b_attr;//属性B的name
    unsigned  int n;//总计
    double  x2;//卡方
};


class CA_Utils {
public:
    /*
     * 功能：载入数据相关属性信息
     * 格式：
     *          A a1 a2 a3
     *          B b1 b2 b3 b4 b5
     *          ...
     */
    static ALLATTR loadAttr(std::string & attr_path,std::vector<std::string> * pos_attr);
    /*
     * 功能：载入待分析属性相关数据
     * 格式：    0  1  2  3  4
     *          A  B  C  D  E ...
     */
    static void loadData(std::string & data_path,AbMatrix * abm,ALLATTR & all_attr,
                         std::vector<std::string> & pos_attr, unsigned int pos_a, unsigned int pos_b);
    /*
     * 功能：计算卡方
     */
    static void calculatexx(AbMatrix * abm);

    /*
     * 功能： 对数值属性的数据求数据的相关性系数 r
     */
    static    double initandcalculate_ab_r(std::string data_path, unsigned int pos1, unsigned int pos2);
};


#endif //CORRELATIONANALYSIS_CA_UTILS_H
