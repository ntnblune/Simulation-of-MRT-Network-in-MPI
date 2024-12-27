#pragma once 

#include "platform.hpp"
#include "message.hpp"
#include <iostream>

struct StationCenter 
{
    size_t rank;
    size_t total_processes;
    size_t id;
    int popularity;
    string name;
    vector<Platform> platforms;
    unordered_map<size_t, size_t> localize_ids;
    unordered_map<size_t, waiting_line> train_list;

    StationCenter(const size_t& rank, const size_t& total_processes, const size_t& id, const int& popularity, const string& name, const adjacency_matrix& mat, const vector<string>& station_names) : rank(rank), total_processes(total_processes), id(id), popularity(popularity), name(name)
    {
        for (size_t i = 0; i < mat.size(); i++)
        {
            if (mat[id][i] > 0)
            {
                Platform platform(popularity, i, mat[id][i], station_names[i]);
                localize_ids[i] = platforms.size();
                platforms.push_back(platform);
            }
        }
        // printf("There are %d\n", platforms.size());
    }

    void assign_spawning_info(vector<Train>& trains)
    {
        // printf("Assigning spawning info %ld\n", rank);
        for (size_t i = 0; i < trains.size(); i++)
        {
            // printf("Train's dest = %d, current id = %d", trains[i].original, id);
            if (trains[i].original == id)
            {
                // trains[i].print();
                int platform_id = localize_ids[trains[i].platform_dest_id];
                // printf("rank %ld: at station %ld, ssignning %s to %d\n", id, rank, trains[i].name().c_str(), platform_id);
                train_list[platform_id].push(new Train(trains[i]));
            }
        }
    }

    void assign_arriving_info(vector<Train>& trains)
    {
        // printf("Assigning spawning info %ld\n", rank);
        for (size_t i = 0; i < trains.size(); i++)
        {
            if (trains[i].original == id)
            {
                // trains[i].print();
                int platform_id = localize_ids[trains[i].platform_dest_id];
                // printf("rank %ld: Arriving %s to %d\n", rank, trains[i].name().c_str(), platform_id);
                platforms[platform_id].pq.push(new Train(trains[i]));
                platforms[platform_id].train_coming();
            }
        }
        // printf("Assigning done %ld\n", rank);
    }

    void spawning() // call each tick
    {
        for (auto& [platform_id, train_queue] : train_list)
        {
            // printf("%d spawing at platform %d\n",rank, platform_id);
            string s = "";
            bool flag = true;
            while (!train_queue.empty() && flag)
            {
                Train* train = train_queue.front();
                if (train == nullptr)
                {
                    train_queue.pop();
                    // printf("Encounter null pointer\n");
                }
                else 
                {
                    if (s.find(train->color) != std::string::npos)
                    {
                        flag = false;
                        // printf("Same train at %s!\n", name.c_str());
                    }
                    else
                    {
                        s.push_back(train->color);
                        // printf("Train %s is coming to %s, which is %d/%d\n", train_queue.front()->name().c_str(), name.c_str(), platform_id, platforms.size());
                        platforms[platform_id].pq.push(train_queue.front());
                        train_queue.pop();
                    }
                }
            }
        }
    } 

    void processing()
    {
        for (size_t i = 0; i < platforms.size(); i++)
        {
            platforms[i].processing();
        }
    }

    void running(const unordered_map<char, vector<size_t>> &station_line_code, vector<vector<Train>>& train_send)
    {
        for (size_t i = 0; i < platforms.size(); i++)
        {
            platforms[i].running(station_line_code, train_send, total_processes);
        }
    }

    void print_platform(stringstream& ss)
    {
        for (size_t i = 0; i < platforms.size(); i++)
        {
            platforms[i].print_out(name, ss);
        }

    }

};

