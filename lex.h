#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
using namespace::std;

//二元记号组
struct Tuple
{
    int typeNum;    //种别码
    char* token;    //记号
};

class Lex
{
public:
    Lex(const char* text);
    ~Lex();
    //匹配整个文本
    bool match_text();
    //一次匹配过程
    bool match(Tuple* &result);
    //判断是否匹配完成
    bool isFinish();
    //当前匹配第几个语句
    int whichState(){return step;}
    //打印结果
    void PrintResult();
    //错误处理
    string error();

protected:
    //读取一个字符
    char getch();
    //读取一个非空白字符
    char getbc();
    //字符串连接
    bool concat(char* &str, char ch);
    //判断是否是字母
    bool isLetter(char ch);
    //判断是否是数字
    bool isDigit(char ch);
    //返回标记的种别码
    int reserve(const char *token);
    //回退一个字符
    bool retract();
    //字符串转数字串
    long str2num(char* token);
    //判断是否是关键字
    bool isKeyword(const char* token, int& num);
    //信息复制
    bool memCopy(char* &source, char* &desti);
    //字符串比较
    int strCmp(const char* str1, const char* str2);

private:
    char* text;     //文本输入流
    long length;    //文本长度
    long p_str;     //当前字符的位置
    int block_len;  //记号的最小长度
    vector<Tuple*> result;    //存放词法分析结果
    int step;
    string errorString;
};

#endif // LEX_H
