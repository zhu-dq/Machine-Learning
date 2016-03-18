#ifndef USER_CFENGINE_H
#define USER_CFENGINE_H
#include "utils.h"

class UCFEngine
{

public:
	UCFEngine();

	~UCFEngine();

	/*
		功能：导入数据&初始化参数
	*/
	void init()const;

	/*
		功能：构建 物品-用户倒排表
	*/
	void constructItemUserInvertList()const;

	/*
		功能：打印物品-用户倒排表
	*/
	void dumpItemUserInvertList()const;

	/*
		功能：构建兴趣相投的用户集合
	*/ 
	void constructSimilarUserList()const;
	
	/*
		功能：打印相似用户列表
	*/

	void dumpSimilarUserList()const;

	/*
		功能：获取相似用户喜欢而目标用户没有的item并对计算感兴趣程度
	*/
	void BuildRecommendItemList()const;

	/*
		功能：打印推荐商品列表
	*/
	void dumpRecommendItemList()const;

	/*
		功能：计算准确率与召回率
	*/
	void testResult()const;

private:

	/*
	shared_ptr<MATRIX> U_I_Rating;
	shared_ptr<IUSET> I_U_Rating;
	shared_ptr<IUInvertTable> I_U_Table;
	shared_ptr<USER> users;
	shared_ptr<USER> items;
	shared_ptr<SIMILAR> U_U_Simiar;
	//shared_ptr<MATRIX> U_Recommend_I;
	shared_ptr<ALLRECOMMEND> ALL_U_Recommend_I;
	shared_ptr<ULIST> th_ulist;
	*/

	MATRIX * U_I_Rating;
	IUSET * I_U_Rating;
	MATRIX * U_I_test;
	IUInvertTable * I_U_Table;
	USER * users;
	USER * items;
	SIMILAR * U_U_Simiar;
	ALLRECOMMEND * ALL_U_Recommend_I;
	ULIST * th_ulist;
};


#endif