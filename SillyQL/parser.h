// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#ifndef _DATABASE_PARSER_H_
#define _DATABASE_PARSER_H_
#include "common.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <string>

// Parser contains functions to parse a line of string into database command

using namespace std;

class CommandParser
{
private:
    Database *database;

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

    vector<string> values = {

        "a 1",
        "b 2",
        "c 3",
        "d 4",
        "e 5",
        "f 6",
        "g 7",
        "h 8",
        "i 9",
        "k 10",
    };
    // size_t step = 0;

    bool handle_invalid_command(Result &result,
                                string message = "unrecognized command")
    {
        result.ok = false;
        result.message = message;
        return false;
    }

public:
    CommandParser() {}
    CommandParser(Database *database) : database(database) {}

    bool parse_command(string &command_line, Command &command, Result &result)
    {
        result.ok = true;
        try
        {
            istringstream iss(command_line);
            string action;
            iss >> action;

            if (action == "CREATE")
            {
                command.action = ACTION::CREATE_TABLE;
                if (!parse_command_create_table(iss, command, result))
                {
                    return false;
                }
            }
            else if (action == "INSERT")
            {
                string second = "";
                iss >> second;
                if (second != "INTO")
                {
                    return handle_invalid_command(result);
                }
                command.action = ACTION::INSERT_INTO;
                if (!parse_command_insert(iss, command, result))
                {
                    return false;
                }
            }
            else if (action == "PRINT")
            {
                string second = "";
                iss >> second;
                if (second != "FROM")
                {
                    return handle_invalid_command(result);
                }
                command.action = ACTION::PRINT_DATA;
                if (!parse_command_print(iss, command, result))
                {
                    return false;
                }
            }
            else if (action == "DELETE")
            {
                string second = "";
                iss >> second;
                if (second != "FROM")
                {
                    return handle_invalid_command(result);
                }
                command.action = ACTION::DELETE_FROM;
                if (!parse_command_delete_data(iss, command, result))
                {
                    return false;
                }
            }
            else if (action == "GENERATE")
            {
                string second = "";
                iss >> second;
                if (second != "FOR")
                {
                    return handle_invalid_command(result);
                }
                command.action = ACTION::CREATE_INDEX;
                if (!parse_command_generate_index(iss, command, result))
                {
                    return false;
                }
            }
            else if (action == "REMOVE")
            {
                iss >> command.table;
                if (command.table == "")
                {
                    return handle_invalid_command(result);
                }
                command.action = ACTION::REMOVE_TABLE;
                if (!validate_table(command, result))
                {
                    return false;
                }
            }
            else if (action == "JOIN")
            {
                command.action = ACTION::JOIN_TABLES;
                return parse_command_join(iss, command, result);
            }
            else
            {
                return handle_invalid_command(result);
            }
        }
        catch (...)
        {
            return handle_invalid_command(result);
        }

        return true;
    }

    bool parse_command_create_table(istringstream &iss, Command &command,
                                    Result &result)
    {
        try
        {

            iss >> command.table;
            iss >> command.count;
            if (command.table == "" || command.count == 0)
            {
                return handle_invalid_command(result);
            }
            if (!validate_table(command, result))
            {
                return false;
            }
            for (size_t i = 0; i < command.count; i++)
            {
                string col_type_str = "";
                iss >> col_type_str;
                if (col_type_str == "" || !has_column_type(col_type_str))
                {
                    return handle_invalid_command(result);
                }
                EntryType col_type = get_column_type(col_type_str);
                command.column_types.push_back(col_type);
            }
            for (size_t i = 0; i < command.count; i++)
            {
                string col_name;
                iss >> col_name;
                if (col_name == "")
                {
                    return handle_invalid_command(result);
                }
                command.columns.push_back(col_name);
            }
        }
        catch (...)
        {
            return handle_invalid_command(result);
        }
        return true;
    }

    bool parse_command_insert(istringstream &iss, Command &command,
                              Result &result)
    {
        try
        {
            string temp = "";
            iss >> command.table;
            iss >> command.count;
            iss >> temp;

            if (command.table == "" || command.count == 0 || temp == "")
            {
                return handle_invalid_command(result);
            }

            if (!validate_table(command, result))
            {
                return false;
            }

            return true;
        }
        catch (...)
        {
            return handle_invalid_command(result);
        }
    }

