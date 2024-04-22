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
        _chunk_t->noiseThreshold = config.noiseThreshold;
        _chunk_t->fillCutOff = config.fillCutOff;
        _chunk_t->chunkData->offset = offset;

        srand(glfwGetTime() * getRandomSeed());

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

 // Creates Vertex and Index listing simultaneously.
void ChunkGenerator::GenVertexData(int bIndex, float _z, float _y, float _x)
    {
        int _idx = 0;
        int _vec_idx = 0;

        while (indexTable[bIndex][_idx] != -1)
            {
                glm::vec3 _vertices;
                Logger::Verbose("Index: %d\n", indexTable[bIndex][_idx]);
                switch (indexTable[bIndex][_idx])
                    {
                        case 0:  // // find the isoverts between 0 and 1
                            _vertices.x = Isovert(_x);
                            _vertices.y = _y;
                            _vertices.z = _z;
                            break;
                        case 1: // // find the isoverts between 1 and 2
                            _vertices.x = _x + 1;
                            _vertices.y = _y;
                            _vertices.z = Isovert(_z);
                            break;
                        case 2: // // find the isoverts between 2 and 3
                            _vertices.x = Isovert(_x);
                            _vertices.y = _y;
                            _vertices.z = _z + 1;
                            break;
                        case 3: // // find the isoverts between 3 and 0 
                            _vertices.x = _x;
                            _vertices.y = _y;
                            _vertices.z = Isovert(_z);
                            break;
                        case 4: // // find the isoverts between 4 and 5
                            _vertices.x = Isovert(_x);
                            _vertices.y = _y + 1;
                            _vertices.z = _z;
                            break;
                        case 5: // // find the isoverts between 5 and 6
                            _vertices.x = _x + 1;
                            _vertices.y = _y + 1;
                            _vertices.z = Isovert(_z);
                            break;
                        case 6: // // find the isoverts between 6 and 7
                            _vertices.x = Isovert(_x);
                            _vertices.y = _y + 1; 
                            _vertices.z = _z + 1;
                            break;
                        case 7: // // find the isoverts between 7 and 4
                            _vertices.x = _x;
                            _vertices.y = _y + 1;
                            _vertices.z = Isovert(_z);
                            break;
                        case 8: // // find the isoverts between 0 and 4
                            _vertices.x = _x;
                            _vertices.y = Isovert(_y);
                            _vertices.z = _z;
                            break;
                        case 9: // // find the isoverts between 1 and 5
                            _vertices.x = _x + 1;
                            _vertices.y = Isovert(_y);
                            _vertices.z = _z;
                            break;
                        case 10: // // find the isoverts between 2 and 6
                            _vertices.x = _x + 1;
                            _vertices.y = Isovert(_y);
                            _vertices.z = _z + 1;
                            break;
                        case 11: // // find the isoverts between 0 and 7
                            _vertices.x = _x;
                            _vertices.y = Isovert(_y);
                            _vertices.z = _z + 1;
                            break;
                    }


                // auto it = std::find(positions.begin(), positions.end(), _vertices);
                // if(it == positions.end())
                //     {
                //         _vec_idx = positions.size();
                //         positions.push_back(_vertices);
                //         indices.push_back(_vec_idx);
                //     }
                // else
                //     { indices.push_back(it - positions.begin()); }

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
        
        for(int z = 0; z < size - 1; z+=2) 
            {
                for(int y = 0; y < size - 1; y+=2)
                    {
                        for(int x = 0; x < size - 1; x+=2)
                            {
                                binaryFun = Cube2Bin(z,y,x); // finds binary value of a cube

                                if(binaryFun != 0 && binaryFun != 255)
                                    { GenVertexData(binaryFun, (float)z, (float)y, (float)x); }
                            }
                      }
            }
    }

 // Populates Color vectors.
void ChunkGenerator::Colorize()
    {
        Logger::Verbose("Populating Vertices:\n");
        Logger::Verbose("Vertices: %d\n", positions.size());

        if (positions.size() == 0)
            { return; }

        // I'm pretty sure we can just make the new_vertices vector the vertices vector and skip this step
        for(int i = 0; i < positions.size(); i++)
            {
                float colorvar = (float)(rand() % 33);

                if( colorvar < 16.5 )
                    { colorvar = colorvar / 100.0f; } 
                else 
                    { colorvar = (colorvar / 100.0f) * 2.0f; }

                colors.push_back({(positions[i].y / size), positions[i].y / (size + 10) * colorvar, colorvar / (positions[i].y + 0.2f)});
            }
    }

 // Finds the coordinates of vertices based on index, and calculates normal directions.
void ChunkGenerator::Normalize()
    {
        Logger::Debug("Normalizing Vertices:\n");
        Logger::Debug("Vertices: %d\n", positions.size());
        // Calculate normals for each triangle and add them to corresponding vertices
        for (int i = 0; i < indices.size(); i += 3) 
            {
                glm::vec3 p1 = positions[indices[i]];
                glm::vec3 p2 = positions[indices[i + 1]];
                glm::vec3 p3 = positions[indices[i + 2]];

                Logger::Debug("P1: %f %f %f\n", p1.x, p1.y, p1.z);
                Logger::Debug("P2: %f %f %f\n", p2.x, p2.y, p2.z);
                Logger::Debug("P3: %f %f %f\n", p3.x, p3.y, p3.z);

                // Calculate the edges of the triangle
                glm::vec3 edge1 = p2 - p1;
                glm::vec3 edge2 = p3 - p1;

                Logger::Debug("Edge1: %f %f %f\n", edge1.x, edge1.y, edge1.z);
                Logger::Debug("Edge2: %f %f %f\n", edge2.x, edge2.y, edge2.z);

                // Calculate the face normal
                glm::vec3 faceNormal = glm::cross(edge1, edge2);
                glm::vec3 normalized = glm::normalize(faceNormal);

                Logger::Debug("FaceNormal: %f %f %f\n", faceNormal.x, faceNormal.y, faceNormal.z);
                Logger::Debug("Normal: %f %f %f\n", normalized.x, normalized.y, normalized.z);

                // Add the face normal to each vertex normal
                normals.push_back(normalized);
                normals.push_back(normalized);
                normals.push_back(normalized);
                Logger::Debug("Normals Size: %d\n", normals.size());
            }
    }

void ChunkGenerator::constructChunk() 
    {
        // Add normals to vertices
        for(int i = 0; i < positions.size(); i++)
            { 
                if (i < colors.size() && i < normals.size())
                    { chunkData->vertices.push_back(
                        {
                            {
                                positions[i].x + chunkData->offset.x, 
                                positions[i].y + chunkData->offset.y,
                                positions[i].z + chunkData->offset.z
                            },
                            colors[i], 
                            normals[i]}
                        );  }
            }
        
        // Add indices to chunk
        chunkData->indices = indices;

        //chunkData->log();
    }