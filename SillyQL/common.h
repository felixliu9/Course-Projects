// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef _DATABASE_COMMON_H_
#define _DATABASE_COMMON_H_

#include <string>
#include <vector>
#include <iostream>
#include "TableEntry.h"

using namespace std;

using KEY_TYPE = size_t;

enum INDEX_TYPE
{
    hash_map,
    binary_search_tree
};

enum OPERATOR_TYPE
{
    gt,
    lt,
    eq
};

enum ACTION
{
    CREATE_TABLE,
    INSERT_INTO,
    DELETE_FROM,
    CREATE_INDEX,
    PRINT_DATA,
    JOIN_TABLES,
    REMOVE_TABLE
};

static TableEntry convert_value(EntryType type, string &str_value)
{
    if (type == EntryType::Bool)
    {
        return TableEntry(str_value == "true");
    }
    else if (type == EntryType::Double)
    {
        return TableEntry(stod(str_value));
    }
    else if (type == EntryType::Int)
    {
        return TableEntry(stoi(str_value));
    }
    else
    {
        return TableEntry(str_value);
    }
}

#endif