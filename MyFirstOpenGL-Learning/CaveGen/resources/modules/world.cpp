
#include "./world.h"

// Need to seperate World and Chunk classes
World::World(){ 
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

World::~World() {
    // Offload all chunks to disk - equivalent to saving
    for (auto it = MapTable.begin(); it != MapTable.end(); it++) {
        OffloadChunkData(it->first, &(it->second));

        // delete chunk data
        it->second.vertices.clear();
        it->second.colors.clear();
        it->second.normals.clear();
        it->second.indices.clear();

        if (created_chunks.find(it->first) != created_chunks.end()) { created_chunks.erase(it->first); }
        if (visible_chunks.find(it->first) != visible_chunks.end()) { visible_chunks.erase(it->first); }

        MapTable.erase(it);
    }
}


void World::UpdateChunks(glm::vec3 &playerLoc) {
    // Calculate current chunk based on player's position
    glm::ivec3 currentChunk = glm::ivec3(
        static_cast<int>(playerLoc.x / chunkSize),
        static_cast<int>(playerLoc.y / chunkSize),
        static_cast<int>(playerLoc.z / chunkSize)
    );

    // Check if player has moved to a new chunk
    if (currentChunk != lastChunk) {
        // Update visible chunks
        visible_chunks.clear();

        for (int x = 0; x <= 2; x++) {
            for (int y = -1; y <= 1; y++) {
                for (int z = -1; z <= 1; z++) {
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
        while (it != MapTable.end()) {
            if (visible_chunks.find(it->first) == visible_chunks.end()) {
                glm::ivec3 chunk = it->first;
                printf("Removing Chunk: %d, %d, %d\n", chunk.x, chunk.y, chunk.z);
                OffloadChunkData(chunk, &(it->second));
                MapTable.erase(it);
            } else {
                ++it;
            }
        }

        // Update created chunks and load new chunks
        for (auto it = visible_chunks.begin(); it != visible_chunks.end(); it++) {
          if (MapTable.find(*it) == MapTable.end()) {
            printf("Loading or Creating Chunk: %d, %d, %d\n", it->x, it->y, it->z);
            ChunkData MapChunk = LoadChunkData(*it);

            if (MapChunk.vertices.empty()) {
                printf("Creating Chunk: %d, %d, %d\n", it->x, it->y, it->z);
                glm::ivec3 offset = *it * chunkSize;
                ChunkData* NewMapChunk = Chunk::Generate(offset, chunkSize, noiseThreshold, fillCutOff, scalar, howSmooth);
                MapTable[*it] = *NewMapChunk;
                created_chunks.insert(*it);
                delete NewMapChunk;
            } else {
                MapTable[*it] = MapChunk;
            }
          }
        }

        printf("Visible Chunks: %d\n", visible_chunks.size());
        // Update last chunk
        lastChunk = currentChunk;
    }
}