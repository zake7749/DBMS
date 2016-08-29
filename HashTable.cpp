#include "HashTable.h"

HashTable::HashTable()
{

}

HashTable::HashTable(string tableName)
{
    this->tableName = tableName;
}

HashTable::~HashTable()
{

}

void HashTable::printHashTable(){

    unsigned int i,j;

    for(i=0;i<10;i++){
        printf("Bucket %d",i);
        for(j=0;j<bucket[i].size();j++){
            cout << bucket[i][j].attr << ",";
        }
        printf("\n");
    }
}

void HashTable::printHashTable(string path){

    ofstream fout(path.c_str());

    unsigned int i,j;

    for(i=0;i<10;i++){
        fout << "|Bucket " << i  << "|\t";
        for(j=0;j<bucket[i].size();j++){
            if( j+1 == bucket[i].size())
                fout << bucket[i][j].attr;
            else
                fout << bucket[i][j].attr << ",";
        }
        fout << endl;
    }
}


void HashTable::setTableName(string tableName){
    this->tableName = tableName;
}

string HashTable::getTableName(){
    return this->tableName;
}

void HashTable::insertElement(string key, int index){

    char tp[1024];
    strcpy(tp, key.c_str());
    int bucketNumber = hash33(tp);

    grid g;
    g.attr = key;
    g.origIndex = index;
    //cout << tableName << "GRID: " << g.attr << " #" << g.origIndex << " AT: " << bucketNumber <<endl;
    bucket[bucketNumber].push_back(g);
}

int HashTable::hash33(char *key){
    int i, k;
    unsigned int hv = 0;
    k=strlen(key);
    for(i=0;i<k;i++){
        hv=(hv<<5)+hv+key[i];
    }
    hv=hv%10;
    return hv;
}

vector<int> HashTable::findbyKey(string key){

    char tp[1024];
    strcpy(tp, key.c_str());
    int buckNum = hash33(tp);
    vector<int> indexSet;
    size_t i;

    for(i=0;i<bucket[buckNum].size();i++)
        if(bucket[buckNum][i].attr == key )
            indexSet.push_back(bucket[buckNum][i].origIndex);

    return indexSet;
}
