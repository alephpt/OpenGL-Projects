#pragma once

#include "chunk.h"
#include "chunkconfig.h"

class ChunkGenerator{
	public: 
		ChunkGenerator();
		~ChunkGenerator();

		int howSmooth, scalar;
		float noiseThreshold, fillCutOff;
		static Chunk* Generate(glm::ivec3, int, ChunkConfig);	// Generates a chunk of data

	private:
		Chunk* chunkData;

		int size;
		std::vector<float> new_vertices;
		std::vector<std::vector<float>> vertices;

		void NoiseGeneration();									// Generates grid of random numbers
		void Smoother();										// Cellular Automata
		float Cellular(int, int, int);							// Finds the value of neighboring vertices
		void March();											// Creates Mesh Data
		int Cube2Bin(int, int, int); 							// Returns a binary representation of a cube
		int EvaluateVertex(int, int, int);  					// Returns 1 or 0 depending on the cutoff value of a vertex
		void GenVertexData(int, float, float, float); 	        // Creates vertex and index information
		void PopulateVertices();								// Orders vertices and color vectors
		void CalculateNormals();							    // Calculates normal data
};
