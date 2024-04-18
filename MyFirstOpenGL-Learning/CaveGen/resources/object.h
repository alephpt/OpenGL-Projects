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

	// to JSON
	std::string Serialize() const;
	static MapData Deserialize(const std::string& json);
};

// Implement Serialize and Deserialize for MapData
inline std::string MapData::Serialize() const {
	std::stringstream ss;
	ss << "{";
	ss << "\"vertices\": [";
	for (size_t i = 0; i < vertices.size(); i++) {
		ss << vertices[i];
		if (i < vertices.size() - 1) {
			ss << ",";
		}
	}
	ss << "],";
	ss << "\"colors\": [";
	for (size_t i = 0; i < colors.size(); i++) {
		ss << colors[i];
		if (i < colors.size() - 1) {
			ss << ",";
		}
	}
	ss << "],";
	ss << "\"normals\": [";
	for (size_t i = 0; i < normals.size(); i++) {
		ss << normals[i];
		if (i < normals.size() - 1) {
			ss << ",";
		}
	}
	ss << "],";
	ss << "\"indices\": [";
	for (size_t i = 0; i < indices.size(); i++) {
		ss << indices[i];
		if (i < indices.size() - 1) {
			ss << ",";
		}
	}
	ss << "],";
	ss << "\"offset\": [" << offset.x << "," << offset.y << "," << offset.z << "]";
	ss << "}";
	return ss.str();
}

inline MapData MapData::Deserialize(const std::string& json) {
    MapData data;
    std::stringstream ss(json);
    std::string token;

    // Expecting '{'
    if (!(ss >> token && token == "{")) 
		{ throw std::invalid_argument("Invalid JSON format: missing opening brace '{'."); 
			printf("Received: %s\n", token.c_str());
		}

    // Parsing "vertices"
    if (!(ss >> token && token == "\"vertices\":")) 
		{ throw std::invalid_argument("Invalid JSON format: missing or incorrect 'vertices' field."); }
    if (!(ss >> token && token == "[")) 
		{ throw std::invalid_argument("Invalid JSON format: missing opening bracket '[' for 'vertices'."); }
    
	while (true) 
		{ 
			ss >> token;
        
			if (token == "]") { break; }

        	try { data.vertices.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
				{ throw std::invalid_argument("Invalid JSON format: invalid floating-point value for 'vertices'."); }
		}

    // Parsing "colors"
	if (!(ss >> token && token == "\"colors\":")) 
		{ throw std::invalid_argument("Invalid JSON format: missing or incorrect 'colors' field."); }
	if (!(ss >> token && token == "[")) 
		{ throw std::invalid_argument("Invalid JSON format: missing opening bracket '[' for 'colors'."); }
	
	while (true) 
		{
			ss >> token;
	
			if (token == "]") { break; }

			try { data.colors.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
				{ throw std::invalid_argument("Invalid JSON format: invalid floating-point value for 'colors'."); }
		}

	// Parsing "normals"
	if (!(ss >> token && token == "\"normals\":")) {
		throw std::invalid_argument("Invalid JSON format: missing or incorrect 'normals' field.");
	}
	if (!(ss >> token && token == "[")) {
		throw std::invalid_argument("Invalid JSON format: missing opening bracket '[' for 'normals'.");
	}

	while (true) 
		{
			ss >> token;

			if (token == "]") { break; }
			
			try { data.normals.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
				{ throw std::invalid_argument("Invalid JSON format: invalid floating-point value for 'normals'."); }
		}

	// Parsing "indices"
	if (!(ss >> token && token == "\"indices\":")) 
		{ throw std::invalid_argument("Invalid JSON format: missing or incorrect 'indices' field."); }
	if (!(ss >> token && token == "[")) 
		{ throw std::invalid_argument("Invalid JSON format: missing opening bracket '[' for 'indices'."); }
	
	while (true) 
		{
			ss >> token;
	
			if (token == "]") { break; }
			
			try { data.indices.push_back(std::stoi(token)); } catch (const std::invalid_argument&) 
				{ throw std::invalid_argument("Invalid JSON format: invalid integer value for 'indices'."); }
		}

	// Parsing "offset"
	if (!(ss >> token && token == "\"offset\":")) 
		{ throw std::invalid_argument("Invalid JSON format: missing or incorrect 'offset' field."); }
	if (!(ss >> token && token == "[")) 
		{ throw std::invalid_argument("Invalid JSON format: missing opening bracket '[' for 'offset'."); }
	if (!(ss >> token)) 
		{ throw std::invalid_argument("Invalid JSON format: missing value for 'offset'."); }
	try { data.offset.x = std::stoi(token); } catch (const std::invalid_argument&) 
		{ throw std::invalid_argument("Invalid JSON format: invalid integer value for 'offset'."); }
	if (!(ss >> token && token == ",")) 
		{ throw std::invalid_argument("Invalid JSON format: missing comma ',' after 'offset' x value."); }
	if (!(ss >> token)) 
		{ throw std::invalid_argument("Invalid JSON format: missing value for 'offset'."); }
	try { data.offset.y = std::stoi(token); } catch (const std::invalid_argument&) 
		{ throw std::invalid_argument("Invalid JSON format: invalid integer value for 'offset'."); }
	if (!(ss >> token && token == ",")) 
		{ throw std::invalid_argument("Invalid JSON format: missing comma ',' after 'offset' y value."); }
	if (!(ss >> token)) 
		{ throw std::invalid_argument("Invalid JSON format: missing value for 'offset'."); }
	try { data.offset.z = std::stoi(token); } catch (const std::invalid_argument&)
		{ throw std::invalid_argument("Invalid JSON format: invalid integer value for 'offset'."); }
	if (!(ss >> token && token == "]"))
		{ throw std::invalid_argument("Invalid JSON format: missing closing bracket ']' for 'offset'."); }

	// Expecting '}'
	if (!(ss >> token && token == "}")) {
		throw std::invalid_argument("Invalid JSON format: missing closing brace '}'.");
	}

    // Return deserialized MapData
    return data;
}


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
	void deduplicateVertices(MapData*);														// Removes duplicate vertices
};



class World{
public:
	Chunk MapGen;
	World();
	MapData ChunkData;
	void UpdateChunks(glm::vec3&);
	glm::ivec3 currentChunk, lastChunk;
	int chunkSize, visibleChunks;
	std::map<glm::ivec3, MapData, Vec3Compare> MapTable;
	std::set<glm::ivec3, Vec3Compare> created_chunks;
	std::set<glm::ivec3, Vec3Compare> visible_chunks;


private: 
	const int maxFOV = 60;
};

