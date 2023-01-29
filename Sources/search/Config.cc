#include <search/Config.hpp>

#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

namespace search
{
Config Config::Load(const std::string& filename)
{
    using namespace std::string_literals;

    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("cannot open config file \""s + filename + "\"");
    }

    auto j = nlohmann::json::parse(file);

    Config cfg;

    // nn configs
    cfg.nn.NumWorkers = j["nn"].value<int>("num_workers", cfg.nn.NumWorkers);
    cfg.nn.MaxBatch = j["nn"].value<int>("max_batch", cfg.nn.MaxBatch);
    cfg.nn.EngineFile = j["nn"].value<std::string>("engine_file", cfg.nn.EngineFile);
    cfg.nn.EngineType = j["nn"].value<std::string>("engine_type", cfg.nn.EngineType);

    return cfg;
}
}  // namespace search
