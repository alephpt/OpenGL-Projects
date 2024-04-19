#include "./chunkdata.h"

#include <sstream>

// Implement Serialize and Deserialize for ChunkData
inline std::string ChunkData::Serialize() const 
    {
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

inline ChunkData ChunkData::Deserialize(const std::string& json) 
    {
        ChunkData data;
        std::stringstream ss(json);
        std::string token;

        // Expecting '{'
        if (!(ss >> token && token == "{")) 
            { printf("[DESERIALIZE]::ERROR - Expected '{' - Received Token: %s\n", token.c_str()); return data; }

        // Parsing "vertices"
        if (!(ss >> token && token == "\"vertices\":")) 
            { printf("[DESERIALIZE]::ERROR - Expected 'vertices' - Received Token: %s\n", token.c_str()); return data; }
        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - vertices Expected '[' - Received Token: %s\n", token.c_str()); return data; }
        
        while (true) 
            { 
                if (!(ss >> token) && token == "]") { break; }
                    { printf("[DESERIALIZE]::ERROR - Expected ']' - Received Token: %s\n", token.c_str()); return data;}

                try { data.vertices.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
                    { printf("[DESERIALIZE]::ERROR - vertices - Received Token: %s\n", token.c_str()); return data; }
            }

        // Parsing "colors"
        if (!(ss >> token && token == "\"colors\":"))
            { printf("[DESERIALIZE]::ERROR - Expected 'colors' - Received Token: %s\n", token.c_str()); return data; }
        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - colors Expected '[' - Received Token: %s\n", token.c_str()); return data; }
        
        while (true) 
            {
                if (!(ss >> token) && token == "]") { break; }
                    { printf("[DESERIALIZE]::ERROR - Expected ']' - Received Token: %s\n", token.c_str()); return data; }

                try { data.colors.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
                    { printf("[DESERIALIZE]::ERROR - colors - Received Token: %s\n", token.c_str()); return data; }
            }

        // Parsing "normals"
        if (!(ss >> token && token == "\"normals\":"))
            { printf("[DESERIALIZE]::ERROR - Expected 'normals' - Received Token: %s\n", token.c_str()); return data; }

        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - normals Expected '[' - Received Token: %s\n", token.c_str()); return data; }

        while (true) 
            {
                if (!(ss >> token) && token == "]") { break; }
                    { printf("[DESERIALIZE]::ERROR - Expected ']' - Received Token: %s\n", token.c_str()); return data; }

                try { data.normals.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
                    { printf("[DESERIALIZE]::ERROR - normals - Received Token: %s\n", token.c_str()); return data; }
            }

        // Parsing "indices"
        if (!(ss >> token && token == "\"indices\":")) 
            { printf("[DESERIALIZE]::ERROR - Expected 'indices' - Received Token: %s\n", token.c_str()); return data; }
        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - indices - Expected '[' - Received Token: %s\n", token.c_str()); return data; }
        
        while (true) 
            {
                if (!(ss >> token) && token == "]") { break; }
                    { printf("[DESERIALIZE]::ERROR - Expected ']' - Received Token: %s\n", token.c_str()); return data; }
                    
                try { data.indices.push_back(std::stoi(token)); } catch (const std::invalid_argument&) 
                    { printf("[DESERIALIZE]::ERROR - indices - Received Token: %s\n", token.c_str()); return data; }
            }

        // Parsing "offset"
        if (!(ss >> token && token == "\"offset\":")) 
            { printf("[DESERIALIZE]::ERROR - Expected 'offset' - Received Token: %s\n", token.c_str()); return data; }
        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - offset - Expected '[' - Received Token: %s\n", token.c_str()); return data; }
        if (!(ss >> token)) 
            { printf("[DESERIALIZE]::ERROR - offset - Expected Token - Received Token: %s\n", token.c_str()); return data;}
        try { data.offset.x = std::stoi(token); } catch (const std::invalid_argument&) 
            { printf("[DESERIALIZE]::ERROR - offset - Received Token: %s\n", token.c_str()); return data;}
        if (!(ss >> token && token == ",")) 
            { printf("[DESERIALIZE]::ERROR - offset - Expected ',' - Received Token: %s\n", token.c_str()); return data;}
        if (!(ss >> token)) 
            { printf("[DESERIALIZE]::ERROR - offset - Expected Token - Received Token: %s\n", token.c_str()); return data;}
        try { data.offset.y = std::stoi(token); } catch (const std::invalid_argument&) 
            { printf("[DESERIALIZE]::ERROR - offset - Received Token: %s\n", token.c_str()); return data;}
        if (!(ss >> token && token == ",")) 
            { printf("[DESERIALIZE]::ERROR - offset - Expected ',' - Received Token: %s\n", token.c_str()); return data;}
        if (!(ss >> token)) 
            { printf("[DESERIALIZE]::ERROR - offset - Expected Token - Received Token: %s\n", token.c_str()); return data;}
        try { data.offset.z = std::stoi(token); } catch (const std::invalid_argument&)
            { printf("[DESERIALIZE]::ERROR - offset - Received Token: %s\n", token.c_str()); return data;}
        if (!(ss >> token && token == "]"))
            { printf("[DESERIALIZE]::ERROR - Expected ']' - Received Token: %s\n", token.c_str()); return data; }

        // Expecting '}'
        if (!(ss >> token && token == "}")) 
            { printf("[DESERIALIZE]::ERROR - Expected '}' - Received Token: %s\n", token.c_str()); return data; }

        // Return deserialized ChunkData
        return data;
    }

void OffloadChunkData(glm::ivec3 chunk, ChunkData *MapChunk)
    {
        // Offload chunk data to disk
        std::string filename = "/home/persist/mine/repos/map_chunks/" + std::to_string(chunk.x) + std::to_string(chunk.y) + std::to_string(chunk.z) + ".chunk";
        std::ofstream ofs(filename, std::ios::binary);

        if (!ofs) {
            std::cerr << " [OffloadChunkData]: Failed to Offload Chunk to File: " << filename << std::endl;

            return;
        }

        ofs << MapChunk->Serialize().c_str();
        ofs.close();
    }

ChunkData LoadChunkData(glm::ivec3 chunk) 
    {
        // Check if file exists
        std::string filename = "/home/persist/mine/repos/map_chunks/" + std::to_string(chunk.x) + std::to_string(chunk.y) + std::to_string(chunk.z) + ".chunk";
        if (access(filename.c_str(), F_OK) == -1) {
            std::cerr << " [LoadChunkData]: File does not exist: " << filename << std::endl;
            return ChunkData();
        }

        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) {
            std::cerr << " [LoadChunkData]: Failed to open file: " << filename << std::endl;
            return ChunkData();
        }

        // deserialize data from file
        std::string serializedData;
        ifs.seekg(0, std::ios::end);
        serializedData.reserve(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        serializedData.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        // Deserialize data to ChunkData object
        ChunkData MapChunk = ChunkData::Deserialize(serializedData);


        ifs.close();
        return MapChunk;
    }