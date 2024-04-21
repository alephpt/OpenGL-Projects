#include "chunkfactory.h"

#include "../../components/marching.h"

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
            { _chunk_t->Smoother(); }
        
        _chunk_t->March(offset);
        _chunk_t->PopulateVertices(offset);
        _chunk_t->CalculateNormals();

        return _chunk_t->chunkData;
    }

// Generates random numbers and assigns weighted values 
// to the source index containers based on the noise threshold.
void ChunkGenerator::NoiseGeneration()
    {
        for(int z = 0; z < size; z++)
            {
                for(int y = 0; y < size; y++)
                    {
                        for(int x = 0; x < size; x++)
                            {
                                int determinant = (float)(rand() % 100);

                                if(determinant >= noiseThreshold)
                                    { new_vertices.push_back(determinant); } 
                                else if (determinant < noiseThreshold) 
                                    { new_vertices.push_back(0); }
                            } 
                    } 
            }
    }

// Passes grid coordinates to a comparitor to populate the destination index list.
void ChunkGenerator::Smoother()
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

        new_vertices = _temp_verts;
    }

static inline int index(int z, int y, int x, int size) { return (z * size + y) * size + x; }

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
                                        
                                        cellAvg += new_vertices[index(_z, _y, _x, size)]; 
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

// iterating vertices, creates isovertex positions
// finds the binary value of a cube, and assigns vertexes to indices.
void ChunkGenerator::March(glm::ivec3 &offset)
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

// turn my cube into a binary index
int ChunkGenerator::Cube2Bin(int z, int y, int x)
    {
        int _binary_value = 0;
        
        _binary_value |= (new_vertices[index(z, y, x, size)] < (fillCutOff ? 1 : 0) << 0);              // lower left back
        _binary_value |= (new_vertices[index(z, y, x + 1, size)] < (fillCutOff ? 1 : 0) << 1);          // lower right back
        _binary_value |= (new_vertices[index(z + 1, y, x + 1, size)] < (fillCutOff ? 1 : 0) << 2);      // lower right front
        _binary_value |= (new_vertices[index(z + 1, y, x, size)] < (fillCutOff ? 1 : 0) << 3);          // lower left front
        _binary_value |= (new_vertices[index(z, y + 1, x, size)] < (fillCutOff ? 1 : 0) << 4);          // upper left back
        _binary_value |= (new_vertices[index(z, y + 1, x + 1, size)] < (fillCutOff ? 1 : 0) << 5);      // upper right back
        _binary_value |= (new_vertices[index(z + 1, y + 1, x + 1, size)] < (fillCutOff ? 1 : 0) << 6);  // upper right front
        _binary_value |= (new_vertices[index(z + 1, y + 1, x, size)] < (fillCutOff ? 1 : 0) << 7);      // upper left front

        return _binary_value;
    }

 // Finds the midpoint between two vertex locations.
static inline int Isovert(float V) { return ((V + (V + 1)) / 2); }

 // Creates Vertex and Index listing simultaneously.
void ChunkGenerator::GenVertexData(int bIndex, float locZ, float locY, float locX)
    {
        int _idx = 0;
        int _vec_idx = 0;
        std::vector<float> _vertices;

        while (indexTable[bIndex][_idx] != -1)
            {
                switch (indexTable[bIndex][_idx])
                    {
                        case 0:  // // find the isoverts between 0 and 1
                            _vertices.push_back(Isovert(locX));
                            _vertices.push_back(locY);
                            _vertices.push_back(locZ);
                            break;
                        case 1: // // find the isoverts between 1 and 2
                            _vertices.push_back(locX + 1);
                            _vertices.push_back(locY);
                            _vertices.push_back(Isovert(locZ));
                            break;
                        case 2: // // find the isoverts between 2 and 3
                            _vertices.push_back(Isovert(locX));
                            _vertices.push_back(locY);
                            _vertices.push_back(locZ + 1);
                            break;
                        case 3: // // find the isoverts between 3 and 0 
                            _vertices.push_back(locX);
                            _vertices.push_back(locY);
                            _vertices.push_back(Isovert(locZ));
                            break;
                        case 4: // // find the isoverts between 4 and 5
                            _vertices.push_back(Isovert(locX));
                            _vertices.push_back(locY + 1);
                            _vertices.push_back(locZ);
                            break;
                        case 5: // // find the isoverts between 5 and 6
                            _vertices.push_back(locX + 1);
                            _vertices.push_back(locY + 1);
                            _vertices.push_back(Isovert(locZ));
                            break;
                        case 6: // // find the isoverts between 6 and 7
                            _vertices.push_back(Isovert(locX));
                            _vertices.push_back(locY + 1); 
                            _vertices.push_back(locZ + 1);
                            break;
                        case 7: // // find the isoverts between 7 and 4
                            _vertices.push_back(locX);
                            _vertices.push_back(locY + 1);
                            _vertices.push_back(Isovert(locZ));
                            break;
                        case 8: // // find the isoverts between 0 and 4
                            _vertices.push_back(locX);
                            _vertices.push_back(Isovert(locY));
                            _vertices.push_back(locZ);
                            break;
                        case 9: // // find the isoverts between 1 and 5
                            _vertices.push_back(locX + 1);
                            _vertices.push_back(Isovert(locY));
                            _vertices.push_back(locZ);
                            break;
                        case 10: // // find the isoverts between 2 and 6
                            _vertices.push_back(locX + 1);
                            _vertices.push_back(Isovert(locY));
                            _vertices.push_back(locZ + 1);
                            break;
                        case 11: // // find the isoverts between 0 and 7
                            _vertices.push_back(locX);
                            _vertices.push_back(Isovert(locY));
                            _vertices.push_back(locZ + 1);
                            break;
                    }

                auto it = std::find(vertices.begin(), vertices.end(), _vertices);
                if(it == vertices.end())
                    {
                        _vec_idx = vertices.size();
                        vertices.push_back(_vertices);
                        chunkData->indices.push_back(_vec_idx);
                    }
                else
                    { chunkData->indices.push_back(it - vertices.begin()); }

                _vertices.clear();
                _idx++;
            }
    }

 // Populates Vertex Coords and Color vectors.
