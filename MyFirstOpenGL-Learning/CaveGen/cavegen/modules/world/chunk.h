#pragma once

#include "atomic.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <glm/glm.hpp>


struct Vertex{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
};

struct Chunk{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	glm::ivec3 offset;
	FillMode fillMode;

	// to JSON
	//std::string Serialize() const;
	//static Chunk* Deserialize(const std::string&);
	void log() const;
};

void OffloadChunk(std::pair<const glm::ivec3, Chunk>*, const char*);
bool LoadChunk(Chunk*, const char*, glm::ivec3);