// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

// felixliu

#include "zombbb.hpp"

#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <getopt.h>
#include <iostream>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[])
{
    bool verbose = false;
    bool median = false;
    uint32_t stat_entries = 0;
    int opt;
    const struct option longopts[] = {
        {"verbose", no_argument, nullptr, 'v'},
        {"statistics", required_argument, nullptr, 's'},
        {"median", no_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}};
    int index = 0;

    while ((opt = getopt_long(argc, argv, "vs:mh", longopts, &index)) != -1)
    {
        switch (opt)
        {
        case 'v':
            verbose = true;
            break;

        case 's':
            stat_entries = uint32_t(stoi(optarg));
            break;

        case 'm':
            median = true;
            break;

        case 'h':
            cout << "help" << endl;
            exit(0);

        default:
            cerr << "Unknown command line option" << endl;
            exit(1);
        }
    }
    try
    {
        Zomb_defense game(verbose, stat_entries, median, cin);
        game.start_defense(cin);
    }
    catch (...)
    {
        cerr << "Something is wrong!" << endl;
        exit(1);
    }
    exit(0);
}

Zomb_defense::Zomb_defense(bool verbose, uint32_t stat_entries, bool median, istream &is)
    : verbose(verbose), stat_entries(stat_entries), median(median)
{
    read_header(is);
    rand_generator.initialize(seed, max_rand_dist, max_rand_speed, max_rand_health);
}

void Zomb_defense::read_header(istream &is)
{
    is.ignore(256, '\n');
    is.ignore(256, ' ');
    is >> quiver_capacity;
    is.ignore(256, ' ');
    is >> seed;
    is.ignore(256, ' ');
    is >> max_rand_dist;
    is.ignore(256, ' ');
    is >> max_rand_speed;
    is.ignore(256, ' ');
    is >> max_rand_health;
}

uint32_t Zomb_defense::read_next_round_number(istream &is)
{
    uint32_t round_number;
    is.ignore(256, '\n');
    is.ignore(256, ' ');
    if (is.good())
    {
        is >> round_number;
        return round_number;
    }
    return UINT32_MAX;
}
void Zomb_defense::read_and_add_round(istream &is)
{
    uint32_t rand_zomb_num;
    uint32_t named_zomb_num;

    cin.ignore(256, ' ');
    cin >> rand_zomb_num;
    cin.ignore(256, ' ');
    cin >> named_zomb_num;

    for (size_t i = 0; i < rand_zomb_num; ++i)
    {
        Zombie *zomb = new Zombie(rand_generator.getNextZombieName(),
                                  rand_generator.getNextZombieDistance(),
                                  rand_generator.getNextZombieSpeed(),
                                  rand_generator.getNextZombieHealth());

        active_zombies.push(zomb);
        all_zombies.push_back(zomb);
        if (verbose)
        {
            cout << "Created: " << zomb->get_name() << " (distance: "
                 << zomb->get_distance() << ", speed: " << zomb->get_speed()
                 << ", health: " << zomb->get_health() << ")" << endl;
        }
    }

    for (size_t i = 0; i < named_zomb_num; ++i)
    {
        string name;
        uint32_t distance;
        uint32_t speed;
        uint32_t health;
        is >> name;
        is.ignore(256, ' ');
        is.ignore(256, ' ');
        is >> distance;
        is.ignore(256, ' ');
        is.ignore(256, ' ');
        is >> speed;
        is.ignore(256, ' ');
        is.ignore(256, ' ');
        is >> health;
        Zombie *zomb = new Zombie(name, distance, speed, health);
        active_zombies.push(zomb);
        all_zombies.push_back(zomb);
        if (verbose)
        {
            cout << "Created: " << zomb->get_name() << " (distance: "
                 << zomb->get_distance() << ", speed: " << zomb->get_speed()
                 << ", health: " << zomb->get_health() << ")" << endl;
        }
    }
}

void Zomb_defense::start_defense(istream &is)
{
    cur_round_num = 1;
    next_round_num_from_file = read_next_round_number(is);
    do
    {
        play_round(is);
        if (!game_over())
        {
            cur_round_num++;
        }
    } while (!game_over());
    output(cout);
}

