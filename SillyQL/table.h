// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef _TABLE_H_
#define _TABLE_H_

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <map>

#include "TableEntry.h"
#include "common.h"

using namespace std;

using index_bst_iterator =
    map<TableEntry, vector<KEY_TYPE>>::iterator;
using index_hash_iterator =
    unordered_map<TableEntry, vector<KEY_TYPE>>::iterator;
using record_iterator = vector<vector<TableEntry>>::iterator;

class Table
{
private:
    vector<vector<TableEntry>> records;
    vector<string> columns;
    unordered_map<string, size_t> column_indexes;
    unordered_map<string, EntryType> column_types;

    // indexes
    INDEX_TYPE index_type;
    string indexed_column_name = "";

    map<TableEntry, vector<KEY_TYPE>> bst_index;
    unordered_map<TableEntry, vector<KEY_TYPE>> hash_index;
    size_t cur_size = 0;
    size_t cur_col_size = 0;

public:
    Table() {}
    Table(size_t column_count)
    {
        columns.resize(column_count);
    }

    void clear_index()
    {
        indexed_column_name = "";
        hash_index.clear();
        bst_index.clear();
    }
    void increase_records_capacity(size_t records_capacity)
    {
        records.resize(records.size() + records_capacity);
        for (size_t i = 0; i < records_capacity; ++i)
        {
            records[cur_size + i].reserve(columns.size());
        }
    }

    void add_column(string name, EntryType type)
    {
        columns[cur_col_size] = name;
        column_indexes[name] = cur_col_size;
        column_types[name] = type;
        cur_col_size++;
    }

    void add_row(vector<string> &row)
    {
        for (size_t i = 0; i < row.size(); i++)
        {
            records[cur_size].push_back(convert_value(get_column_type(get_column_name(i)), row[i]));
        }
        cur_size++;
    }
    
    void update_index()
    {
        if (indexed_column_name != "")
        {
            hash_index.clear();
            bst_index.clear();
            string prev = indexed_column_name;
            indexed_column_name = "";
            create_index(prev, index_type);
        }
    }

    vector<TableEntry> get_record(size_t &id)
    {
        return records[id];
    }

    TableEntry get_value(size_t &index, string &col_name)
    {
        return records[index][get_column_index(col_name)];
    }
    string get_indexed_column()
    {
        return indexed_column_name;
    }

    INDEX_TYPE get_index_type()
    {
        return index_type;
    }

    void remove_index()
    {
        indexed_column_name = "";
        hash_index.clear();
        bst_index.clear();
    }

    bool has_column(string column)
    {
        return column_indexes.find(column) != column_indexes.end();
    }

    size_t get_column_index(string column)
    {
        return column_indexes[column];
    }

    string get_column_name(size_t sequence)
    {
        return columns[sequence];
    }

    EntryType get_column_type(string name)
    {
        return column_types[name];
    }
    EntryType get_column_type(size_t sequence)
    {
        return column_types[get_column_name(sequence)];
    }

    size_t get_column_count()
    {
        return columns.size();
    }

    size_t get_size()
    {
        return records.size();
    }

    void clear_data()
    {

        for (vector<TableEntry> record : records)
        {
            record.clear();
        }
        records.clear();
    }

    void clear()
    {
        clear_data();
        columns.clear();
        column_indexes.clear();
        column_types.clear();
        hash_index.clear();
        bst_index.clear();
    }
    size_t query_count(string &column_name, OPERATOR_TYPE &opr, TableEntry value);
    size_t query_count(string &column_name, OPERATOR_TYPE &opr, string &str_value)
    {
        return query_count(column_name, opr, convert_value(get_column_type(column_name), str_value));
    }
    size_t delete_record(string &column_name, OPERATOR_TYPE &opr, string &value);
    vector<KEY_TYPE> query(string &column_name, OPERATOR_TYPE &opr, TableEntry value);
    vector<KEY_TYPE> query(string &column_name, OPERATOR_TYPE &opr, string &str_value)
    {
        return query(column_name, opr, convert_value(get_column_type(column_name), str_value));
    }
    void create_index(string &column_name, INDEX_TYPE type);

    ~Table()
    {
        clear();
    }
};

#endif