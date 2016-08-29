#ifndef ITEMPARSER_H
#define ITEMPARSER_H

#include <cstdio>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

typedef struct condition{
    string leftAttr;
    string rightAttr;
    /*
        attr = "value" : type 1
        attr = attr : type 2
     */
    int type;
}condition;

class ItemParser
{
    public:
        ItemParser();
        virtual ~ItemParser();

        vector<string> parseAttribute(string cmd,string startSymbol,string endSymbol);
        vector<condition> parseConditionClause(string cmd);

    protected:
    private:
        condition parseCondition(string conditionClause);
};

#endif // ITEMPARSER_H
