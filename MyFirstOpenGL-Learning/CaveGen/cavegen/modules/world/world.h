#pragma once

#include "./chunk.h"
#include "./chunkconfig.h"

#include <map>
#include <set>
#include <glm/glm.hpp>

class World{
    public:
        int area = 0;
        int chunkSize = 10;
        ChunkConfig config = ChunkConfig(fillMode);
        FillMode fillMode = FillMode::Edges;
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

