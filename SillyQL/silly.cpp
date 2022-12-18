// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <iostream>
#include <sstream>
#include <unordered_map>
#include "database.h"
#include "table.h"
#include "common.h"

using namespace std;

void handle_error(string error);
void create();
void insert();
void print();
void generate_index();
void delete_record();
void remove_table();
void join_tables();
bool check_table(string &table_name);
bool check_column(string table_name, string col_name);

unordered_map<ACTION, string> action_map = {
    {ACTION::CREATE_INDEX, "GENERATE"},
    {ACTION::CREATE_TABLE, "CREATE"},
    {ACTION::DELETE_FROM, "DELETE"},
    {ACTION::INSERT_INTO, "INSERT"},
    {ACTION::JOIN_TABLES, "JOIN"},
    {ACTION::PRINT_DATA, "PRINT"},
    {ACTION::REMOVE_TABLE, "REMOVE"}};

unordered_map<string, EntryType> column_type_map = {
    {"int", EntryType::Int},
    {"bool", EntryType::Bool},
    {"double", EntryType::Double},
    {"string", EntryType::String}};

unordered_map<string, OPERATOR_TYPE> operator_type_map = {
    {"=", OPERATOR_TYPE::eq},
    {">", OPERATOR_TYPE::gt},
    {"<", OPERATOR_TYPE::lt}};

unordered_map<string, INDEX_TYPE> index_type_map = {
    {"hash", INDEX_TYPE::hash_map},
    {"bst", INDEX_TYPE::binary_search_tree}};

Database database;
string command;
string temp;

bool quiet = false;

unordered_map<string, double> times;

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cin.sync_with_stdio(false);
    cout << boolalpha;
    cin >> boolalpha;

    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            cout << "Use the commands: CREATE, # (comment), REMOVE, INSERT, "
                 << "PRINT, DELETE, JOIN, GENERATE, and QUIT to interact with "
                 << "this database platform!" << endl;
            exit(0);
        }
        else if (arg == "-q" || arg == "--quiet")
        {
            quiet = true;
        }
        else
        {
            cerr << "Error: Unknown command line option";
            exit(1);
        }
    }

    while (true)
    {
        try
        {
            cout << "% ";
            cin >> command;
            if (command == "QUIT")
            {
                cout << "Thanks for being silly!" << endl;
                break;
            }
            else if (command.at(0) == '#' || command == "")
            {
                getline(cin, temp);
            }
            else if (command == "CREATE")
            {
                create();
            }
            else if (command == "INSERT")
            {
                insert();
            }
            else if (command == "PRINT")
            {
                print();
            }
            else if (command == "GENERATE")
            {
                generate_index();
            }
            else if (command == "DELETE")
            {
                delete_record();
            }
            else if (command == "REMOVE")
            {
                remove_table();
            }
            else if (command == "JOIN")
            {
                join_tables();
            }
            else
            {
                cout << "Error: unrecognized command" << endl;
                getline(cin, temp);
            }
        }
        catch (...)
        {
            cout << "Error: Something wrong!" << endl;
        }
    }

    for (pair<string, double> t : times)
    {
        cout << t.first << ": " << t.second << endl;
    }
    database.clear();
    exit(0);
}

void create()
{
    string table_name;

    string col_type;
    string col_name;
    size_t col_count;

    vector<EntryType> col_types;
    vector<string> col_names;

    cin >> table_name;
    if (database.has_table(table_name))
    {
        cout << "Error during CREATE: Cannot create already existed table " << table_name << endl;
        getline(cin, temp);
        return;
    }
    cin >> col_count;
    database.add_table(table_name, col_count);

    for (size_t i = 0; i < col_count; i++)
    {
        cin >> col_type;
        col_types.push_back(column_type_map[col_type]);
    }
    for (size_t i = 0; i < col_count; i++)
    {
        cin >> col_name;
        col_names.push_back(col_name);
    }

    for (size_t i = 0; i < col_count; i++)
    {
        database.add_column(table_name, col_names[i], col_types[i]);
    }

    cout << "New table " << table_name << " with column(s) ";
    for (string name : col_names)
    {
        cout << name << " ";
    }
    cout << "created" << std::endl;
}

