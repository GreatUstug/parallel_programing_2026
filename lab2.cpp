#include <string>
#include <iostream>

const int MIN_NUM_OF_ARGS = 5;
const int MAX_NUM_OF_ARGS = 6;

// Прямоугольник пикселей [x0, x1) × [y0, y1)
struct Rect { int x0, y0, x1, y1; };

// Размывает пиксели rc: читает из src, пишет в dst
// void BlurRect(const Image& src, Image& dst, Rect rc, int radius);

// Для каждого потока — список его прямоугольников (зависит от варианта)
// std::vector<std::vector<Rect>> MakeWork(int width, int height, int threadCount);

struct Params {
    std::string inputFile;
    std::string outputFile;
    int countThreads;
    int radius;
};

void Blur()
{

}

void PrintHelp()
{
    std::cout << "The command syntax: blur <input.bmp> <output.bmp> <threads> [radius]" << std::endl;
}

Params GetParams(int argc, char* argv[])
{
    if (argc < MIN_NUM_OF_ARGS || argc > MAX_NUM_OF_ARGS) 
    {
        throw std::invalid_argument("Incorrect number of params in cmd!");
    }
    if (std::string(argv[1]) != "blur")
    {
        throw std::invalid_argument("Unknown command: " + std::string(argv[1]));
    }
    Params params;
    params.inputFile = argv[2];
    params.outputFile = argv[3];
    try
    {
        params.countThreads = std::stoi(argv[4]);
        params.radius = argc == MAX_NUM_OF_ARGS ? std::stoi(argv[5]) : 1;
    } catch (const std::invalid_argument&)
    {
        throw std::invalid_argument("The count of threads or radius not a number!");
    }
    if (params.countThreads < 1)
    {
        throw std::invalid_argument("threads must be >= 1");
    }
    if (params.radius < 0)
    {
        throw std::invalid_argument("radius must be >= 0");
    }
    return params;
};

int main(int argc, char* argv[])
{
    try{
        Params params = GetParams(argc, argv);
    } catch (const std::invalid_argument& e){
        std::cerr << e.what() << std::endl;
        PrintHelp();
        return 1;
    }
    return 0;
}

// ...в main, после чтения файла
// Image dst = /* пустое изображение того же размера */;
// auto work = MakeWork(src.width, src.height, threadCount);

// const auto start = std::chrono::steady_clock::now();
// {
//     std::vector<std::jthread> threads;
//     for (int k = 0; k < threadCount; ++k)
//     {
//         threads.emplace_back(Worker, std::cref(src), std::ref(dst),
//                              std::move(work[k]), radius);
//     }
// } // здесь все потоки завершены: деструкторы jthread сделали join
// const auto finish = std::chrono::steady_clock::now();

