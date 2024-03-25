#pragma once
#include "../utilities/entities.h"

// The Manifest is the Factory that Instantiates the World Contents
class Manifest {
    public:
        Manifest(Entities<Orientation>& instance);
        ~Manifest(){};

        void createInstance(glm::vec3 position, glm::vec3 direction)

    private:
        unsigned int entities = 0;
        std::vector<unsigned int> artifacts;
        Entities<Orientation>& instance;

        unsigned int createIdentity();
}