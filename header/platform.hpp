#pragma once

#include <sstream>

#include "train.hpp"

using waiting_line = std::queue<Train*>;
using std::stringstream;

struct Platform 
{
    int id;
    int distance;
    string dest_station;
    Train* train;
    Train* run_away_train;
    waiting_line holding_area;
    PlatformLoadTimeGen time_function;
    bool is_ready_to_run = false;
    priority_queue<Train*, vector<Train*>, TrainComparator> pq;

    Platform(int popularity, int id, int distance, const string& dest_station) : 
        id(id), distance(distance), dest_station(dest_station), train(nullptr), run_away_train(nullptr), time_function(popularity){}

    void train_coming() 
    {
        while (!pq.empty())
        {
            holding_area.push(pq.top());
            // printf("Train %s is coming to %s-%d\n", pq.top()->name().c_str(), dest_station.c_str(), id);
            pq.pop();
        }
    }

    void start_unloading()
    {
        if (train == nullptr)
        {
            if (!holding_area.empty())
            {
                train = holding_area.front();
                holding_area.pop();
                if (train == nullptr)
                {
                    printf("Encounter null pointer\n");
                }
                else
                {
                    train->time = time_function.next(train->id);
                }
                // printf("Start unloading with train %d, time %d\n", train->id, train->time);
            }
            else if (!pq.empty())
            {
                train = pq.top();
                pq.pop();
                if (train == nullptr)
                {
                    printf("Encounter null pointer\n");
                }
                else 
                {
                    train->time = time_function.next(train->id);
                }
            }
        }
    }

    void unloading() // call every move
    {
        if (train != nullptr)
        {
            if (train->time <= 1)
            {
                train->time = 0;
                // printf("Train %s is finished unloading\n", train->name().c_str());
                is_ready_to_run = true;
                return;
                // printf("%d time left %d\n", train->id,  train->time);
            }
            else
            {
                train->time--;
            }
        }
        is_ready_to_run = false;
    }

    void processing() // call after train is reached dest
    {
        train_coming();
        start_unloading();
        unloading();
    }

    void running(const unordered_map<char, vector<size_t>> &station_line_code, vector<vector<Train>>& sending, const size_t& total_processes) // call before train_comming, return True if the train here ready to travel, else False
    {
        if (run_away_train != nullptr)
        {
            if (run_away_train->time> 1)
            {
                run_away_train->time--;
                // printf("still running\n");
            }
            else
            {
                run_away_train->time = 0;
                // send to other process
                run_away_train->update_destination(station_line_code.at(run_away_train->color));
                // printf("Train %s is reaching next station %s-%d, and come to %d\n", run_away_train->name().c_str(), dest_station.c_str(), run_away_train->original, run_away_train->platform_dest_id);
                sending[id % total_processes].push_back(Train(run_away_train));
                // printf("Here added, ready to send\n");
                delete run_away_train;
                run_away_train = nullptr;
                if (train && is_ready_to_run)
                {
                    // printf("Train %s is running\n", train->name().c_str());
                    run_away_train = train;
                    run_away_train->time = distance;
                    train = nullptr;
                }
            }
        }

        else
        {
            // printf("Im null at %s-%d\n", dest_station.c_str(), id);
            if (is_ready_to_run)
            {
                run_away_train = train;
                run_away_train->time = distance;
                // printf("Running hehe \n");
                train = nullptr;
                unloading();
            }
        }
    }

    void print_out(const string& station_name, stringstream& ss) // print platform and holding area
    {
        if (train != nullptr)
        { // at the platform
            ss << train->name() << "-" << station_name << "% ";
            // but above output to a string
        }
        if (!holding_area.empty())
        { // at the holding area
            waiting_line copy_to_print = holding_area;
            while (!copy_to_print.empty())
            {
                ss << copy_to_print.front()->name() << "-" << station_name << "# ";
                copy_to_print.pop();
            }
        }
        if (run_away_train != nullptr)
        {
            ss << run_away_train->name() << "-" << station_name << "->" << dest_station << " ";
        }
    }

    // ~Platform()
    // {
    //     for (auto& element_train : coming_trains)
    //     {
    //         delete element_train;
    //     }

    //     while (!holding_area.empty())
    //     {
    //         delete holding_area.front();
    //         holding_area.pop();
    //     }

    //     if (train != nullptr)
    //     {
    //         delete train;
    //     }
    // }
};