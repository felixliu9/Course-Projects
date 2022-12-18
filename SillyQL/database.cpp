// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include "database.h"
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

void Database::clear()
{
    for (pair<string, Table *> item : tables)
    {
        item.second->clear();
        delete item.second;
    }
    tables.clear();
}

void Database::remove_table(string & table_name){
    tables[table_name]->clear();
    delete tables[table_name];
    tables.erase(table_name);
}