#pragma once

#include "./chunk.h"
#include "./chunkconfig.h"

#include <map>
#include <set>
#include <glm/glm.hpp>

class World{
    public:
        World();
        ~World();

        std::map<glm::ivec3, Chunk, Vec3Compare> MapTable;
        glm::ivec3 currentChunk;
        glm::ivec3 offset;
        FillMode fillMode = FillMode::Edges;
        ChunkConfig config = ChunkConfig(fillMode);
        int chunkSize = 30;
        int area = 1;

        void UpdateChunks(glm::vec3&);
        std::set<glm::ivec3, Vec3Compare> visible_chunks;
        glm::ivec3 lastChunk;

    private: 
        int visibleChunks;
        const int maxFOV = 60;
        std::set<glm::ivec3, Vec3Compare> created_chunks;
};

