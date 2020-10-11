#pragma once

#include <iostream>
#include <vector>

#include <glm.hpp>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"
#include "Mesh.h"

void loadOBJ(std::string inputfile, 
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

    int size_texcoords = attrib.texcoords.size();
    int size_normals = attrib.normals.size();

    std::cout << attrib.vertices.size() << "\n";
    std::cout << size_texcoords << "\n";
    std::cout << size_normals << "\n";

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                // Extracting position of vertex
                glm::vec3 position{ 
                    attrib.vertices[3 * idx.vertex_index + 0], // x
                    attrib.vertices[3 * idx.vertex_index + 1], // y
                    attrib.vertices[3 * idx.vertex_index + 2]  // z
                };
                vertices.push_back(position);
                
                // Add uvs or normals onyl when they are available
                
                if (size_texcoords > 0)
                {
                    // Extracting uv at vertex
                    glm::vec2 uv{
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    };
                    uvs.push_back(uv);
                }

                if (size_normals > 0)
                {
                    // Extracting normal at vertex
                    glm::vec3 normal{
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    };
                    normals.push_back(normal);
                }

                //// Optional: Extracting color at vertex
                //glm::vec3 position{
                //    attrib.colors[3*idx.vertex_index+0],
                //    attrib.colors[3*idx.vertex_index+1],
                //    attrib.colors[3*idx.vertex_index+2]
                //};
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
}