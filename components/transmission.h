#pragma once
#include <vector>

// This is the Rendering Engine.. Need to move GL Functions here
class Transmission {
    public:
        Transmission(unsigned int apparition, std::vector<unsigned int>& shader_modules);
        ~Transmission();

        void update();

    private:
        std::vector<unsigned int>& shader_modules;
        unsigned int VAO, apparition;
};