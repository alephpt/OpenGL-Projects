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