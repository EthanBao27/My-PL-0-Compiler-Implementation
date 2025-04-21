#include "lexer.h"

void error(int n){
    printf("Error %3d: %s\n", n, err_msg[n]);
}

/**
 * @brief 
 * PL/0词法分析器：DFA实现
 * @param sourceCode 
 * @return vjector<pair<string, string>> 
 */
vector<pair<string, string>> lexer(const string &sourceCode)
{
    // 结果记录
    vector<pair<string, string>> tokens;

    // --- 添加行号和列号跟踪变量 ---
    int currentLine = 1;
    int currentColumn = 1;
    int tokenStartLine = 1;
    int tokenStartColumn = 1;
    // --- 行号列号变量结束 ---

    int cur_num = 0;                    // 识别中的数字
    float cur_float = 0.0;                // 识别小数字
    int cur_float_index = 0;            // 小数点后几位
    int cur_num_len = 0;                // 识别中数字的长度
    char cur_token[MAXIDLEN + 1];       // 识别中的标识符or关键字
    int cur_token_index = 0;            // 识别中标识符or关键字的下标
    memset(cur_token, 0, MAXIDLEN + 1); // 初始化

    currentState = START; // 确保每次调用 lexer 时状态重置

    // 按字符读取源程序
    for (size_t i = 0; i < sourceCode.size(); /* i 在循环内部或回退时管理 */)
    {
        char raw_c = sourceCode[i]; // 获取原始字符
        char c = tolower(raw_c);    // 转小写用于逻辑判断

        // --- 记录 Token 起始位置 ---
        if (currentState == START && !(isspace(raw_c) || raw_c == '{'))
        {
            tokenStartLine = currentLine;
            tokenStartColumn = currentColumn;
        }
        // --- Token 起始位置记录结束 ---

        // 状态转移 (保持原有逻辑)
        switch (currentState)
        {
        case START:
            if (isspace(raw_c))
            { /* 跳过 */
            }
            else if (raw_c == '{')
            {
                currentState = COMMENT;
            }
            else if (isdigit(c))
            {
                currentState = INNUM;
                cur_num = c - '0';
                cur_num_len = 1;
            }
            else if (isalpha(c))
            {
                currentState = INID;
                cleanTokenMem(cur_token, cur_token_index);
                if (cur_token_index < MAXIDLEN)
                {
                    cur_token[cur_token_index++] = c;
                }
                else
                {
                    error(26); /* 处理错误，可能需要跳过 */
                    // exit(1);
                }
            }
            else if (c == ':')
            {
                currentState = INBECOMES;
            }
            else if (c == '>')
            {
                currentState = GTR;
            }
            else if (c == '<')
            {
                currentState = LES;
            }
            else if (isSingleOperator(c))
            {
                currentState = START;
                string sym(1, c);
                string type = operators.find(sym)->second;
                // --- 打印信息 ---
                cout << "Token: (" << type << ", " << sym << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, sym));
            }
            else if (c == ',')
            {
                currentState = START;
                string type = delimiters.find(",")->second;
                // --- 打印信息 ---
                cout << "Token: (" << type << ", " << "," << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, ","));
            }
            else if (c == ';')
            {
                currentState = START;
                string type = delimiters.find(";")->second;
                // --- 打印信息 ---
                cout << "Token: (" << type << ", " << ";" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, ";"));
            }
            else if (c == '=')
            {
                currentState = START;
                string type = operators.find("=")->second;
                // --- 打印信息 ---
                cout << "Token: (" << type << ", " << "=" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, "="));
            }
            else if (c == '(')
            {
                currentState = START;
                string type = delimiters.find("(")->second;
                // --- 打印信息 ---
                cout << "Token: (" << type << ", " << "(" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, "("));
            }
            else if (c == ')')
            {
                currentState = START;
                string type = delimiters.find(")")->second;
                // --- 打印信息 ---
                cout << "Token: (" << type << ", " << ")" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, ")"));
            }
            // --- 添加对句点的处理 ---
            else if (c == '.')
            {
                currentState = START;
                string type = delimiters.find(".")->second;
                // --- 打印信息 ---
                cout << "Token: (" << type << ", " << "." << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, "."));
            }
            else
            {
                cout << "报错字符：" << raw_c << " at Line " << currentLine << ", Column " << currentColumn << endl;
                error(0);
                // exit(1); 
            }
            break; // START 结束

        case INNUM:
            if (isdigit(c))
            {
                if (cur_num_len >= MAXNUMLEN)
                {
                    error(25); /* 处理错误 */
                    // exit(1);
                }
                else
                {
                    cur_num = cur_num * 10 + (c - '0');
                    cur_num_len++;
                }
            }else if(c=='.'){
                // 读取小数点
                currentState = INFLOAT;
                cur_float = cur_num;

            }else{
                currentState = START;
                string val = to_string(cur_num);
                // --- 打印信息 ---
                cout << "Token: (" << number << ", " << val << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(number, val));
                cur_num = 0;
                cur_num_len = 0;
                i--; // 回退
                currentColumn--;
            }
            break; // INNUM 结束

        case INFLOAT:
           if (isdigit(c))
            {
                if (cur_num_len >= MAXNUMLEN)
                {
                    error(25); /* 处理错误 */
                    // exit(1);
                }
                else
                {
                    cur_float_index--;
                    cur_float = cur_float + (c - '0') * powf(10, cur_float_index);
                }
            }else{
                currentState = START;
                string val = to_string(cur_float);
                // --- 打印信息 ---
                cout << "Token: (" << number << ", " << val << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(number, val));
                cur_num = 0;
                cur_num_len = 0;
                cur_float = 0.0;
                cur_float_index=0;
                i--; // 回退
                currentColumn--;
            } 
            break;

        case COMMENT:
            if (raw_c == '}')
            {
                currentState = START;
            }
            break; // COMMENT 结束

        case INID:
            if (isalnum(c))
            {
                if (cur_token_index >= MAXIDLEN)
                {
                    error(26); /* 处理错误 */
                    // exit(1);
                }
                else
                {
                    cur_token[cur_token_index++] = c;
                }
            }
            else
            {
                currentState = START;
                cur_token[cur_token_index] = '\0';
                string tokenValue = cur_token;
                string tokenType;
                if (isKeyword(tokenValue))
                {
                    tokenType = keywords.find(tokenValue)->second;
                    if (tokenValue == "end")
                    {
                        currentState = END;
                    } 
                }
                else
                {
                    tokenType = identifier;
                }
                // --- 打印信息 ---
                cout << "Token: (" << tokenType << ", " << tokenValue << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(tokenType, tokenValue));
                cleanTokenMem(cur_token, cur_token_index);
                i--; // 回退
                currentColumn--;
            }
            break; // INID 结束

        case INBECOMES:
            if (c == '=')
            {
                currentState = BECOMES;
            }
            else
            {
                currentState = START;
                i--; /* 回退 */
                currentColumn--;
            }
            break; // INBECOMES 结束

        case GTR:
            if (c == '=')
            {
                currentState = GEQ;
            }
            else
            {
                currentState = START;
                string type = operators.find(">")->second;
                // --- 打印信息 ---
                cout << "Token: (" << type << ", " << ">" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, ">"));
                i--; // 回退
                currentColumn--;
            }
            break; // GTR 结束

        case LES:
            if (c == '=')
            {
                currentState = LEQ;
            }
            else if (c == '>')
            {
                currentState = NEQ;
            }
            else
            {
                currentState = START;
                string type = operators.find("<")->second;
                // --- 打印信息 ---
                cout << "Token: (" << type << ", " << "<" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, "<"));
                i--; // 回退
                currentColumn--;
            }
            break; // LES 结束

        case BECOMES: // 识别出 :=
            currentState = START;
            // --- 打印信息 ---
            cout << "Token: (" << operators.find(":=")->second << ", " << ":=" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
            tokens.push_back(make_pair(operators.find(":=")->second, ":="));
            i--; // 回退，因为 BECOMES 状态是在读到 '=' 后进入的，但 for 循环还会自增 i
            currentColumn--;
            break;

        case GEQ: // 识别出 >=
            currentState = START;
            // --- 打印信息 ---
            cout << "Token: (" << operators.find(">=")->second << ", " << ">=" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
            tokens.push_back(make_pair(operators.find(">=")->second, ">="));
            i--; // 回退
            currentColumn--;
            break;

        case LEQ: // 识别出 <=
            currentState = START;
            // --- 打印信息 ---
            cout << "Token: (" << operators.find("<=")->second << ", " << "<=" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
            tokens.push_back(make_pair(operators.find("<=")->second, "<="));
            i--; // 回退
            currentColumn--;
            break;

        case NEQ: // 识别出 <>
            currentState = START;
            // --- 打印信息 ---
            cout << "Token: (" << operators.find("<>")->second << ", " << "<>" << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
            tokens.push_back(make_pair(operators.find("<>")->second, "<>"));
            i--; // 回退
            currentColumn--;
            break;

        case END: // 识别出 end 关键字后进入的状态
            // 保持原有逻辑：在 end 之后如果遇到 '.'，则识别句点
            if (c == '.')
            {
                string type = delimiters.find(".")->second;
                // --- 打印信息 ---
                // 注意：这里的起始位置是 '.' 的位置，不是 'end' 的位置
                tokenStartLine = currentLine; // 更新句点的起始位置
                tokenStartColumn = currentColumn;
                cout << "Token: (" << type << ", " << "." << ") at Line " << tokenStartLine << ", Col " << tokenStartColumn << endl;
                tokens.push_back(make_pair(type, "."));
            }
            else
            {
                // 如果 end 后面不是 '.'，需要回退，让 '.' 或其他字符在 START 状态被处理
                i--;
                currentColumn--;
            }
            currentState = START; // 无论如何都回到 START
            break;                // END 结束

        } // switch(currentState) 结束

        // --- 更新行号和列号 ---
        if (raw_c == '\n')
        {
            currentLine++;
            currentColumn = 1;
        }
        else
        {
            currentColumn++;
        }
        // --- 行号列号更新结束 ---

        // 移动到下一个字符 (如果前面没有 continue 或 i--)
        i++;

    } // for 循环结束

    // --- 添加文件结束符 EOF Token 的打印信息 (但不加入返回列表) ---
    cout << "Token: (EOF, ) at Line " << currentLine << ", Col " << currentColumn << endl;
    // --- EOF Token 打印结束 ---

    // 词法分析结束
    cout << "Lexical analysis END." << endl;
    return tokens; // 返回值类型和内容不变
}
