#include "Grammar.h"

Grammar::Grammar(const char*text)
{
    if(!text)
    {
        this->scanner = NULL;
        this->result = NULL;
    }
    else
    {
        scanner = new Lex(text);
        this->result = NULL;
    }
    errorString.clear();
    temp = 0;
    key = NULL;
    step = 0;
    label = 0;
    mem = 0;
    isNum = false;
}

Grammar::~Grammar()
{
    delete scanner;
}

//程序分析
bool Grammar::analysis()
{
    //检测是否已构造了词法分析器
    if(!scanner)
    {
        errorString += "No text\n";
        return false;
    }

    //分析第一个词语
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    //判断是否是关键字begin
    if(result->typeNum != 1)
    {
        errorString += "procedure must begin with \"begin\"\n";
        return false;
    }

    //调用语句串分析器
    if( !stateBunch() )
        return false;

    //分析最后一个词语
    //判断是否是关键字begin
    if(result->typeNum !=6 )
    {
        errorString += "procedure must end with \"end\"\n";
        return false;
    }
    return true;
}

//语句串分析
bool Grammar::stateBunch()
{
    if( !statement() )
        return false;
    bool state;
    while(true)
    {
        //分析下一个词语
        //分析是否是分号
        //分析下一个句子
        state = statement();
        if(state && result->typeNum==6)
            return true;
        else if(!state && !scanner->isFinish())
            return false;
        else if(!state && scanner->isFinish())
            return true;
    }
}

//语句分析
bool Grammar::statement()
{
    //分析第一个词语
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    //判断是否是变量
    if(result->typeNum == 10)
    {
        if( !equState() )
            return false;
        else
            return true;
    }
    //判断是否是{
    if(result->typeNum == 29)
    {
        if( !stateBlock() )
            return false;
        else
            return true;
    }
    //判断是否是if
    if(result->typeNum == 2)
    {
        if( !ifState() )
            return false;
        else
            return true;
    }
    //判断是否是do
    if(result->typeNum == 5)
    {
        if( !whileState() )
            return false;
        else
            return true;
    }
    if(result->typeNum == 8)
    {
        if( !Disp() )
            return false;
        else
            return true;
    }
    //判断是否是end
    if(result->typeNum == 6)
    {
        return true;
    }
    string token = result->token;
    token = "\"" + token +"\"\n";
    intTostr(scanner->whichState(), errorString);
    errorString += "in the "+errorString+"th statement:\n";
    errorString += "error begin of statement: "+token;
    return false;
}

//赋值语句分析
bool Grammar::equState()
{
    string str = result->token;
    string str1 = str;
    //分析下一个词语
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    //判断是否是关键字:=或=
    if(result->typeNum!=18 && result->typeNum!=25)
    {
        string token = result->token;
        token = "\"" + token +"\"\n";
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack equation before " + token;
        return false;
    }

    str = str+" "+result->token+" ";

    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    if( !expression() )
        return false;
    //判断是否是;
    if(result->typeNum != 26)
    {
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack ;\n";
        return false;
    }

    if(!exist(str1.c_str()))
    {
        var.push_back(str1);
    }

    str += key;
    str = Getseq()+str;
    //code.push_back(str);
    string num;
    intTostr(where(str1.c_str()), num);
    if(isNum)
    {
        str = "lui $r1,";
        str += key;
        code.push_back(str);
        str = "sw $r1,"+num+"($r0)";
        code.push_back(str);
    }
    else
    {
        str = "sw ";
        str += key;
        str += ","+num+"($r0)";
        code.push_back(str);
    }
    return true;
}

//整体语句分析
bool Grammar::stateBlock()
{
    if( !statement() )
        return false;
    while(true)
    {
        //分析下一个句子
        if( !statement() )
            break;
    }
    //判断是否是关键字}
    if(result->typeNum != 30)
    {
        string token = result->token;
        token = "\"" + token +"\"\n";
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack } before " + token;
        return false;
    }
    return true;
}

