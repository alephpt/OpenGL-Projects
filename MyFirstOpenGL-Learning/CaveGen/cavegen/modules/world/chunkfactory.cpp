#include "chunkfactory.h"

#include "../../components/marching.h"

#include <algorithm>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


// Creates Map Segments
ChunkGenerator::ChunkGenerator(){ chunkData = new Chunk(); }
ChunkGenerator::~ChunkGenerator(){ delete chunkData; }

// Generates weighted noise, averages values and populates vertices based
// on the cutoff during marching cubes indexing.
Chunk* ChunkGenerator::Generate(glm::ivec3 offset, int chunkSize, ChunkConfig config)
    {
        ChunkGenerator *chunk = new ChunkGenerator();
        chunk->scalar = config.scalar;
        chunk->howSmooth = config.howSmooth;
        chunk->noiseThreshold = config.noiseThreshold;
        chunk->fillCutOff = config.fillCutOff;
        chunk->chunkData->offset = offset;

        srand(glfwGetTime());
        
        //printf("Generating Noise\n");
        chunk->NoiseGeneration();
        
        //printf("Smoothing Noise\n");
        for (int smooth = 0; smooth < chunk->howSmooth; smooth++)
            { chunk->Smoother(); }
        
        //printf("Marching Cubes\n");
        chunk->March();
        
        printf("Generated Chunk:\n");
        chunk->chunkData->log();
        return chunk->chunkData;
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
        std::vector<float> tempVerts;

        for(int z = 0; z < size; z++)
            {
                for(int y = 0; y < size; y++)
                    {
                        for(int x = 0; x < size; x++)
                          { tempVerts.push_back(Cellular(z, y, x)); }
                    }
            }

        new_vertices = tempVerts;
    }

// takes a set of coordinates, searches their neighboring indices for +/- values
// and creates a ramped average based on the value and the number of neighboring cells.
float ChunkGenerator::Cellular(int currentZ, int currentY, int currentX)
   {
        float cellAvg = 0.0f;
        float cellularCount = 0;

        for(int rangerZ = currentZ - 1; rangerZ <= currentZ + 1; rangerZ++)
            {
                for(int rangerY = currentY - 1; rangerY <= currentY + 1; rangerY++)
                    {
                        for(int rangerX = currentX - 1; rangerX <= currentX + 1; rangerX++)
                            {
                                int indexer = (rangerZ * size + rangerY) * size + rangerX;

                                if(rangerX >= 0 && rangerX < size && rangerY >= 0 && rangerY < size && rangerZ >= 0 && rangerZ < size)
                                    { cellAvg += new_vertices[indexer]; cellularCount++; } 
                                else if (currentX == 0 && rangerX == -1 || currentY == 0 && rangerY == -1 || currentZ == 0 && rangerZ == -1 || 
                                        rangerX == size || rangerY == size || rangerZ == size) 
                                    { cellAvg += noiseThreshold; }
                            }
                    }
            }

        cellAvg = cellAvg / cellularCount;
        return cellAvg;
   }

// iterating vertices, creates isovertex positions
// finds the binary value of a cube, and assigns vertexes to indices.
void ChunkGenerator::March()
    {
        int binaryFun = 0;
        chunkData->indices.clear();
        std::vector<std::vector<float>> *vertexData = new std::vector<std::vector<float>>;

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

        PopulateVertices();
        CalculateNormals();
    }

static inline int index(int z, int y, int x, int size) { return (z * size + y) * size + x; }

