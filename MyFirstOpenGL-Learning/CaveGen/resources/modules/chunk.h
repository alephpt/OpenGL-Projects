#pragma once

#include "atomic.h"
#include "chunkdata.h"

class Chunk{
public: 
 	Chunk();
    ~Chunk();

	int mapChunkSize = 0;
	int howSmooth, scalar;
	float noiseThreshold, fillCutOff;
    static ChunkData* Generate(glm::ivec3, int, float, float, int, int);					// Generates a chunk of data
	
private:
    ChunkData* chunkData;

	static constexpr int length = 30; 
	static constexpr int height = 30;
	static constexpr int width = 30;
	void NoiseGeneration(std::vector<float>*);													// Generates grid of random numbers
	void Smoother(std::vector<float>*);															// Cellular Automata
	float Cellular(int, int, int, std::vector<float>&);											// Finds the value of neighboring vertices
	void March(std::vector<float>&, glm::ivec3&);									            // Creates Mesh Data
	int Cube2Bin(int, int, int, std::vector<float>&); 											// Returns a binary representation of a cube
	int EvaluateVertex(int, int, int, std::vector<float>&);  									// Returns 1 or 0 depending on the cutoff value of a vertex
	float Isovert(float); 																		// Returns the difference between two vertex locations
	void GenVertexData(int, float, float, float, std::vector<std::vector<float>>&); 	        // Creates vertex and index information
	void PopulateVertices(std::vector<std::vector<float>>&, glm::ivec3&);						// Orders vertices and color vectors
	void CalculateNormals(std::vector<std::vector<float>>&);							        // Calculates normal data
	void deduplicateVertices();														            // Removes duplicate vertices
};
