#include <search/Config.hpp>

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <sstream>

namespace search
{
Config Config::Load(const std::string& filename)
{
    using namespace std::string_literals;

    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("cannot open config file \""s + filename +
                                 "\"");
    }

    auto j = nlohmann::json::parse(file);

    Config cfg;

    // nn configs
    cfg.nn.NumWorkers = j["nn"].value<int>("num_workers", cfg.nn.NumWorkers);
    cfg.nn.MaxBatch = j["nn"].value<int>("max_batch", cfg.nn.MaxBatch);
    cfg.nn.EngineFile =
        j["nn"].value<std::string>("engine_file", cfg.nn.EngineFile);
    cfg.nn.EngineType =
        j["nn"].value<std::string>("engine_type", cfg.nn.EngineType);

    // search config
    cfg.search.NumWorkers =
        j["search"].value<int>("num_workers", cfg.search.NumWorkers);

    cfg.search.MaxAction =
        j["search"].value<int>("max_action", cfg.search.MaxAction);

    cfg.search.VirtualLoss =
        j["search"].value<float>("virtual_loss", cfg.search.VirtualLoss);
    cfg.search.cPUCT = j["search"].value<float>("c_puct", cfg.search.cPUCT);

    cfg.search.MaxSimulation = j["search"].value<std::uint64_t>(
        "max_simulation", cfg.search.MaxSimulation);

    cfg.search.EnableDirichletNoise = j["search"].value<bool>(
        "enable_dirichlet", cfg.search.EnableDirichletNoise);
    cfg.search.DirichletNoiseAlpha = j["search"].value<float>(
        "dirichlet_alpha", cfg.search.DirichletNoiseAlpha);
    cfg.search.DirichletNoiseEps =
        j["search"].value<float>("dirichlet_eps", cfg.search.DirichletNoiseEps);

    return cfg;
}

std::string Config::ToString() const
{
    std::stringstream ss;

    ss << "[nn]\n"
       << "num_workers = " << nn.NumWorkers << "\n"
       << "max_batch = " << nn.MaxBatch << "\n"
       << "engine_file = " << nn.EngineFile << "\n"
       << "engine_type = " << nn.EngineType << "\n"
       << std::endl;

    ss << "[search]\n"
       << "num_workers = " << search.NumWorkers << "\n"
       << "max_action = " << search.MaxAction << "\n"
       << "virtual_loss = " << search.VirtualLoss << "\n"
       << "c_puct = " << search.cPUCT << "\n"
       << "max_simulation = " << search.MaxSimulation << "\n"
       << "enable_dirichlet = " << search.EnableDirichletNoise << "\n"
       << "dirichlet_alpha = " << search.DirichletNoiseAlpha << "\n"
       << "dirichlet_eps = " << search.DirichletNoiseEps << "\n";

    return ss.str();
}
}  // namespace search
