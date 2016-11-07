#include "lex.h"

Lex::Lex(const char *text)
{
    if(!text)
    {
        this->length = 0;
        this->text = NULL;
        this->p_str = 0;
        this->block_len = 0;
    }
    else
    {
        this->length = strlen(text);
        this->text = (char*)malloc((length+1)*sizeof(char));
        if(!this->text)
        {
            puts("no more memmory!");
            return;
        }
        for(int i=0; i<length; i++)
            this->text[i] = text[i];
        this->text[length] = '\0';
        this->p_str = 0;
        this->block_len = 10;
        this->step = 1;
    }
}

Lex::~Lex()
{
    free(text);
    result.clear();
}

bool Lex::match_text()
{
    Tuple *part = NULL;
    while( match(part) )
    {
        result.push_back(part);
    }
    return true;
}

//判断是否匹配完成
bool Lex::isFinish()
{
    if( getbc() != '\0')
        return false;
    else
    {
        retract();
        return true;
    }
    //return p_str>=length;
}

void Lex::PrintResult()
{
    Tuple *part = NULL;
    for(unsigned int i=0; i<result.size(); i++)
    {
       part = (Tuple*)result.at(i);
       if(part->typeNum == 10)
            printf("( %d, \"%s\" )\n", part->typeNum, part->token);
       else
            printf("( %d, %s )\n", part->typeNum, part->token);
    }
}

//错误处理
string Lex::error()
{
    return errorString;
}

bool Lex::match(Tuple *&result)
{
    result = new Tuple();
    if(!result)
        return false;
    char *token = NULL; //保存当前已读取的字符串
    char ch = getbc();  //当前读取的字符

    //匹配字符串
    if( isLetter(ch) )
    {
        while( isLetter(ch) || isDigit(ch) )
        {
            concat(token, ch);
            ch = getch();
        }
        retract();
        result->typeNum = reserve(token);
        memCopy(token, result->token);
    }

    //匹配数字串
    else if( isDigit(ch) )
    {
        while( isDigit(ch) )
        {
            concat(token, ch);
            ch = getch();
        }
        retract();
        result->typeNum = 11;
        memCopy(token, result->token);
    }

    //匹配操作符
    else if( ch == '=' )
    {
        concat(token, ch);
        ch = getch();
        if( ch == '=' )
        {
            concat(token, ch);
            result->typeNum = 18;
            memCopy(token, result->token);
        }
        else
        {
            result->typeNum = -1;
            concat(token, '\0');
            memCopy(token, result->token);
            errorString = result->token;
            errorString = "matching error:\nundefined string :\""+errorString+"\"";
            return false;
        }
    }

    else if( ch == '>' )
    {
        concat(token, ch);
        ch = getch();
        if( ch == '=' )
        {
            concat(token, ch);
            result->typeNum = 24;
            memCopy(token, result->token);
        }
        else
        {
            retract();
            result->typeNum = 23;
            memCopy(token, result->token);
        }
    }

    else if( ch == '<' )
    {
        concat(token, ch);
        ch = getch();
        if( ch == '=' )
        {
            concat(token, ch);
            result->typeNum = 22;
            memCopy(token, result->token);
        }
        if( ch == '>' )
        {
            concat(token, ch);
            result->typeNum = 21;
            memCopy(token, result->token);
        }
        else
        {
            retract();
            result->typeNum = 20;
            memCopy(token, result->token);
        }
    }

    else switch(ch)
    {
    case '(':
        result->typeNum = 27;
        concat(token, ch);
        memCopy(token, result->token);
        break;
    case ')':
        result->typeNum = 28;
        concat(token, ch);
        memCopy(token, result->token);
        break;
    case '#':
        result->typeNum = 0;
        concat(token, ch);
        memCopy(token, result->token);
        break;
    default:
        if(p_str <= length)
        {
            result->typeNum = -1;
            concat(token, '\0');
            memCopy(token, result->token);
            errorString = result->token;
            errorString = "matching error:\nundefined string :\""+errorString+"\"";
            //this->result.clear();
            return false;
        }
        else
        {
            result->typeNum = -1;
            concat(token, '\0');
            memCopy(token, result->token);
            return false;
        }
    }
    return true;
}

