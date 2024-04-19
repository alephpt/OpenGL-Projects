#pragma once

#include "./atomic.h"
#include "./chunkdata.h"

#include <map>
#include <set>
#include <glm/glm.hpp>

class World{
    public:
        World();
        ~World();

        std::map<glm::ivec3, ChunkData, Vec3Compare> MapTable;
        glm::ivec3 currentChunk;
        glm::ivec3 offset;
        int chunkSize = 30;
        float noiseThreshold = 21.29f;
        float fillCutOff = 81.75f;
        int scalar = 16;
        int howSmooth = 5;

        void UpdateChunks(glm::vec3&);
        std::set<glm::ivec3, Vec3Compare> visible_chunks;

    private: 
        int visibleChunks;
        const int maxFOV = 60;
        std::set<glm::ivec3, Vec3Compare> created_chunks;
        glm::ivec3 lastChunk;
};

