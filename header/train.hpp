#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <memory>
#include <mpi.h>
#include <algorithm>
#include <sstream>

#include "../platform_load_time_gen.hpp"

using std::string;
using std::stringstream;
using std::unordered_map;
using std::string;
using adjacency_matrix = std::vector<std::vector<size_t>>;
using std::vector;
using std::pair;
using std::priority_queue;

enum TrainState
{
    WAITING = '#',
    UNLOADING = '%',
    MOVING = '-'
};

enum LineColor {
    GREEN = 'g',
    YELLOW = 'y',
    BLUE = 'b',
    RED = 'r',
    BROWN = 'w',
    PURPLE = 'p',
    TURQUOISE = 't',
    PINK = 'k',
    LIME = 'l',
    GREY = 'e'
};

const char LINE_COLORS[] = { GREEN, YELLOW, BLUE, RED, BROWN, PURPLE, TURQUOISE, PINK, LIME, GREY };

struct Train
{
    int id = -1; // train id
    int next_local_id = -1; 
    size_t platform_dest_id = 0;
    size_t original = 0;
    int time = 0; // running/ waiting time
    char color = 'x'; // train line
    bool is_forward = false; 

    Train(){}

    Train(int id,int next_local_id, size_t platform_dest_id, size_t original, char color, bool is_forward) : 
        id(id), next_local_id(next_local_id), platform_dest_id(platform_dest_id), original(original), color(color), is_forward(is_forward){}

    Train(Train* train) : 
        id(train->id), next_local_id(train->next_local_id), platform_dest_id(train->platform_dest_id), original(train->original), color(train->color), is_forward(train->is_forward){}

    Train(const Train& train) : 
        id(train.id), next_local_id(train.next_local_id), platform_dest_id(train.platform_dest_id), original(train.original), color(train.color), is_forward(train.is_forward)
    {
    }

    Train& operator=(const Train& train)
    {
        if (this != &train) 
        {
            id = train.id;
            next_local_id = train.next_local_id;
            platform_dest_id = train.platform_dest_id;
            time = train.time;
            color = train.color;
            original = train.original;
            is_forward = train.is_forward;
        }
        return *this;
    }
    string name() const
    {
        return color + std::to_string(id);
    }

    void update_destination(const vector<size_t>& station_line_code)
    {
        original = platform_dest_id;
        int size = station_line_code.size();
        if (next_local_id == size - 1)
        {
            is_forward = false;
            next_local_id--;
        }

        else if (next_local_id == 0)
        {
            is_forward = true;
            next_local_id++;
        }

        else
        {
            if (is_forward)
            {
                next_local_id++;
            }

            else
            {
                next_local_id--;
            }
        }

        platform_dest_id = station_line_code[next_local_id];
    }
    void print() const
    {
        printf("Train %c%d, from %ld,  next_local_id = %d, platform_dest_id = %ld, time = %d, is_forward = %d\n", color, id, original, next_local_id, platform_dest_id, time, is_forward);
    }
};

struct TrainComparator 
{
    bool operator()(Train* a, Train* b) 
    {
        return a->id > b->id;
    }
};