#pragma once

#include "train.hpp"

void scatter_trains(const vector<vector<Train>>& all_trains, const size_t& total_processes, const size_t& mpi_rank, vector<Train>& result)
{
    size_t num_stations = all_trains.size();
    int* send_counts = (int*)malloc(sizeof(int) * total_processes);
    int* displs = (int*)malloc(sizeof(int) * total_processes);
    vector<Train> flatten_data;

    if (mpi_rank == 0)
    {
        // printf("\n\n");
        size_t offset = 0;
        for (size_t rank = 0; rank < total_processes; rank++)
        {
            send_counts[rank] = 0;
            for (size_t index = rank; index < num_stations; index += total_processes)
            {
                send_counts[rank] += all_trains[index].size() * sizeof(Train);
                flatten_data.insert(flatten_data.end(), all_trains[index].begin(), all_trains[index].end());
            }
            displs[rank] = offset;
            offset += send_counts[rank];
            // printf("send_counts[%ld] = %d, displs[%ld] = %d\n", rank, send_counts[rank], rank, displs[rank]);
        }
    }

    int recv_count;
    MPI_Scatter(send_counts, 1, MPI_INT, &recv_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    result.resize(recv_count / sizeof(Train));

    MPI_Scatterv(flatten_data.data(), send_counts, displs, MPI_BYTE, result.data(), recv_count, MPI_BYTE, 0, MPI_COMM_WORLD);
    // printf("------------------\n\n\n");
    
    // for (size_t i = 0; i < result.size(); i++)
    // {
    //     result[i].print();
    // }
    // printf("Scatter successfully! %ld/%ld to process %ld\n", result.size(), recv_count / sizeof(Train), mpi_rank);
    // free memory
    free(send_counts);
    free(displs);
}

void gather_string(const stringstream& ss, const size_t& mpi_rank, const size_t& total_processes, const size_t& tick)
{
    string local_string = ss.str();
    int local_size = local_string.size();
    // printf("%d, %d\n", mpi_rank, local_size);
    int* sizes = (int*)malloc(sizeof(int) * total_processes);
    int* displs = (int*)malloc(sizeof(int) * total_processes);
    int total_size = 0;
    // printf("Tick tick %d , local size %s\n", tick, local_string.c_str());
    MPI_Gather(&local_size, 1, MPI_INT, sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (mpi_rank == 0)
    {
        for (size_t i = 0; i < total_processes; i++)
        {
            displs[i] = total_size;
            total_size += sizes[i];
            // printf("%d: %d\n", mpi_rank, sizes[i]);
        }
    }

    vector<char> recv_buffer(total_size);


    string output;
    MPI_Gatherv(local_string.c_str(), local_size, MPI_CHAR, recv_buffer.data(), sizes, displs, MPI_CHAR, 0, MPI_COMM_WORLD);
    // printf("%ld: %ld Done outputing\n", tick, mpi_rank);

    if (mpi_rank == 0)
    {
        string output(recv_buffer.begin(), recv_buffer.end()); // Convert gathered buffer to string
        stringstream ss(output);
        vector<string> words;
        string word;
        while (ss >> word)
        {
            words.push_back(word);
        }

        std::sort(words.begin(), words.end());
        printf("%ld:", tick);
        for (const auto& w : words)
        {
            printf(" %s", w.c_str());
        }
        printf("\n");
    }
    free(displs);
    free(sizes);
}

void all_to_all_train(vector<vector<Train>>& train_send, vector<Train>& result, const size_t& total_processes)
{
    // printf("Sending \n");
    int* send_counts = (int*)malloc(sizeof(int) * total_processes);
    int* recv_counts = (int*)malloc(sizeof(int) * total_processes);
    int* send_displs = (int*)malloc(sizeof(int) * total_processes);
    int* recv_displs = (int*)malloc(sizeof(int) * total_processes);
    vector<Train> flatten_data;
    int total_size = 0;
    for (size_t i = 0; i < total_processes; i++)
    {
        // printf("Process %ld has %ld trains\n", i, train_send[i].size());
        send_counts[i] = (int)(train_send[i].size() * sizeof(Train));
        recv_counts[i] = 0;
        if (send_counts[i] > 0)
        {
            flatten_data.insert(flatten_data.end(), train_send[i].begin(), train_send[i].end());
        }
    }

    MPI_Alltoall(send_counts, 1, MPI_INT, recv_counts, 1, MPI_INT, MPI_COMM_WORLD);
    int send_offset = 0, recv_offset = 0;
    for (size_t i = 0; i < total_processes; i++)
    {
        send_displs[i] = send_offset;
        recv_displs[i] = recv_offset;
        recv_offset += recv_counts[i];
        send_offset += send_counts[i];
        // printf("Send %d to %ld (id:%d), recv %d from %ld (id:%d)\n", send_counts[i], i, send_displs[i], recv_counts[i], i, recv_displs[i]);
        total_size += recv_counts[i];
    }
    if (total_size > 0)
    {
        result.resize(total_size / sizeof(Train));
    }
    MPI_Alltoallv(flatten_data.data(), send_counts, send_displs, MPI_BYTE, result.data(), recv_counts, recv_displs, MPI_BYTE, MPI_COMM_WORLD);
    // printf("All to all successfully! %ld/%ld to process\n", result.size(), total_size / sizeof(Train));
    // for (size_t i = 0; i < result.size(); i++)
    // {
    //     // printf("%d\n", rank);
    //     result[i].print();
    // }
    free(send_counts);
    free(recv_counts);
    free(send_displs);
    free(recv_displs);

    for (size_t i = 0; i < total_processes; i++)
    {
        train_send[i].clear();
    }
}