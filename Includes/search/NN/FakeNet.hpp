#pragma once

#include <search/NN/NNBase.hpp>

namespace search::NN
{
class FakeNet : public NNBase
{
 public:
    void Initialize(int rank, const std::string& filename) override;
    void Shutdown() override;

 private:
    void predictImpl(const std::vector<Tensor>& state,
                     std::vector<PolicyVal>& pol,
                     std::vector<float>& val) override;
};
}  // namespace search::NN
