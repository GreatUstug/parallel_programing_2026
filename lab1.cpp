#include <thread>
#include <vector>
#include <exception>
#include <iostream>
#include <string>
#include <syncstream>
#include <Windows.h>

void Worker(int index)
{
    std::osyncstream syncedOut(std::cout);
    syncedOut << "Поток № " << index << " выполняет свою работу" << '\n';
}

int GetThreadsCount(const std::string& str) 
{
    int threadsCount = std::stoi(str);
    if (threadsCount <= 0) 
    {
        throw std::invalid_argument("The number of threads must be greater than 0");
    }
    return threadsCount;
}

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(65001);
    try {
        if (argc != 2) {
            throw std::invalid_argument("Invalid count of arguments from console");
        }
        int threadCount = GetThreadsCount(argv[1]);
        std::vector<std::jthread> threads;
        threads.reserve(threadCount);
        for (int i = 1; i <= threadCount; ++i)
        {
            threads.emplace_back(Worker, i);
        }
    } catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
