#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

typedef struct grid{
    string attr;
    int origIndex;
}grid;

class HashTable
{
    public:
        HashTable();
        HashTable(string tableName);
        virtual ~HashTable();

        void printHashTable(); /* print bucket's content to file */
        void printHashTable(string path); /* print bucket's content to file */
        void insertElement(string key, int index);
        vector<int> findbyKey(string key);

        /* get & set method */
        void setTableName(string s);
        string getTableName();
        vector<grid> bucket[10];

    private:
        int hash33(char* key); /* generate hash code */
        string tableName;
};

#endif // HASHTABLE_H
