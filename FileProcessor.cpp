#include "FileProcessor.h"

FileProcessor::FileProcessor()
{
    //ctor
}

FileProcessor::~FileProcessor()
{
    //dtor
}

vector<DataRow> FileProcessor::parseInputFile(const char* fileName, int dataType){

    ifstream fin;
    fin.open(fileName,ios::in);
    string str;
    vector<DataRow> DataRows; /* a container hold each row in the txtfile*/

    getline(fin,str);/* drop the comment line. */
    while(getline(fin,str)){
        DataRows.push_back(parseLine(str,dataType));
    }
    fin.close();
    return DataRows;
}

DataRow FileProcessor::parseLine(string line, int type){

    stringstream ss(line);
    string item;
    DataRow dr;
    dr.rowType = type;
    int i = 0;

    while (std::getline(ss, item, '|')) {
        dr.attr[i++] = item;
    }

    #if DEBUG
        for(i=0;i<5;i++){
            cout << dr.attr[i] << "\t";
        }
        printf("Row type:%d\n",dr.rowType);
    #endif // DEBUG
    return dr;
}
