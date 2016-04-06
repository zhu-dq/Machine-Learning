#include <iostream>
#include "CorrelationAnalysis.h"
using namespace std;

int main() {
    CorrelationAnalysis ca;
    ca.initCA("/home/zhudanqi/ClionProjects/CorrelationAnalysis/attr.txt",
              "/home/zhudanqi/ClionProjects/CorrelationAnalysis/data.txt",0,1, false);

    ca.calculateX2();
    ca.result();
    std::string str;
    std::cin>>str;
    cout << "Hello, World!" << endl;
    return 0;
}