//条件语句分析
bool Grammar::ifState()
{
    int step_next;
    int goto_if;
    string str;

    if( !Bool() )
        return false;

   //str += key;
    //str = Getseq()+"if !"+str+" goto ";
    str = "lui $r3, 1";
    code.push_back(str);
    str = "bne $r3,";
    str += key;
    code.push_back(str);
    goto_if = code.size()-1;

    //判断是否是then
    if(result->typeNum != 3)
    {
        string token = result->token;
        token = "\"" + token +"\"\n";
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack then before " + token;
        return false;
    }
    /*
    if( !statement() )
        return false;*/
    //分析下一个词语
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    //判断是否是{
    if(result->typeNum != 29)
    {
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack {\n";
        return false;
    }

    if( !stateBlock() )
        return false;

    //判断是否是else
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }

    if(result->typeNum != 7)
    {
        step_next = step;
        intTostr(++step_next, str);
       // str ;
        //str = "("+str+")";
        code[goto_if] += str;
        return true;
    }

    step_next = step;
    intTostr(step_next+2, str);
    str = "("+str+")";
    //code[goto_if] += str;

    str = "jump ";
    code.push_back(str);
    goto_if = code.size()-1;

    //分析下一个词语
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    //判断是否是{
    if(result->typeNum != 29)
    {
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack {\n";
        return false;
    }
    if( !stateBlock() )
        return false;

    step_next = step;
    intTostr(++step_next, str);
    str = "("+str+")";
    code[goto_if] += str;

    return true;
}

bool Grammar::Disp()
{
    string str = result->token;
    //分析下一个词语
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    //判断是否是变量
    if(result->typeNum!=10)
    {
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack var\n";
        return false;
    }

    if(!exist(result->token))
    {
        string token = result->token;
        errorString += "var "+token+" not exist\n";
        return false;
    }
    str += " ";
    str += result->token;

    //分析下一个词语
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    //判断是否是：
    if(result->typeNum!=17)
    {
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack ：\n";
        return false;
    }

    str += " ";
    str += result->token;

    //分析下一个词语
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    //判断是否是数字
    if(result->typeNum!=11)
    {
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack num\n";
        return false;
    }
    str += " ";
    str += result->token;


    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    //判断是否是;
    if(result->typeNum != 26)
    {
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack ;\n";
        return false;
    }

    str = Getseq()+str;
    code.push_back(str);
    return true;
}

//循环语句分析
bool Grammar::whileState()
{
    int current_step = step;
    if( !statement() )
        return false;
    //判断是否是while
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }

    if(result->typeNum != 4)
    {
        string token = result->token;
        token = "\"" + token +"\"\n";
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack while before " + token;
        return false;
    }

    //判断是否是(
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }

    if(result->typeNum != 27)
    {
        string token = result->token;
        token = "\"" + token +"\"\n";
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack ( before " + token;
        return false;
    }

    if( !Bool() )
        return false;
    //判断是否是)
    if(result->typeNum != 28)
    {
        string token = result->token;
        token = "\"" + token +"\"\n";
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack ) before " + token;
        return false;
    }
    //判断是否是;
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }

    if(result->typeNum != 26)
    {
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack ;\n";
        return false;
    }

    string str;
    intTostr(++current_step, str);
    str = Getseq()+"if "+key+" goto ("+str+")";
    code.push_back(str);
    return true;
}

//表达式分析
bool Grammar::expression()
{
    string str;
    if( !term() )
        return false;

    while(true)
    {
        str.clear();
        str += key;

        if(isNum)
        {
            str = "lui $r1,";
            str += key;
            code.push_back(str);
            key = "$r1";
        }

        //分析下一个词语
        //分析是否是加号或减号
        if(result->typeNum!=13 && result->typeNum!=14)
        {
            return true;
        }
        if(result->typeNum == 1)
        {
            str = "add $r3,";
            str += key;
        }

        //str += result->token;

        delete result;
        if( !scanner->match(result) )
        {
            scannerError();
            return false;
        }
        if( !term() )
            return false;

        //str += key;
        if(isNum)
        {
            str = "lui $r2,";
            str += key;
            code.push_back(str);
            key = "$r2";
        }
        str += ",";
        str += key;
        code.push_back(str);
        str = "$r3";
        Print(str);
    }
}

//比较语句分析
bool Grammar::Bool()
{
    string str;
    int cmp;

    //分析第一个词语
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    if( !expression() )
        return false;

    //str += key;
    if(isNum)
    {
        str = "lui $r1,";
        str += key;
        code.push_back(str);
        key = "$r1";
    }


    //判断是否是变量
    if(result->typeNum!=20 && result->typeNum!=22
       && result->typeNum!=23 && result->typeNum!=24)
    {
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack comparison\n";
        return false;
    }

    cmp = result->typeNum;
    str = "slt $r3,";
    string r1 = key;
    //str += result->token;

    //分析下一个词语
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    if( !expression() )
        return false;

    if(isNum)
    {
        str = "lui $r2,";
        str += key;
        code.push_back(str);
        key = "$r2";
    }

    if(cmp==20 || cmp==22)
    {
        str += r1+",";
        str += key;
        code.push_back(str);
    }
    else
    {
        str += key;
        str += ","+r1;
        code.push_back(str);;
    }
    str = "$r3";
    //str += key;
    Print(str);

    return true;
}

