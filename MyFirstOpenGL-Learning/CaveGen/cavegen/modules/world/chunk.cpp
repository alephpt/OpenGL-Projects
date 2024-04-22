#include "chunk.h"
#include "../../components/utility/logger.h"

#include <sstream>
#include <string.h>

// Implement Serialize and Deserialize for Chunk
inline std::string Chunk::Serialize() const 
    {
        std::stringstream ss;
        ss << "{\r";
        
        ss << "\"vertex\": [\r";
        for (auto& vertex : vertices) 
            {
                ss << "{ " << vertex.position.x << "," << vertex.position.y << "," << vertex.position.z << "|";
                ss << vertex.color.x << "," << vertex.color.y << "," << vertex.color.z << "|";
                ss << vertex.normal.x << "," << vertex.normal.y << "," << vertex.normal.z << " },\r";
            }
        ss << "\r],\r";

        ss << "\"indices\": [\r";
        for (auto& index : indices) 
            { ss << index << ","; }
        ss << "\r],\r";

        ss << "\"offset\": [\r" << offset.x << "," << offset.y << "," << offset.z;
        ss << "\r]\r}\r";

        // Append EOF character
        ss << '\0';

        return ss.str();
    }

//
static inline bool consume(std::stringstream& ss, const std::string& token) 
    {
        std::string t;
        if (ss >> t && t != token) 
            { printf("[DESERIALIZE]::ERROR - Expected Token: %s Received Token: %s\n", token.c_str(), t.c_str()); return false; }
        return true;
    }

inline Chunk* Chunk::Deserialize(const std::string& json) 
    {
        Chunk* data = new Chunk();
        std::stringstream ss(json);
        std::string token;

        std::vector<Vertex> vertices;

        // Deserialize vertices
        if (ss >> token && token != "{") 
            { printf("[DESERIALIZE]::ERROR - vertex - Expected Token: { Received Token: %s\n", token.c_str()); return data; }

        if (ss >> token && token != "\"vertex\":") 
            { printf("[DESERIALIZE]::ERROR - vertex - Expected Token: \"vertex\": Received Token: %s\n", token.c_str()); return data; }

        if (ss >> token && token != "[") 
            { printf("[DESERIALIZE]::ERROR - vertex - Expected Token: [ Received Token: %s\n", token.c_str()); return data; }

        // disassemble vertex { (x,y,z), (r,g,b), (nx,ny,nz) }
        while (ss >> token && token != "}") 
            { 
                if (token == "{") continue;
                
                std::istringstream iss(token);
                while(std::getline(iss, token, '|'))
                    {
                        std::istringstream iss(token);
                        std::string value;
                        
                        
                    }

            }

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
        data->log();

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

        if (access(filename.c_str(), F_OK) == -1) 
            { std::cerr << "\t [LoadChunk]: File does not exist: " << filename << std::endl; return false; }

        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) 
            { std::cerr << "\t [LoadChunk]: Failed to open file: " << filename << std::endl; return false; }

        printf("\t [LoadChunk]: Loading Chunk from File: %s\n", filename.c_str());
        // deserialize data from file
        std::string serializedData;
        ifs.seekg(0, std::ios::end);
        serializedData.reserve(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        serializedData.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        // Deserialize data to Chunk object
        MapChunk = Chunk::Deserialize(serializedData);

        // TODO: Fix Format of Serialized Data
        ifs.close();

        if (MapChunk->vertices.size() == 0) 
            { std::cerr << "\t [LoadChunk]: Failed to Load Chunk from File: " << filename << std::endl; return false; }

        return true;
    }

void Chunk::log() const
    {
        Logger::Verbose("Vertices: %d\n", vertices.size());
        Logger::Verbose("\tPosition: %f, %f, %f\n", vertices[0].position.x, vertices[0].position.y, vertices[0].position.z);
        Logger::Verbose("\tColor: %f, %f, %f\n", vertices[0].color.x, vertices[0].color.y, vertices[0].color.z);
        Logger::Verbose("\tNormal: %f, %f, %f\n", vertices[0].normal.x, vertices[0].normal.y, vertices[0].normal.z);
        Logger::Verbose("Indices: %d\n", indices.size());
        Logger::Verbose("Offset: %d, %d, %d\n", offset.x, offset.y, offset.z);
    }