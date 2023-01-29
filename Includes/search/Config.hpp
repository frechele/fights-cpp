#pragma once

#include <string>

namespace search
{
struct Config final
{
    struct 
    {
        int MaxBatch{ 1 };
        int NumWorkers{ 1 };

        std::string EngineFile;
        std::string EngineType{ "fake" };
    } nn;

    static Config Load(const std::string& filename);
};
}