void insert()
{
    string table_name;
    size_t count;
    vector<string> values;
    string value;
    string value_line = "";
    unordered_map<size_t, EntryType> col_types;

    cin >> temp >> table_name >> count >> temp;

    if (!check_table(table_name))
    {
        return;
    }

    Table *table = database.get_table(table_name);
    size_t col_count = table->get_column_count();
    for (size_t i = 0; i < col_count; ++i)
    {
        col_types[i] = table->get_column_type(i);
    }
    size_t start = table->get_size();
    values.reserve(col_count);

    getline(cin, value_line);
    table->increase_records_capacity(count);
    for (size_t i = 0; i < count; ++i)
    {
        getline(cin, value_line);
        istringstream iss(value_line);
        for (size_t j = 0; j < col_count; ++j)
        {
            iss >> value;
            values.push_back(value);
        }
        table->add_row(values);
        values.clear();
    }
    table->update_index();

    cout << "Added " << count << " rows to " << table_name
         << " from position " << start << " to "
         << (count + start - 1) << endl;
}

void print()
{
    string table_name;
    string line;
    size_t count;
    vector<string> col_names;
    string all;
    string condition_column;
    string col_name;
    OPERATOR_TYPE condition_opr;

    cin >> line >> table_name;
    if (!check_table(table_name))
    {
        return;
    }

    Table *table = database.get_table(table_name);
    cin >> count;

    for (size_t i = 0; i < count; ++i)
    {
        cin >> col_name;
        if (!check_column(table_name, col_name))
        {
            return;
        }
        col_names.push_back(col_name);
    }

    cin >> all;
    if (all == "ALL")
    {
        if (!quiet)
        {

            for (string col : col_names)
            {
                cout << col << " ";
            }
            cout << endl;

            for (size_t i = 0; i < table->get_size(); ++i)
            {
                for (string col : col_names)
                {
                    cout << table->get_value(i, col) << " ";
                }
                cout << endl;
            }
        }
        cout << "Printed " << table->get_size() << " matching rows from "
             << table_name << endl;
    }
    else
    {
        cin >> condition_column;
        if (!check_column(table_name, condition_column))
        {
            return;
        }
        cin >> line;
        condition_opr = operator_type_map[line];

        string str_value;
        cin >> str_value;

        if (!quiet)
        {

            for (string col : col_names)
            {
                cout << col << " ";
            }
            cout << endl;

            vector<KEY_TYPE> result = table->query(condition_column, condition_opr, str_value);

            for (size_t i : result)
            {
                for (string col : col_names)
                {
                    cout << table->get_value(i, col) << " ";
                }
                cout << endl;
            }
            cout << "Printed " << result.size() << " matching rows from "
                 << table_name << endl;
        }
        else
        {
            if (condition_opr == OPERATOR_TYPE::eq)
            {
                table->create_index(condition_column, INDEX_TYPE::hash_map);
            }else{
                table->create_index(condition_column, INDEX_TYPE::binary_search_tree);
            }
            cout << "Printed " << table->query_count(condition_column, condition_opr, str_value) << " matching rows from "
                 << table_name << endl;
        }
    }
}

void generate_index()
{
    string table_name;
    string col_name;
    string index_type;

    cin >> temp >> table_name;
    if (!check_table(table_name))
    {
        return;
    }

    cin >> index_type;
    cin >> temp >> temp >> col_name;
    if (!check_column(table_name, col_name))
    {
        return;
    }

    database.get_table(table_name)->create_index(col_name, index_type_map[index_type]);

    cout << "Created " << index_type << " index for table " << table_name << " on column "
         << col_name << endl;
}

