#include <cstdio>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>

#include "HashTable.h"
#include "FileProcessor.h"
#include "ItemParser.h"

#define DEBUG 0
#define DEBUGPARSE 1

#define BOOK 1
#define SELL 2

using namespace std;

enum attributeName{isbn, author, title, price, subject, uid, no, isbn_no};
string attrArray[8] = {"isbn","author","title","price","subject","uid","no","isbn_no"};
string opArray[8] = {"books_isbn.txt","books_author.txt","books_title.txt","books_price.txt","books_subject.txt","sellRecord_uid.txt","sellRecord_no.txt","sellRecord_isbn_no.txt"};
string tableName[2] = {"books","sellRecord"};
HashTable ht[8];

/* Structure declaration */

typedef struct joinPair{
    int book;
    int sell;
    int distinct;
    int type; /* 1=LL,2=LR,3=RR */
}join;

/* Parse a SQL command */
void parseSQLCommand(string command,vector<DataRow> &bookRows,vector<DataRow> &sellRows);

/* Build the projection and selection array */
void buildProjectionArray(bool attr[8], vector<string> &attrTerms);
vector<join> buildSelectionArray(vector<condition> &conditions, bool bookIndex[], bool sellIndex[], size_t blens, size_t selens);

void fixSelectionByDistinct(vector<join> &joins,vector<DataRow> br, vector<DataRow> sr, bool attr[8], bool bookIndex[], bool sellIndex[]);
void fixProjectionByTable(bool attr[8], bool get[2], vector<string> &tableTerms);

void printHeader(bool need[8],int table);

/* debug_method */
void debug_parse_term(vector<string> &v);
void debug_condition_term(vector<condition> &conditionTerms);

int main(int argc, char *argv[]){

    /* read data rows for each table, store in the corresponding vector */
    FileProcessor fp;
    vector<DataRow> bookRows,sellRows;
    bookRows = fp.parseInputFile("books.txt",BOOK);
    sellRows = fp.parseInputFile("sellRecord.txt",SELL);

    /* build the hash tables */
    unsigned int i,j;

    for(i=0;i<8;i++){
        ht[i].setTableName(attrArray[i]);
    }

    for(i=0;i<bookRows.size();i++){
        for(j=0;j<5;j++){
            ht[j].insertElement(bookRows[i].attr[j],i);
        }
    }
    for(i=0;i<sellRows.size();i++){
        for(j=0;j<3;j++){
            ht[j+5].insertElement(sellRows[i].attr[j],i);
        }
    }

    for(i=0;i<8;i++)
        ht[i].printHashTable(opArray[i]);

    /* query waiting loop */
    while(1){
        string query;
        getline(cin,query);
        #if DEBUG
            cout << "GET QUERY : " << query << endl;
        #endif
        parseSQLCommand(query,bookRows,sellRows);
    }

    return 0;
}

