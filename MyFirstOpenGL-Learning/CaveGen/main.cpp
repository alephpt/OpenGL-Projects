#include "./cavegen/cavegen.h"
#include "./cavegen/modules/world/chunkfactory.h"

int main()
    {
//        CaveGeneration cavegen;
//        cavegen.render();

        Chunk* chunk = ChunkGenerator::Generate(glm::ivec3(0, 0, 0), 0, ChunkConfig(FillMode::Edges));

        return 0;
    }