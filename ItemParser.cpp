#include "ItemParser.h"

ItemParser::ItemParser()
{
    //ctor
}

ItemParser::~ItemParser()
{
    //dtor
}

vector<string> ItemParser::parseAttribute(string cmd,string startSymbol,string endSymbol){

    size_t stPos = cmd.find(startSymbol,0);
    size_t edPos = cmd.find(endSymbol,0);
    size_t subStringLens = 0;

    /*
       FROM XXX,XXX,XXX WHERE
       012345         012
       SELECT XXX,XXX,XXX FROM
       01234567         012
    */
    if(edPos == string::npos){ /* No WHERE clause */
        subStringLens = cmd.length() - stPos - startSymbol.length();
    }else{
        subStringLens = edPos - 2 - stPos - startSymbol.length();
    }

    string attributeString = cmd.substr(stPos + startSymbol.length() + 1, subStringLens );
    #if DEBUGPARSE
        cout << "AttributeString is : " << attributeString << endl;
    #endif // DEBUGPARSE

    stringstream ss(attributeString);
    string term;
    vector<string> AttrTerms;

    while (std::getline(ss, term, ',')) {
        AttrTerms.push_back(term);
    }

    return AttrTerms;
}

vector<condition> ItemParser::parseConditionClause(string cmd){

    size_t whereClausePos = cmd.find("WHERE");
    vector<condition> v;

    if(whereClausePos == string::npos){
        cout << "[S] NO WHERE CLAUSE" << endl;
        return v;
    }

    /* Get the where clause */
    string clause = cmd.substr(whereClausePos+6,cmd.length() - whereClausePos - 5);
    cout << "CLAUSE is :<" << clause << ">" << endl;

    /* split clause into conditions */
    /* XXX=XXX AND XXX=XXX
             21P0123    */

    while(1){
        size_t andPos = clause.find("AND");

        if(andPos == string::npos)/* No more and clause */
            break;

        string cdn = clause.substr(0,andPos-1);
        v.push_back(parseCondition(cdn));
        clause = clause.substr(andPos+4,clause.length()-andPos-4);
    }

    /* Deal the last one */
    string cdn = clause.substr(0,clause.length());
    v.push_back(parseCondition(cdn));
    return v;
}

condition ItemParser::parseCondition(string clause){

    cout << "Get a tuple : <" << clause << ">";
    int EqPos = clause.find("=");

    string leftTerm = clause.substr(0,EqPos);
    string rightTerm = clause.substr(EqPos+1,clause.length()-EqPos-1);
    cout << " Divide it to <" << leftTerm << ">" << " AND " << "<" << rightTerm << ">" << endl;

    condition c;
    size_t quateMarkPos = rightTerm.find("'");
    c.leftAttr = leftTerm;

    if(quateMarkPos != string::npos){
        c.type = 1; /* attr = constant */
        size_t quateMarkLastPos = rightTerm.find_last_of("'");
        c.rightAttr = rightTerm.substr(quateMarkPos+1,quateMarkLastPos-quateMarkPos-1);
    }else{
        c.type = 2; /* attr = attr */
        c.rightAttr = rightTerm;
    }

    return c;
}
