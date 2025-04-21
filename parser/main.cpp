#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

/* ---------- 工具：去掉行首行尾空白 ---------- */
static inline void trim(std::string& s){
    while(!s.empty() && std::isspace(s.front())) s.erase(s.begin());
    while(!s.empty() && std::isspace(s.back()))  s.pop_back();
}

int main(int argc,char* argv[])
{
    if(argc!=2){
        std::cerr << "用法: " << argv[0] << " <tokens.txt>\n";
        return 1;
    }

    std::ifstream fin(argv[1]);
    if(!fin){
        std::cerr << "无法打开 " << argv[1] << '\n';
        return 1;
    }

    /* 1️⃣ 读取 token 文件 -> vector<RawToken> */
    std::vector<RawToken> tokens;
    std::string line;
    while(std::getline(fin,line)){
        trim(line);
        if(line.empty()) continue;
        if(line.front()!='(' || line.back()!=')'){
            std::cerr<<"格式错误: "<<line<<'\n'; return 1;
        }
        std::string inner = line.substr(1,line.size()-2);
        auto comma = inner.find(',');
        if(comma==std::string::npos){
            std::cerr<<"缺少逗号: "<<line<<'\n'; return 1;
        }
        std::string type  = inner.substr(0,comma);
        std::string lexeme= inner.substr(comma+1);
        trim(type); trim(lexeme);
        tokens.push_back({type,lexeme});
    }

    /* 2️⃣ 语法分析 */
    Parser p(tokens);
    p.parse();                 // 成功打印“语法正确”
    return 0;
}