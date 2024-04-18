#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <set>

struct MapData{
	std::vector<float> vertices;
	std::vector<float> colors;
	std::vector<float> normals;
	std::vector<unsigned int> indices;
	glm::ivec3 offset;
};

struct Vec3Compare {
    bool operator()(const glm::ivec3& lhs, const glm::ivec3& rhs) const {
        if (lhs.x != rhs.x) return lhs.x < rhs.x;
        if (lhs.y != rhs.y) return lhs.y < rhs.y;
        return lhs.z < rhs.z;
    }
};

class Chunk{
public: 
 	Chunk();
	void CleanUp(MapData*);
	int mapChunkSize = 30;
	int howSmooth, scalar;
	float noiseThreshold, fillCutOff;
    MapData MapGeneration(MapData*, glm::ivec3);
	
private:
	static constexpr int length = 30; 
	static constexpr int height = 30;
	static constexpr int width = 30;
	void NoiseGeneration(std::vector<float>*);													// Generates grid of random numbers
	void Smoother(std::vector<float>*);															// Cellular Automata
	float Cellular(int, int, int, std::vector<float>&);											// Finds the value of neighboring vertices
	void March(std::vector<float>&, glm::ivec3&, MapData*);													// Creates Mesh Data
	int Cube2Bin(int, int, int, std::vector<float>&); 											// Returns a binary representation of a cube
	int EvaluateVertex(int, int, int, std::vector<float>&);  									// Returns 1 or 0 depending on the cutoff value of a vertex
	float Isovert(float); 																		// Returns the difference between two vertex locations
	void GenVertexData(int, float, float, float, std::vector<std::vector<float>>&, MapData*); 	// Creates vertex and index information
	void PopulateVertices(std::vector<std::vector<float>>&, glm::ivec3&, MapData*);							// Orders vertices and color vectors
	void CalculateNormals(std::vector<std::vector<float>>&,  MapData*);							// Calculates normal data
};



class World{
public:
	Chunk MapGen;
	World();
	MapData ChunkData;
	void UpdateChunks(glm::vec3&);
	glm::ivec3 currentChunk, lastChunk;
	int chunkSize, visibleChunks;
	std::map<glm::ivec3, MapData, Vec3Compare> TotalMap;
	std::map<glm::ivec3, MapData, Vec3Compare> MapTable;
	std::set<glm::ivec3, Vec3Compare> created_chunks;
	std::set<glm::ivec3, Vec3Compare> visible_chunks;


private: 
	const int maxFOV = 60;
};

