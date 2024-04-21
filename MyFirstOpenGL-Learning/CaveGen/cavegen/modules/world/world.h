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
        int chunkSize = 15;
        int area = 1;

        void UpdateChunks(glm::vec3&);
        void reset();

        std::set<glm::ivec3, Vec3Compare> new_chunks;
        std::set<glm::ivec3, Vec3Compare> delete_chunks;
        std::set<glm::ivec3, Vec3Compare> visible_chunks;
        glm::ivec3 lastChunk;
};

