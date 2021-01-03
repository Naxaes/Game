#include "loader.h"

#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "debug.h"


// Stolen from SO. Should be temporary.
struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;
    }
};



// TODO(ted): Stupidly slow and probably buggy.
std::vector<SoftwareMesh> LoadScene(const std::string& input_file, const std::string& material_directory)
{
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = material_directory;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(input_file, reader_config))
        ASSERT(!reader.Error().empty(), reader.Error().data());

    if (!reader.Warning().empty())
        WARNING(reader.Warning().data());

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::unordered_map<std::pair<std::size_t, std::size_t>, SoftwareMesh, pair_hash> meshes;
    for (size_t s = 0; s < shapes.size(); s++)
    {
        std::vector<Vertex> vertices;
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            auto m  = shapes[s].mesh.material_ids[f];  // per-face material
            auto i  = std::pair(std::size_t(s), std::size_t(m));
            auto it = meshes.find(i);
            if (it == meshes.end())
            {
                meshes[i] = std::move(SoftwareMesh {
                    i.first, i.second, shapes[s].name, {},
                    SofwareMaterial {
                        materials[m].name,
                        materials[m].diffuse_texname,
                        { materials[m].ambient[0],  materials[m].ambient[1],  materials[m].ambient[2]  },
                        { materials[m].diffuse[0],  materials[m].diffuse[1],  materials[m].diffuse[2]  },
                        { materials[m].specular[0], materials[m].specular[1], materials[m].specular[2] },
                        materials[m].shininess,
                        0.0,
                        1.0,
                        (materials[m].illum == 1) ? SofwareMaterial::NO_SPECULAR : SofwareMaterial::HAS_SPECULAR,
                        (materials[m].diffuse_texname.size() > 0) ? std::move(Image::from_path(materials[m].diffuse_texname, material_directory)) : std::optional<Image>()
                    }
                });
            };

            auto& mesh = meshes[i];

            int fv = shapes[s].mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                mesh.vertices.push_back({
                   {vx, vy, vz}, {tx, ty}, {nx, ny, nz}
               });
            }
            index_offset += fv;
        }
    }

    std::vector<SoftwareMesh> all_meshes;
    all_meshes.reserve(meshes.size());

    for(auto it : meshes)
        all_meshes.push_back(it.second);

    return all_meshes;
}



std::vector<Vertex> LoadAsset(const std::string& input_file, const std::string& material_directory)
{
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = material_directory; // "./"; // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(input_file, reader_config))
        ASSERT(!reader.Error().empty(), reader.Error().data());

    if (!reader.Warning().empty())
        WARNING(reader.Warning().data());

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<Vertex> vertices;
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                vertices.push_back({
                   {vx, vy, vz}, {tx, ty}, {nx, ny, nz}
                });
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }

    return vertices;
}

