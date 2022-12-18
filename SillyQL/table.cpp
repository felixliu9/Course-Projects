// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "table.h"

using namespace std;

bool match(TableEntry &entry, OPERATOR_TYPE &opr, TableEntry &value)
{
    return ((opr == OPERATOR_TYPE::eq && entry == value) ||
            (opr == OPERATOR_TYPE::gt && entry > value) ||
            (opr == OPERATOR_TYPE::lt && entry < value));
}

size_t Table::delete_record(string &column_name, OPERATOR_TYPE &opr, string &value)
{
    size_t deleted = 0;
    size_t col_index = get_column_index(column_name);
    EntryType type = get_column_type(column_name);
    TableEntry entry(convert_value(type, value));
    auto func = [&col_index, &opr, &entry](vector<TableEntry> const &record)
    {
        if (opr == OPERATOR_TYPE::eq)
        {
            return record[col_index] == entry;
        }
        else if (opr == OPERATOR_TYPE::gt)
        {
            return record[col_index] > entry;
        }
        else
        {
            return record[col_index] < entry;
        }
    };
    auto delete_start = remove_if(records.begin(), records.end(), func);
    deleted = (size_t)distance(delete_start, records.end());
    records.erase(delete_start, records.end());

    if (deleted > 0 && indexed_column_name != "")
    {
        hash_index.clear();
        bst_index.clear();
        string cur_indexed_column_name = indexed_column_name;
        indexed_column_name = "";
        create_index(cur_indexed_column_name, index_type);
    }
    cur_size -= deleted;
    records.resize(cur_size);
    return deleted;
}

void Table::create_index(string &column_name, INDEX_TYPE type)
{
    if (indexed_column_name == column_name && index_type == type)
    {
        return;
    }

    if (indexed_column_name != "")
    {
        // clear current index
        hash_index.clear();
        bst_index.clear();
    }

    indexed_column_name = column_name;
    index_type = type;
    size_t sequence = column_indexes[column_name];

    for (size_t i = 0; i < get_size(); ++i)
    {
        if (index_type == INDEX_TYPE::hash_map)
        {
            hash_index[records[i][sequence]].push_back(i);
        }
        else
        {
            bst_index[records[i][sequence]].push_back(i);
        }
    }
}

size_t Table::query_count(string &column_name, OPERATOR_TYPE &opr, TableEntry value)
{
    size_t total = 0;
    vector<KEY_TYPE> result;

    if (column_name == indexed_column_name)
    {
        // query using index
        if (index_type == INDEX_TYPE::hash_map)
        {
            switch (opr)
            {
            case OPERATOR_TYPE::eq:
            {
                if (hash_index.count(value) > 0)
                {
                    total += hash_index[value].size();
                }
                break;
            }
            case OPERATOR_TYPE::gt:
            case OPERATOR_TYPE::lt:
            {
                for (pair<TableEntry, vector<KEY_TYPE>> item : hash_index)
                {
                    if (match(item.first, opr, value))
                    {
                        total += hash_index[item.first].size();
                    }
                }
                break;
            }
            }
        }
        else
        {
            switch (opr)
            {
            case OPERATOR_TYPE::eq:
            {
                if (bst_index.count(value) > 0)
                {
                    total += bst_index[value].size();
                }
                break;
            }
            case OPERATOR_TYPE::gt:
            {
                for (index_bst_iterator it =
                         bst_index.upper_bound(value);
                     it != bst_index.end(); ++it)
                {
                    total += it->second.size();
                }
                break;
            }
            case OPERATOR_TYPE::lt:
            {
                index_bst_iterator lower_bound =
                    bst_index.lower_bound(value);

                for (index_bst_iterator it = bst_index.begin();
                     it != lower_bound; ++it)
                {
                    total += it->second.size();
                }
                break;
            }
            }
        }
    }
    else
    {
        // no index query from original data
        for (size_t i = 0; i < records.size(); ++i)
        {
            if (match(records[i][get_column_index(column_name)], opr, value))
            {
                total++;
            }
        }
    }

    return total;
}

vector<KEY_TYPE> Table::query(string &column_name, OPERATOR_TYPE &opr, TableEntry value)
{
    vector<KEY_TYPE> result;

    if (column_name == indexed_column_name)
    {
        // query using index
        if (index_type == INDEX_TYPE::hash_map)
        {
            switch (opr)
            {
            case OPERATOR_TYPE::eq:
            {
                if (hash_index.count(value) > 0)
                {
                    result.insert(result.end(), hash_index[value].begin(), hash_index[value].end());
                }
                break;
            }
            case OPERATOR_TYPE::gt:
            case OPERATOR_TYPE::lt:
            {
                for (pair<TableEntry, vector<KEY_TYPE>> item : hash_index)
                {
                    if (match(item.first, opr, value))
                    {
                        result.insert(result.end(), item.second.begin(), item.second.end());
                    }
                }
                sort(result.begin(), result.end());
                break;
            }
            }
        }
        else
        {
            switch (opr)
            {
            case OPERATOR_TYPE::eq:
            {
                if (bst_index.count(value) > 0)
                {
                    result.insert(result.end(), bst_index[value].begin(), bst_index[value].end());
                }
                break;
            }
            case OPERATOR_TYPE::gt:
            {
                for (index_bst_iterator it =
                         bst_index.upper_bound(value);
                     it != bst_index.end(); ++it)
                {
                    result.insert(result.end(), it->second.begin(), it->second.end());
                }
                break;
            }
            case OPERATOR_TYPE::lt:
            {
                index_bst_iterator lower_bound =
                    bst_index.lower_bound(value);
                for (index_bst_iterator it = bst_index.begin();
                     it != lower_bound; ++it)
                {
                    result.insert(result.end(), it->second.begin(), it->second.end());
                }
                break;
            }
            }
        }
    }
    else
    {
        // no index query from original data
        for (size_t i = 0; i < records.size(); ++i)
        {
            if (match(records[i][get_column_index(column_name)], opr, value))
            {
                result.push_back(i);
            }
        }
    }

    return result;
}