#pragma once
#include "RenderItem.hpp"
#include "glad/glad.h"
#include <unordered_map>
#include <string>
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.position) ^
                   (hash<glm::vec4>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
}

inline bool operator==(const Vertex& left, const Vertex& right) {
    return left.position == right.position && left.color == right.color && left.uv == right.uv;
}


struct MeshData {
    GLuint vaoID;
    GLuint indexCount;
};

class MeshManager{
  private:
    std::unordered_map<std::string, MeshData> meshes;

    MeshManager() = default;
    ~MeshManager() = default;
    MeshManager(const MeshManager&) = delete;
    MeshManager& operator=(const MeshManager&) = delete;

  public:
    void init(){
      return;
    }
    static MeshManager& Get() {
      static MeshManager instance;
      return instance;
    }

    MeshData loadMesh(const std::string& filepath, GLuint instanceVBO); 
    // MeshData loadMesh(const std::string& filePath);
    void Release(const std::string& filePath);
};
