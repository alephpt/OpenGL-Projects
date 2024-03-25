#include "./manifest.h"

Manifest::Manifest(Entities<Orientation>& instance): instance(instance) {}

unsigned int Manifest::createIdentity() {
    if (artifacts.size() > 0) {
        uint32_t id = artifacts[artifacts.size() - 1];
        artifacts.pop_back();
        return id;
    }

    return entities++;
}

void Manifest::createInstance(glm::vec3 position, glm::vec3 direction) {
    unsigned int identity = createIdentity();

    Orientation orientation;
    orientation.position = position;
    orientation.direction = direction;
    instance.insert(identity, orientation);
}