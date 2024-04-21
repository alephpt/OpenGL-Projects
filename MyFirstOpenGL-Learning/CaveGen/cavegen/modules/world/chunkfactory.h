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
		std::vector<float> noise;
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> colors;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		void NoiseGeneration();									// Generates grid of random numbers
		void Automata();										// Cellular Automata
		float Cellular(int, int, int);							// Finds the value of neighboring vertices
		void March();											// Creates Mesh Data
		int Cube2Bin(int, int, int); 							// Returns a binary representation of a cube
		int EvaluateVertex(int, int, int);  					// Returns 1 or 0 depending on the cutoff value of a vertex
		void GenVertexData(int, float, float, float); 	        // Creates vertex and index information
		void Colorize();										// Orders vertices and color vectors
		void Normalize();							    		// Calculates normal data
		void constructChunk();									// Constructs the chunk data with the generated data
};
