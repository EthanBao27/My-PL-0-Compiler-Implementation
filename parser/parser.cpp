#include "parser.h"
#include <iostream>
#include <cstdlib>

/* —— 将字符串映射到枚举 —— */
static const std::unordered_map<std::string,Tok> tbl = {
 /* 关键字 */
 {"beginsym",Tok::BEGINSYM}, {"endsym",Tok::ENDSYM},
 {"constsym",Tok::CONSTSYM}, {"varsym",Tok::VARSYM},
 {"proceduresym",Tok::PROCEDURESYM}, {"callsym",Tok::CALLSYM},
 {"ifsym",Tok::IFSYM}, {"elsesym",Tok::ELSESYM},{"thensym",Tok::THENSYM},
 {"whilesym",Tok::WHILESYM}, {"dosym",Tok::DOSYM},
 {"oddsym",Tok::ODDSYM},
 {"readsym",Tok::READSYM}, {"writesym",Tok::WRITESYM},
 /* 标识符 / 常数 */
 {"ident",Tok::IDENT}, {"number",Tok::NUMBER},
 /* 运算符 */
 {"plus",Tok::PLUS}, {"minus",Tok::MINUS},
 {"times",Tok::TIMES}, {"slash",Tok::SLASH},
 {"eql",Tok::EQL}, {"neq",Tok::NEQ},
 {"lss",Tok::LSS}, {"leq",Tok::LEQ},
 {"gtr",Tok::GTR}, {"geq",Tok::GEQ},
 {"becomes",Tok::BECOMES},
 /* 分隔符 */
 {"lparen",Tok::LPAREN}, {"rparen",Tok::RPAREN},
 {"comma",Tok::COMMA}, {"semicolon",Tok::SEMICOLON},
 {"period",Tok::PERIOD}
};

// 当前缩进层次
int indent_level = 0;

/**
 * @brief 
 * 输出ast树
 * @param label 
 */
void printNode(const std::string& label) {
    for (int i = 0; i < indent_level; ++i) std::cout << "  ";
    std::cout << label << std::endl;
}

/* ------------ 构造 & 小工具 ------------ */
/**
 * @brief Construct a new Parser:: Parser object
 * 将原始Tokens符号流转换为解析后的Tok类型
 * @param raw 
 */
Parser::Parser(const std::vector<RawToken>& raw)
{
    for (auto& r: raw) {
        auto it = tbl.find(r.type);
        toks.push_back({ it==tbl.end()?Tok::END : it->second, r.lexeme });
    }
    toks.push_back({Tok::END,""});
}
/**
 * @brief 获取当前Token
 */
Parser::Token& Parser::cur()            { return toks[pos]; }
/**
 * @brief 
 * 当前是否是特定Token
 * @param t 
 * @return true 
 * @return false 
 */
bool  Parser::is(Tok t)                 { return cur().t==t; }

/**
 * @brief 
 * 移动到下一个Token
 */
void Parser::adv()                     { ++pos; }

/**
 * @brief 
 * 报错
 * @param m 
 */
void Parser::err(const std::string& m)
{
    std::cerr<<"语法错误: "<<m<<"，near '"<<cur().lex<<"'\n"; 
    std::exit(1);
}

/**
 * @brief 
 * 预期的Token
 * @param t 
 */
void Parser::expect(Tok t)
{
    if (!is(t)) {
        std::string expected = tokToString(t);
        std::string found = tokToString(cur().t);
        err("缺少预期符号: '" + expected + "'，但遇到 '" + found + "'");
    }
    adv();
}

/* ------------ 递归下降实现 ------------ */

/**
 * @brief 
 * 语法分析总函数
 */
void Parser::parse(){ 
    program(); 
    if(!is(Tok::END)) err("多余符号"); 
    else std::cout << "语法正确\n";
}

/**
 * @brief 
 * 程序=[块][结束符]
 */
void Parser::program(){ 
    printNode("Program");
    indent_level++;
    block();
    if(!is(Tok::PERIOD)) err("缺少 '.'"); adv();
    indent_level--;
}

/**
 * @brief 
 * 块=[常量声明][变量声明][过程声明]<语句>
 */
