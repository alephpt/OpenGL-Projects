#include "./chunk.h"

#include <sstream>
#include <string.h>

// Implement Serialize and Deserialize for Chunk
inline std::string Chunk::Serialize() const 
    {
        std::stringstream ss;
        ss << "{";
        ss << "\"vertices\": [\r";
        
        for (size_t i = 0; i < vertices.size(); i++) 
            {
                ss << vertices[i];
                
                if (i < vertices.size() - 1) 
                    { ss << ","; }
            }
        
        ss << "\r],\r";
        ss << "\"colors\": [\r";
        
        for (size_t i = 0; i < colors.size(); i++) 
            {
                ss << colors[i];
        
                if (i < colors.size() - 1) 
                    { ss << ","; }
            }

        ss << "\r],\r";
        ss << "\"normals\": [\r";

        for (size_t i = 0; i < normals.size(); i++) 
            {
                ss << normals[i];
                
                if (i < normals.size() - 1) 
                { ss << ","; }
            }

        ss << "\r],\r";
        ss << "\"indices\": [\r";

        for (size_t i = 0; i < indices.size(); i++) 
            {
                ss << indices[i];

                if (i < indices.size() - 1) 
                    { ss << ","; }
            }

        ss << "\r],\r";
        ss << "\"offset\": [\r" << offset.x << "," << offset.y << "," << offset.z;
        ss << "\r]}\r";

        // Append EOF character
        ss << '\0';

        return ss.str();
    }

inline Chunk* Chunk::Deserialize(const std::string& json) 
    {
        Chunk* data = new Chunk();
        std::stringstream ss(json);
        std::string token;

        // Parsing "vertices"
        if (!(ss >> token && token == "{\"vertices\":")) 
            { printf("[DESERIALIZE]::ERROR - Expected '{vertices' - Received Token: %s\n", token.c_str()); return data; }
        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - vertices Expected '[' - Received Token: %s\n", token.c_str()); return data; }
        
        if (ss >> token && token != "],") 
            { 
                std::istringstream iss(token);

                while(std::getline(iss, token, ','))
                    {
                        try { data->vertices.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
                            { printf("[DESERIALIZE]::ERROR - vertices - Received Token: %s\n", token.c_str()); return data; }
                    }
            }

        // Parsing "colors"
        if (!(ss >> token && token == "\"colors\":"))
            { printf("[DESERIALIZE]::ERROR - Expected 'colors' - Received Token: %s\n", token.c_str()); return data; }
        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - colors Expected '[' - Received Token: %s\n", token.c_str()); return data; }
        
        if (ss >> token && token != "],") 
            { 
                std::istringstream iss(token);
                while(std::getline(iss, token, ','))
                    {
                        try { data->colors.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
                            { printf("[DESERIALIZE]::ERROR - colors - Received Token: %s\n", token.c_str()); return data; }
                    }
            }

        // Parsing "normals"
        if (!(ss >> token && token == "\"normals\":"))
            { printf("[DESERIALIZE]::ERROR - Expected 'normals' - Received Token: %s\n", token.c_str()); return data; }

        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - normals Expected '[' - Received Token: %s\n", token.c_str()); return data; }

        if (ss >> token && token != "],") 
            { 
                std::istringstream iss(token);
                while(std::getline(iss, token, ','))
                    {
                        try { data->normals.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
                            { printf("[DESERIALIZE]::ERROR - normals - Received Token: %s\n", token.c_str()); return data; }
                    }
            }

        // Parsing "indices"
        if (!(ss >> token && token == "\"indices\":")) 
            { printf("[DESERIALIZE]::ERROR - Expected 'indices' - Received Token: %s\n", token.c_str()); return data; }
        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - indices - Expected '[' - Received Token: %s\n", token.c_str()); return data; }
        
        if (ss >> token && token != "],") 
            { 
                std::istringstream iss(token);

                while(std::getline(iss, token, ','))
                    {
                        try { data->indices.push_back(std::stoi(token)); } catch (const std::invalid_argument&) 
                            { printf("[DESERIALIZE]::ERROR - indices - Received Token: %s\n", token.c_str()); return data; }
                    }
            }

        // Parsing "offset"
        if (!(ss >> token && token == "\"offset\":")) 
            { printf("[DESERIALIZE]::ERROR - Expected 'offset' - Received Token: %s\n", token.c_str()); return data; }
        if (!(ss >> token && token == "[")) 
            { printf("[DESERIALIZE]::ERROR - offset - Expected '[' - Received Token: %s\n", token.c_str()); return data; }
        
        if (ss >> token && token != "]}") 
            { 
                std::vector<int> offset;
                std::istringstream iss(token);
                while(std::getline(iss, token, ','))
                    {
                        try { offset.push_back(std::stoi(token)); } catch (const std::invalid_argument&) 
                            { printf("[DESERIALIZE]::ERROR - offset - Received Token: %s\n", token.c_str()); return data; }
                    }

                data->offset = glm::ivec3(offset[0], offset[1], offset[2]);
            }

        ss.clear();
//        data->log();

        // Return deserialized Chunk
        return data;
    }

void OffloadChunk(std::pair<const glm::ivec3, Chunk>* chunk, const char* type)
    {
        // Offload chunk data to disk
        std::ostringstream oss;
        oss << "/home/persist/mine/repos/map_chunks/" << type << "_" << chunk->first.x << chunk->first.y << chunk->first.z << ".chunk";
        std::string filename = oss.str();
        printf("Offloading Chunk to File: %s\n", filename.c_str());
        std::ofstream ofs(filename, std::ios::binary);

        if (!ofs) 
            { std::cerr << " [OffloadChunk]: Failed to Offload Chunk to File: " << filename << std::endl; return; }

        ofs << chunk->second.Serialize().c_str();
        ofs.close();
    }

bool LoadChunk(Chunk* MapChunk, const char* type, glm::ivec3 chunk) 
    {
        // Check if file exists
        std::ostringstream oss;
        oss << "/home/persist/mine/repos/map_chunks/" << type << "_" << chunk.x << chunk.y << chunk.z << ".chunk";
        std::string filename = oss.str();

        printf("Loading Chunk from File: %s\n", filename.c_str());
        if (access(filename.c_str(), F_OK) == -1) 
            { std::cerr << " [LoadChunk]: File does not exist: " << filename << std::endl; return false; }

        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) 
            { std::cerr << " [LoadChunk]: Failed to open file: " << filename << std::endl; return false; }

        // deserialize data from file
        std::string serializedData;
        ifs.seekg(0, std::ios::end);
        serializedData.reserve(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        serializedData.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        // Deserialize data to Chunk object
        MapChunk = Chunk::Deserialize(serializedData);
        ifs.close();
        return true;
    }

void Chunk::log() const
    {
        printf("Vertices: %d\n", vertices.size());
        printf("Colors: %d\n", colors.size());
        printf("Normals: %d\n", normals.size());
        printf("Indices: %d\n", indices.size());
        printf("Offset: %d, %d, %d\n", offset.x, offset.y, offset.z);
    }