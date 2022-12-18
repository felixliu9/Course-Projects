// 9B734EC0C043C5A836EA0EBE4BEFEA164490B2C7

// felixliu

#include <vector>
#include <iostream>
#include <limits>
#include <math.h>
#include <getopt.h>
#include <iomanip>

using namespace std;

uint32_t node_count;
bool has_lab = false;
bool has_decontam = false;
bool has_outer = false;
double optimal_cost;
double zero_cost = 0;
vector<uint32_t> optimal_route;
vector<vector<double>> M;

enum ROOM_TYPE
{
    OUTSIDE,
    LAB,
    DECONTAM
};

struct Room_MST
{
    int x, y;
    uint32_t index;
    double cost = numeric_limits<double>::max();
    uint32_t from_which = numeric_limits<uint32_t>::max();
    ROOM_TYPE type;
    Room_MST(int x_in, int y_in, uint32_t index_in) : x(x_in), y(y_in), index(index_in)
    {
        if (x < 0 && y < 0)
        {
            type = LAB;
            has_lab = true;
        }
        else if ((x <= 0 && y == 0) || (y <= 0 && x == 0))
        {
            type = DECONTAM;
            has_decontam = true;
        }
        else
        {
            type = OUTSIDE;
            has_outer = true;
        }
    }
};

struct Room_FAST
{
    int x, y;
    uint32_t index;
    double cost = numeric_limits<double>::max();
    uint32_t from_which = numeric_limits<uint32_t>::max();
    Room_FAST(int x_in, int y_in, uint32_t index_in) : x(x_in), y(y_in), index(index_in)
    {
    }
};

struct Room
{
    int x, y;
    uint32_t index;
    double cost = numeric_limits<double>::max();
    uint32_t from_which = numeric_limits<uint32_t>::max();
    bool selected = false;
    Room(int x_in, int y_in, uint32_t index_in) : x(x_in), y(y_in), index(index_in)
    {
    }
};

void calculate_optimal_cost(uint32_t start, double &cost);
void swap(uint32_t &i1, uint32_t &i2);
vector<Room *> rooms;
vector<Room_FAST *> rooms_fast;
vector<Room_MST *> rooms_mst;

void swap_mst(uint32_t &i1, uint32_t &i2)
{
    swap(rooms_mst[i1], rooms_mst[i2]);
}

void swap(uint32_t &i1, uint32_t &i2)
{
    swap(rooms[i1], rooms[i2]);
}

double find_min_connection_cost(uint32_t &start)
{
    double min_start_connection = numeric_limits<double>::infinity();
    double min_end_connection = numeric_limits<double>::infinity();
    for (uint32_t i = start; i < node_count; ++i)
    {
        if (M[rooms[i]->index][rooms[start - 1]->index] < min_end_connection)
        {
            min_end_connection = M[rooms[i]->index][rooms[start - 1]->index];
        }
        if (M[rooms[i]->index][rooms[0]->index] < min_start_connection)
        {
            min_start_connection = M[rooms[i]->index][rooms[0]->index];
        }
    }
    return min_end_connection + min_start_connection;
}

void read_mst()
{
    cin >> node_count;
    int x, y;
    rooms_mst.resize(node_count);
    for (uint32_t i = 0; i < node_count; ++i)
    {
        cin >> x >> y;
        rooms_mst[i] = new Room_MST(x, y, i);
    }
}

void read_fast()
{
    cin >> node_count;
    int x, y;
    rooms_fast.resize(node_count);
    for (uint32_t i = 0; i < node_count; ++i)
    {
        cin >> x >> y;
        rooms_fast[i] = new Room_FAST(x, y, i);
    }
}

void read()
{
    cin >> node_count;
    int x, y;
    rooms.resize(node_count);
    for (uint32_t i = 0; i < node_count; ++i)
    {
        cin >> x >> y;
        rooms[i] = new Room(x, y, i);
    }
}

