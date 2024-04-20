#include "chunkfactory.h"

#include "../../components/marching.h"

#include <algorithm>
#include <map>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Creates Map Segments
ChunkGenerator::ChunkGenerator(){ chunkData = new Chunk; }
ChunkGenerator::~ChunkGenerator(){ delete chunkData; }

// Generates weighted noise, averages values and populates vertices based
// on the cutoff during marching cubes indexing.
Chunk* ChunkGenerator::Generate(glm::ivec3 offset, int chunkSize, ChunkConfig config)
    {
        ChunkGenerator *chunk = new ChunkGenerator();
        chunk->mapChunkSize = chunkSize;
        chunk->scalar = config.scalar;
        chunk->howSmooth = config.howSmooth;
        chunk->noiseThreshold = config.noiseThreshold;
        chunk->fillCutOff = config.fillCutOff;
        chunk->chunkData->offset = offset;

        std::vector<float> *newVerts = new std::vector<float>;
        srand(glfwGetTime());
        
        //printf("Generating Noise\n");
        chunk->NoiseGeneration(newVerts);
        
        //printf("Smoothing Noise\n");
        for (int smooth = 0; smooth < chunk->howSmooth; smooth++)
            { chunk->Smoother(newVerts); }
        
        //printf("Marching Cubes\n");
        chunk->March(*newVerts, offset);
        delete newVerts;
        
        // log chunk generation values`
        //printf("\tChunk Offset: %d, %d, %d\n", offset.x, offset.y, offset.z);
        //printf("\tVertices: %d\n", chunkData->vertices.size());
        //printf("\tColors: %d\n", chunkData->colors.size());
        //printf("\tNormals: %d\n", chunkData->normals.size());
        //printf("\tIndices: %d\n", chunkData->indices.size());
        
        return chunk->chunkData;
    }

// Generates random numbers and assigns weighted values 
// to the source index containers based on the noise threshold.
void ChunkGenerator::NoiseGeneration(std::vector<float> *newVerts)
    {
        for(int z = 0; z < length; z++)
            {
                for(int y = 0; y < height; y++)
                    {
                        for(int x = 0; x < width; x++)
                            {
                                int determinant = (float)(rand() % 100);

                                if(determinant >= noiseThreshold)
                                    { newVerts->push_back(determinant); } 
                                else if (determinant < noiseThreshold) 
                                    { newVerts->push_back(0); }
                            } 
                    } 
            }
    }

// Passes grid coordinates to a comparitor to populate the destination index list.
void ChunkGenerator::Smoother(std::vector<float> *newVerts)
    {
        std::vector<float> *tempVerts = new std::vector<float>;

        for(int z = 0; z < length; z++)
            {
                for(int y = 0; y < height; y++)
                    {
                        for(int x = 0; x < width; x++)
                          { tempVerts->push_back(Cellular(z, y, x, *newVerts)); }
                    }
            }

        *newVerts = *tempVerts;
        delete tempVerts;
    }

// takes a set of coordinates, searches their neighboring indices for +/- values
// and creates a ramped average based on the value and the number of neighboring cells.
float ChunkGenerator::Cellular(int currentZ, int currentY, int currentX, std::vector<float> &newVerts)
   {
        float cellAvg = 0.0f;
        float cellularCount = 0;

        for(int rangerZ = currentZ - 1; rangerZ <= currentZ + 1; rangerZ++)
            {
                for(int rangerY = currentY - 1; rangerY <= currentY + 1; rangerY++)
                    {
                        for(int rangerX = currentX - 1; rangerX <= currentX + 1; rangerX++)
                            {
                                int indexer = (rangerZ * height + rangerY) * width + rangerX;

                                if(rangerX >= 0 && rangerX < width && rangerY >= 0 && rangerY < height && rangerZ >= 0 && rangerZ < length)
                                    { cellAvg += newVerts[indexer]; cellularCount++; } 
                                else if (currentX == 0 && rangerX == -1 || currentY == 0 && rangerY == -1 || currentZ == 0 && rangerZ == -1 || 
                                        rangerX == width || rangerY == height || rangerZ == length) 
                                    { cellAvg += noiseThreshold; }
                            }
                    }
            }

        cellAvg = cellAvg / cellularCount;
        return cellAvg;
   }

// iterating vertices, creates isovertex positions
// finds the binary value of a cube, and assigns vertexes to indices.
void ChunkGenerator::March(std::vector<float> &newVerts, glm::ivec3 &offset)
    {
        int binaryFun = 0;
        chunkData->indices.clear();
        std::vector<std::vector<float>> *vertexData = new std::vector<std::vector<float>>;

        for(int z = 0; z < length - 1; z+=2)
            {
                for(int y = 0; y < height - 1; y+=2)
                    {
                        for(int x = 0; x < width - 1; x+=2)
                            {
                                binaryFun = Cube2Bin(z,y,x, newVerts); // finds binary value of a cube

                                if(binaryFun != 0 && binaryFun != 255)
                                    { GenVertexData(binaryFun, (float)z, (float)y, (float)x, *vertexData); }
                            }
                      }
            }

        PopulateVertices(*vertexData, offset);
        CalculateNormals(*vertexData);
        delete vertexData;
    }