void parseSQLCommand(string cmd,vector<DataRow> &bookRows,vector<DataRow> &sellRows){

    /* Drop semicolon */
    size_t semicolon = cmd.find(";");
    if( semicolon != string::npos )
        cmd.erase(semicolon,1);

    /* distinct check */
    bool distinct = false;
    /* SELECT DISTINCT a,b,c...
            21P       L
    */
    size_t distinctPos = cmd.find("DISTINCT");

    if(distinctPos != string::npos){
        cmd.erase(distinctPos,9);
        distinct = true;
        cout << "[DISTINCT Mode]" << cmd << endl;
    }

    /* get attribute,tableNames and conditions from command */
    ItemParser parser;

    vector<string> attrTerms = parser.parseAttribute(cmd,"SELECT","FROM");/* get attributes */

    vector<string> tableTerms = parser.parseAttribute(cmd,"FROM","WHERE");/* get tables */

    vector<condition> conditionTerms = parser.parseConditionClause(cmd);/* get conditions */
    debug_condition_term(conditionTerms);

    /* build projection boolean array */

    bool projAttr[8],get[2];
    bool bookIndex[bookRows.size()];
    bool sellIndex[sellRows.size()];

    buildProjectionArray(projAttr,attrTerms);

    size_t i,j;
    cout << "SELCET ATTRIBUTE:" << endl;
    cout << "-------------------------" << endl;
    for(i=0;i<8;i++)
        cout << attrArray[i] << "\t"<< projAttr[i] << endl;
    cout << "-------------------------" << endl;

    vector<join> joinSet = buildSelectionArray(conditionTerms,bookIndex,sellIndex,bookRows.size(),sellRows.size());

    /* remove the useless attribute and duplicate value */

    fixProjectionByTable(projAttr, get, tableTerms);
    if(distinct){
        fixSelectionByDistinct(joinSet, bookRows, sellRows, projAttr, bookIndex, sellIndex);
    }

    cout << "-------------------------" << endl;

    /* Print the result */

    if(joinSet.empty()){/* with where clause, but one table only*/

        if(get[0]){

            printHeader(projAttr,BOOK);

            for(i=0;i<bookRows.size();i++){
                if(bookIndex[i]){
                    for(j=0;j<5;j++)
                        if(projAttr[j])
                            cout << bookRows[i].attr[j] << "\t";
                    cout << endl;
                }
            }
        }
        if(get[1]){

            printHeader(projAttr,SELL);

            for(i=0;i<sellRows.size();i++){
                if(sellIndex[i]){
                    for(j=0;j<3;j++)
                        if(projAttr[j+5])
                            cout << sellRows[i].attr[j] << "\t";
                    cout << endl;
                }
            }
        }
    }else{/* with where clause, join 2 table */


        for(i=0;i<joinSet.size();i++){

            /* join type = 2 */
            if(joinSet[i].type== 2 && bookIndex[joinSet[i].book] && sellIndex[joinSet[i].sell] ){

                if(distinct && joinSet[i].distinct )
                    continue;

                for(j=0;j<5;j++)
                    if(projAttr[j])
                        cout << bookRows[joinSet[i].book].attr[j] << "\t";
                for(j=0;j<3;j++)
                    if(projAttr[j+5])
                        cout << sellRows[joinSet[i].sell].attr[j] << "\t";
                cout << endl;
            }

            /* join type = 1 */
            if(joinSet[i].type== 1 && joinSet[i].book == joinSet[i].sell ){


                if(distinct && joinSet[i].distinct )
                    continue;

                for(j=0;j<5;j++)
                    if(projAttr[j+5])
                        cout << bookRows[joinSet[i].book].attr[j] << "\t";
                cout << endl;
            }

            /* join type = 3 */
            if(joinSet[i].type== 3 && joinSet[i].book == joinSet[i].sell ){


                if(distinct && joinSet[i].distinct )
                    continue;

                for(j=0;j<3;j++)
                    if(projAttr[j+5])
                        cout << sellRows[joinSet[i].book].attr[j] << "\t";
                cout << endl;
            }
        }
    }
}

void buildProjectionArray(bool attr[8],vector<string> &attrTerms){

    size_t i,j;
    if(find(attrTerms.begin(), attrTerms.end(), "*") != attrTerms.end()){
        for(i=0;i<8;i++)
            attr[i] = 1;
    }else{
        for(i=0;i<8;i++)
            attr[i] = 0;

        for(i=0;i<attrTerms.size();i++)
            for(j=0;j<8;j++)
                if(attrTerms[i] == attrArray[j]){
                    attr[j] = 1;
                    break;
                }
    }
}

vector<join> buildSelectionArray(vector<condition> &conditions, bool bookIndex[], bool sellIndex[], size_t blens, size_t selens){

    vector<join> joinSet;
    vector<int> mid;

    size_t i,j,k,rightIndex;

    int bookClauseNum = 0, sellClauseNum = 0,book[blens],sell[selens];/* For intersection */

    for(i=0;i<blens;i++){
        bookIndex[i] = 0;
        book[i] = 0;
    }
    for(i=0;i<selens;i++){
        sellIndex[i] = 0;
        sell[i] = 0;
    }

    for(i=0;i<conditions.size();i++){

        for(j=0;j<8;j++)
            if(conditions[i].leftAttr == attrArray[j]) /* get hashtable index*/
                break;

        if(conditions[i].type == 1){
            /* constant check (type1) */
            mid = ht[j].findbyKey(conditions[i].rightAttr);

            if(j<5){
                bookClauseNum++;
                for(k=0;k<mid.size();k++)
                    book[mid[k]]++;
            }else{
                sellClauseNum++;
                for(k=0;k<mid.size();k++)
                    sell[mid[k]]++;
            }
        }else{
            /* join condition check (type2) */
            for(rightIndex=0;rightIndex<8;rightIndex++)
                if(conditions[i].rightAttr == attrArray[rightIndex]) /* get hashtable index*/
                    break;


            /* join type check */
            if(j > rightIndex ){
                int temp = 0;
                temp = j;
                j = rightIndex;
                rightIndex = temp;
            }

            int joinType = 0;
            cout << "[JOIN]: DEAL WITH " << attrArray[j] << " AND " << attrArray[rightIndex] << endl;

            if(j < 5 && rightIndex < 5)
                joinType = 1;
            else if(j < 5 && rightIndex >= 5)
                joinType = 2;
            else
                joinType = 3;

            int bi;
            size_t slot,kk;
            vector<int> sameKey;
            join lrJoin;

            for(bi=0;bi<10;bi++){
                for(slot=0;slot<ht[j].bucket[bi].size();slot++){

                    string lstr = ht[j].bucket[bi][slot].attr;
                    sameKey = ht[rightIndex].findbyKey(lstr);


                    for(kk=0;kk<sameKey.size();kk++){
                        lrJoin.book = ht[j].bucket[bi][slot].origIndex;
                        lrJoin.sell = sameKey[kk];
                        lrJoin.distinct = 0;
                        lrJoin.type = joinType;

                        if(joinType == 1)
                            cout << "[Join]: Find Book#" <<lrJoin.book << " = Book#"<< lrJoin.sell << endl;
                        if(joinType == 2)
                            cout << "[Join]: Find Book#" <<lrJoin.book << " = Sell#"<< lrJoin.sell << endl;
                        if(joinType == 3)
                            cout << "[Join]: Find Sell#" <<lrJoin.book << " = Sell#"<< lrJoin.sell << endl;

                        joinSet.push_back(lrJoin);
                    }

                }
            }
        }
    }

    /* Do intersection */
    for(i=0;i<blens;i++)
        if(book[i] == bookClauseNum){
            bookIndex[i] = 1;
        }

    for(i=0;i<selens;i++)
        if(sell[i] == sellClauseNum)
            sellIndex[i] = 1;

    return joinSet;
}

