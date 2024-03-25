#include <stdio.h>
#include <stdlib.h>
#include "./modules/choreographer.h"
#include "./components/manifest.h"

int main() {
    // Main OpenGL Context
    World* world = new World();
    Manifest* manifestation = new Manifest(world->instance);
    
    // This is essentially our camera
    manifestation->createInstance(
        {0.0f, 0.0f, 1.0f}, // Position
        {0.0f, 0.0f, 0.0f} // Direction
    );

    // Create Graphics Pipeline and Run
    world->initGL();
    world->createChannel();
    world->persist();

    // Clean Up
    delete world;
    delete manifestation;
    return 0;
}