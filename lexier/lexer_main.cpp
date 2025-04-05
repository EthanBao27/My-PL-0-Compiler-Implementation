#include <iostream>
#include <fstream>

#include "lexer.cpp"

using namespace std;

int main(){
    ifstream inputFile("./tests/case05.txt");
    ofstream outputFile("./out/case05_output.txt");

    if(!inputFile.is_open()||!outputFile.is_open()){
        cerr << "error:无法打开文件" << endl;
        return 1;
    }

    string sourceCode;
    string line;
    
    // 读取源代码程序作为字符流
    while(getline(inputFile,line)){
        sourceCode += line + "\n";
    }

    // 定义并通过词法分析，获取记号流
    vector<pair<string,string>> tokens;
    tokens=lexer(sourceCode);

    // 以(单词种类，值)方式输出
    for(const auto &pair:tokens){
        outputFile<<"("<<pair.first<<","<<pair.second<<")"<<endl;
    }

    inputFile.close();
    outputFile.close();

    cout << "词法分析结果已写入output.txt" << endl;

    return 0;
}