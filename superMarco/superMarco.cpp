// B99292359FFD910ED13A7E6C7F9705B8742F0D79

// felixliu

#define xcode_redirect_hpp

#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <unordered_map>
#include <cstdlib>
#include <sstream>
#include <string>
#include <getopt.h>
#include "superMarco.h"
#include "xcode_redirect.hpp"

using namespace std;

/*
vector<string> split(const string &s, char delim)
{
    vector<string> result;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim))
    {
        result.push_back(item);
    }

    return result;
}
*/

short cell_short_type(char c)
{
    for (size_t i = 0; i < VALID_CELL_TYPES.size(); ++i)
    {
        if (VALID_CELL_TYPES[i] == c)
        {
            return VALID_CELL_VALUES[i];
        }
    }
    return -1;
}

int main(int argc, char **argv)
{
    int opt;
    const struct option longopts[] = {
        {"queue", no_argument, nullptr, 'q'},
        {"stack", no_argument, nullptr, 's'},
        {"output", required_argument, nullptr, 'o'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}};
    int index = 0;

    string search_type;
    int count_search_type = 0;
    string output_type = "M";
    while ((opt = getopt_long(argc, argv, "qso:h", longopts, &index)) != -1)
    {
        switch (opt)
        {
        case 'q':
            search_type = "queue";
            count_search_type++;
            break;

        case 's':
            search_type = "stack";
            count_search_type++;
            break;

        case 'o':
            output_type = optarg;
            break;

        case 'h':
            cout << "Description : this program can find the path from "
                 << "Marco's starting location to Countess Cherry's "
                 << "location by using two different routing schemes"
                 << endl
                 << endl;
            cout << "--stack, -s: If this switch is set, use the stack-based "
                 << "routing scheme" << endl
                 << endl;
            cout << "--queue, -q: If this switch is set, use the queue-based "
                 << "routing scheme" << endl
                 << endl;
            cout << "--output (M|L), -o (M|L): Indicates the output file "
                 << "format by following the flag with an M (map format) "
                 << "or L (coordinate list format). If the --output option "
                 << "is not specified, default to map output format (M), if "
                 << "--output is specified on the command line, the argument "
                 << "(either M or L) to it is required." << endl
                 << endl;
            cout << "--help, -h: help menu" << endl
                 << endl;
            return 0;

        default:
            cerr << "Unknown command line option" << endl;
            exit(1);
        }
    }

    if (count_search_type > 1)
    {
        cerr << "Stack or queue can only be specified once" << endl;
        exit(1);
    }
    else if (count_search_type < 1)
    {
        cerr << "Stack or queue must be specified" << endl;
        exit(1);
    }
    Game_map game_map(cin);
    if (!game_map.is_read_successful())
    {
        exit(1);
    }
    int total_visited = 0;
    if (game_map.start_search(search_type, total_visited))
    {
        game_map.output(output_type, cout);
    }
    else
    {
        cout << "No solution, " << total_visited << " tiles discovered."
             << endl;
    }
    return 0;
}

Game_map::Game_map(istream &in)
{
    read_successful = read(in);
}

vector<Location> Game_map::get_next_moves(Location &loc)
{
    vector<Location> next;
    int cur_room_number = loc.room;
    int cur_row = loc.row;
    int cur_col = loc.col;
    short cur_type = get_value(loc) % TOTAL_TYPES;
    if (cur_type < EMPTY)
    {
        short room_number = cur_type;
        if (!has_room(room_number) || room_number == cur_room_number)
        {
            return next;
        }
        Location new_loc{room_number, cur_row, cur_col};
        short new_type = get_value(new_loc);
        if (!is_visitable(new_type))
        {
            return next;
        }
        if (new_type == COUNTESS_TYPE)
        {
            next.clear();
            next.push_back(new_loc);
            set_value(new_loc, new_type + (cur_room_number + 5) * TOTAL_TYPES);
            return next;
        }
        set_value(new_loc, new_type + (cur_room_number + 5) * TOTAL_TYPES);
        next.push_back(new_loc);
        return next;
    }

    for (short i = 0; i < 4; ++i)
    {
        int r = cur_row + MOVES[i][0];
        int c = cur_col + MOVES[i][1];
        if (r < 0 || r >= size || c < 0 || c >= size)
        {
            continue;
        }
        Location new_loc{cur_room_number, r, c};
        short new_type = get_value(new_loc);
        if (new_type == COUNTESS_TYPE)
        {
            next.clear();
            next.push_back(new_loc);
            set_value(new_loc, new_type + (i + 1) * TOTAL_TYPES);
            return next;
        }
        if (!is_visitable(new_type))
        {
            continue;
        }
        set_value(new_loc, new_type + (i + 1) * TOTAL_TYPES);
        next.push_back(new_loc);
    }
    return next;
}

bool Game_map::start_search(string search_type, int &total_visited)
{
    deque<Location> deck;
    bool found = false;
    Location cur_loc{start_room, start_row, start_col};
    deck.push_back(cur_loc);
    while (!deck.empty())
    {
        if (search_type == "stack")
        {
            cur_loc = deck.back();
            deck.pop_back();
        }
        else if (search_type == "queue")
        {
            cur_loc = deck.front();
            deck.pop_front();
        }
        total_visited++;
        vector<Location> next = get_next_moves(cur_loc);
        for (Location next_loc : next)
        {
            if (get_value(next_loc) % TOTAL_TYPES == COUNTESS_TYPE)
            {
                found = true;
                break;
            }
            deck.push_back(next_loc);
        }
        if (found)
        {
            break;
        }
    }
    if (found)
    {
        get_path();
    }
    return found;
}

