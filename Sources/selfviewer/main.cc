#include <cassert>
#include <cstdio>
#include <iomanip>
#include <iostream>

#include <search/Game/Environment.hpp>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }

    FILE* fp = fopen(argv[1], "rb");
    if (!fp)
    {
        std::cerr << "error: cannot open file " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    int turns, stateSize, actionSpaceSize;
    fread(&turns, 4, 1, fp);
    fread(&stateSize, 4, 1, fp);
    fread(&actionSpaceSize, 4, 1, fp);

    std::cout << "turns: " << turns << std::endl
              << "state size: " << stateSize << std::endl
              << "action space size: " << actionSpaceSize << std::endl
              << std::endl;

    search::Game::Environment game;

    std::vector<float> state(stateSize);
    std::vector<float> pis(actionSpaceSize);
    float z;
    int omit;

    for (int i = 0; i < turns; ++i)
    {
        std::cout << "[turn " << i << "]\n" << game.ToString();

        fread(state.data(), 4, stateSize, fp);
        fread(pis.data(), 4, actionSpaceSize, fp);
        fread(&z, 4, 1, fp);
        fread(&omit, 4, 1, fp);

        int maxAction = 0;
        float maxPiValue = -999;

        std::cout << "PI (avail " << game.GetValidActions().size() << ") "
                  << std::endl;
        for (int j = 0; j < actionSpaceSize; ++j)
        {
            std::cout << std::setw(3) << pis[j] << " ";

            if (pis[j] > maxPiValue)
            {
                maxAction = j;
                maxPiValue = pis[j];
            }
        }
        std::cout << std::endl
                  << "max action: "
                  << game.GetActionSpace()[maxAction].action->ToString() << " ["
                  << maxAction << "]\n"
                  << "z: " << z << "\n"
                  << "omit: " << omit << "\n"
                  << std::endl;

        game.Play(*game.GetActionSpace()[maxAction].action);
    }

    std::cout << "[Last board]\n" << game.ToString();

    fclose(fp);

    return EXIT_SUCCESS;
}
