#include "lexer.h"

void error(int n){
    printf("Error %3d: %s\n", n, err_msg[n]);
}


/**
 * @brief PL/0词法分析器：DFA实现
 * 
 * @param sourceCode 
 * @return vector<pair<string,string>> 
 */
vector<pair<string,string>> lexer(const string &sourceCode){
    // 结果记录
    vector<pair<string, string>> tokens;

    int cur_num=0;    // 识别中的数字
    int cur_num_len = 0;    // 识别中数字的长度
    char cur_token[MAXIDLEN + 1];   //识别中的标识符or关键字
    int cur_token_index = 0;    // 识别中标识符or关键字的下标

    // 按字符读取源程序
    for (size_t i = 0; i < sourceCode.size();i++)
    {
        // 大小写不敏感
        char c = tolower(sourceCode[i]);

        // cout << "目前读取字符：" << c << endl;

        // 状态转移
        switch (currentState)
        {
        // 默认状态
        case START:
            if(c==' '||c=='\t'||c=='\r'||c=='\n'){
                // 跳过空格，缩进，换行
            }else if(c=='{'){
                // 注释
                currentState = COMMENT;
            }
            else if (isdigit(c)) {
                // 转换为数字
                currentState = INNUM;
                // 读到下一个数字，上一位进位
                cur_num = cur_num * 10 + c - '0';
                cur_num_len++;
            }else if(isalpha(c)){
                // 是字母,进入标识符或关键字识别
                currentState = INID;
                // 越界异常
                if(cur_token_index>MAXIDLEN){
                    error(26);
                    exit(1);
                }
                // 保存
                cur_token[cur_token_index] = c;
                cur_token_index++;
            }
            else if(c==':')
                currentState = INBECOMES;
            else if(c=='>')
                currentState = GTR;
            else if(c=='<')
                currentState = LES;
            // 如果是单字符的运算符
            else if(isSingleOperator(c)){
                currentState = START;
                string sym(1, c);
                tokens.push_back(make_pair(operators.find(sym)->second,sym));
            }
            // 是逗号分隔符
            else if(c==','){
                currentState = START;
                tokens.push_back(make_pair(delimiters.find(",")->second, ","));
            }
            // 是分号分隔符
            else if (c == ';')
            {
                currentState = START;
                tokens.push_back(make_pair(delimiters.find(";")->second, ";"));
            }
            // 是等于号
            else if(c=='='){
                currentState = START;
                tokens.push_back(make_pair(operators.find("=")->second, "="));
            }
            // 是左括号
            else if(c=='('){
                currentState = START;
                tokens.push_back(make_pair(delimiters.find("(")->second, "("));
            }
            // 是右括号
            else if(c==')'){
                currentState = START;
                tokens.push_back(make_pair(delimiters.find(")")->second, ")"));
            }
            else
            {
                // 其余情况，报错非法字符
                cout << "报错字符：" << c << endl;
                error(0);
                exit(1);
            }
            break;
        // 读取数字
        case INNUM:
            if(isdigit(c))
            {
                cur_num = cur_num * 10 + c - '0';
                cur_num_len++;
            }
            else
            {
                // token识别完毕
                currentState = START;
                // 回退一个字符
                i--;
                // 超过最大允许数字长度
                if(cur_num_len > MAXNUMLEN){
                    error(25);  
                    exit(1);
                }
                
                tokens.push_back(make_pair(number, to_string(cur_num)));
                cur_num = 0;
                cur_num_len = 0;
            }
            break;
        // 读取注释
        case COMMENT:
            if(c=='}'){
                // 注释结束
                currentState = START;
            }
            break;
        // 读取标识符或关键字
        case INID:
            if(isalpha(c)||isdigit(c)){
                // 越界
                if(cur_token_index>MAXIDLEN){
                    error(26);
                    exit(1);
                }
                cur_token[cur_token_index] = c;
                cur_token_index++;
            }else {
                // token识别完毕
                currentState = START;
                // 回退一个字符
                i--;
                cur_token[cur_token_index] = '\0';
                // 检查是否为关键字
                if(isKeyword(cur_token)){
                    tokens.push_back(make_pair( keywords.find(cur_token)->second,cur_token));
                    // 对于终止关键字
                    if(strcmp(cur_token,"end")==0){
                        currentState = END;
                    }
                }else {
                    // 不是关键字，则为普通标识符
                    tokens.push_back(make_pair(identifier, cur_token));
                }
                
                // 清理
                cleanTokenMem(cur_token, cur_token_index);
            }
            break;
        // 赋值语句
        case INBECOMES:
            if(c=='='){
                currentState = BECOMES;
            }else {
                currentState = START;
                // 回退一个字符
                if (i > 0) i--;
            }
            break;
        // >
        case GTR:
            if(c=='='){
                currentState = GEQ;
            }else{
                // token识别完毕
                currentState = START;
                if (i > 0) i--;    // 回退
                tokens.push_back(make_pair(operators.find(">")->second, ">"));
            }
            break;
        // <
        case LES:
            if(c=='='){
                currentState = LEQ;
            }else if(c=='>'){
                currentState = NEQ;
            }
            else {
                // token识别完毕
                currentState = START;
                if (i > 0) i--;    // 回退
                tokens.push_back(make_pair(operators.find("<")->second, "<"));
            }
            break;
        // 识别出完整的赋值操作符 :=
        case BECOMES:
            currentState = START;
            i--;    //回退
            tokens.push_back(make_pair(operators.find(":=")->second, ":="));
            break;
        // 识别出完整>=符号
        case GEQ:
            currentState = START;
            i--;
            tokens.push_back(make_pair(operators.find(">=")->second, ">="));
            break;
        // 识别出完整<=符号
        case LEQ:
            currentState = START;
            i--;
            tokens.push_back(make_pair(operators.find("<=")->second, "<="));
            break;
        // 识别出<>不等于符号
        case NEQ:
            currentState = START;
            i--;
            tokens.push_back(make_pair(operators.find("<>")->second, "<>"));
            break;
        // 终止符
        case END:
            // 终止符已经添加过了
            if(c=='.'){
                tokens.push_back(make_pair(delimiters.find(".")->second, "."));
            }else {
                i--;
            }
            currentState = START;
            break;
        }
    }
    // 词法分析结束
    cout << "Lexical analysis END." << endl;
    return tokens;
}
