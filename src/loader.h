#pragma once

#include <string>
#include <vector>
#include <optional>

#include "maths.h"
#include "texture.h"

// newmtl name
//    defines the name of the material.
// Ka r g b
//    defines the ambient color of the material to be (r,g,b). The default is (0.2,0.2,0.2);
// Kd r g b
//     defines the diffuse color of the material to be (r,g,b). The default is (0.8,0.8,0.8);
// Ks r g b
//     defines the specular color of the material to be (r,g,b). This color shows up in highlights. The default is (1.0,1.0,1.0);
// d alpha
//     defines the non-transparency of the material to be alpha. The default is 1.0 (not transparent at all). The quantities d and Tr are the opposites of each other, and specifying transparency or nontransparency is simply a matter of user convenience.
// Tr alpha
//     defines the transparency of the material to be alpha. The default is 0.0 (not transparent at all). The quantities d and Tr are the opposites of each other, and specifying transparency or nontransparency is simply a matter of user convenience.
// Ns s
//     defines the shininess of the material to be s. The default is 0.0;
// illum n
//     denotes the illumination model used by the material. illum = 1 indicates a flat material with no specular highlights, so the value of Ks is not used. illum = 2 denotes the presence of specular highlights, and so a specification for Ks is required.
// map_Ka filename
//    names a file containing a texture map, which should just be an ASCII dump of RGB values;
struct SofwareMaterial
{
    enum Illumination
    {
        NO_SPECULAR = 1, HAS_SPECULAR
    };

    std::string name = "default";      // newmtl
    std::string file = "";             // map_Ka

    vec3 ambient  = {0.2, 0.2, 0.2};   // Ka
    vec3 diffuse  = {0.8, 0.8, 0.8};   // Kd
    vec3 specular = {1.0, 1.0, 1.0};   // Ks

    float shininess     = 0.0;         // NS
    float transparency  = 0.0;         // Tr
    float opaqueness    = 1.0;         // d

    Illumination illumination = NO_SPECULAR;  // illum

    std::optional<Image> image_data;
};

struct SoftwareMesh
{
    std::size_t mesh_id     = 0;
    std::size_t material_id = 0;
    std::string                    name     = "";
    std::vector<Vertex>            vertices = {};
    std::optional<SofwareMaterial> material = {};
};



std::vector<Vertex> LoadAsset(const std::string& input_file, const std::string& material_directory = "");
std::vector<SoftwareMesh> LoadScene(const std::string& input_file, const std::string& material_directory = "");