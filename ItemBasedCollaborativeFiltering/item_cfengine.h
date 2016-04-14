//
// Created by zhudanqi on 16-4-13.
//

#ifndef ITEMBASEDCOLLABORATIVEFILTERING_ITEM_CFENGINE_H
#define ITEMBASEDCOLLABORATIVEFILTERING_ITEM_CFENGINE_H

#include "utils.h"
class params{
public:
    string tarin_path;
    string test_path;
    index_t k;
};
class ICFEngine {
public:
    ICFEngine();
    ~ICFEngine();
    /*
     *  功能： 载入数据并初始化参数
     */
    void init(string  tarin_path,string  test_path,index_t k);
    /*
     * 功能： 构建用户-物品倒排表
     */
    void constructUserItemTable()const;
    /*
     *功能：构建兴趣相投的物品集合
     */
    void constructSimilarItemList()const;
    /*
     * 功能：构建用户推荐表
     */
    void BuildRecommendItemList()const;
    /*
     * 功能：展示推荐内容
     */
    void  showRecommmendItem()const;
    /*
    功能：计算准确率与召回率
*/
    void testResult()const;
private:
    MATRIX  * U_I_Rating;
    IUR * I_U_Rating;
    MATRIX  * U_I_test;
    UIRT  * U_I_InvertTable;
    SIMILAR  * I_I_Similar;
    set<index_t > * itemset;
    set<index_t > * userset;
    params p;
    ULIST * th_ulist;
    ALLRECOMMEND * ALL_U_Recommend_I;
};


#endif //ITEMBASEDCOLLABORATIVEFILTERING_ITEM_CFENGINE_H
