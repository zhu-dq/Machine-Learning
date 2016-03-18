#include "user_cfengine.h"

int main(int argc,char * argv[])
{
	time_t start, stop;
	start = time(NULL);

	UCFEngine  ucf;

	ucf.init();
	stop = time(NULL);
	cout << "init Time:" << (stop - start) << endl;


	ucf.constructItemUserInvertList();
	ucf.dumpItemUserInvertList();
	stop = time(NULL);
	cout << "constructItemUserInvertList Time:" << (stop - start) << endl;


	ucf.constructSimilarUserList();
	ucf.dumpSimilarUserList();
	stop = time(NULL);
	cout << "constructSimilarUserList Time:" << (stop - start) << endl;


	ucf.BuildRecommendItemList();
	ucf.dumpRecommendItemList();
	stop = time(NULL);
	cout << "BuildRecommendItemList Time:" << (stop - start) << endl;

	ucf.testResult();
	stop = time(NULL);
	cout << "testResult Time:" << (stop - start) << endl;


	system("pause");
	return 0;
}