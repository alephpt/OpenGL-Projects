#pragma once

#include "./chunk.h"
#include "./chunkconfig.h"

#include <map>
#include <set>
#include <glm/glm.hpp>

class World{
    public:
        int area = 1;
        int chunkSize = 45;
        FillMode fillMode = FillMode::Edges;
        ChunkConfig config = ChunkConfig(fillMode);
        glm::ivec3 lastChunk;
        glm::ivec3 currentChunk;
        std::set<glm::ivec3, Vec3Compare> new_chunks;
        std::set<glm::ivec3, Vec3Compare> delete_chunks;
        std::set<glm::ivec3, Vec3Compare> visible_chunks;
        std::map<glm::ivec3, Chunk, Vec3Compare> MapTable;

        World();
        ~World();

        void UpdateChunks(glm::vec3&);
        void reset();
        void solidFill();
        void edgeFill();
        void tunnelFill();
        void cellFill();
};

