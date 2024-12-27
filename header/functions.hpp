#include <algorithm>
#include "message.hpp"

// auto key_selector = [](const auto& pair){return pair.first;};

void indexing_trains(const size_t& num_stations, const unordered_map<char, vector<size_t>> &station_line_code, const unordered_map<char, size_t>& num_trains, vector<vector<Train>>& all_trains)
{
    unordered_map<char, size_t> current_trains;
    all_trains.resize(num_stations);
    size_t id = 0, count = 0, offset = 0;
    vector<char> line_colors = {'g', 'y', 'b'};
    // for (const char& c : LINE_COLORS)
    // {
    //     if (station_line_code.find(c) != station_line_code.end())
    //     {
    //         line_colors.push_back(c);
    //     }
    // }

    for (const char& color : line_colors)
    {
        current_trains[color] = 0;
        count += num_trains.at(color);
    }
    // printf("Total %ld trains\n", count);

    while (id < count)
    {
        char color = line_colors[offset];
        const vector<size_t>* line = &station_line_code.at(color);
        // printf("Consider color %c, first %ld\n", color, line->at(0));
        if (num_trains.at(color) > current_trains[color])
        {
            size_t first_platform_code = line->at(1);
            size_t first_station_code = line->at(0);
            Train train(id, 1, first_platform_code, first_station_code, color, true);
            all_trains[first_station_code].push_back(train);
            current_trains[color]++;
            // printf("Train %c%ld is created for %ld\n", color, id, first_station_code);
            id++;
        }

        if (num_trains.at(color) > current_trains[color])
        {
            size_t last_platform_code = line->at(line->size() - 2);
            size_t last_station_code = line->at(line->size() - 1);
            Train train(id, line->size() - 2, last_platform_code, last_station_code, color, false);
            all_trains[last_station_code].push_back(train);
            current_trains[color]++;
            // printf("Train %c%ld is created for %ld\n", color, id, last_station_code);
            id++;
        }
        
        offset = (offset + 1) % line_colors.size();
    }
}