    bool parse_command_print(istringstream &iss, Command &command,
                             Result &result)
    {
        try
        {
            iss >> command.table;
            iss >> command.count;
            if (command.table == "" || command.count == 0)
            {
                return handle_invalid_command(result);
            }

            if (!parse_column_list(iss, command, result))
            {
                return false;
            }

            if (!validate_table(command, result))
            {
                return false;
            }

            string where;
            iss >> where;
            if (where == "ALL")
            {
                command.condition.all = true;
                return true;
            }

            command.condition.all = false;
            return parse_condition(iss, command, result);
        }
        catch (...)
        {
            return handle_invalid_command(result);
        }
    }

    bool parse_command_delete_data(istringstream &iss, Command &command,
                                   Result &result)
    {
        try
        {
            iss >> command.table;
            if (!validate_table(command, result))
            {
                return false;
            }
            string where;
            iss >> where;
            if (where != "WHERE")
            {
                return handle_invalid_command(result);
            }
            command.condition.all = false;
            return parse_condition(iss, command, result);
        }
        catch (...)
        {
            return handle_invalid_command(result);
        }
    }

    bool parse_command_generate_index(istringstream &iss, Command &command,
                                      Result &result)
    {
        try
        {
            iss >> command.table;
            string temp = "";
            iss >> temp;
            command.index_type = get_index_type(temp);
            if (command.index_type == INDEX_TYPE::index_type_unknown)
            {
                return handle_invalid_command(result);
            }
            iss >> temp;
            iss >> temp;
            iss >> temp;
            command.columns.push_back(temp);

            if (!validate_table(command, result))
            {
                return false;
            }
        }
        catch (...)
        {
            return handle_invalid_command(result);
        }
        return true;
    }

    bool parse_command_join(istringstream &iss, Command &command,
                            Result &result)
    {
        try
        {
            iss >> command.table;
            if (command.table == "")
            {
                return handle_invalid_command(result);
            }
            string temp = "";
            iss >> temp;
            if (temp != "AND")
            {
                return handle_invalid_command(result);
            }
            iss >> command.table2;
            if (command.table2 == "")
            {
                return handle_invalid_command(result);
            }
            iss >> temp;
            if (temp != "WHERE")
            {
                return handle_invalid_command(result);
            }
            iss >> command.column1;
            iss >> temp;
            if (temp != "=")
            {
                return handle_invalid_command(result);
            }
            iss >> command.column2;
            iss >> temp;
            if (temp != "AND")
            {
                return handle_invalid_command(result);
            }
            iss >> temp;
            if (temp != "PRINT")
            {
                return handle_invalid_command(result);
            }
            iss >> command.count;
            for (size_t i = 0; i < command.count; ++i)
            {
                if (!iss.good())
                {
                    return handle_invalid_command(result);
                }
                size_t num;
                iss >> temp;
                command.columns.push_back(temp);
                iss >> num;
                command.column_tables.push_back(num);
            }
            if (!validate_table(command, result))
            {
                return false;
            }
        }
        catch (...)
        {
            return handle_invalid_command(result);
        }
        return true;
    }

    bool parse_column_list(istringstream &iss, Command &command,
                           Result &result)
    {
        for (size_t i = 0; i < command.count; i++)
        {
            if (!iss.good())
            {
                return handle_invalid_command(result);
            }
            string col = "";
            iss >> col;
            command.columns.push_back(col);
        }
        return true;
    }

    bool parse_condition(istringstream &iss, Command &command, Result &result)
    {
        if (!iss.good())
        {
            return handle_invalid_command(result);
        }
        iss >> command.condition.column;
        if (!database->has_column(command.table, command.condition.column))
        {
            result.ok = false;
            result.message = command.condition.column +
                             " does not name a column in " + command.table;
            return false;
        }

        if (!iss.good())
        {
            return handle_invalid_command(result);
        }
        string opr_str = "";
        iss >> opr_str;
        OPERATOR_TYPE opr = get_operator_type(opr_str);
        if (opr == OPERATOR_TYPE::operator_unknown)
        {
            return handle_invalid_command(result);
        }
        command.condition.opr = opr;

        if (!iss.good())
        {
            return handle_invalid_command(result);
        }

        string value = "";
        iss >> value;

        Column *col = database->get_column(command.table,
                                           command.condition.column);

        command.condition.value = convert_value(col->type, value);
        return true;
    }

