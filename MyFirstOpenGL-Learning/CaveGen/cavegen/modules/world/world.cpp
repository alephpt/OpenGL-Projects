
#include "./world.h"
#include "./chunkfactory.h"

// Need to seperate World and Chunk classes
World::World()
    { 
        visibleChunks = maxFOV / chunkSize;
        lastChunk = glm::ivec3(0, 0, 0);
        MapTable.clear();
        created_chunks.clear();
        visible_chunks.clear();
        
        printf("Creating World\n");
        printf("Chunk Size: %d\n", chunkSize);
        printf("Visible Chunks: %d\n", visibleChunks);
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

                if (created_chunks.find(it->first) != created_chunks.end()) 
                    { created_chunks.erase(it->first); }

                if (visible_chunks.find(it->first) != visible_chunks.end()) 
                    { visible_chunks.erase(it->first); }

                MapTable.erase(it);
            }
    }


void World::UpdateChunks(glm::vec3 &playerLoc) 
    {
        if (fillMode != FillMode::Custom) 
            { config = ChunkConfig(fillMode); }

        // Calculate current chunk based on player's position
        glm::ivec3 currentChunk = glm::ivec3(
            static_cast<int>(playerLoc.x / chunkSize),
            static_cast<int>(playerLoc.y / chunkSize),
            static_cast<int>(playerLoc.z / chunkSize)
        );

        // TODO: Improve this to only show +/-1 chunk from the current direction and +1 chunk further away

        // Check if player has moved to a new chunk
        if (currentChunk != lastChunk) 
            {
                // Update visible chunks
                visible_chunks.clear();

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
                printf("Visible Chunks: %d\n", visible_chunks.size());
                
                // Remove chunks that are no longer visible
                auto it = MapTable.begin();
                while (it != MapTable.end()) 
                    {
                        if (visible_chunks.find(it->first) == visible_chunks.end()) 
                            {
                                glm::ivec3 chunk = it->first;
                                printf("Removing Chunk: %d, %d, %d\n", chunk.x, chunk.y, chunk.z);
                                OffloadChunk(&*it, FILL_MODE_STR(fillMode));
                                it = MapTable.erase(it);
                            } 
                        else 
                            { ++it; }
                    }

                printf("Visible Chunks: %d\n", visible_chunks.size());

                // Update created chunks and load new chunks
                for (auto it = visible_chunks.begin(); it != visible_chunks.end(); it++) 
                    {
                        if (MapTable.find(*it) == MapTable.end()) 
                            {
                                printf("Sourcing %d%d%d.chunk\n", it->x, it->y, it->z);
                                Chunk* MapChunk = LoadChunk(*it, FILL_MODE_STR(fillMode));

                                if (MapChunk->vertices.size() == 0)
                                    {
                                        printf("\t .. Generating New Chunk: %d, %d, %d\n", it->x, it->y, it->z);
                                        glm::ivec3 offset = *it * chunkSize;
                                        MapChunk = ChunkGenerator::Generate(offset, chunkSize, config);
                                        MapTable[*it] = *MapChunk;
                                        created_chunks.insert(*it);
                                    } 
                                else 
                                    { 
                                        printf("\t .. Chunk %d, %d, %d Loaded.\n", it->x, it->y, it->z);
                                        MapTable[*it] = *MapChunk; 
                                        delete MapChunk;
                                    }
                            }
                    }

                printf("Visible Chunks: %d\n", visible_chunks.size());
                // Update last chunk
                lastChunk = currentChunk;
            }
    }