#pragma once

#include <torch/script.h>
#include <torch/torch.h>

#include <search/NN/NNBase.hpp>

namespace search::NN
{
class TorchNet : public NNBase
{
 public:
    void Initialize(int rank, const std::string& filename) override;
    void Shutdown() override;

 private:
    void predictImpl(const std::vector<Tensor>& state,
                     std::vector<PolicyVal>& pol,
                     std::vector<float>& val) override;

 private:
    int gpuID_;
    torch::jit::script::Module impl_;
};
}  // namespace search::NN
