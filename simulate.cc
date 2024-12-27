#define OMPI_SKIP_MPICXX 1
#include <vector>
#include <string>
#include <unordered_map>
#include <mpi.h>
#include <vector>
#include <string>
#include <queue>
#include <stdexcept>

#include "header/functions.hpp"
#include "header/stationcenter.hpp"

using std::string;
using std::unordered_map;
using std::vector;
using adjacency_matrix = std::vector<std::vector<size_t>>;
using std::vector;
using std::pair;
using std::priority_queue;


void simulate(size_t num_stations, const vector<string> &station_names, const std::vector<size_t> &popularities,
              const adjacency_matrix &mat, const unordered_map<char, vector<string>> &station_lines, size_t ticks,
              const unordered_map<char, size_t> num_trains, size_t num_ticks_to_print, size_t mpi_rank,
              size_t total_processes) 
{
    unordered_map<string, size_t> convert_name_to_code;
    unordered_map<char, vector<size_t>> station_line_code;
    for (size_t i = 0; i < station_names.size(); i++)
    {
        convert_name_to_code[station_names[i]] = i;
    }

    for (const auto& [color, line] : station_lines)
    {
        for (size_t i = 0; i < line.size(); i++)
        {
            station_line_code[color].push_back(convert_name_to_code[line[i]]);
        }
    }
    
    vector<vector<Train>> all_trains(num_stations);
    vector<StationCenter> station_centers;
    for (size_t index = mpi_rank; index < num_stations; index += total_processes)
    {
        StationCenter station_center(mpi_rank, total_processes, index, popularities[index], station_names[index], mat, station_names);
        station_centers.push_back(station_center);
        // printf("Station %s is created at %ld\n", station_names[index].c_str(), mpi_rank);
    }

    if (mpi_rank == 0)
    { // master of spawning all_trains
        indexing_trains(num_stations, station_line_code, num_trains, all_trains);
    }

    vector<Train> spawning_trains;
    scatter_trains(all_trains, total_processes, mpi_rank, spawning_trains);
    for (size_t i = 0; i < station_centers.size(); i++)
    {
        station_centers[i].assign_spawning_info(spawning_trains);
    }
    spawning_trains.clear();
    vector<vector<Train>> train_send(total_processes);
    for (size_t tick = 0; tick < ticks; tick++)
    {
        for (size_t i = 0; i < station_centers.size(); i++)
        {
            station_centers[i].spawning();
            station_centers[i].running(station_line_code, train_send);
        }
        // MPI_Barrier(MPI_COMM_WORLD);
        // printf("Send at %ld\n", tick);
        all_to_all_train(train_send, spawning_trains, total_processes);
        MPI_Barrier(MPI_COMM_WORLD);
        for (size_t i = 0; i < station_centers.size(); i++)
        {
            station_centers[i].assign_arriving_info(spawning_trains);
            station_centers[i].processing();
        }
        spawning_trains.clear();
        MPI_Barrier(MPI_COMM_WORLD);
        // output
        if (ticks - tick <= num_ticks_to_print)
        {
            stringstream ss;
            for (size_t i = 0; i < station_centers.size(); i++)
            {
                station_centers[i].print_platform(ss);
            }
            gather_string(ss, mpi_rank, total_processes, tick);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}