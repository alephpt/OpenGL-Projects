
#include "./world.h"
#include "./chunkfactory.h"

#include <algorithm>
#include "world.h"

// Need to seperate World and Chunk classes
World::World()
    { 
        lastChunk = glm::ivec3(0, 0, 0);
        currentChunk = lastChunk;
        MapTable.clear();
        new_chunks.clear();
        delete_chunks.clear();
        visible_chunks.insert(lastChunk);
        
        printf("Creating World\n");
        printf("Chunk Size: %d\n", chunkSize);
        return ; 
    }

World::~World() 
    {
        // Offload all chunks to disk - equivalent to saving
        for (auto it = MapTable.begin(); it != MapTable.end(); it++) 
            {
                OffloadChunk(&*it, FILL_MODE_STR(fillMode));

                // delete chunk data
                it->second.vertices.clear();
                it->second.colors.clear();
                it->second.normals.clear();
                it->second.indices.clear();

                if (visible_chunks.find(it->first) != visible_chunks.end()) 
                    { visible_chunks.erase(it->first); }

                MapTable.erase(it);
            }
    }


// TODO: Improve this to only show +/-1 chunk from the current direction and +1 chunk further away
void World::UpdateChunks(glm::vec3 &playerLoc) 
    {
        // Calculate current chunk based on player's position
        glm::ivec3 currentChunk = glm::ivec3(
            static_cast<int>(playerLoc.x / chunkSize),
            static_cast<int>(playerLoc.y / chunkSize),
            static_cast<int>(playerLoc.z / chunkSize)
        );

        std::set<glm::ivec3, Vec3Compare> previous_chunks = visible_chunks;

        // Check if player has moved to a new chunk
        if (currentChunk != lastChunk) 
            {
                // Update visible chunks
                visible_chunks.clear();

                // Create a new set of visible chunks
                for (int x = 1 - area; x <= 1 + area; x++)                   // -1 is left
                    {
                        for (int y = 0 - area; y <= 0 + area; y++)            // -1 is up
                            {
                                for (int z = 1 - area; z <= 1 + area; z++)    // +1 is back
                                    {
                                        glm::ivec3 chunk = glm::ivec3(
                                            currentChunk.x + x,
                                            currentChunk.y + y,
                                            currentChunk.z + z
                                        );

                                        visible_chunks.insert(chunk);
                                    }
                            }
                    }
                
                // the new chunks are the chunks in in the visible_chunks set that are not in the previous_chunks set
                std::set_difference(visible_chunks.begin(), visible_chunks.end(), previous_chunks.begin(), previous_chunks.end(), std::inserter(new_chunks, new_chunks.begin()), Vec3Compare());

                // the delete chunks are the chunks in the previous_chunks set that are not in the visible_chunks set
                std::set_difference(previous_chunks.begin(), previous_chunks.end(), visible_chunks.begin(), visible_chunks.end(), std::inserter(delete_chunks, delete_chunks.begin()), Vec3Compare());

                const char* type = FILL_MODE_STR(fillMode);

                // Remove chunks that are no longer visible
                for (auto it = delete_chunks.begin(); it != delete_chunks.end(); it++) 
                    {
                        if (MapTable.find(*it) == MapTable.end()) { continue; }

                        printf("Deleting %s_%d%d%d.chunk\n", type, it->x, it->y, it->z);
                        OffloadChunk(&*MapTable.find(*it), type);
                        MapTable.erase(*it);
                    }

                // Update created chunks and load new chunks
                for (auto it = new_chunks.begin(); it != new_chunks.end(); it++)
                    {
                        Chunk* MapChunk = new Chunk();

                        if (!LoadChunk(MapChunk, type, *it))
                            {
                                printf("\t Generating New Chunk: %s_%d%d%d.chunk\n", type, it->x, it->y, it->z);
                                glm::ivec3 offset = *it * chunkSize;
                                MapChunk = ChunkGenerator::Generate(offset, chunkSize, config);
                            }

                        MapTable.emplace(*it, *MapChunk);
                        delete MapChunk;
                    }            

                printf("\t MapTable Size: %d\n", MapTable.size());

                printf("Visible Chunks: %d\n", visible_chunks.size());
                // Update last chunk
                lastChunk = currentChunk;
            }
    }
    
void World::reset()
    {
        MapTable.clear();  
        delete_chunks = visible_chunks;
        visible_chunks.clear();
    }

void World::solidFill()
    {
        fillMode = FillMode::Solid;
        config = ChunkConfig(fillMode);
        reset();
    }

void World::edgeFill()
    {
        fillMode = FillMode::Edges;
        config = ChunkConfig(fillMode);
        reset();
    }

void World::tunnelFill()
    {
        fillMode = FillMode::Tunnels;
        config = ChunkConfig(fillMode);
        reset();
    }

void World::cellFill()
    {
        fillMode = FillMode::Cells;
        config = ChunkConfig(fillMode);
        reset();
    }