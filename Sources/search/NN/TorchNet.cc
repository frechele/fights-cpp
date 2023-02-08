#include <search/NN/TorchNet.hpp>

namespace search::NN
{
void TorchNet::Initialize(int rank, const std::string& filename)
{
    gpuID_ = torch::cuda::device_count() % (rank + 1);
    impl_ = torch::jit::load(filename);
    impl_.to(torch::Device(torch::kCUDA, gpuID_));
    impl_.eval();
}

void TorchNet::Shutdown()
{
}

void TorchNet::predictImpl(const std::vector<Tensor>& state,
                           std::vector<PolicyVal>& pols,
                           std::vector<float>& vals)
{
    const int batchSize = state.size();
    if (batchSize == 0)
        return;

    std::vector<float> stateVec(batchSize * state[0].size());
    for (int i = 0; i < batchSize; ++i)
    {
        std::copy(state[i].begin(), state[i].end(),
                  stateVec.begin() + i * state[i].size());
    }

    torch::Tensor stateTensor =
        torch::from_blob(stateVec.data(), { batchSize, 25, 9, 9 },
                         torch::kFloat32)
            .to(torch::Device(torch::kCUDA, gpuID_));

    torch::NoGradGuard guard;

    const auto preds = impl_.forward({ stateTensor }).toTuple();

    const auto policyTensor = preds->elements()[0].toTensor().cpu();
    const auto valueTensor = preds->elements()[1].toTensor().cpu();

    const auto policyAccessor = policyTensor.accessor<float, 2>();
    const auto valueAccessor = valueTensor.accessor<float, 2>();

    for (int i = 0; i < batchSize; ++i)
    {
        for (int j = 0; j < Game::Environment::ACTION_SPACE_SIZE; ++j)
        {
            pols[i][j] = policyAccessor[i][j];
        }
        vals[i] = valueAccessor[i][0];
    }
}
}  // namespace search::NN