    EntryType get_column_type(string type)
    {
        return column_type_map[type];
    }

    bool has_column_type(string type)
    {
        return column_type_map.find(type) != column_type_map.end();
    }

    OPERATOR_TYPE get_operator_type(string type)
    {
        if (operator_type_map.find(type) != operator_type_map.end())
        {
            return operator_type_map[type];
        }
        return OPERATOR_TYPE::operator_unknown;
    }

    INDEX_TYPE get_index_type(string type)
    {
        if (index_type_map.find(type) != index_type_map.end())
        {
            return index_type_map[type];
        }
        return INDEX_TYPE::index_type_unknown;
    }

    bool validate_table(Command &command, Result &result)
    {
        if (command.action == ACTION::CREATE_TABLE &&
            database->has_table(command.table))
        {
            result.ok = false;
            result.message = "Cannot create already existed table " + command.table;
            return false;
        }

        if (command.action == ACTION::PRINT_DATA ||
            command.action == ACTION::INSERT_INTO ||
            command.action == ACTION::DELETE_FROM ||
            command.action == ACTION::CREATE_INDEX ||
            command.action == ACTION::REMOVE_TABLE ||
            command.action == ACTION::JOIN_TABLES)
        {
            if (!database->has_table(command.table))
            {
                result.ok = false;
                result.message = command.table + " does not name a table in the database";
                return false;
            }

            if (command.action == ACTION::JOIN_TABLES &&
                !database->has_table(command.table2))
            {
                result.ok = false;
                result.message = command.table2 + " does not name a table in the database";
                return false;
            }
        }

        // validate columns
        if (command.action == ACTION::PRINT_DATA ||
            command.action == ACTION::CREATE_INDEX ||
            command.action == ACTION::JOIN_TABLES)
        {
            if (command.columns.size() == 0)
            {
                result.ok = false;
                result.message = "unrecognized command";
                return false;
            }
            if (command.action != ACTION::JOIN_TABLES)
            {
                for (string col : command.columns)
                {

                    if (!database->has_column(command.table, col))
                    {
                        result.ok = false;
                        result.message = col + " does not name a column in " + command.table;
                        return false;
                    }
                }
            }
            else
            {
                if (command.action == ACTION::JOIN_TABLES)
                {
                    if (command.columns.size() == 0)
                    {
                        result.ok = false;
                        result.message = "unrecognized command";
                        return false;
                    }
                    if (!database->has_column(command.table, command.column1))
                    {
                        result.ok = false;
                        result.message = command.column1 + " does not name a column in " + command.table;
                        return false;
                    }
                    if (!database->has_column(command.table2, command.column2))
                    {
                        result.ok = false;
                        result.message = command.column2 + " does not name a column in " + command.table2;
                        return false;
                    }

                    for (size_t i = 0; i < command.count; ++i)
                    {
                        string col = command.columns[i];
                        if (col == "")
                        {
                            result.ok = false;
                            result.message = "unrecognized command";
                            return false;
                        }
                        if (command.column_tables[i] == 1)
                        {

                            if (!database->has_column(command.table, col))
                            {
                                result.ok = false;
                                result.message = col + " does not name a column in " + command.table;
                                return false;
                            }
                        }
                        else if (command.column_tables[i] == 2)
                        {
                            if (!database->has_column(command.table2, col))
                            {
                                result.ok = false;
                                result.message = col + " does not name a column in " + command.table2;
                                return false;
                            }
                        }
                        else
                        {
                            result.ok = false;
                            result.message = "unrecognized command";
                            return false;
                        }
                    }
                }
            }
        }
        result.ok = true;
        return true;
    }
};

#endif