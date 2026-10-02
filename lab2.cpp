#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>

const int MIN_NUM_OF_ARGS = 5;
const int MAX_NUM_OF_ARGS = 6;

struct Rect { int x0, y0, x1, y1; };

struct Image {
    int width = 0, height = 0, channels = 4;
    std::vector<uint8_t> pixels;
};

struct Params {
    std::string inputFile;
    std::string outputFile;
    int countThreads;
    int radius;
};

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
        params.radius = argc == MAX_NUM_OF_ARGS ? std::stoi(argv[5]) : 4;
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

Image LoadImage(const std::string& path)
{
    int w, h, ch;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!data)
        throw std::runtime_error("Cannot load image: " + path);

    Image img;
    img.width    = w;
    img.height   = h;
    img.channels = 4;
    img.pixels.assign(data, data + (size_t)w * h * 4);
    stbi_image_free(data);
    return img;
}

void SaveImage(const std::string& path, const Image& img)
{
    if (!stbi_write_bmp(path.c_str(), img.width, img.height, 4, img.pixels.data()))
        throw std::runtime_error("Cannot save image: " + path);
}

void BlurRect(const Image& src, Image& dst, Rect rc, int radius)
{
    const int W = src.width;
    const int H = src.height;
    const int C = src.channels;
    const int winArea = (2 * radius + 1) * (2 * radius + 1);

    for (int y = rc.y0; y < rc.y1; ++y) {
        for (int x = rc.x0; x < rc.x1; ++x) {
            int sum[4] = {0, 0, 0, 0};

            for (int dy = -radius; dy <= radius; ++dy) {
                int ny = std::clamp(y + dy, 0, H - 1);
                for (int dx = -radius; dx <= radius; ++dx) {
                    int nx = std::clamp(x + dx, 0, W - 1);
                    size_t idx = ((size_t)ny * W + nx) * C;
                    for (int c = 0; c < C; ++c)
                        sum[c] += src.pixels[idx + c];
                }
            }

            size_t outIdx = ((size_t)y * W + x) * C;
            for (int c = 0; c < C; ++c)
                dst.pixels[outIdx + c] = (uint8_t)(sum[c] / winArea);
        }
    }
}

std::vector<std::vector<Rect>> MakeWork(int width, int height, int threadCount)
{
    std::vector<std::vector<Rect>> work(threadCount);

    int base  = width / threadCount;
    int extra = width % threadCount;

    int x = 0;
    for (int k = 0; k < threadCount; ++k) {
        int w = base + (k < extra ? 1 : 0);
        if (w > 0) {
            work[k].push_back(Rect{ x, 0, x + w, height });
            x += w;
        }
    }
    return work;
}

void Worker(const Image& src, Image& dst, std::vector<Rect> rects, int radius)
{
    for (const Rect& r : rects)
        BlurRect(src, dst, r, radius);
}

int main(int argc, char* argv[])
{
    try{
        Params params = GetParams(argc, argv);
        Image src = LoadImage(params.inputFile);
        Image dst;
        dst.width    = src.width;
        dst.height   = src.height;
        dst.channels = src.channels;
        dst.pixels.resize(src.pixels.size());
        auto work = MakeWork(src.width, src.height, params.countThreads);

        const auto start = std::chrono::steady_clock::now();
        {
            std::vector<std::jthread> threads;
            for (int k = 0; k < params.countThreads; ++k)
            {
                threads.emplace_back(Worker, std::cref(src), std::ref(dst),
                                    std::move(work[k]), params.radius);
            }
        }
        const auto finish = std::chrono::steady_clock::now();
                SaveImage(params.outputFile, dst);

        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
        int hw = (int)std::thread::hardware_concurrency();

    
        std::cout << params.countThreads << "," << params.radius << "," << hw << "," << ms << "\n";


    } catch (const std::invalid_argument& e){
        std::cerr << e.what() << std::endl;
        PrintHelp();
        return 1;
    } catch (const std::runtime_error& e){
        std::cerr << e.what() << std::endl;
        return 2;
    }
    return 0;
}
