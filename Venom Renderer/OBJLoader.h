#pragma once

#include <iostream>
#include <vector>

#include <glm.hpp>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"
#include "Face.h"
#include "Mesh.h"
#include "Material.h"

static std::string GetBaseDir(const std::string& filepath) {
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

void loadOBJ(std::string inputfile,
    std::vector<Face>& faces,
    std::vector<Material>& materials
)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> t_materials;

    std::string warn;
    std::string err;

    // Important for loading material file along obj
    std::string base_dir = GetBaseDir(inputfile);
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &t_materials, &warn, &err, inputfile.c_str(), base_dir.c_str());

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    int size_texcoords = attrib.texcoords.size();
    int size_normals = attrib.normals.size();

    std::cout << "Objects:       " << shapes.size() << "\n";
    std::cout << "Materials:    " << t_materials.size() << "\n";
    std::cout << "Vertices:     " << attrib.vertices.size() << "\n";
    std::cout << "UV's:    " << size_texcoords << "\n";
    std::cout << "Normals:      " << size_normals << "\n";

    // Creating materials
    for (size_t m=0; m<t_materials.size(); m++)
    {
        // Blender exports non-pbr materials
        // So we need to work around plugging in the values into our pbr material

        Material temp_mat;

        // Fill the values in temp_mat
        // Setting the baseColor
        temp_mat.baseColor.x = t_materials[m].diffuse[0];
        temp_mat.baseColor.y = t_materials[m].diffuse[1];
        temp_mat.baseColor.z = t_materials[m].diffuse[2];

        temp_mat.subsurface = std::max({ float(t_materials[m].transmittance[0]),
            float(t_materials[m].transmittance[1]),
            float(t_materials[m].transmittance[2]) });

        // Picking the max of three as roughness amount
        temp_mat.roughness = std::max({ float(t_materials[m].diffuse[0]),
            float(t_materials[m].diffuse[1]),
            float(t_materials[m].diffuse[2]) });

        // Picking the max of three as specular amount
        temp_mat.specular = std::max({ float(t_materials[m].specular[0]),
            float(t_materials[m].specular[1]),
            float(t_materials[m].specular[2]) });

        temp_mat.clearcoat = t_materials[m].shininess;

        materials.push_back(temp_mat);
    }

    std::cout << "Materials successfully create\n";

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            // Creating a Face object
            Face temp_face;
            temp_face.index = f;
            temp_face.mat = &materials[shapes[s].mesh.material_ids[f]]; // Assigning mat id

            // per-face material
            //shapes[s].mesh.material_ids[f];

            int fv = shapes[s].mesh.num_face_vertices[f];
            if (fv > 3)
                throw "ERROR::Please make sure that all objects are triangulated";

            // Loop over vertices in the face.
            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                // Extracting position of vertex
                glm::vec3 position{ 
                    attrib.vertices[3 * idx.vertex_index + 0], // x
                    attrib.vertices[3 * idx.vertex_index + 1], // y
                    attrib.vertices[3 * idx.vertex_index + 2]  // z
                };
                temp_face.vertices[v] = position;
                
                // Add uvs or normals onyl when they are available
                if (size_texcoords > 0)
                {
                    // Problem with uvs at the moment
                    //// Extracting uv at vertex
                    //glm::vec2 uv{
                    //    attrib.texcoords[2 * idx.texcoord_index + 0],
                    //    attrib.texcoords[2 * idx.texcoord_index + 1]
                    //};
                    //temp_face.uvs[v] = uv;
                }

                if (size_normals > 0)
                {
                    // Extracting normal at vertex
                    glm::vec3 normal{
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    };
                    temp_face.normals[v] = normal;
                }

                //// Optional: Extracting color at vertex
                //glm::vec3 position{
                //    attrib.colors[3*idx.vertex_index+0],
                //    attrib.colors[3*idx.vertex_index+1],
                //    attrib.colors[3*idx.vertex_index+2]
                //};
            }
            index_offset += fv;

            faces.push_back(temp_face);
        }
    }
}