void clear_rooms()
{
    for (Room_MST *r1 : rooms_mst)
    {
        delete r1;
    }
    rooms_mst.clear();
    for (Room_FAST *r2 : rooms_fast)
    {
        delete r2;
    }
    rooms_fast.clear();
    for (Room *r : rooms)
    {
        delete r;
    }
    rooms.clear();
}

double calculate_cost_mst(uint32_t &ind1, uint32_t &ind2, bool consider_barrier)
{
    if (consider_barrier)
    {
        if ((rooms_mst[ind1]->type == ROOM_TYPE::LAB && rooms_mst[ind2]->type == ROOM_TYPE::OUTSIDE) || (rooms_mst[ind1]->type == ROOM_TYPE::OUTSIDE && rooms_mst[ind2]->type == ROOM_TYPE::LAB))
        {
            return numeric_limits<double>::infinity();
        }
    }
    double x_double = (double)(rooms_mst[ind1]->x - rooms_mst[ind2]->x);
    double y_double = (double)(rooms_mst[ind1]->y - rooms_mst[ind2]->y);

    return sqrt(x_double * x_double + y_double * y_double);
}

double calculate_cost(uint32_t &ind1, uint32_t &ind2)
{
    double x_double = (double)(rooms[ind1]->x - rooms[ind2]->x);
    double y_double = (double)(rooms[ind1]->y - rooms[ind2]->y);

    return sqrt(x_double * x_double + y_double * y_double);
}

uint32_t find_min_cost_room_mst(uint32_t last_selected, bool consider_barrier)
{
    double min_cost = numeric_limits<double>::infinity();
    uint32_t min_i = 0;
    for (uint32_t i = last_selected + 1; i < node_count; ++i)
    {
        double new_cost = calculate_cost_mst(last_selected, i, consider_barrier);
        if (rooms_mst[i]->cost > new_cost)
        {
            rooms_mst[i]->from_which = rooms_mst[last_selected]->index;
            rooms_mst[i]->cost = new_cost;
        }
        if (rooms_mst[i]->cost < min_cost)
        {
            min_cost = rooms_mst[i]->cost;
            min_i = i;
        }
    }
    return min_i;
}

uint32_t find_min_cost_room(uint32_t &start, uint32_t &last_selected)
{
    double min_cost = numeric_limits<double>::infinity();
    uint32_t min_i = 0;
    for (uint32_t i = start + 1; i < node_count; ++i)
    {
        if (rooms[i]->selected)
        {
            continue;
        }
        double new_cost = M[rooms[last_selected]->index][rooms[i]->index];
        if (rooms[i]->cost > new_cost)
        {
            rooms[i]->from_which = rooms[last_selected]->index;
            rooms[i]->cost = new_cost;
        }
        if (rooms[i]->cost < min_cost)
        {
            min_cost = rooms[i]->cost;
            min_i = i;
        }
        // cout << "min cost : " << i << " = " << rooms[i]->cost << endl;
    }
    return min_i;
}

/*
uint32_t find_min_cost_room(uint32_t start)
{
    double min_cost = numeric_limits<double>::infinity();
    uint32_t min_i = 0;
    for (uint32_t i = start; i < node_count; ++i)
    {
        if (rooms[i]->cost < min_cost)
        {
            min_cost = rooms[i]->cost;
            min_i = i;
        }
    }
    return min_i;
}
*/

double find_mst(uint32_t &start, uint32_t &end, bool consider_barrier)
{
    rooms_mst[start]->cost = 0;
    uint32_t start_i = start;
    uint32_t next_room = find_min_cost_room_mst(start_i, consider_barrier);
    while (next_room > 0 && start_i < end)
    {
        uint32_t start_i_p1 = start_i + 1;
        swap_mst(start_i_p1, next_room);
        // iter_swap(rooms.begin() + (int)start_i + 1, rooms.begin() + (int)next_room);
        start_i++;
        next_room = find_min_cost_room_mst(start_i, consider_barrier);
    }
    double total_cost = 0;
    for (uint32_t i = start + 1; i < end; ++i)
    {
        Room_MST *r = rooms_mst[i];
        total_cost += r->cost;
    }
    return total_cost;
}

