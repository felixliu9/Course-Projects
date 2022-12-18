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
#include "xcode_redirect.hpp"

using namespace std;

static const int MOVES[4][2]{{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
static const vector<char> VALID_CELL_TYPES{'0', '1', '2', '3', '4', '5', '6',
                                           '7', '8', '9', '.', '!', '#', 'S',
                                           'C'};
static const vector<char> VALID_CELL_VALUES{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                            11, 12, 13, 14};
static const vector<char> UNVISITABLE_CELL_TYPES{'!', '#', 'S', 'C'};
static const vector<char> VISITABLE_CELL_TYPES{'.', '0', '1', '2', '3', '4',
                                               '5', '6', '7', '8', '9'};
static const short START_TYPE = 13;
static const short COUNTESS_TYPE = 14;
static const short MINION_TYPE = 11;
static const short WALL_TYPE = 12;
static const short EMPTY = 10;
static const short NORTH = 230;
static const short EAST = 231;
static const short SOUTH = 232;
static const short WEST = 233;
static const short PORTAL = 234;
static const short TOTAL_TYPES = 15;

struct Location
{
    int room;
    int row;
    int col;
};

class Game_map
{
private:
    vector<vector<vector<short>>> cells;
    vector<Location> path;
    int size;
    int num_rooms;
    int start_room;
    int start_row;
    int start_col;
    int countess_room;
    int countess_row;
    int countess_col;
    bool read_successful = false;
    bool read_list(istream &in);
    bool read_map(int size, istream &in);
    void output_map(ostream &os);
    void output_list(ostream &os);

public:
    explicit Game_map(istream &in);

    short get_value(int room, int row, int col)
    {
        return cells[size_t(room)][size_t(row)][size_t(col)];
    }

    short get_value(Location &loc)
    {
        return get_value(loc.room, loc.row, loc.col);
    }

    void set_value(int room, int row, int col, short value)
    {
        cells[size_t(room)][size_t(row)][size_t(col)] = value;
    }

    void set_value(int room, int row, int col, int value)
    {

        short short_value = (short)value;
        cells[size_t(room)][size_t(row)][size_t(col)] = short_value;
    }

    void set_value(Location &loc, short value)
    {
        set_value(loc.room, loc.row, loc.col, value);
    }

    void set_value(Location &loc, int value)
    {
        short short_value = (short)value;
        set_value(loc, short_value);
    }

    bool has_room(int room_number)
    {
        return room_number < num_rooms && room_number > -1;
    }

    bool is_visited(short cell_type)
    {
        return cell_type >= TOTAL_TYPES;
    }

    bool is_visitable(short cell_type)
    {
        if (cell_type == MINION_TYPE || cell_type == WALL_TYPE || is_visited(cell_type) || cell_type == START_TYPE)
        {
            return false;
        }
        return true;
    }

    bool is_read_successful()
    {
        return read_successful;
    }

    vector<Location> get_next_moves(Location &loc);

    bool start_search(string search_type, int &total_visited);

    bool read(istream &in);

    void output(string output_type, ostream &os);

    void get_path();

    char get_char_value(short short_value)
    {
        if (short_value == NORTH)
        {
            return 'n';
        }
        else if (short_value == EAST)
        {
            return 'e';
        }
        else if (short_value == SOUTH)
        {
            return 's';
        }
        else if (short_value == WEST)
        {
            return 'w';
        }
        else if (short_value == PORTAL)
        {
            return 'p';
        }
        else
        {
            return VALID_CELL_TYPES[size_t(short_value % TOTAL_TYPES)];
        }
    }
};
