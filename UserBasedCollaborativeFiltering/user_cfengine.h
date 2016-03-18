#ifndef USER_CFENGINE_H
#define USER_CFENGINE_H
#include "utils.h"

class UCFEngine
{

public:
	UCFEngine();

	~UCFEngine();

	/*
		���ܣ���������&��ʼ������
	*/
	void init()const;

	/*
		���ܣ����� ��Ʒ-�û����ű�
	*/
	void constructItemUserInvertList()const;

	/*
		���ܣ���ӡ��Ʒ-�û����ű�
	*/
	void dumpItemUserInvertList()const;

	/*
		���ܣ�������Ȥ��Ͷ���û�����
	*/ 
	void constructSimilarUserList()const;
	
	/*
		���ܣ���ӡ�����û��б�
	*/

	void dumpSimilarUserList()const;

	/*
		���ܣ���ȡ�����û�ϲ����Ŀ���û�û�е�item���Լ������Ȥ�̶�
	*/
	void BuildRecommendItemList()const;

	/*
		���ܣ���ӡ�Ƽ���Ʒ�б�
	*/
	void dumpRecommendItemList()const;

	/*
		���ܣ�����׼ȷ�����ٻ���
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