void delete_record()
{
    string table_name;
    string condition_column;
    string condition_operator;
    cin >> temp >> table_name;
    if (!check_table(table_name))
    {
        return;
    }

    cin >> temp >> condition_column;
    if (!check_column(table_name, condition_column))
    {
        return;
    }

    cin >> condition_operator;
    Table *table = database.get_table(table_name);
    OPERATOR_TYPE condition_opr = operator_type_map[condition_operator];

    string str_value;
    cin >> str_value;

    size_t deleted_count = 0;

    deleted_count = table->delete_record(condition_column, condition_opr, str_value);

    cout << "Deleted " << deleted_count << " rows from " << table_name << endl;
}

void remove_table()
{
    string table_name;
    cin >> table_name;
    if (!check_table(table_name))
    {
        return;
    }

    database.remove_table(table_name);
    cout << "Table " + table_name + " deleted" << endl;
}

void join_tables()
{
    string table_name1;
    string table_name2;
    string col1;
    string col2;
    size_t count;

    vector<string> columns;
    vector<size_t> column_tables;

    size_t temp_num;

    cin >> table_name1 >> temp >> table_name2;
    if (!check_table(table_name1))
    {
        return;
    }
    if (!check_table(table_name2))
    {
        return;
    }

    cin >> temp >> col1 >> temp >> col2;
    if (!check_column(table_name1, col1))
    {
        return;
    }
    if (!check_column(table_name2, col2))
    {
        return;
    }

    cin >> temp >> temp >> count;
    for (size_t i = 0; i < count; i++)
    {
        cin >> temp >> temp_num;

        if (temp_num == 1)
        {
            if (!check_column(table_name1, temp))
            {
                return;
            }
        }
        else if (temp_num == 2)
        {
            if (!check_column(table_name2, temp))
            {
                return;
            }
        }
        columns.push_back(temp);
        column_tables.push_back(temp_num);
    }

    // print columns
    if (!quiet)
    {
        for (string col : columns)
        {
            cout << col << " ";
        }
        cout << endl;
    }

    size_t total = 0;

    Table *table1 = database.get_table(table_name1);
    Table *table2 = database.get_table(table_name2);
    string prev_index_column = table2->get_indexed_column();
    INDEX_TYPE prev_type = table2->get_index_type();
    table2->create_index(col2, INDEX_TYPE::hash_map);

    OPERATOR_TYPE opr = operator_type_map["="];

    for (size_t i = 0; i < table1->get_size(); ++i)
    {
        if (quiet)
        {
            total += table2->query_count(col2, opr, table1->get_value(i, col1));
        }
        else
        {
            vector<KEY_TYPE> keys = table2->query(col2, opr, table1->get_value(i, col1));
            total += keys.size();
            for (KEY_TYPE key : keys)
            {
                for (size_t c = 0; c < columns.size(); c++)
                {
                    if (column_tables[c] == 1)
                    {
                        cout << table1->get_value(i, columns[c]) << " ";
                    }
                    else
                    {
                        cout << table2->get_value(key, columns[c]) << " ";
                    }
                }
                cout << endl;
            }
        }
    }
    if (!quiet)
    {
        table2->clear_index();
        if (prev_index_column != "")
        {
            table2->create_index(prev_index_column, prev_type);
        }
    }
    cout << "Printed " << total << " rows from joining "
         << table_name1 << " to " << table_name2 << endl;
}

bool check_table(string &table_name)
{
    if (!database.has_table(table_name))
    {
        cout << "Error during " << command << ": " << table_name << " does not name a table in the database" << endl;
        getline(cin, temp);
        return false;
    }
    return true;
}

bool check_column(string table_name, string col_name)
{
    if (!database.get_table(table_name)->has_column(col_name))
    {
        cout << "Error during " << command << ": " << col_name << " does not name a column in " << table_name << endl;
        getline(cin, temp);
        return false;
    }
    return true;
}
