#define TINYOBJLOADER_IMPLEMENTATION
#include "MeshManager.hpp"
#include "RenderItem.hpp"
#include <iostream>



// void MeshManager::Release(const std::string& filePath) {
//   auto it = meshes.find(filePath); 
//   if (it != meshes.end()) {
//     it->second.refCount--;
//
//     if (it->second.refCount == 0) {
//       glDeleteMeshs(1, &it->second.id);
//       meshes.erase(it);
//     }
//   }
// }


MeshData MeshManager::loadMesh(const std::string& filepath, GLuint instanceVBO) {
  if (meshes.count(filepath)) return meshes[filepath];

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, filepath.c_str(), nullptr)) {
    std::cerr << "Failed to load OBJ: " << warn << err << "\n";
    return MeshData{};
  }

  std::vector<Vertex> vertices;

  std::vector<Vertex> normals;
  std::vector<GLuint> indices;
  std::unordered_map<Vertex, uint32_t> uniqueVertices;

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex{};

      vertex.position = {
        attrib.vertices[3 * index.vertex_index + 0],
        attrib.vertices[3 * index.vertex_index + 1],
        attrib.vertices[3 * index.vertex_index + 2]
      };
      if (index.normal_index >= 0) {
        vertex.normal = {
          attrib.normals[3 * index.normal_index + 0],
          attrib.normals[3 * index.normal_index + 1],
          attrib.normals[3 * index.normal_index + 2]
        };
      } else {
        vertex.normal = {0.0f, 1.0f, 0.0f};
      }

      if (index.texcoord_index >= 0) {
        vertex.uv = {
          attrib.texcoords[2 * index.texcoord_index + 0],
          1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // FLIP V!
        };
      }

      vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
      vertex.texIndex = 0.0f;

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }

      indices.push_back(uniqueVertices[vertex]);
    }
  }

  GLuint vao, vbo, ibo;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // upload geometry
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  // upload indices
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  // standard vertex attributes (0: pos, 1: color, 2: uv, 3: texindex)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texIndex));

  // instance attributes (link this vao to your global instance vbo)
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  std::size_t vec4Size = sizeof(glm::vec4);
  for (int i = 0; i < 4; i++) {
    GLuint attribLocation = 5 + i;
    glEnableVertexAttribArray(attribLocation);
    glVertexAttribPointer(attribLocation, 4, GL_FLOAT, GL_FALSE, 
        sizeof(glm::mat4), (void*)(i * vec4Size));
    glVertexAttribDivisor(attribLocation, 1); 
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  MeshData data = { vao, static_cast<GLuint>(indices.size()) };
  meshes[filepath] = data;
  return data;
}
// MeshData MeshManager::loadMesh(const std::string& filepath) {
//   if (meshes.count(filepath)) return meshes[filepath];
//
//   std::vector<Vertex> parsedVertices;
//   std::vector<GLuint> parsedIndices;
//
//   GLuint vao, vbo, ibo;
//   glGenVertexArrays(1, &vao);
//   glBindVertexArray(vao);
//
//   // upload geometry
//   glGenBuffers(1, &vbo);
//   glBindBuffer(GL_ARRAY_BUFFER, vbo);
//   glBufferData(GL_ARRAY_BUFFER, parsedVertices.size() * sizeof(Vertex), parsedVertices.data(), GL_STATIC_DRAW);
//
//   // upload indices
//   glGenBuffers(1, &ibo);
//   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//   glBufferData(GL_ELEMENT_ARRAY_BUFFER, parsedIndices.size() * sizeof(GLuint), parsedIndices.data(), GL_STATIC_DRAW);
//
//   // Set Vertex Attributes (Position, Color, UV)
//   // ... (Same as your 2D setup) ...
//   glBindVertexArray(0);
//
//   MeshData data = { vao, static_cast<GLuint>(parsedIndices.size()) };
//   meshes[filepath] = data;
//   return data;
// }

// parsing:
/* 
   v 1.0 1.0 -1.0: vertex 
   vt 0.625 0.500: 2d UV 

   vn 0.0 1.0 0.0: normal vector  

   f 1/1/1 5/2/1 7/3/1: face -> triangle
// of format VertIndex / UVindex / NormalIndex

*/