void fixProjectionByTable(bool attr[8],bool get[2], vector<string> &tableTerms){

    unsigned int i,j;
    for(i=0;i<2;i++)
        get[i] = false;

    for(i=0;i<tableTerms.size();i++)
        for(j=0;j<2;j++)
            if(tableTerms[i]==tableName[j]){
                get[j] = true;
                break;
            }

    if(!get[0]){
        /* not select from book*/
        for(i=0;i<5;i++){
            if(attr[i]){
                attr[i] = false;
                cout << "Close attribute: " << attrArray[i] << endl;
            }
        }
    }
    if(!get[1]){
        /* not select from sellRecord*/
        for(i=5;i<8;i++){
            if(attr[i]){
                attr[i] = false;
                cout << "Close attribute: " << attrArray[i] << endl;
            }
        }
    }
}

void fixSelectionByDistinct(vector<join> &joins,vector<DataRow> br, vector<DataRow> sr, bool attr[8], bool bookIndex[], bool sellIndex[]){

    size_t disAttr,i;
    set<string> s;

    for(disAttr=0;disAttr<8;disAttr++)
        if(attr[disAttr])
            break;

    if(joins.empty()){

        if(disAttr < 5){

            for(i=0;i<br.size();i++){

                if(bookIndex[i]){
                    if( s.find(br[i].attr[disAttr]) != s.end() ){
                        bookIndex[i] = false;
                        //cout <<br[i].attr[disAttr] << "\t"<< "²^¨O\n";
                    }
                    else{
                        s.insert(br[i].attr[disAttr]);
                        //cout <<br[i].attr[disAttr] << "\t"<< "®Ê¯Å\n";
                    }
                }
            }

        } else {

            for(i=0;i<sr.size();i++){
                if(sellIndex[i]){
                    if(s.find(sr[i].attr[disAttr-5]) != s.end() )
                        sellIndex[i] = false;
                    else
                        s.insert(sr[i].attr[disAttr-5]);
                }
            }
        }

    } else {

        if(disAttr < 5){

            for(i=0;i<joins.size();i++){

                int bookIdx = joins[i].book;

                if(bookIndex[bookIdx]){
                    if(s.find(br[bookIdx].attr[disAttr]) != s.end() )
                        joins[i].distinct = 1;
                    else
                        s.insert(br[bookIdx].attr[disAttr]);
                }
            }

        } else {

            for(i=0;i<joins.size();i++){

                int sellIdx = joins[i].sell;

                if(sellIndex[sellIdx]){

                    if(s.find(sr[sellIdx].attr[disAttr-5]) != s.end())
                        joins[i].distinct = 1;
                    else
                        s.insert(sr[sellIdx].attr[disAttr - 5]);
                }

            }

        }
    }
}

void debug_parse_term(vector<string> &v){
    size_t i;
    for(i=0;i<v.size();i++)
        cout << "TERM#" << i << ": " << v[i] << endl;
}

void debug_condition_term(vector<condition> &v){
    size_t i;
    for(i=0;i<v.size();i++){
        cout << "Type:" << v[i].type << " LV:<" << v[i].leftAttr << "> RV:<" << v[i].rightAttr << ">\n";
    }
}

void printHeader(bool need[8],int table){

    int i;

    if(table == BOOK){
        for(i=0;i<5;i++)
            if(need[i])
                cout << attrArray[i] << "\t";
        cout << endl;
    } else if(table == SELL ) {
        for(i=5;i<8;i++)
            if(need[i])
                cout << attrArray[i] << "\t";
        cout << endl;
    } else {
        for(i=0;i<8;i++)
            if(need[i])
                cout << attrArray[i] << "\t";
        cout << endl;
    }

}
