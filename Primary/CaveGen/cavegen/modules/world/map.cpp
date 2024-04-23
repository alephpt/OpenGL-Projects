
#include "map.h"
#include "chunkfactory.h"
#include "../../components/utility/logger.h"

#include <algorithm>

World::World()
    {
        MapTable.clear();
        new_chunks.clear();
        delete_chunks.clear();
        visible_chunks.clear();
        
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
                it->second.indices.clear();

                if (visible_chunks.find(it->first) != visible_chunks.end()) 
                    { visible_chunks.erase(it->first); }

                MapTable.erase(it);
            }
    }

static inline auto calcMaxX = [](const glm::vec3& playerFront, const glm::vec3& playerLoc, const glm::vec3& playerRight, float halfFOV, int depthOfView) 
    { return (int)((playerFront * (float)depthOfView + playerLoc + playerRight * halfFOV).x); };

static inline auto calcMinX = [](const glm::vec3& playerFront, const glm::vec3& playerLoc, const glm::vec3& playerRight, float halfFOV, int depthOfView) 
    { return (int)((playerFront * (float)depthOfView + playerLoc - playerRight * halfFOV).x); };

static inline auto calcMaxY = [](const glm::vec3& playerFront, const glm::vec3& playerLoc, const glm::vec3& playerUp, float halfFOV, int depthOfView) 
    { return (int)((playerFront * (float)depthOfView + playerLoc + playerUp * halfFOV).y); };

static inline auto calcMinY = [](const glm::vec3& playerFront, const glm::vec3& playerLoc, const glm::vec3& playerUp, float halfFOV, int depthOfView) 
    { return (int)((playerFront * (float)depthOfView + playerLoc - playerUp * halfFOV).y); };

static inline auto calcMaxZ = [](const glm::vec3& playerFront, const glm::vec3& playerLoc, float halfFOV, int depthOfView) 
    { return (int)((playerFront * (float)depthOfView + playerLoc + playerFront * halfFOV).z); };

static inline auto calcMinZ = [](const glm::vec3& playerFront, const glm::vec3& playerLoc, float halfFOV, int depthOfView) 
    { return (int)((playerFront * (float)depthOfView + playerLoc - playerFront * halfFOV).z); };

// TODO: Improve this to only show +/-1 chunk from the current direction and +1 chunk further away
void World::UpdateChunks(glm::vec3 &playerLoc, glm::vec3 &playerFront, glm:: vec3 &playerUp, const float &halfFOV)
    {
        // Calculate current chunk based on player's position
        glm::ivec3 currentChunk = {
            playerLoc.x >= 0 ? (int)playerLoc.x / chunkSize : (int)playerLoc.x / chunkSize - 1, // We need to subtract 1 if the player is in the negative
            playerLoc.y >= 0 ? (int)playerLoc.y / chunkSize : (int)playerLoc.y / chunkSize - 1, // to get the correct chunk because +/-1 is the same chunk
            playerLoc.z >= 0 ? (int)playerLoc.z / chunkSize : (int)playerLoc.z / chunkSize - 1  // and puts us off by one
        };

        // Determine the visible chunks based on the player's position
        glm::vec3 playerRight = glm::cross(playerFront, playerUp);


        std::set<glm::ivec3, Vec3Compare> previous_chunks = visible_chunks;

        // Add visible chunks to the visible_chunks set
        visible_chunks.clear();
        for (int i = depthOfView; i < 0; i--) {
            int max_x = calcMaxX(playerFront, playerLoc, playerRight, halfFOV, i) / chunkSize;
            int min_x = calcMinX(playerFront, playerLoc, playerRight, halfFOV, i) / chunkSize;
            int max_y = calcMaxY(playerFront, playerLoc, playerUp, halfFOV, i) / chunkSize;
            int min_y = calcMinY(playerFront, playerLoc, playerUp, halfFOV, i) / chunkSize;
            int max_z = calcMaxZ(playerFront, playerLoc, halfFOV, i) / chunkSize;
            int min_z = calcMinZ(playerFront, playerLoc, halfFOV, i) / chunkSize;

            for (int x = min_x; x < max_x; x++) {
                for (int y = min_y; y < max_y; y++) {
                    for (int z = min_z; z < max_z; z++) {
                        visible_chunks.emplace(x, y, z);
                    }
                }
            }

        }

        // the delete chunks are the chunks in the previous_chunks set that are not in the visible_chunks set
        std::set_difference(previous_chunks.begin(), previous_chunks.end(), visible_chunks.begin(), visible_chunks.end(), std::inserter(delete_chunks, delete_chunks.begin()), Vec3Compare());

        // the new chunks are the chunks in in the visible_chunks set that are not in the previous_chunks set
        std::set_difference(visible_chunks.begin(), visible_chunks.end(), previous_chunks.begin(), previous_chunks.end(), std::inserter(new_chunks, new_chunks.begin()), Vec3Compare());

        const char* type = FILL_MODE_STR(fillMode);

        // Remove chunks that are no longer visible
        for (auto it = delete_chunks.begin(); it != delete_chunks.end(); it++) 
            {
                if (MapTable.find(*it) == MapTable.end()) { continue; }

                //Logger::Verbose("\t - Deleting %s Chunk %d %d %d \n", type, it->x, it->y, it->z);
                OffloadChunk(&*MapTable.find(*it), type);
                MapTable.erase(*it);        // We could potentially keep the chunk in memory and just remove it from the map table
            }

        // Update created chunks and load new chunks
        for (auto it = new_chunks.begin(); it != new_chunks.end(); it++)
            {
                Chunk* MapChunk = new Chunk();

                if (!LoadChunk(MapChunk, type, *it))
                    {
                        //Logger::Verbose("\t - Generating New %s Chunk %d %d %d\n", type, it->x, it->y, it->z);
                        glm::ivec3 offset = *it * chunkSize;
                        MapChunk = ChunkGenerator::Generate(offset, chunkSize, config);
                        //MapChunk->log();
                    }

                //MapChunk->log();
                MapTable.emplace(*it, *MapChunk);
                delete MapChunk;
            }            

        Logger::Verbose("\n\tMapTable Size: %d\n", MapTable.size());
        Logger::Verbose("\tVisible Chunks: %d\n", visible_chunks.size());
        Logger::Verbose("\tNew Chunks: %d\n", new_chunks.size());
        Logger::Verbose("\tDelete Chunks: %d\n\n", delete_chunks.size());
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