//单项分析
bool Grammar::term()
{
    string str;
    if( !factor() )
        return false;

    while(true)
    {
        str.clear();
        str += key;
        //分析下一个词语
        //分析是否是乘号或除号
        delete result;
        if( !scanner->match(result) )
        {
            scannerError();
            return false;
        }
        if(result->typeNum!=15 && result->typeNum!=16)
        {
            return true;
        }

        str += result->token;

        delete result;
        if( !scanner->match(result) )
        {
            scannerError();
            return false;
        }
        if( !factor() )
            return false;

        str += key;
        //Print(str);
    }
}

//因子分析
bool Grammar::factor()
{
    string str;
    //分析第一个词语
    //判断是否是变量
    if(result->typeNum == 10)
    {
        if(!exist(result->token))
        {
            string token = result->token;
            errorString += "var "+token+" not exist\n";
            return false;
        }
        memcopy(key, result->token);
        mem ++;
        mem = (mem%2) +1;
        string num1, num2;
        intTostr(mem, num1);
        str = "lw $r"+num1+",";
        intTostr(where(key), num2);
        str += num2+"($r0)";
        code.push_back(str);
        num2 = "$r"+num1;
        memcopy(key, num2.c_str());
        isNum = false;
        return true;
    }
    //判断是否是数字
    else if(result->typeNum == 11)
    {
        memcopy(key, result->token);
        isNum = true;
        return true;
    }
    //判断是否是括号括起来的表达式
    else if(result->typeNum != 27)
    {
        string token = result->token;
        token = "\"" + token +"\"\n";
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack operand before " + token;
        return false;
    }
    //匹配表达式
    delete result;
    if( !scanner->match(result) )
    {
        scannerError();
        return false;
    }
    if( !expression() )
        return false;
    //匹配右括号
    if( result->typeNum == 28)
    {
        return true;
    }
    else
    {
        string token = result->token;
        token = "\"" + token +"\"\n";
        intTostr(scanner->whichState(), errorString);
        errorString += "in the "+errorString+"th statement:\n";
        errorString += "lack ) before "+token;
        return false;
    }
}

//返回错误原因
string Grammar::error()
{
    return errorString;
}

//整数转换为字符串
bool Grammar::intTostr(int num, string &str)
{
    char ch[40];
    sprintf(ch, "%d", num);
    str = ch;
    return true;
}

//获取词法分析的具体错误
inline void Grammar::scannerError()
{
    intTostr(scanner->whichState(), errorString);
    errorString += "in the "+errorString+"th statement:\n";
    errorString += errorString + scanner->error() +"\n";
}

//字符串拷贝
bool Grammar::memcopy(char* &key, const char* result)
{
    if( key )
        delete key;
    key = (char*)malloc((strlen(result)+1)*sizeof(char));
    strcpy(key, result);
    return true;
}

void Grammar::Print(string str)
{
    string T = Getvar();
    var.push_back(T);
    //输出中间代码
    string num;
    intTostr(where(T.c_str()), num);
    str = "sw "+str+","+num+"($r0)";
    //str = Getseq()+T+" = "+str;
    code.push_back(str);
    mem ++;
    mem = (mem%2) +1;
    string num1;
    intTostr(mem, num1);
    num = "$r"+num1;
    memcopy(key, num.c_str());
}

string Grammar::Getseq()
{
    //获取语句编号
    string s1;
    intTostr(++step, s1);
    s1 = "("+s1+")";
    return s1;
}

string Grammar::Getvar()
{
    //新增变量名
    string s;
    intTostr(++temp, s);
    s = "T"+s;

    memcopy(key, s.c_str());
    isNum = false;
    return s;
}

bool Grammar::exist(const char* token)
{
    for(int i=0; i<var.size(); i++)
    {
        if(strcmp(var[i].c_str(), token) == 0)
            return true;
    }
    return false;
}

int Grammar::where(const char* token)
{
    for(int i=0; i<var.size(); i++)
    {
        if(strcmp(var[i].c_str(), token) == 0)
            return i;
    }
    return -1;
}