bool Zomb_defense::game_over()
{
    return (death_zomb != nullptr ||
            (active_zombies.empty() &&
             cur_round_num >= next_round_num_from_file &&
             !more_rounds_available));
}

void Zomb_defense::play_round(istream &is)
{
    if (verbose)
    {
        cout << "Round: " << cur_round_num << endl;
    }
    update_zomb();
    if (death_zomb != nullptr)
    {
        return;
    }

    // add new round of zombies if round matches
    if (cur_round_num == next_round_num_from_file)
    {
        read_and_add_round(is);
        if (is.good())
        {
            uint32_t next = read_next_round_number(is);
            if (next != UINT32_MAX)
            {
                next_round_num_from_file = next;
            } else {
                more_rounds_available = false;
            }
        }
        else
        {
            more_rounds_available = false;
        }
    }

    shoot();

    if (median && !lifespans.empty())
    {
        cout << "At the end of round " << cur_round_num
             << ", the median zombie lifetime is "
             << lifespans.get_median() << endl;
    }
}

void Zomb_defense::update_zomb()
{
    for (Zombie *z : all_zombies)
    {
        if (!z->is_dead())
        {
            z->increase_lifespan();
            z->move();
            if (z->get_distance() == 0 && death_zomb == nullptr)
            {
                death_zomb = z;
            }
            if (verbose)
            {
                cout << "Moved: " << z->get_name() << " (distance: "
                     << z->get_distance() << ", speed: " << z->get_speed()
                     << ", health: " << z->get_health() << ")" << endl;
            }
        }
    }
}

void Zomb_defense::shoot()
{
    if (active_zombies.empty())
    {
        return;
    }
    for (size_t i = 0; i < quiver_capacity; ++i)
    {
        Zombie *cur_zomb = active_zombies.top();
        cur_zomb->take_damage();
        if (cur_zomb->is_dead())
        {
            dead_zombies.push_back(cur_zomb);
            if (verbose)
            {
                cout << "Destroyed: " << cur_zomb->get_name() << " (distance: "
                     << cur_zomb->get_distance() << ", speed: " << cur_zomb->get_speed()
                     << ", health: " << cur_zomb->get_health() << ")" << endl;
            }
            lifespans.push(cur_zomb->get_lifespan());
            active_zombies.pop();
        }
        if (active_zombies.empty())
        {
            last_zomb = cur_zomb;
            return;
        }
    }
}

void Zomb_defense::output(ostream &os)
{
    if (death_zomb == nullptr)
    {
        os << "VICTORY IN ROUND " << cur_round_num << "! " << last_zomb->get_name() << " was the last zombie." << endl;
    }
    else
    {
        os << "DEFEAT IN ROUND " << cur_round_num << "! " << death_zomb->get_name() << " ate your brains!" << endl;
    }
    if (stat_entries > 0)
    {
        os << "Zombies still active: " << active_zombies.size() << endl;
        os << "First zombies killed:" << endl;
        for (size_t i = 0; i < stat_entries; ++i)
        {
            if (i >= dead_zombies.size())
            {
                break;
            }
            os << dead_zombies[i]->get_name() << " " << i + 1 << endl;
        }
        os << "Last zombies killed:" << endl;
        size_t max_entries = dead_zombies.size() < stat_entries ? dead_zombies.size() : stat_entries;
        for (size_t i = dead_zombies.size(); i > dead_zombies.size() - max_entries; --i)
        {
            os << dead_zombies[i - 1]->get_name() << " " << max_entries - (dead_zombies.size() - i) << endl;
        }
        sort(all_zombies.begin(), all_zombies.end(), Compare_Lifespans_Most());
        os << "Most active zombies:" << endl;
        max_entries = all_zombies.size() < stat_entries ? all_zombies.size() : stat_entries;
        for (size_t i = all_zombies.size(); i > all_zombies.size() - max_entries; --i)
        {
            os << all_zombies[i - 1]->get_name() << " " << all_zombies[i - 1]->get_lifespan() << endl;
        }
        sort(all_zombies.begin(), all_zombies.end(), Compare_Lifespans_Least());
        os << "Least active zombies:" << endl;
        for (size_t i = 0; i < max_entries; ++i)
        {
            os << all_zombies[i]->get_name() << " " << all_zombies[i]->get_lifespan() << endl;
        }
    }
}