void ChunkGenerator::PopulateVertices(glm::ivec3 &offset)
    {
        printf("Populating Vertices\n");
        chunkData->vertices.clear();
        chunkData->colors.clear();

        if (vertices.size() == 0)
            { return; }

        // I'm pretty sure we can just make the new_vertices vector the vertices vector and skip this step
        for(int i = 0; i < vertices.size(); i++)
            {
                if (vertices[i].size() == 0)
                    { continue; }

                float colorvar = (float)(rand() % 33);

                if( colorvar < 16.5 )
                    { colorvar = colorvar / 100.0f; } 
                else 
                    { colorvar = (colorvar / 100.0f) * 2.0f; }

                chunkData->vertices.push_back(vertices[i][0] - offset.x);
                chunkData->vertices.push_back(vertices[i][1] - offset.y);
                chunkData->vertices.push_back(vertices[i][2] - offset.z);

                chunkData->colors.push_back(vertices[i][1] / size);
                chunkData->colors.push_back(vertices[i][1] / (size + 10) * colorvar);
                chunkData->colors.push_back(colorvar / (vertices[i][1] + 0.2f));
            }
    }

 // Finds the coordinates of vertices based on index, and calculates normal directions.
void ChunkGenerator::CalculateNormals()
    {
        printf("Calculating Normals\n");
        chunkData->normals.clear();

        if (vertices.size() == 0 || chunkData->indices.size() == 0)
            { return; }

        // Initialize normals for each vertex to zero
        std::vector<glm::vec3> normals(vertices.size(), glm::vec3(0.0f));

        // Calculate normals for each triangle and add them to corresponding vertices
        for (int i = 0; i < chunkData->indices.size(); i += 3) 
            {
                if (i + 2 >= chunkData->indices.size()) // Prevent out of bounds if the number of indices is not a multiple of 3
                    { break; }

                // Retrieve vertices for the current triangle
                int index1 = chunkData->indices[i];
                int index2 = chunkData->indices[i + 1];
                int index3 = chunkData->indices[i + 2];

                // Check if indices are within bounds
                if (index1 < 0 || index1 >= vertices.size() ||
                    index2 < 0 || index2 >= vertices.size() ||
                    index3 < 0 || index3 >= vertices.size()) 
                    {
                        printf("Yo!! Index is out of bounds!");
                        printf("We have a problem: %d, %d, %d\n", index1, index2, index3);
                        printf("Indices: %d\n", chunkData->indices.size());
                        printf("Vertices: %d\n", vertices.size());
                        continue; // Skip if any index is out of bounds
                    }

                // Retrieve vertices for the current triangle
                const std::vector<float>& v0 = vertices[index1];
                const std::vector<float>& v1 = vertices[index2];
                const std::vector<float>& v2 = vertices[index3];

                glm::vec3 normal = glm::cross(glm::vec3(v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2]), glm::vec3(v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2]));

                normals[index1] += normal;
                normals[index2] += normal;
                normals[index3] += normal;
            }

        // Normalize the normals
        for (int i = 0; i < normals.size(); ++i) 
            {
                glm::vec3 normal = glm::normalize(normals[i]);
                chunkData->normals.push_back(normal.x);
                chunkData->normals.push_back(normal.y);
                chunkData->normals.push_back(normal.z);
            }
    }