bool Game_map::read(istream &in)
{
    string type;
    getline(in, type);
    string line;
    getline(in, line);
    num_rooms = stoi(line);
    getline(in, line);
    size = stoi(line);
    for (int i = 0; i < num_rooms; ++i)
    {
        vector<vector<short>> new_room;
        for (int r = 0; r < size; ++r)
        {
            vector<short> new_row(size_t(size), EMPTY);
            new_room.push_back(new_row);
        }
        cells.push_back(new_room);
    }
    if (type == "L")
    {
        return read_list(in);
    }
    else if (type == "M")
    {
        return read_map(size, in);
    }
    return false;
}

bool Game_map::read_list(istream &in)
{
    string line;
    while (getline(in, line))
    {
        if (line == "" || line.compare(0, 2, "//") == 0)
        {
            continue;
        }
        int room_number = line.at(1) - '0';
        line = line.substr(3, line.length() - 4);
        int row = 0;
        int col = 0;
        size_t cur_pos = 0;
        for (size_t i = 0; i < line.length(); ++i)
        {
            if (line[i] == ',')
            {
                if (cur_pos == 0)
                {
                    row = stoi(line.substr(0, i));
                    cur_pos = i + 1;
                }
                else
                {
                    col = stoi(line.substr(cur_pos, i - cur_pos + 1));
                    cur_pos = i + 1;
                    break;
                }
            }
        }
        short type = cell_short_type(line.at(line.length() - 1));
        if (!has_room(room_number))
        {
            cerr << "Invalid room number" << endl;
            return false;
        }
        if (row >= size || row < 0)
        {
            cerr << "Invalid row number" << endl;
            return false;
        }
        if (col >= size || col < 0)
        {
            cerr << "Invalid column number" << endl;
            return false;
        }

        if (type < 0)
        {
            cerr << "Invalid map type" << endl;
            return false;
        }
        if (type == COUNTESS_TYPE)
        {
            countess_room = room_number;
            countess_row = row;
            countess_col = col;
        }
        else if (type == START_TYPE)
        {
            start_room = room_number;
            start_row = row;
            start_col = col;
        }
        set_value(room_number, row, col, type);
    }
    return true;
}

bool Game_map::read_map(int size, istream &in)
{
    string line;
    int room_number = 0;
    int row = 0;
    while (getline(in, line))
    {
        if (line == "" || line.compare(0, 2, "//") == 0)
        {
            continue;
        }
        if (row == size)
        {
            row = 0;
            room_number++;
        }
        for (int col = 0; col < size; ++col)
        {
            short type = cell_short_type(line.at(size_t(col)));
            if (type < 0)
            {
                cerr << "Invalid map type" << endl;
                return false;
            }
            if (type == COUNTESS_TYPE)
            {
                countess_room = room_number;
                countess_row = row;
                countess_col = col;
            }
            else if (type == START_TYPE)
            {
                start_room = room_number;
                start_row = row;
                start_col = col;
            }
            set_value(room_number, row, col, type);
        }
        row++;
    }
    return true;
}

void Game_map::output(string output_type, ostream &os)
{
    if (output_type == "M")
    {
        output_map(os);
    }
    else if (output_type == "L")
    {
        output_list(os);
    }
}

void Game_map::output_list(ostream &os)
{
    os << "Path taken:" << endl;
    reverse(path.begin(), path.end());

    for (vector<Location>::iterator it = path.begin(); it != path.end(); ++it)
    {
        char cur_type = get_char_value(get_value(*it));
        os << "(" << it->room << "," << it->row << ","
           << it->col << "," << cur_type << ")" << endl;
    }
}

void Game_map::output_map(ostream &os)
{
    char cur_type;
    os << "Start in room " << start_room << ", row " << start_row
       << ", column " << start_col << endl;
    for (int i = 0; i < num_rooms; ++i)
    {
        os << "//castle room " << i << endl;
        for (int r = 0; r < size; ++r)
        {
            for (int c = 0; c < size; ++c)
            {
                cur_type = get_char_value(get_value(i, r, c));
                os << cur_type;
            }
            os << endl;
        }
    }
}

void Game_map::get_path()
{
    int cur_room = countess_room;
    int cur_row = countess_row;
    int cur_col = countess_col;
    short cur_type = get_value(cur_room, cur_row, cur_col);

    while (cur_type != START_TYPE)
    {
        int multiplier = (int)(cur_type / 15);
        if (multiplier < 5)
        {
            cur_row -= MOVES[multiplier - 1][0];
            cur_col -= MOVES[multiplier - 1][1];

            Location new_loc{cur_room, cur_row, cur_col};
            path.push_back(new_loc);
            cur_type = get_value(new_loc);
            if (multiplier == 1)
            {
                set_value(cur_room, cur_row, cur_col, NORTH);
            }
            else if (multiplier == 2)
            {
                set_value(cur_room, cur_row, cur_col, EAST);
            }
            else if (multiplier == 3)
            {
                set_value(cur_room, cur_row, cur_col, SOUTH);
            }
            else if (multiplier == 4)
            {
                set_value(cur_room, cur_row, cur_col, WEST);
            }
        }
        else
        {
            cur_room = multiplier - 5;
            Location new_loc{cur_room, cur_row, cur_col};
            path.push_back(new_loc);
            cur_type = get_value(new_loc);
            set_value(cur_room, cur_row, cur_col, PORTAL);
        }
    }
}
