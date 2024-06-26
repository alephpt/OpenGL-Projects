#include "chunkfactory.h"

#include "../../components/marching.h"
#include "../../components/utility/logger.h"

#include <algorithm>
#include <map>
#include <vector>
#include <chrono>
#include <random>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

static unsigned int getRandomSeed()
    {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    }

static inline int index(int z, int y, int x, int size) { return (z * size + y) * size + x; }

// Creates Map Segments
ChunkGenerator::ChunkGenerator(){ chunkData = new Chunk(); }
ChunkGenerator::~ChunkGenerator(){ delete chunkData; }

// Generates weighted noise, averages values and populates vertices based
// on the cutoff during marching cubes indexing.
Chunk* ChunkGenerator::Generate(glm::ivec3 offset, int chunkSize, ChunkConfig config)
    {
        ChunkGenerator *_chunk_t = new ChunkGenerator();
        _chunk_t->size = chunkSize;
        _chunk_t->scalar = config.scalar;
        _chunk_t->howSmooth = config.howSmooth;
        // Scale the X and Y offsets to be between 0 and 2
        float xy_offset = ((float)offset.x + (float)offset.y) / (float)chunkSize;
        float zy_offset = ((float)offset.z + (float)offset.y) / (float)chunkSize;
        _chunk_t->noiseThreshold = config.noiseThreshold + zy_offset;
        _chunk_t->fillCutOff = config.fillCutOff + xy_offset;
        //Logger::Debug("Noise Threshold: %f\n", _chunk_t->noiseThreshold);
        //Logger::Debug("Fill Cut Off: %f\n", _chunk_t->fillCutOff);
        _chunk_t->chunkData->offset = offset;

        srand(glfwGetTime() * getRandomSeed() * (offset.x + offset.y + offset.z));

        _chunk_t->NoiseGeneration();
        
        for (int smooth = 0; smooth < _chunk_t->howSmooth; smooth++)
            { _chunk_t->Automata(); }

        _chunk_t->March();
        _chunk_t->Colorize();
        _chunk_t->Normalize();
        _chunk_t->constructChunk();

        return _chunk_t->chunkData;
    }

// Generates random numbers and assigns weighted values 
// to the source index containers based on the noise threshold.
void ChunkGenerator::NoiseGeneration()
    {
        for(int idx = 0; idx < index(size, size, size, size); idx++) 
            {
                int determinant = (float)(rand() % 100);

                if(determinant >= noiseThreshold)
                    { noise.push_back(determinant); } 
                else if (determinant < noiseThreshold) 
                    { noise.push_back(0); }
            }
    }

// Cellular Automata
void ChunkGenerator::Automata()
    {
        std::vector<float> _temp_verts;

        for(int z = 0; z < size; z++)
            {
                for(int y = 0; y < size; y++)
                    {
                        for(int x = 0; x < size; x++)
                          { _temp_verts.push_back(Cellular(z, y, x)); }
                    }
            }

        noise = _temp_verts;
    }


// takes a set of coordinates, searches their neighboring indices for +/- values
// and creates a ramped average based on the value and the number of neighboring cells.
float ChunkGenerator::Cellular(int z, int y, int x)
   {
        float cellAvg = 0.0f;
        float cellularCount = 0;

        for(int _z = z - 1; _z <= z + 1; _z++)
            {
                for(int _y = y - 1; _y <= y + 1; _y++)
                    {
                        for(int _x = x - 1; _x <= x + 1; _x++)
                            {
                                // if the cell is within the bounds of the grid, then we want to
                                // add the value to the average to be divided by the number of cells
                                if (_x >= 0 && _x < size && 
                                    _y >= 0 && _y < size && 
                                    _z >= 0 && _z < size)
                                    { 
                                        
                                        cellAvg += noise[index(_z, _y, _x, size)]; 
                                        cellularCount++; 
                                    } 
                                else // if the cell is on the edge, add the noise threshold
                                if (x == 0 && _x == -1 || 
                                    y == 0 && _y == -1 || 
                                    z == 0 && _z == -1 || 
                                    _x == size || _y == size || _z == size) 
                                    { cellAvg += noiseThreshold; } 
                            }
                    }
            }

        cellAvg = cellAvg / cellularCount;
        return cellAvg;
   }