char Lex::getch()
{
    if(p_str>=0 && p_str<=length)
        return text[p_str++];
    else
        return '\0';
}

//读取一个非空白字符
char Lex::getbc()
{
    char ch = getch();
    while( ch==' ' || ch=='\n' )
        ch = getch();
    return ch;
}

//字符串连接
bool Lex::concat(char* &str, char ch)
{
    if(!str)
    {
        str = (char*)malloc(block_len*sizeof(char));
        if(!str)
            return false;
        str[0] = ch;
        for(int i=1; i<block_len-1; i++)
            str[i] = ' ';
        str[block_len-1] = '\0';
        return true;
    }

    int i, len = strlen(str);
    //寻找是否有空间存储此字符
    for(i=0; i<len; i++)
    {
        if(str[i] == ' ')
        {
            str[i] = ch;
            return true;
        }
    }
    //若空间不足，增加一块空间
    realloc(str, (len+block_len)*sizeof(char));
    if(!str)
        return false;
    str[i] = ch;
    //往多余的空间内填充规定字符
    for(i=len+1; i<len+block_len-1; i++)
        str[i] = ' ' ;
    str[i] = '\0';
    return true;
}

//判断是否是字母
bool Lex::isLetter(char ch)
{
    if(ch >= 'a' && ch <= 'z')
        return true;
    else if(ch >= 'A' && ch <= 'Z')
        return true;
    else if(ch == '.')
            return true;
    else
        return false;
}

//判断是否是数字
bool Lex::isDigit(char ch)
{
    if(ch >= '0' && ch <= '9')
        return true;
    else
        return false;
}

//返回标记的种别码
int Lex::reserve(const char *token)
{
    int num;
    if( isKeyword(token, num) )
        return num;
    else
        return 10;
}

//回退一个字符
bool Lex::retract()
{
    if(p_str <= 0)
        return false;
    p_str--;
    return true;
}

bool is_subject_att(char* str)
{
    if( strlen(str) <= 2)
        return false;
    if(str[0]=='s' && str[1]=='.')
    {
        for(int i=0; i<strlen(str)-2; i++)
            str[i] = str[i+2];
        str[strlen(str)-2]=0;
        return true;
    }
    return false;
}

bool is_object_att(char* str)
{
    if( strlen(str) <= 2)
        return false;
    if(str[0]=='o' && str[1]=='.')
    {
        for(int i=0; i<strlen(str)-2; i++)
            str[i] = str[i+2];
        str[strlen(str)-2]=0;
        return true;
    }

    return false;
}

//判断是否是关键字
bool Lex::isKeyword(const char* token, int& num)
{
    if(!token)
        return false;

    num = 0;

    if(strCmp(token, "in") == 0)
        num = 1;
    if(strCmp(token, "not") == 0)
        num = 2;
    if(strCmp(token, "and") == 0)
        num = 3;
    if(strCmp(token, "or") == 0)
        num = 4;
    if(strCmp(token, "like") == 0)
        num = 5;
    if( is_subject_att((char*)token) )
        num = 6;
    if( is_object_att((char*)token) )
        num = 7;

    if(num)
        return true;
    else
        return false;
}

//信息复制
bool Lex::memCopy(char* &source, char* &desti)
{
    desti = source;
    for(unsigned int i=0; i<strlen(desti); i++)
    {
        if(desti[i] == ' ')
        {
            desti[i] = '\0';
            break;
        }
    }
    source = NULL;
    return true;
}

//字符串比较
int Lex::strCmp(const char* str1, const char* str2)
{
    if(strlen(str1) < strlen(str2))
        return str1[0]>str2[0]?1:-1;

    unsigned int i;
    for(i=0; i<strlen(str2); i++)
    {
        if(str1[i] != str2[i])
            return str1[i]>str2[i]?1:-1;
    }

    if(str1[i]==' ' || str1[i] == '\0')
        return 0;
    else
        return 1;
}
