#pragma once

#include <string>
#include <cstdint>

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

    struct
    {
        int NumWorkers{ 1 };

        int MaxAction{ 20 };

        float VirtualLoss{ 0 };
        float cPUCT{ 1 };

        std::uint64_t MaxSimulation{ 100 };

        bool EnableDirichletNoise{ false };
        float DirichletNoiseAlpha{ 0.03f };
        float DirichletNoiseEps{ 0.75f };
    } search;

    static Config Load(const std::string& filename);
};
}
