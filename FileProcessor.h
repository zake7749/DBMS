#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#define MAXROW 5

#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

typedef struct txtRowData{
    string attr[MAXROW]; /* row's attributes */
    int rowType; /* based on table BOOK:1 SELL:2 */
}DataRow;

class FileProcessor
{
    public:
        FileProcessor();
        virtual ~FileProcessor();
        vector<DataRow> parseInputFile(const char* fileName, int dataType);

    protected:
    private:
        DataRow parseLine(string line, int type);
};

#endif // FILEPROCESSOR_H
