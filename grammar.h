#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include "lex.h"

class Grammar
{
public:
    Grammar(const char*text);
    ~Grammar();
    //�������
    bool analysis();
    //���ش���ԭ��
    string error();

protected:
    //��䴮����
    bool stateBunch();
    //������
    bool statement();
    //��ֵ������
    bool equState();
    //����������
    bool stateBlock();
    //����������
    bool ifState();
    bool Disp();
    //ѭ��������
    bool whileState();
    //���ʽ����
    bool expression();
    //bool��������
    bool Bool();
    //�������
    bool term();
    //���ӷ���
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
