#include "grammar.h"

bool ReadFile(const char* filename, char* &text)
{
    FILE *fp = fopen(filename, "r");
    if(!fp)
        return false;
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    text = (char*)malloc((len+1)*sizeof(char));
    if(!text)
        return false;
    int i= 0;
    while(!feof(fp))
    {
        text[i++] = fgetc(fp);
        if(text[i-1]<0)
        {
            i--;
            break;
        }
    }
    text[i] = '\0';
    fclose(fp);
    return true;
}

int main(int argc, char* argv[])
{
    FILE *file = fopen("h.txt", "w");
    char* text;
    //读取本地源码
    ReadFile("text.txt", text);
    Lex lex(text);
    lex.match_text();
    puts("Lex result:");
    lex.PrintResult();/*
    Grammar test(text);
    puts("grammar analysis result:");
    if( test.analysis() )
    {
        puts("true");
        for(int i=0; i<test.code.size(); i++)
            {
                fputs(test.code[i].c_str(), file);
                fputs("\n", file);
            }

        for(int i=0; i<test.var.size(); i++)
            puts(test.var[i].c_str());
    }
    else
    {
        string error = test.error();
        puts(error.c_str());
    }*/
    //getchar();
    fclose(file);
    return 0;
}



