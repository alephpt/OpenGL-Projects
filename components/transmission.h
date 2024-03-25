#pragma once
#include <vector>

// This is the Rendering Engine.. Need to move the main GL Functions out of the App and Move it here
// and then make this a Module.. Or move it all to Reflection and combine these
class Transmission {
    public:
        Transmission(unsigned int apparition, unsigned int& shader_program);
        ~Transmission();

        void update();

    private:
        unsigned int& shader_program;
        unsigned int VAO, apparition;
};