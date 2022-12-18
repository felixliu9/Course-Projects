// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

// felixliu

#include <string>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <getopt.h>
#include <iostream>
#include <algorithm>
#include "P2random.h"
#include "median.hpp"

using namespace std;

class Zombie
{
private:
    string name;
    uint32_t health;
    uint32_t distance;
    uint32_t speed;
    uint32_t lifespan = 1;

public:
    Zombie(string name, uint32_t distance, uint32_t speed,
           uint32_t health) : name(name),
                              health(health),
                              distance(distance),
                              speed(speed){};

    string get_name()
    {
        return name;
    }

    uint32_t get_health()
    {
        return health;
    }

    uint32_t get_distance()
    {
        return distance;
    }

    uint32_t get_speed()
    {
        return speed;
    }
    void move()
    {
        distance = uint32_t(max(0, static_cast<int>(distance - speed)));
    }
    void take_damage()
    {
        health--;
    }
    void increase_lifespan()
    {
        lifespan++;
    }

    bool is_dead()
    {
        return health == 0;
    }
    uint32_t get_lifespan()
    {
        return lifespan;
    }
};

class Compare_Zombies
{
public:
    bool operator()(Zombie *a, Zombie *b)
    {
        uint32_t etaA = a->get_distance() / a->get_speed();
        uint32_t etaB = b->get_distance() / b->get_speed();
        if (etaA != etaB)
        {
            return etaA > etaB;
        }
        if (a->get_health() != b->get_health())
        {
            return a->get_health() > b->get_health();
        }
        return a->get_name() > b->get_name();
    }
};

class Compare_Lifespans_Most
{
public:
    Compare_Lifespans_Most() {}
    bool operator()(Zombie *a, Zombie *b)
    {
        if (a->get_lifespan() == b->get_lifespan())
        {
            return a->get_name() > b->get_name();
        }
        return a->get_lifespan() < b->get_lifespan();
    }
};

class Compare_Lifespans_Least
{

public:
    Compare_Lifespans_Least() {}

    bool operator()(Zombie *a, Zombie *b)
    {
        if (a->get_lifespan() == b->get_lifespan())
        {
            return a->get_name() < b->get_name();
        }
        return a->get_lifespan() < b->get_lifespan();
    }
};

class Zomb_defense
{
private:
    P2random rand_generator;
    Median lifespans;
    priority_queue<Zombie *, vector<Zombie *>, Compare_Zombies> active_zombies;
    vector<Zombie *> all_zombies;
    vector<Zombie *> dead_zombies;

    uint32_t cur_round_num = 0;
    uint32_t next_round_num_from_file = 1;
    bool more_rounds_available = true;
    uint32_t quiver_capacity;
    uint32_t seed;
    uint32_t max_rand_dist;
    uint32_t max_rand_speed;
    uint32_t max_rand_health;
    bool verbose;
    uint32_t stat_entries;
    bool median;
    Zombie *death_zomb = nullptr;
    Zombie *last_zomb = nullptr;

    // private functions
    void read_header(istream &is);
    uint32_t read_next_round_number(istream &is);
    void read_and_add_round(istream &is);

public:
    Zomb_defense(bool verbose, uint32_t stat_entries, bool median, istream &is);
    void start_defense(istream &is);
    bool game_over();
    void update_zomb();
    void shoot();
    void play_round(istream &is);
    void output(ostream &os);
    ~Zomb_defense()
    {
        for (Zombie *z : all_zombies)
        {
            delete z;
        }
        all_zombies.clear();
    }
};