// turn my cube into a binary index
int ChunkGenerator::Cube2Bin(int locZ, int locY, int locX)
    {
        int binaryVal = 0;
        binaryVal += new_vertices[index(locZ, locY, locX, size)] < fillCutOff ? 1 : 0;
        binaryVal += (2 * new_vertices[index(locZ, locY, locX + 1, size)] < fillCutOff ? 1 : 0);
        binaryVal += (4 * new_vertices[index(locZ, locY + 1, locX + 1, size)] < fillCutOff ? 1 : 0);
        binaryVal += (8 * new_vertices[index(locZ, locY + 1, locX, size)] < fillCutOff ? 1 : 0);
        binaryVal += (16 * new_vertices[index(locZ + 1, locY, locX, size)] < fillCutOff ? 1 : 0);
        binaryVal += (32 * new_vertices[index(locZ + 1, locY, locX + 1, size)] < fillCutOff ? 1 : 0);
        binaryVal += (64 * new_vertices[index(locZ + 1, locY + 1, locX + 1, size)] < fillCutOff ? 1 : 0);
        binaryVal += (128 * new_vertices[index(locZ + 1, locY + 1, locX, size)] < fillCutOff ? 1 : 0);
        return binaryVal;
    }


 // Finds the midpoint between two vertex locations.
static inline int Isovert(float V) { return ((V + (V + 1)) / 2); }

 // Creates Vertex and Index listing simultaneously.
void ChunkGenerator::GenVertexData(int bIndex, float locZ, float locY, float locX)
    {
        std::vector<float> *tempVecs = new std::vector<float>;
        int indexCount = 0;
        int vectorIndex = 0;

        while (indexTable[bIndex][indexCount] != -1)
            {
                switch(indexTable[bIndex][indexCount])
                    {
                        case 0:  // // find the isoverts between 0 and 1
                            tempVecs->push_back(Isovert(locX));
                            tempVecs->push_back(locY);
                            tempVecs->push_back(locZ);
                            break;
                        case 1: // // find the isoverts between 1 and 2
                            tempVecs->push_back(locX + 1);
                            tempVecs->push_back(locY);
                            tempVecs->push_back(Isovert(locZ));
                            break;
                        case 2: // // find the isoverts between 2 and 3
                            tempVecs->push_back(Isovert(locX));
                            tempVecs->push_back(locY);
                            tempVecs->push_back(locZ + 1);
                            break;
                        case 3: // // find the isoverts between 3 and 0 
                            tempVecs->push_back(locX);
                            tempVecs->push_back(locY);
                            tempVecs->push_back(Isovert(locZ));
                            break;
                        case 4: // // find the isoverts between 4 and 5
                            tempVecs->push_back(Isovert(locX));
                            tempVecs->push_back(locY + 1);
                            tempVecs->push_back(locZ);
                            break;
                        case 5: // // find the isoverts between 5 and 6
                            tempVecs->push_back(locX + 1);
                            tempVecs->push_back(locY + 1);
                            tempVecs->push_back(Isovert(locZ));
                            break;
                        case 6: // // find the isoverts between 6 and 7
                            tempVecs->push_back(Isovert(locX));
                            tempVecs->push_back(locY + 1); 
                            tempVecs->push_back(locZ + 1);
                            break;
                        case 7: // // find the isoverts between 7 and 4
                            tempVecs->push_back(locX);
                            tempVecs->push_back(locY + 1);
                            tempVecs->push_back(Isovert(locZ));
                            break;
                        case 8: // // find the isoverts between 0 and 4
                            tempVecs->push_back(locX);
                            tempVecs->push_back(Isovert(locY));
                            tempVecs->push_back(locZ);
                            break;
                        case 9: // // find the isoverts between 1 and 5
                            tempVecs->push_back(locX + 1);
                            tempVecs->push_back(Isovert(locY));
                            tempVecs->push_back(locZ);
                            break;
                        case 10: // // find the isoverts between 2 and 6
                            tempVecs->push_back(locX + 1);
                            tempVecs->push_back(Isovert(locY));
                            tempVecs->push_back(locZ + 1);
                            break;
                        case 11: // // find the isoverts between 0 and 7
                            tempVecs->push_back(locX);
                            tempVecs->push_back(Isovert(locY));
                            tempVecs->push_back(locZ + 1);
                            break;
                    }

                auto vectIt = std::find(vertices.begin(), vertices.end(), *tempVecs);                 // find the calculated vertex
                if (vectIt != vertices.end())                                                           // if it's in the list
                    {                                                    
                      vectorIndex = std::distance(vertices.begin(), vectIt);                                // get the index
                      chunkData->indices.push_back(vectorIndex);                                              // add it to the indices list
                    } 
                else if (vectIt == vertices.end()) 
                    {                                                                                         // if it's not in the list
                      vertices.push_back(*tempVecs);                                                        // add it to the vertex list
                      auto vectIt = std::find(vertices.begin(), vertices.end(), *tempVecs);               // find the vertex
                      vectorIndex = std::distance(vertices.begin(), vectIt);                                // get the index
                      chunkData->indices.push_back(static_cast<unsigned int>(vectorIndex));                   // add it to the indices list
                    }

                tempVecs->clear();
                indexCount++;
            }

        delete tempVecs;
    }

 // Populates Vertex Coords and Color vectors.
