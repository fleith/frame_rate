//
// Created by Alvaro on 2019-02-02.
//


#include <chrono>
#include <numeric>
#include <iostream>
#include <vector>
#include <thread>
#include <unordered_map>

//std::unordered_map<std::thread::id, std::pair<double, int>> time_iterations; //race condition
std::vector<std::pair<double, int>> time_iterations;

int busyWait(double seconds)
{
    auto iterations = 0;
    auto start = std::chrono::steady_clock::now();
    bool wait = true;
    while(wait)
    {
        iterations++;
        auto end = std::chrono::steady_clock::now();
        wait = std::chrono::duration<double>(end-start).count() < seconds;
    }

    return iterations;
}

static std::atomic_uint worker_c(0);

void worker(double time_to_wait)
{
    auto start = std::chrono::steady_clock::now();
    auto it = busyWait(time_to_wait);
    auto end = std::chrono::steady_clock::now();
    auto tid = std::this_thread::get_id();
    time_iterations[worker_c++] = std::make_pair(std::chrono::duration<double>(end-start).count(), it);
//    time_iterations[tid] = std::make_pair(std::chrono::duration<double>(end-start).count(), it); //race condition
//    std::cout << "[" << tid << "]"<< " Time: " << std::chrono::duration<double>(end-start).count() << " second[s]" << std::endl;
//    std::cout << "[" << tid << "]"<< " Iterations: " << it << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: [application] [time to wait]\n";
        return -1;
    }
    double time_to_wait = std::stod(argv[1]);
    auto start = std::chrono::steady_clock::now();
////    auto it = busyWait(0.000000500);
//    auto it = busyWait(time_to_wait);
//    auto tid = std::this_thread::get_id();
//    std::cout << "[" << tid << "]"<< " Elapsed time: " << std::chrono::duration<double>(end-start).count() << " second[s]" << std::endl;
//    std::cout << "[" << tid << "]"<< " Number of iterations: " << it << std::endl;

//    worker(time_to_wait);
    const size_t number_of_workers = 250;
    time_iterations.resize(number_of_workers);
    std::vector<std::thread> runners(number_of_workers);
    for (int i = 0; i < number_of_workers; ++i) {
        runners[i] = std::move(std::thread(&worker, time_to_wait));
    }

    for(auto& r: runners){
        if (r.joinable()) r.join();
    }
    auto end = std::chrono::steady_clock::now();

    int c = 0;
    for(auto& r: time_iterations){
        std::cout << "[" << c << "]"<< " Time: " << r.first << " second[s]" << std::endl;
        std::cout << "[" << c << "]"<< " Iterations: " << r.second << std::endl;
        c++;
    }


    auto tid = std::this_thread::get_id();
    std::cout << "[" << tid << "]"<< " Whole elapsed time: " << std::chrono::duration<double>(end-start).count() << " second[s]" << std::endl;

    return 0;
}