// turn my cube into a binary index
int ChunkGenerator::Cube2Bin(int locZ, int locY, int locX, std::vector<float> &newVerts)
    {
        int binaryVal = 0;
        binaryVal += (1 * EvaluateVertex(locZ, locY, locX, newVerts));
        binaryVal += (2 * EvaluateVertex(locZ, locY, locX + 1, newVerts));
        binaryVal += (4 * EvaluateVertex(locZ + 1, locY, locX + 1, newVerts));
        binaryVal += (8 * EvaluateVertex(locZ + 1, locY, locX, newVerts));
        binaryVal += (16 * EvaluateVertex(locZ, locY + 1, locX, newVerts));
        binaryVal += (32 * EvaluateVertex(locZ, locY + 1, locX + 1, newVerts));
        binaryVal += (64 * EvaluateVertex(locZ + 1, locY + 1, locX + 1, newVerts));
        binaryVal += (128 * EvaluateVertex(locZ + 1, locY + 1, locX, newVerts));
        return binaryVal;
    }

 // Returns a 1 or 0 value for valid coordinates
int ChunkGenerator::EvaluateVertex(int z, int y, int x, std::vector<float> &newVerts)
    {
        int indexer = (z * height + y) * width + x;

        if (newVerts[indexer] < fillCutOff)
            { return 1; } 
        else 
            { return 0; }
    }

 // Creates Vertex and Index listing simultaneously.
void ChunkGenerator::GenVertexData(int bIndex, float locZ, float locY, float locX, std::vector<std::vector<float>> &vertexData)
    {
        std::vector<float> *tempVecs = new std::vector<float>;
        int indexCount = 0;
        int vectorIndex = 0;
        const int* bIndexTable = indexTable[bIndex];

        while (bIndexTable[indexCount++])
            {
                const int index = bIndexTable[indexCount - 1];
                switch (index)
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

                auto it = std::find(vertexData.begin(), vertexData.end(), *tempVecs);
                if(it == vertexData.end())
                    {
                        vectorIndex = vertexData.size();
                        vertexData.push_back(*tempVecs);
                        chunkData->indices.push_back(vectorIndex);
                    }
                else
                    { chunkData->indices.push_back(it - vertexData.begin()); }

                tempVecs->clear();
            }

        delete tempVecs;
    }

 // Finds the midpoint between two vertex locations.
float ChunkGenerator::Isovert(float V)
    { return ((V + (V + 1)) / 2); }

 // Populates Vertex Coords and Color vectors.
void ChunkGenerator::PopulateVertices(std::vector<std::vector<float>> &vertexData, glm::ivec3 &offset)
    {
        printf("Populating Vertices\n");
        chunkData->vertices.clear();
        chunkData->colors.clear();

        if (vertexData.size() == 0)
            { return; }

        for(int i = 0; i < vertexData.size(); i++)
            {
                if (vertexData[i].size() == 0)
                    { continue; }

                float colorvar = (float)(rand() % 33);

                if( colorvar < 16.5 )
                    { colorvar = colorvar / 100.0f; } 
                else 
                    { colorvar = (colorvar / 100.0f) * 2.0f; }

                chunkData->vertices.push_back(vertexData[i][0] - offset.x);
                chunkData->vertices.push_back(vertexData[i][1] - offset.y);
                chunkData->vertices.push_back(vertexData[i][2] - offset.z);

                chunkData->colors.push_back(vertexData[i][1] / height);
                chunkData->colors.push_back(vertexData[i][1] / (height + 10) * colorvar);
                chunkData->colors.push_back(colorvar / (vertexData[i][1] + 0.2f));
            }

        //deduplicateVertices();
    }

 // Finds the coordinates of vertices based on index, and calculates normal directions.
void ChunkGenerator::CalculateNormals(std::vector<std::vector<float>> &vertexData)
    {
        printf("Calculating Normals\n");
        chunkData->normals.clear();

        if (vertexData.size() == 0 || chunkData->indices.size() == 0)
            { return; }

        // Initialize normals for each vertex to zero
        std::vector<glm::vec3> normals(vertexData.size(), glm::vec3(0.0f));

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
                if (index1 < 0 || index1 >= vertexData.size() ||
                    index2 < 0 || index2 >= vertexData.size() ||
                    index3 < 0 || index3 >= vertexData.size()) 
                    {
                        printf("Yo!! Index is out of bounds!");
                        printf("We have a problem: %d, %d, %d\n", index1, index2, index3);
                        printf("Indices: %d\n", chunkData->indices.size());
                        printf("Vertices: %d\n", vertexData.size());
                        continue; // Skip if any index is out of bounds
                    }

                // Retrieve vertices for the current triangle
                const std::vector<float>& v0 = vertexData[index1];
                const std::vector<float>& v1 = vertexData[index2];
                const std::vector<float>& v2 = vertexData[index3];

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
