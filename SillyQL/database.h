// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <map>

#include "TableEntry.h"
#include "common.h"
#include "table.h"

using namespace std;

class Database
{
private:
    unordered_map<string, Table *> tables;

public:
    Database() {}

    void add_table(string & name, size_t & col_count)
    {
        tables[name] = new Table(col_count);
    }

    void add_column(string table_name, string name, EntryType col_type)
    {
        tables[table_name]->add_column(name, col_type);
    }

    Table *get_table(string name)
    {
        return tables[name];
    }
    bool has_table(string table_name)
    {
        return tables.find(table_name) != tables.end();
    }

    bool has_column(string table_name, string column_name)
    {
        if (!has_table(table_name))
        {
            return false;
        }
        Table *table = get_table(table_name);
        return table->has_column(column_name);
    }

    string get_column_name(string table_name, size_t sequence)
    {
        return tables[table_name]->get_column_name(sequence);
    }

    void clear();
    void remove_table(string & table_name);

    ~Database()
    {
        
        clear();
    }
};
#endif