void Parser::block()
{
    printNode("Block");
    indent_level++;
    
    // 常量声明
    if (is(Tok::CONSTSYM)) {
    printNode("Const Declaration");
    indent_level++;

    printNode("CONST");
    adv();

    if (!is(Tok::IDENT)) err("const 后应为标识符");
    printNode("IDENT: " + cur().lex);
    adv();

    if (!is(Tok::EQL)) err("缺少 '='");
    printNode("EQL '='");
    adv();

    if (!is(Tok::NUMBER)) err("常数缺失");
    printNode("NUMBER: " + cur().lex);
    adv();

    while (is(Tok::COMMA)) {
        printNode("COMMA ','");
        adv();
        if (!is(Tok::IDENT)) err("标识符缺失");
        printNode("IDENT: " + cur().lex);
        adv();

        if (!is(Tok::EQL)) err("缺少 '='");
        printNode("EQL '='");
        adv();

        if (!is(Tok::NUMBER)) err("常数缺失");
        printNode("NUMBER: " + cur().lex);
        adv();
    }

    if (!is(Tok::SEMICOLON)) err("缺少 ';'");
    printNode("SEMICOLON ';'");
    adv();

    indent_level--;
    }
    // 变量声明
    if (is(Tok::VARSYM)) {
    printNode("Var Declaration");
    indent_level++;

    printNode("VAR");
    adv();

    if (!is(Tok::IDENT)) err("var 后应为标识符");
    printNode("IDENT: " + cur().lex);
    adv();

    while (is(Tok::COMMA)) {
        printNode("COMMA ','");
        adv();
        if (!is(Tok::IDENT)) err("标识符缺失");
        printNode("IDENT: " + cur().lex);
        adv();
    }

    if (!is(Tok::SEMICOLON)) err("缺少 ';'");
    printNode("SEMICOLON ';'");
    adv();

    indent_level--;
    }
    while(is(Tok::PROCEDURESYM)){
        adv(); if(!is(Tok::IDENT)) err("过程名缺失"); adv();
        if(!is(Tok::SEMICOLON)) err("缺少 ;"); adv();
        block(); if(!is(Tok::SEMICOLON)) err("缺少 ;"); adv();
    }
    statement();
    indent_level--;
}

/**
 * @brief
 * 常量声明=CONST<常量定义>{,<常量定义>};
 */
void Parser::constDecl()
{
    if(!is(Tok::IDENT)) err("const 后应为标识符"); adv();
    if(!is(Tok::EQL))   err("= 缺失");            adv();
    if(!is(Tok::NUMBER))err("常数缺失");          adv();
    while(is(Tok::COMMA)){
        adv(); if(!is(Tok::IDENT)) err("标识符缺失"); adv();
        if(!is(Tok::EQL)) err("= 缺失"); adv();
        if(!is(Tok::NUMBER)) err("常数缺失"); adv();
    }
}

/**
 * @brief 
 * 变量声明=VAR<变量定义>{,<变量定义>};
 */
void Parser::varDecl()
{
    if(!is(Tok::IDENT)) err("var 后应为标识符"); adv();
    while(is(Tok::COMMA)){ adv(); if(!is(Tok::IDENT)) err("标识符缺失"); adv(); }
}

/**
 * @brief 
 * 语句=<赋值语句>|<条件语句>|<当循环语句>|<过程调用语句>
 |<复合语句>|<读语句><写语句>|<空>
 */
