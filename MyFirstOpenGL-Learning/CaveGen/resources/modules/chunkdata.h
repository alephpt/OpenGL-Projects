#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <glm/glm.hpp>

struct ChunkData{
	std::vector<float> vertices;
	std::vector<float> colors;
	std::vector<float> normals;
	std::vector<unsigned int> indices;
	glm::ivec3 offset;

	// to JSON
	std::string Serialize() const;
	static ChunkData Deserialize(const std::string& json);
};


void OffloadChunkData(glm::ivec3 chunk, ChunkData *MapChunk);
ChunkData LoadChunkData(glm::ivec3 chunk);

