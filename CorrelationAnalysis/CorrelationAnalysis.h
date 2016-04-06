//
// Created by zhudanqi on 16-4-5.
//

#ifndef CORRELATIONANALYSIS_CORRELATIONANALYSIS_H
#define CORRELATIONANALYSIS_CORRELATIONANALYSIS_H

#include <unordered_map>
#include <vector>
#include <string>
#include "CA_Utils.h"
class CorrelationAnalysis {

public:
    /*
     * 功能：初始化
     *
     * 两种情况：
     *
     * 1.abIsData 为 true    :   pos1 位置的属性与pos2位置的属性值都是数值(1/2,9.5/4.7)
     *
     * 2.abIsData 为 false   :   pos1 位置的属性与pos2位置的属性值都是标称数值（高/低，男/女之类）
     */
    void    initCA(std::string attr_path,std::string data_path, unsigned int pos1, unsigned int pos2,bool abIsData);
    /*
     * 功能：卡方计算
     */
    void calculateX2();
    /*
     * 功能：输出结果
     */
    void result();

private:
    std::vector<AbMatrix> all_ab;//所有参与分析的属性对
    ALLATTR allattr;//所有属性信息
    std::vector<std::string> pos_attr;//属性与位置对应表
    bool abisdata;
    double  ab_r;
};


#endif //CORRELATIONANALYSIS_CORRELATIONANALYSIS_H