void Parser::statement()
{
    printNode("Statement");
    indent_level++;

    // 赋值语句=<标识符>=<表达式>;
    if (is(Tok::IDENT)) {
        printNode("Assignment");
        indent_level++;
        printNode("IDENT: " + cur().lex);
        adv();
        printNode("BECOMES ':='");
        expect(Tok::BECOMES);
        expression();
        indent_level--;
    }
    // 过程调用语句=call <标识符>;
    else if (is(Tok::CALLSYM)) {
        printNode("Procedure Call");
        indent_level++;
        printNode("CALL");
        adv();
        printNode("IDENT: " + cur().lex);
        expect(Tok::IDENT);
        indent_level--;
    }
    // 复合语句=begin<语句>{;<语句>}end
    else if (is(Tok::BEGINSYM)) {
        printNode("Begin-End Block");
        indent_level++;
        printNode("BEGIN");
        adv();
        statement();
        while (is(Tok::SEMICOLON)) {
            printNode("SEMICOLON ';'");
            adv();
            statement();
        }
        printNode("END");
        expect(Tok::ENDSYM);
        indent_level--;
    }
    // 条件语句=if <条件> then <语句> [else <语句>]
    else if (is(Tok::IFSYM)) {
        printNode("If Statement");
        indent_level++;
        printNode("IF");
        adv();
        condition();
        printNode("THEN");
        expect(Tok::THENSYM);
        statement();
        if (is(Tok::ELSESYM)) {
            printNode("ELSE");
            adv();
            statement();
        }
        indent_level--;
    }
    // 当循环语句=while <条件> do <语句>
    else if (is(Tok::WHILESYM)) {
        printNode("While Loop");
        indent_level++;
        printNode("WHILE");
        adv();
        condition();
        printNode("DO");
        expect(Tok::DOSYM);
        statement();
        indent_level--;
    }
    // 读语句=read(<标识符>);
    else if (is(Tok::READSYM)) {
        printNode("Read Statement");
        indent_level++;
        printNode("READ");
        adv();
        printNode("LPAREN '('");
        expect(Tok::LPAREN);
        printNode("IDENT: " + cur().lex);
        expect(Tok::IDENT);
        printNode("RPAREN ')'");
        expect(Tok::RPAREN);
        indent_level--;
    }
    // 写语句=write(<表达式>);
    else if (is(Tok::WRITESYM)) {
        printNode("Write Statement");
        indent_level++;
        printNode("WRITE");
        adv();
        printNode("LPAREN '('");
        expect(Tok::LPAREN);
        expression();
        printNode("RPAREN ')'");
        expect(Tok::RPAREN);
        indent_level--;
    }
    /* 空语句允许 —— 什么都不做 */
    indent_level--;
}

/**
 * @brief 
 * 条件=ODD<表达式>|<表达式><比较运算符><表达式>
 */
void Parser::condition()
{
    printNode("Condition");
    indent_level++;

    if (is(Tok::ODDSYM)) {
        printNode("ODD");
        adv();
        expression();
    } else {
        expression();
        if (is(Tok::EQL)||is(Tok::NEQ)||is(Tok::LSS)||is(Tok::LEQ)||is(Tok::GTR)||is(Tok::GEQ)) {
            printNode("CompareOp: " + cur().lex);
            adv();
        } else {
            err("比较运算符缺失");
        }
        expression();
    }

    indent_level--;
}

/**
 * @brief 
 * 表达式=[+ | -]<项>{<加法运算符><项>}
 */
void Parser::expression()
{
    printNode("Expression");
    indent_level++;
    // 检查第一个合法的项是否存在
    if (!(is(Tok::PLUS) || is(Tok::MINUS) || is(Tok::IDENT) || is(Tok::NUMBER) || is(Tok::LPAREN)))
        err("表达式应以标识符、数字或 '(' 开始");

    if(is(Tok::PLUS)||is(Tok::MINUS)) {
        printNode("UnaryOp:"+cur().lex);
        adv();
    }
    term();
    while (is(Tok::PLUS) || is(Tok::MINUS)) {
        printNode("BinaryOp: " + cur().lex);
        adv();
        term();
    }
    indent_level--;
}

/**
 * @brief 
 * 项=<因子>{<乘法运算符><因子>}
 */
void Parser::term()
{
    printNode("Term");
    indent_level++;

    factor();
    while(is(Tok::TIMES)||is(Tok::SLASH)) {
        printNode("BinaryOp: " + cur().lex);
        adv(); 
        factor();
    }

    indent_level--;
}

/**
 * @brief 
 * 因子=<标识符>|<无符号整数>|(<表达式>)
 */
void Parser::factor()
{
    printNode("Factor");
    indent_level++;

    if (is(Tok::IDENT)) {
        printNode("IDENT: " + cur().lex);
        adv();
    } 
    else if (is(Tok::NUMBER)) {
        printNode("NUMBER: " + cur().lex);
        adv();
    } 
    else if (is(Tok::LPAREN)) {
        printNode("(");
        adv();
        expression();
        if(!is(Tok::RPAREN)) err("')' 缺失");
        printNode(")");
        adv();
    } 
    else {
        err("非法因子");
    }

    indent_level--;
}