// turn my cube into a binary index
int ChunkGenerator::Cube2Bin(int z, int y, int x)
    {
        int _binary_value = 0;
        
        _binary_value |= ((noise[index(z, y, x, size)] < fillCutOff) ? 1 : 0) << 0;              // lower left back
        _binary_value |= ((noise[index(z, y, x + 1, size)] < fillCutOff) ? 1 : 0) << 1;          // lower right back
        _binary_value |= ((noise[index(z + 1, y, x + 1, size)] < fillCutOff) ? 1 : 0) << 2;      // lower right front
        _binary_value |= ((noise[index(z + 1, y, x, size)] < fillCutOff) ? 1 : 0) << 3;          // lower left front
        _binary_value |= ((noise[index(z, y + 1, x, size)] < fillCutOff) ? 1 : 0) << 4;          // upper left back
        _binary_value |= ((noise[index(z, y + 1, x + 1, size)] < fillCutOff) ? 1 : 0) << 5;      // upper right back
        _binary_value |= ((noise[index(z + 1, y + 1, x + 1, size)] < fillCutOff) ? 1 : 0) << 6;  // upper right front
        _binary_value |= ((noise[index(z + 1, y + 1, x, size)] < fillCutOff) ? 1 : 0) << 7;      // upper left front

        return _binary_value;
    }

 // Finds the midpoint between two vertex locations.
static inline int Isovert(float V) { return ((V + (V + 1)) / 2); }

static inline glm::vec3 bIndex2Vertex(int index, float _z, float _y, float _x)
    {
        Logger::Verbose("findVertex() - Index: %d\n", index);
        switch (index)
            {
                case 0:  // // find the isoverts between 0 and 1
                    return {Isovert(_x), _y, _z};
                case 1: // // find the isoverts between 1 and 2
                    return {_x + 1, _y, Isovert(_z)};
                case 2: // // find the isoverts between 2 and 3
                    return {Isovert(_x), _y, _z + 1};
                case 3: // // find the isoverts between 3 and 0
                    return {_x, _y, Isovert(_z)};
                case 4: // // find the isoverts between 4 and 5
                    return {Isovert(_x), _y + 1, _z};
                case 5: // // find the isoverts between 5 and 6
                    return {_x + 1, _y + 1, Isovert(_z)};
                case 6: // // find the isoverts between 6 and 7
                    return {Isovert(_x), _y + 1, _z + 1};
                case 7: // // find the isoverts between 7 and 4
                    return {_x, _y + 1, Isovert(_z)};
                case 8: // // find the isoverts between 0 and 4
                    return {_x, Isovert(_y), _z};
                case 9: // // find the isoverts between 1 and 5
                    return {_x + 1, Isovert(_y), _z};
                case 10: // // find the isoverts between 2 and 6
                    return {_x + 1, Isovert(_y), _z + 1};
                case 11: // // find the isoverts between 0 and 7
                    return {_x, Isovert(_y), _z + 1};
                default:
                    Logger::Error("findVertex() - Index: %d\n", index);
                    return {-1, -1, -1};
            }
    }

 // Creates Vertex and Index listing simultaneously.
void ChunkGenerator::GenVertexData(int bIndex, float _z, float _y, float _x)
    {
        int _idx = 0;
        int _vec_idx = 0;

        while (indexTable[bIndex][_idx] != -1)
            {
                glm::vec3 _vertices = bIndex2Vertex(indexTable[bIndex][_idx], _z, _y, _x);

                auto it = std::find(positions.begin(), positions.end(), _vertices);
                if(it == positions.end())
                    {
                        _vec_idx = std::distance(positions.begin(), it);
                        positions.push_back(_vertices);
                        indices.push_back(_vec_idx);
                    }
                else
                    { 
                        _vec_idx = std::distance(positions.begin(), it);
                        indices.push_back(_vec_idx);
                    }

                _idx++;
            }
    }

