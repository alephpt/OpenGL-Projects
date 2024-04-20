#pragma once

#include "../atomic.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <glm/glm.hpp>

struct Chunk{
	std::vector<float> vertices;
	std::vector<float> colors;
	std::vector<float> normals;
	std::vector<unsigned int> indices;
	glm::ivec3 offset;
	FillMode fillMode;

	// to JSON
	std::string Serialize() const;
	static Chunk* Deserialize(const std::string&);
	void log() const;
};

void OffloadChunk(std::pair<const glm::ivec3, Chunk>*, const char*);
Chunk* LoadChunk(glm::ivec3, const char*);

