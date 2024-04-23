#include "chunk.h"
#include "../../components/utility/logger.h"

#include <sstream>
#include <string.h>

// Implement Serialize and Deserialize for Chunk
inline std::string Chunk::Serialize() const 
    {
        std::stringstream ss;
        ss << "{\r";
        
        ss << "\"vertex\":[\r";
        for (auto& vertex : vertices) 
            {
                ss << "{ " << vertex.position.x << "," << vertex.position.y << "," << vertex.position.z << ",";
                ss << vertex.color.x << "," << vertex.color.y << "," << vertex.color.z << ",";
                ss << vertex.normal.x << "," << vertex.normal.y << "," << vertex.normal.z << " },\r";

            }
        ss << "],\r";

        ss << "\"indices\":[\r";
        for (auto& index : indices) 
            { 
                ss << index; 
                
                if (&index != &indices.back()) { ss << ","; }
            }
        ss << "\r],\r";

        ss << "\"offset\":[\r" << offset.x << "," << offset.y << "," << offset.z;
        ss << "\r]\r}\r";

        // Append EOF character
        ss << '\0';

        return ss.str();
    }

inline bool Chunk::Deserialize(const std::string& file, Chunk* data) 
    {
        std::stringstream ss(file);
        std::string token;

        std::vector<Vertex> vertices;

        // Deserialize vertices
        if (ss >> token && token != "{") 
            { Logger::Error("[DESERIALIZE]::ERROR - vertex - Expected Token: { Received Token: %s\n", token.c_str()); return false; }

        if (ss >> token && token != "\"vertex\":[")
            { Logger::Error("[DESERIALIZE]::ERROR - vertex - Expected Token: \"vertex\":[ Received Token: %s\n", token.c_str()); return false; }

        // disassemble vertex { (x,y,z), (r,g,b), (nx,ny,nz) }
        while (ss >> token && token != "],")
            { 
                if (token != "{") 
                    { Logger::Error("[DESERIALIZE]::ERROR - vertex - Expected Token: { Received Token: %s\n", token.c_str()); }

                ss >> token;
                std::istringstream iss(token);
                std::vector<float> vertex;
                while(std::getline(iss, token, ','))
                    {
                        try { vertex.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
                            { Logger::Error("[DESERIALIZE]::ERROR - vertex - Received Token: %s\n", token.c_str()); return false; }
                    }
                        
                // create vertex
                Vertex v;
                v.position = glm::vec3(vertex[0], vertex[1], vertex[2]);
                v.color = glm::vec3(vertex[3], vertex[4], vertex[5]);
                v.normal = glm::vec3(vertex[6], vertex[7], vertex[8]);
                
                // add vertex to vertices
                data->vertices.push_back(v);

                if (ss >> token && token != "},")
                    { Logger::Error("[DESERIALIZE]::ERROR - vertex - Expected Token: '},' Received Token: %s\n", token.c_str()); return false; }
            }

        if (ss >> token && token != "\"indices\":[")
            { Logger::Error("[DESERIALIZE]::ERROR - indices - Expected Token: \"indices\":[ Received Token: %s\n", token.c_str()); return false; }
        
        // Deserialize indices
        while (ss >> token && token != "],")
            {
                std::istringstream iss(token);
                while(std::getline(iss, token, ','))
                    {
                        try { data->indices.push_back(std::stoi(token)); } catch (const std::invalid_argument&) 
                            { Logger::Error("[DESERIALIZE]::ERROR - indices - Received Token: %s\n", token.c_str()); return false; }
                    }
            }

        if (ss >> token && token != "\"offset\":[")
            { Logger::Error("[DESERIALIZE]::ERROR - offset - Expected Token: \"offset\":[ Received Token: %s\n", token.c_str()); return false; }

        // Deserialize offset
        while (ss >> token && token != "]")
            {
                std::vector<float> offset;
                std::istringstream vss(token);
                while(std::getline(vss, token, ','))
                    {
                        try { offset.push_back(std::stof(token)); } catch (const std::invalid_argument&) 
                            { Logger::Error("[DESERIALIZE]::ERROR - offset - Received Token: %s\n", token.c_str()); return false; }
                    }

                data->offset = glm::ivec3(offset[0], offset[1], offset[2]);
            }

        ss.clear();
        //data->log();
        Logger::Debug("Loaded Chunk %d, %d, %d,\n", data->offset.x, data->offset.y, data->offset.z);
        return true;
    }

void OffloadChunk(std::pair<const glm::ivec3, Chunk>* chunk, const char* type)
    {
        // Offload chunk data to disk
        std::ostringstream oss;
        oss << "/home/persist/mine/repos/map_chunks/" << type << "_x" << chunk->first.x << "_y" << chunk->first.y << "_z" << chunk->first.z << ".chunk";
        std::string filename = oss.str();

        // If the file already exists we just exit
        if (access(filename.c_str(), F_OK) != -1) 
            { Logger::Debug(" [OffloadChunk]: File already exists: %s\n", filename.c_str()); return; }

        Logger::Debug("Offloading Chunk to File: %s\n", filename.c_str());
        std::ofstream ofs(filename, std::ios::binary);


        if (!ofs) 
            { Logger::Error(" [OffloadChunk]: Failed to Offload Chunk to File: %s\n", filename); return; }

        ofs << chunk->second.Serialize().c_str();
        ofs.close();
    }

bool LoadChunk(Chunk* MapChunk, const char* type, glm::ivec3 chunk) 
    {
        // Check if file exists
        std::ostringstream oss;
        oss << "/home/persist/mine/repos/map_chunks/" << type << "_x" << chunk.x << "_y" << chunk.y << "_z" << chunk.z << ".chunk";
        std::string filename = oss.str();

        if (access(filename.c_str(), F_OK) == -1) 
            { Logger::Debug("\t [LoadChunk]: File does not exist: %s\n", filename.c_str()); return false; }

        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) 
            { Logger::Debug("\t [LoadChunk]: Failed to open file: %s\n", filename.c_str()); return false;}

        Logger::Debug("\t [LoadChunk]: Loading Chunk from File: %s\n", filename.c_str());
        // deserialize data from file
        std::string serializedData;
        ifs.seekg(0, std::ios::end);
        serializedData.reserve(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        serializedData.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        // Deserialize data to Chunk object
        if (Chunk::Deserialize(serializedData, MapChunk));
            { 
                Logger::Debug("\t [LoadChunk]: Chunk Loaded Successfully.\n"); 
                MapChunk->log();
            }

        // TODO: Fix Format of Serialized Data
        ifs.close();

        if (MapChunk->vertices.size() == 0) 
            { Logger::Error("\t [LoadChunk]: Chunk is Empty.\n"); return false; }

        return true;
    }

void Chunk::log() const
    {
        if (vertices.size() == 0) 
            { Logger::Error("Chunk is Empty\n"); return; }
        Logger::Info("Vertices: %d\n", vertices.size());
        Logger::Info("\tPosition: %f, %f, %f\n", vertices[0].position.x, vertices[0].position.y, vertices[0].position.z);
        Logger::Info("\tColor: %f, %f, %f\n", vertices[0].color.x, vertices[0].color.y, vertices[0].color.z);
        Logger::Info("\tNormal: %f, %f, %f\n", vertices[0].normal.x, vertices[0].normal.y, vertices[0].normal.z);
        Logger::Info("Indices: %d\n", indices.size());
        Logger::Info("Offset: %d, %d, %d\n", offset.x, offset.y, offset.z);
    }