// iterating vertices, creates isovertex positions
// finds the binary value of a cube, and assigns vertexes to indices.
void ChunkGenerator::March()
    {
        int binaryFun = 0;
        
        for(int z = 0; z < size - 1; z++) 
            {
                for(int y = 0; y < size - 1; y++)
                    {
                        for(int x = 0; x < size - 1; x++)
                            {
                                binaryFun = Cube2Bin(z,y,x); // finds binary value of a cube

                                if(binaryFun != 0 && binaryFun != 255)
                                    { GenVertexData(binaryFun, (float)z, (float)y, (float)x); }
                            }
                      }
            }

        // Testing
        // Hijackling March to test our indexTable
        // for (int i = 1; i < 255; i++)
        //     {
        //         const int* _indices = indexTable[i];

        //         int _vec_idx = 0;

        //         while (_indices[_vec_idx] != -1) {
        //             int x = i % 16;
        //             int y = (i / 16) % 16;

        //             glm::vec3 _vertices = bIndex2Vertex(_indices[_vec_idx], 0, y, x);

        //             indices.push_back(positions.size());
        //             positions.push_back(_vertices);
        //             _vec_idx++;
        //         }
        //     }
    }

static inline float clamp(float x, float lower, float upper) { return std::max(lower, std::min(x, upper)); }

 // Populates Color vectors.
void ChunkGenerator::Colorize()
    {
        Logger::Verbose("Populating Vertices:\n");
        Logger::Verbose("Vertices: %d\n", positions.size());

        if (positions.size() == 0)
            { return; }


        for(int i = 0; i < positions.size(); i++) {
            float colorvar = (float)(rand() % 33);

            if (colorvar < 16.5) {
                colorvar = colorvar / 100.0f;
            } else {
                colorvar = (colorvar / 100.0f) * 2.0f;
            }

            // Pretty cloud colors
            float red = 0.2f;
            float green = red + 0.5f * (positions[i].y / (size + 10) * colorvar); // y gradient with randomness
            float blue = 0.7f * (colorvar / (positions[i].y + 0.2f)); // z gradient with randomness


            colors.push_back({red, clamp(green, 0.0f, 1.0f), clamp(blue, 0.0f, 1.0f)});
        }
    }


const float epsilon = 0.0001f;

 // Finds the coordinates of vertices based on index, and calculates normal directions.
void ChunkGenerator::Normalize()
    {
        Logger::Debug("Normalizing Vertices:\n");
        Logger::Debug("Vertices: %d\n", positions.size());
        
        for (int i = 0; i < positions.size(); i+=3)
            {
                glm::vec3 v1 = positions[i];
                glm::vec3 v2 = positions[i + 1];
                glm::vec3 v3 = positions[i + 2];

                // make sure we never end up with -nan
                if (glm::length(glm::cross(v2 - v1, v3 - v1)) < epsilon)
                    { 
                        normals.push_back({0.0f, 0.0f, 0.0f});
                        normals.push_back({0.0f, 0.0f, 0.0f});
                        normals.push_back({0.0f, 0.0f, 0.0f});    
                        continue; 
                    } // skip degenerate triangles (zero area)

                glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

                if (glm::dot(normal, v1) < 0)
                    { normal = -normal; }

                normals.push_back(normal);
                normals.push_back(normal);
                normals.push_back(normal);
            }
    }

void ChunkGenerator::constructChunk() 
    {
        // Add normals to vertices
        for(int i = 0; i < positions.size(); i++)
            { 
                chunkData->vertices.push_back(
                    { 
                        {
                            positions[i].x + chunkData->offset.x, 
                            positions[i].y + chunkData->offset.y,
                            positions[i].z + chunkData->offset.z
                        },
                        colors[i], 
                        normals[i]}
                    );
            }
        
        // Add indices to chunk
        chunkData->indices = indices;

        //chunkData->log();
    }