double find_mst_opt(uint32_t &start, uint32_t &end)
{
    rooms[start]->selected = true;
    rooms[start]->cost = 0;
    uint32_t start_i = start;
    for (uint32_t i = start + 1; i < end; ++i)
    {
        rooms[i]->selected = false;
        rooms[i]->cost = numeric_limits<double>::infinity();
    }
    uint32_t next_room = find_min_cost_room(start, start_i);

    while (next_room > 0 && start_i < end)
    {
        rooms[next_room]->selected = true;
        start_i++;
        next_room = find_min_cost_room(start, next_room);
    }
    double total_cost = 0;
    for (uint32_t i = start + 1; i < end; ++i)
    {
        Room *r = rooms[i];
        total_cost += r->cost;
    }
    return total_cost;
}

double calculate_distance(Room_FAST *r1, Room_FAST *r2)
{

    double x_double = (double)(r1->x - r2->x);
    double y_double = (double)(r1->y - r2->y);

    return sqrt(x_double * x_double + y_double * y_double);
}

double calculate_distance(Room *r1, Room *r2)
{

    double x_double = (double)(r1->x - r2->x);
    double y_double = (double)(r1->y - r2->y);

    return sqrt(x_double * x_double + y_double * y_double);
}

void insert_to_tour_fast(uint32_t &current_insertion_i, uint32_t &current_tour_size, double &total_cost)
{
    uint32_t min_index = 0;
    double min_cost_start = 0;
    double min_cost_end = 0;
    double min_cost = numeric_limits<double>::infinity();

    for (uint32_t i = 0; i < current_tour_size; ++i)
    {
        double cur_cost_start = calculate_distance(rooms_fast[current_insertion_i], rooms_fast[i]);
        double cur_cost_end = calculate_distance(rooms_fast[current_insertion_i], rooms_fast[rooms_fast[i]->from_which]);
        double cur_cost = cur_cost_start + cur_cost_end - rooms_fast[i]->cost;
        if (cur_cost < min_cost)
        {
            min_cost = cur_cost;
            min_index = i;
            min_cost_start = cur_cost_start;
            min_cost_end = cur_cost_end;
        }
    }
    uint32_t temp = rooms_fast[min_index]->from_which;
    rooms_fast[min_index]->from_which = current_insertion_i;
    rooms_fast[min_index]->cost = min_cost_start;
    rooms_fast[current_insertion_i]->from_which = temp;
    rooms_fast[current_insertion_i]->cost = min_cost_end;
    total_cost += min_cost;
}

void insert_to_tour(uint32_t &current_insertion_i, uint32_t &current_tour_size, double &total_cost)
{
    uint32_t min_index = 0;
    double min_cost_start = 0;
    double min_cost_end = 0;
    double min_cost = numeric_limits<double>::infinity();

    for (uint32_t i = 0; i < current_tour_size; ++i)
    {
        double cur_cost_start = calculate_distance(rooms[current_insertion_i], rooms[i]);
        double cur_cost_end = calculate_distance(rooms[current_insertion_i], rooms[rooms[i]->from_which]);
        double cur_cost = cur_cost_start + cur_cost_end - rooms[i]->cost;
        if (cur_cost < min_cost)
        {
            min_cost = cur_cost;
            min_index = i;
            min_cost_start = cur_cost_start;
            min_cost_end = cur_cost_end;
        }
    }
    uint32_t temp = rooms[min_index]->from_which;
    rooms[min_index]->from_which = current_insertion_i;
    rooms[min_index]->cost = min_cost_start;
    rooms[current_insertion_i]->from_which = temp;
    rooms[current_insertion_i]->cost = min_cost_end;
    total_cost += min_cost;
}