void ChunkGenerator::PopulateVertices()
    {
        //printf("Populating Vertices\n");
        chunkData->vertices.clear();

        for(int i = 0; i < vertices.size(); i++)
            {
                float colorvar = (float)(rand() % 33);

                if( colorvar < 16.5 )
                    { colorvar = colorvar / 100.0f; } 
                else 
                    { colorvar = (colorvar / 100.0f) * 2.0f; }

                chunkData->vertices.push_back(vertices[i][0] - chunkData->offset.x);
                chunkData->vertices.push_back(vertices[i][1] - chunkData->offset.y);
                chunkData->vertices.push_back(vertices[i][2] - chunkData->offset.z);

                chunkData->colors.push_back(vertices[i][1] / size);
                chunkData->colors.push_back(vertices[i][1] / (size + 10) * colorvar);
                chunkData->colors.push_back(colorvar / (vertices[i][1] + 0.2f));
            }

        //deduplicateVertices();
    }

 // Finds the coordinates of vertices based on index, and calculates normal directions.
void ChunkGenerator::CalculateNormals()
    {
        //printf("Calculating Normals\n");
        chunkData->normals.clear();

        // Initialize normals for each vertex to zero
        std::vector<glm::vec3> normals(vertices.size(), glm::vec3(0.0f));

        // Calculate normals for each triangle and add them to corresponding vertices
        for (int i = 0; i < chunkData->indices.size(); i += 3) 
            {
                std::vector<float> p1 = vertices[chunkData->indices[i]];
                std::vector<float> p2 = vertices[chunkData->indices[i + 1]];
                std::vector<float> p3 = vertices[chunkData->indices[i + 2]];

                glm::vec3 v1 = glm::vec3(p1[0], p1[1], p1[2]);
                glm::vec3 v2 = glm::vec3(p2[0], p2[1], p2[2]);
                glm::vec3 v3 = glm::vec3(p3[0], p3[1], p3[2]);

                glm::vec3 edge1 = v2 - v1;
                glm::vec3 edge2 = v3 - v1;

                // Calculate the face normal
                glm::vec3 faceNormal = glm::cross(edge1, edge2);

                // Add the face normal to each vertex of the triangle
                normals[chunkData->indices[i]] += faceNormal;
                normals[chunkData->indices[i + 1]] += faceNormal;
                normals[chunkData->indices[i + 2]] += faceNormal;
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
/*
void ChunkGenerator::deduplicateVertices()
    {
        std::vector<float> newVertices;
        std::vector<float> newColors;

        std::map<std::vector<float>, unsigned int> vertexMap;
        std::map<std::vector<float>, unsigned int> colorMap;

        for (int i = 0; i < chunkData->vertices.size(); i += 3) 
            {
                std::vector<float> vertex = {chunkData->vertices[i], chunkData->vertices[i + 1], chunkData->vertices[i + 2]};
                std::vector<float> color = {chunkData->colors[i], chunkData->colors[i + 1], chunkData->colors[i + 2]};

                if (vertexMap.find(vertex) == vertexMap.end()) 
                    {
                      vertexMap[vertex] = newVertices.size() / 3;
                      colorMap[vertex] = newColors.size() / 3;
                      newVertices.insert(newVertices.end(), vertex.begin(), vertex.end());
                      newColors.insert(newColors.end(), color.begin(), color.end());
                    }
            }

        chunkData->vertices = newVertices;
        chunkData->colors = newColors;
    }
    */