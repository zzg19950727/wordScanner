#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include "lex.h"

class Grammar
{
public:
    Grammar(const char*text);
    ~Grammar();
    //程序分析
    bool analysis();
    //返回错误原因
    string error();

protected:
    //语句串分析
    bool stateBunch();
    //语句分析
    bool statement();
    //赋值语句分析
    bool equState();
    //整体语句分析
    bool stateBlock();
    //条件语句分析
    bool ifState();
    bool Disp();
    //循环语句分析
    bool whileState();
    //表达式分析
    bool expression();
    //bool代数分析
    bool Bool();
    //单项分析
    bool term();
    //因子分析
    bool factor();
    inline void scannerError();
    bool intTostr(int num, string &str);
    bool memcopy(char* &key, const char* result);
    void Print(string str);
    string Getseq();
    string Getvar();
    bool exist(const char* token);
    int where(const char* token);

private:
    bool state;
    string errorString;
    Lex *scanner;
    Tuple* result;
    int temp;
    char *key;
    int step;
    int label;
    int mem;
    bool isNum;
public:
    vector<string> code;
    vector<string> var;
};

#endif // GRAMMAR_H