double find_fast_tsp()
{
    if (node_count == 1)
    {
        cout << "0" << endl;
        cout << "0 " << endl;
        return 0;
    }
    double d = calculate_distance(rooms_fast[0], rooms_fast[1]);
    double total_cost = 0;
    rooms_fast[0]->from_which = 1;
    rooms_fast[1]->from_which = 0;
    rooms_fast[0]->cost = d;
    rooms_fast[1]->cost = d;
    total_cost += 2 * d;

    for (uint32_t i = 2; i < node_count; ++i)
    {
        insert_to_tour_fast(i, i, total_cost);
    }
    return total_cost;
}

double find_fast_tsp_for_opt()
{
    if (node_count == 1)
    {
        cout << "0" << endl;
        cout << "0 " << endl;
        return 0;
    }
    double d = calculate_distance(rooms[0], rooms[1]);
    double total_cost = 0;
    rooms[0]->from_which = 1;
    rooms[1]->from_which = 0;
    rooms[0]->cost = d;
    rooms[1]->cost = d;
    total_cost += 2 * d;

    for (uint32_t i = 2; i < node_count; ++i)
    {
        insert_to_tour(i, i, total_cost);
    }
    return total_cost;
}

void print_tsp(double &total_cost)
{
    cout << total_cost << endl;
    Room_FAST *next_room = rooms_fast[0];
    cout << "0 ";
    next_room = rooms_fast[next_room->from_which];

    while (next_room->index != 0)
    {
        cout << next_room->index << " ";
        next_room = rooms_fast[next_room->from_which];
    }
}

double get_sub_tsp(uint32_t &start)
{
    if (start == 1)
    {
        return 0;
    }
    double d = M[rooms[start - 1]->index][rooms[0]->index]; // calculate_distance(rooms[0], rooms[start - 1]);
    double total_cost = 0;
    rooms[0]->from_which = start - 1;
    total_cost += d;

    for (uint32_t i = 1; i < start - 1; ++i)
    {
        uint32_t min_index = 0;
        double min_cost = numeric_limits<double>::infinity();

        for (uint32_t k = 0; k < i; ++k)
        {
            double cur_cost_start = M[rooms[k]->index][rooms[i]->index];                   // calculate_distance(rooms[k], rooms[i]);
            double cur_cost_end = M[rooms[i]->index][rooms[rooms[k]->from_which]->index];  // calculate_distance(rooms[i], rooms[rooms[k]->from_which]);
            double cur_edge_cost = M[rooms[k]->index][rooms[rooms[k]->from_which]->index]; // calculate_distance(rooms[k], rooms[rooms[k]->from_which]);
            double cur_cost = cur_cost_start + cur_cost_end - cur_edge_cost;
            if (cur_cost < min_cost)
            {
                min_cost = cur_cost;
                min_index = k;
            }
        }
        uint32_t temp = rooms[min_index]->from_which;
        rooms[min_index]->from_which = i;
        rooms[i]->from_which = temp;
        total_cost += min_cost;
    }
    return total_cost;
}

void find_opt_tsp()
{
    optimal_cost = find_fast_tsp_for_opt();
    Room *next_room = rooms[0];

    uint32_t index = 0;
    optimal_route[index++] = 0;
    next_room = rooms[next_room->from_which];
    while (next_room->index != 0)
    {
        optimal_route[index++] = next_room->index;
        next_room = rooms[next_room->from_which];
    }
    // find_mst(0, node_count, false);

    for (uint32_t r = 0; r < node_count; ++r)
    {
        for (uint32_t c = r; c < node_count; ++c)
        {
            M[r][c] = calculate_distance(rooms[r], rooms[c]);
            M[c][r] = M[r][c];
        }
    }
    uint32_t start = 1;
    double cost = 0;
    calculate_optimal_cost(start, cost);
    cout << optimal_cost << endl;
    for (auto i : optimal_route)
    {
        cout << i << " ";
    }
}

