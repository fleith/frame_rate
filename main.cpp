//
// Created by Alvaro on 2019-02-02.
//


#include <chrono>
#include <numeric>
#include <iostream>
#include <vector>
#include <thread>
#include <unordered_map>


#include "docopt.h"

#include <iostream>

static const char USAGE[] =
        R"(Real-Time Playback Simulator.
    Usage:
      rtpb --duration=<seconds> [--jobs=<number> --realtime --realfps=<rframes> --fps=<frames>]
      rtpb (-h | --help)
      rtpb --version
    Options:
      -h --help                    Show this screen.
      --version                    Show version.
      -d --duration=<seconds>      Simulation duration.
      -j --jobs=<number>           Number of simultaneous threads [Default: 1].
      --realtime                   Enable real-time FPS.
      --realfps=<rframes>          Real-time FPS [Default: 24].
      --fps=<frames>               Reachable FPS [Default: 100].
)";


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

void worker(double time_to_wait, uint id)
{
    auto start = std::chrono::steady_clock::now();
    auto it = busyWait(time_to_wait);
    auto end = std::chrono::steady_clock::now();
    time_iterations[id] = std::make_pair(std::chrono::duration<double>(end-start).count(), it);
}

int main(int argc, char** argv)
{
    std::map<std::string, docopt::value> args
            = docopt::docopt(USAGE,
                             { argv + 1, argv + argc },
                             true,               // show help if requested
                             "Real-Time Playback Simulator 0.1");  // version string

    const double duration = std::stod(args["--duration"].asString());
    const size_t number_of_jobs = std::stoi(args["--jobs"].asString());
    const double fps = std::stod(args["--fps"].asString());
    const bool real_time = args["--realtime"].asBool();
    const double real_time_fps = std::stod(args["--realfps"].asString());

    std::cout << "____________________________" << std::endl;
    std::cout << "Real-Time Playback Simulator" << std::endl;
    std::cout << "  Reachable FPS: " << fps << std::endl;
    std::cout << "  Real-Time Mode: " << (real_time ? "ON" : "OFF") << std::endl;
    if(real_time) std::cout << "  Real-Time FPS: " << real_time_fps << std::endl;
    std::cout << "____________________________" << std::endl;
    std::cout << std::endl;

    time_iterations.resize(number_of_jobs);
    std::vector<std::thread> runners(number_of_jobs);

    //start timer
    auto start = std::chrono::steady_clock::now();
    //start the jobs
    for (int i = 0; i < number_of_jobs; ++i) {
        runners[i] = std::move(std::thread(&worker, duration, i));
    }
    //waiting jobs
    for(auto& r: runners){
        if (r.joinable()) r.join();
    }
    //end timer
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