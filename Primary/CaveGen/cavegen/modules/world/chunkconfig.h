#pragma once

#include "atomic.h"

struct ChunkConfig {
    float noiseThreshold;
    float fillCutOff;
    int scalar = 1;
    int howSmooth = 5;

    ChunkConfig(FillMode fill) 
        {
            switch (fill) 
                {
                    //Solid, Edges, Tunnels, Cells, Custom 
                    case FillMode::Solid:
                        noiseThreshold = 72.44f;
                        fillCutOff = 27.37f;
                        break;
                    case FillMode::Edges:
                        noiseThreshold = 21.29f;
                        fillCutOff = 85.05f;
                        break;
                    case FillMode::Tunnels:
                        noiseThreshold = 77.18f;
                        fillCutOff = 17.69f;
                        break;
                    case FillMode::Cells:
                        noiseThreshold = 81.36f;
                        fillCutOff = 13.31f;
                        break;
                    case FillMode::Custom:
                        break;
                }
        }
};