void calculate_optimal_cost(uint32_t start, double &cost)
{
    if (start == node_count)
    {

        double total_cost = cost + M[rooms[start - 1]->index][rooms[0]->index]; // calculate_distance(rooms[start - 1], rooms[0]);
        if (total_cost < optimal_cost)
        {
            optimal_route.clear();
            for (Room *r : rooms)
            {
                optimal_route.push_back(r->index);
            }
            optimal_cost = total_cost;
        }
        return;
    }
    if (cost > optimal_cost + 0.001)
    {
        return;
    }
    double sub_tsp = get_sub_tsp(start) + 0.0001;
    if (cost > sub_tsp)
    {
        // cout << "stopped at :: " << start << ", with cost :: " << cost << " > "  << sub_tsp << endl;
        return;
    }
    double mst_unselected = find_mst_opt(start, node_count);
    double min_connection_cost = find_min_connection_cost(start);
    double low_cost = cost + mst_unselected + min_connection_cost;
    // cout << "start : " << start << ", " << low_cost << " = " << cost <<  " + "<< mst_unselected << " + " << min_connection_cost << endl;

    if (low_cost > optimal_cost + 0.0001)
    {
        // cout << "stopped at: " << start << " due to low cost." << endl;
        return;
    }

    for (uint32_t i = start; i < node_count; ++i)
    {
        swap(i, start);
        // iter_swap(rooms.begin() + (int)i, rooms.begin() + (int)start);
        double new_cost_addition = M[rooms[start - 1]->index][rooms[start]->index]; // calculate_distance(rooms[start - 1], rooms[start]);
        cost += new_cost_addition;
        calculate_optimal_cost(start + 1, cost);
        cost -= new_cost_addition;
        swap(i, start);
        // iter_swap(rooms.begin() + (int)i, rooms.begin() + (int)start);
    }
}

int main(int argc, char *argv[])
{
    cout << std::fixed;
    cout << setprecision(2);
    int opt;
    const struct option longopts[] = {
        {"mode", required_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 's'},
        {nullptr, 0, nullptr, 0}};
    int index = 0;
    string mode = "";
    while ((opt = getopt_long(argc, argv, "m:h", longopts, &index)) != -1)
    {
        switch (opt)
        {
        case 'm':

            mode = optarg;
            break;

        case 'h':
            cout << "Help Menu"
                 << endl;
            cout << "Usage: amongus -m, –mode {MST|FASTTSP|OPTTSP}" << endl;
            exit(0);

        default:
            cerr << "Unknown command line option" << endl;
            exit(1);
        }
    }
    if (mode == "MST")
    {
        read_mst();
        if (has_lab && !has_decontam && has_outer)
        {
            cerr << "Cannot construct MST" << endl;
            clear_rooms();
            exit(1);
        }
        else
        {
            uint32_t v = 0;
            cout << find_mst(v, node_count, true) << endl;

            for (uint32_t i = 0; i < node_count; ++i)
            {
                Room_MST *r = rooms_mst[i];
                if (r->from_which != numeric_limits<uint32_t>::max())
                {
                    if (r->from_which < r->index)
                    {
                        cout << r->from_which << " " << r->index << endl;
                    }
                    else
                    {
                        cout << r->index << " " << r->from_which << endl;
                    }
                }
            }
            clear_rooms();
        }
    }
    else if (mode == "FASTTSP")
    {
        read_fast();
        double total_cost = find_fast_tsp();
        print_tsp(total_cost);
        clear_rooms();
    }
    else if (mode == "OPTTSP")
    {
        read();
        M.resize(node_count, vector<double>(node_count));
        optimal_route.resize(node_count);
        find_opt_tsp();
        clear_rooms();
    }
    else
    {
        cerr << "Unrecognized Mode!" << endl;
        exit(1);
    }

    exit(0);
}
