//--------------------------------------------------------------------------------------
// File: WaveFrontReader.h
//
// Code for loading basic mesh data from a WaveFront OBJ file
//
// http://en.wikipedia.org/wiki/Wavefront_.obj_file
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=324981
//--------------------------------------------------------------------------------------

// Modified by Mark Sowden <markelswo@gmail.com> for OpenHoW

#pragma once

#include <PL/platform.h>
#include <PL/platform_math.h>
#include <PL/platform_console.h>
#include <PL/platform_filesystem.h>

class WaveFrontReader {
public:
    typedef unsigned int index_t;

    WaveFrontReader() noexcept : hasNormals(false), hasTexcoords(false) {}

    bool Load(const std::string &szFilePath, bool ccw = true) {
        Clear();

        static const size_t MAX_POLY = 64;

        // For now just read the whole thing into memory...
        PLFile *filePtr = plOpenFile( szFilePath.c_str(), true );
        if ( filePtr == nullptr ) {
			LogWarn("Failed to open \"%s\"!\n", szFilePath.c_str());
			return false;
        }

        std::stringstream InFile;
        InFile << plGetFileData( filePtr );
        plCloseFile( filePtr );

        name = szFilePath;

        size_t cpos = name.find_last_of("\\/");
        if(std::string::npos != cpos) {
            name.erase(0, cpos + 1);
        }

        cpos = name.find_last_of('.');
        if(std::string::npos != cpos) {
            name.erase(cpos);
        }

        std::vector<PLVector3> positions;
        std::vector<PLVector3> normals;
        std::vector<PLVector2> texCoords;

        VertexCache vertexCache;

        Material defmat;
        defmat.strName = "default";

        materials.emplace_back(defmat);

        uint32_t curSubset = 0;

        std::string strMaterialFilename;
        for (;;) {
            std::string strCommand;
            InFile >> strCommand;
            if (!InFile) {
                break;
            }

            if (*strCommand.c_str() == '#') {
                // Comment
            } else if (strCommand == "o") {
                // Object name ignored
            } else if (strCommand == "g") {
                // Group name ignored
            } else if (strCommand == "s") {
                // Smoothing group ignored
            } else if (strCommand == "v") {
                // Vertex Position
                float x, y, z;
                InFile >> x >> y >> z;
                positions.emplace_back(PLVector3(x, y, z));
            } else if (strCommand == "vt") {
                // Vertex TexCoord
                float u, v;
                InFile >> u >> v;
                texCoords.emplace_back(PLVector2(u, 1.0f - v));

                hasTexcoords = true;
            } else if (strCommand == "vn") {
                // Vertex Normal
                float x, y, z;
                InFile >> x >> y >> z;
                normals.emplace_back(PLVector3(x, y, z));

                hasNormals = true;
            } else if (strCommand == "f") {
                // Face
                int iPosition, iTexCoord, iNormal;
                PLVertex vertex;

                uint32_t faceIndex[MAX_POLY];
                size_t iFace = 0;
                for (;;) {
                    if (iFace >= MAX_POLY) {
                        // Too many polygon verts for the reader
                        LogWarn("Too many polygon vertices for the reader (%dvs%d)!\n", iFace, MAX_POLY);
                        return false;
                    }

                    InFile >> iPosition;

                    uint32_t vertexIndex = 0;
                    if (!iPosition) {
                        // 0 is not allowed for index
                        LogWarn("0 is not allowed for index!\n");
                        return false;
                    } else if (iPosition < 0) {
                        // Negative values are relative indices
                        vertexIndex = uint32_t(positions.size() + iPosition);
                    } else {
                        // OBJ format uses 1-based arrays
                        vertexIndex = iPosition - 1;
                    }

                    if (vertexIndex >= positions.size())
                        return false;

                    vertex.position = positions[vertexIndex];

                    if ('/' == InFile.peek()) {
                        InFile.ignore();

                        if ('/' != InFile.peek()) {
                            // Optional texture coordinate
                            InFile >> iTexCoord;

                            uint32_t coordIndex = 0;
                            if (!iTexCoord) {
                                // 0 is not allowed for index
                                return false;
                            } else if (iTexCoord < 0) {
                                // Negative values are relative indices
                                coordIndex = uint32_t(texCoords.size() + iTexCoord);
                            } else {
                                // OBJ format uses 1-based arrays
                                coordIndex = iTexCoord - 1;
                            }

                            if (coordIndex >= texCoords.size())
                                return false;

                            vertex.st[0] = texCoords[coordIndex];
                        }

                        if ('/' == InFile.peek()) {
                            InFile.ignore();

                            // Optional vertex normal
                            InFile >> iNormal;

                            uint32_t normIndex = 0;
                            if (!iNormal) {
                                // 0 is not allowed for index
                                return false;
                            } else if (iNormal < 0) {
                                // Negative values are relative indices
                                normIndex = uint32_t(normals.size() + iNormal);
                            } else {
                                // OBJ format uses 1-based arrays
                                normIndex = iNormal - 1;
                            }

                            if (normIndex >= normals.size()) {
                                // Some Objs we tested provide normal indices, but no normals
                                // ... GREAT!
                                LogWarn("Out of range normal, ignoring!\n");
                                vertex.normal = PLVector3(0, 0, 0);
                            } else {
                                vertex.normal = normals[normIndex];
                            }
                        }
                    }

                    // If a duplicate vertex doesn't exist, add this vertex to the Vertices
                    // list. Store the index in the Indices array. The Vertices and Indices
                    // lists will eventually become the Vertex Buffer and Index Buffer for
                    // the mesh.
                    uint32_t index = AddVertex(vertexIndex, &vertex, vertexCache);
                    if (index == uint32_t(-1)) {
                        return false;
                    }

                    if (index >= 0xFFFFFFFF) {
                        // Too many indices for 32-bit IB!
                        return false;
                    }

                    faceIndex[iFace] = index;
                    ++iFace;

                    // Check for more face data or end of the face statement
                    bool faceEnd = false;
                    for (;;) {
                        wchar_t p = InFile.peek();
                        if ('\n' == p || !InFile) {
                            faceEnd = true;
                            break;
                        } else if (isdigit(p) || p == '-' || p == '+')
                            break;

                        InFile.ignore();
                    }

                    if (faceEnd)
                        break;
                }

                if (iFace < 3) {
                    // Need at least 3 points to form a triangle
                    LogWarn("Invalid number of points to form a triangle (%dvs3)\n", iFace);
                    return false;
                }

                // Convert polygons to triangles
                uint32_t i0 = faceIndex[0];
                uint32_t i1 = faceIndex[1];

                for (size_t j = 2; j < iFace; ++j) {
                    uint32_t index = faceIndex[j];
                    indices.emplace_back(static_cast<index_t>(i0));
                    if (ccw) {
                        indices.emplace_back(static_cast<index_t>(i1));
                        indices.emplace_back(static_cast<index_t>(index));
                    } else {
                        indices.emplace_back(static_cast<index_t>(index));
                        indices.emplace_back(static_cast<index_t>(i1));
                    }

                    attributes.emplace_back(curSubset);

                    i1 = index;
                }

                assert(attributes.size() * 3 == indices.size());
            } else if (strCommand == "mtllib") {
                // Material library
                InFile >> strMaterialFilename;
            } else if (strCommand == "usemtl") {
                // Material
                char strName[PL_SYSTEM_MAX_PATH] = {};
                InFile >> strName;

                bool bFound = false;
                uint32_t count = 0;
                for (auto it = materials.cbegin(); it != materials.cend(); ++it, ++count) {
                    if (it->strName == strName) {
                        bFound = true;
                        curSubset = count;
                        break;
                    }
                }

                if (!bFound) {
                    Material mat;
                    curSubset = static_cast<uint32_t>(materials.size());
                    mat.strName = strName;
                    materials.emplace_back(mat);
                }
            } else {
                LogDebug("Unrecognised command in Obj \"%s\"!\n", szFilePath.c_str());
            }

            InFile.ignore(1000, L'\n');
        }

        if (positions.empty()) {
            return false;
        }

        // If an associated material file was found, read that in as well.
        if (!strMaterialFilename.empty()) {
            const std::string& strMaterialFilePath = szFilePath.substr(0, szFilePath.find_last_of("\\/")) +
                    "/" + strMaterialFilename;
            if(!LoadMTL(strMaterialFilePath)) {
                LogWarn("Failed to load material, \"%s\"!\n", strMaterialFilename.c_str());
            }
        }

        return true;
    }

    bool LoadMTL(const std::string &szFilePath) {
        // Assumes MTL is in CWD along with OBJ
        PLFile *filePtr = plOpenFile( szFilePath.c_str(), true );
        if ( filePtr == nullptr ) {
			LogWarn("Failed to open material, \"%s\"!\n", szFilePath.c_str());
			return false;
        }

		std::stringstream InFile;
		InFile << plGetFileData( filePtr );
		plCloseFile( filePtr );

        auto curMaterial = materials.end();

        for (;;) {
            std::string strCommand;
            InFile >> strCommand;
            if (!InFile) {
                break;
            }

            if (strCommand == "newmtl") {
                // Switching active materials
                char strName[PL_SYSTEM_MAX_PATH] = {};
                InFile >> strName;

                curMaterial = materials.end();
                for (auto it = materials.begin(); it != materials.end(); ++it) {
                    if (it->strName == strName) {
                        curMaterial = it;
                        break;
                    }
                }
            }

            // The rest of the commands rely on an active material
            if (curMaterial == materials.end())
                continue;

            if (strCommand == "#") {
                // Comment
            } else if (strCommand == "Ka") {
                // Ambient color
                float r, g, b;
                InFile >> r >> g >> b;
                curMaterial->vAmbient = PLVector3(r, g, b);
            } else if (strCommand == "Kd") {
                // Diffuse color
                float r, g, b;
                InFile >> r >> g >> b;
                curMaterial->vDiffuse = PLVector3(r, g, b);
            } else if (strCommand == "Ks") {
                // Specular color
                float r, g, b;
                InFile >> r >> g >> b;
                curMaterial->vSpecular = PLVector3(r, g, b);
            } else if (strCommand == "Ke") {
                // Emissive color
                float r, g, b;
                InFile >> r >> g >> b;
                curMaterial->vEmissive = PLVector3(r, g, b);
                if (r > 0.f || g > 0.f || b > 0.f) {
                    curMaterial->bEmissive = true;
                }
            } else if (strCommand == "d") {
                // Alpha
                float alpha;
                InFile >> alpha;
                curMaterial->fAlpha = std::min(1.f, std::max(0.f, alpha));
            } else if (strCommand == "Tr") {
                // Transparency (inverse of alpha)
                float invAlpha;
                InFile >> invAlpha;
                curMaterial->fAlpha = std::min(1.f, std::max(0.f, 1.f - invAlpha));
            } else if (strCommand == "Ns") {
                // Shininess
                int nShininess;
                InFile >> nShininess;
                curMaterial->nShininess = nShininess;
            } else if (strCommand == "illum") {
                // Specular on/off
                int illumination;
                InFile >> illumination;
                curMaterial->bSpecular = (illumination == 2);
            } else if (strCommand == "map_Kd" || strCommand == "map_Ka") {
                // Diffuse texture
                curMaterial->strTexture = szFilePath.substr(0, szFilePath.find_last_of("\\/")) +
                                          "/" + LoadTexturePath(InFile);
            } else if (strCommand == "map_Ks") {
                // Specular texture
                curMaterial->strSpecularTexture = LoadTexturePath(InFile);
            } else if (strCommand == "map_Kn" || strCommand == "norm") {
                // Normal texture
                curMaterial->strNormalTexture = LoadTexturePath(InFile);
            } else if (strCommand == "map_Ke" || strCommand == "map_emissive") {
                // Emissive texture
                curMaterial->strEmissiveTexture = LoadTexturePath(InFile);
                curMaterial->bEmissive = true;
            } else if (strCommand == "map_RMA" || strCommand == "map_ORM") {
                // RMA texture
                curMaterial->strRMATexture = LoadTexturePath(InFile);
            } else {
                // Unimplemented or unrecognized command
            }

            InFile.ignore(1000, L'\n');
        }

        return true;
    }

    void Clear() {
        vertices.clear();
        indices.clear();
        attributes.clear();
        materials.clear();
        name.clear();
        hasNormals = false;
        hasTexcoords = false;
    }

    struct Material {
        PLVector3 vAmbient;
        PLVector3 vDiffuse;
        PLVector3 vSpecular;
        PLVector3 vEmissive;
        uint32_t nShininess;
        float fAlpha;

        bool bSpecular;
        bool bEmissive;

        std::string strName;
        std::string strTexture;
        std::string strNormalTexture;
        std::string strSpecularTexture;
        std::string strEmissiveTexture;
        std::string strRMATexture;

        Material() noexcept :
                vAmbient(0.2f, 0.2f, 0.2f),
                vDiffuse(0.8f, 0.8f, 0.8f),
                vSpecular(1.0f, 1.0f, 1.0f),
                vEmissive(0.f, 0.f, 0.f),
                nShininess(0),
                fAlpha(1.f),
                bSpecular(false),
                bEmissive(false),
                strName{},
                strTexture{},
                strNormalTexture{},
                strSpecularTexture{},
                strEmissiveTexture{},
                strRMATexture{} {
        }
    };

    std::vector<PLVertex> vertices;
    std::vector<index_t> indices;
    std::vector<uint32_t> attributes;
    std::vector<Material> materials;

    std::string name;
    bool hasNormals;
    bool hasTexcoords;

private:
    typedef std::unordered_multimap<uint32_t, uint32_t> VertexCache;
    uint32_t AddVertex(uint32_t hash, const PLVertex *pVertex, VertexCache &cache) {
        auto f = cache.equal_range(hash);
        for (auto it = f.first; it != f.second; ++it) {
            auto &tv = vertices[it->second];
            if (0 == memcmp(pVertex, &tv, sizeof(PLVertex))) {
                return it->second;
            }
        }

        auto index = static_cast<uint32_t>(vertices.size());
        vertices.emplace_back(*pVertex);

        VertexCache::value_type entry(hash, index);
        cache.insert(entry);
        return index;
    }

    static std::string LoadTexturePath(std::stringstream &InFile) {
        char buff[1024] = {};
        InFile.getline(buff, 1024, L'\n');
        InFile.putback(L'\n');

        std::string path = buff;

        // Ignore any end-of-line comment
        size_t pos = path.find_first_of(L'#');
        if (pos != std::string::npos) {
            path = path.substr(0, pos);
        }

        // Texture path should be last element in line
        pos = path.find_last_of(' ');
        if (pos != std::string::npos) {
            path = path.substr(pos + 1);
        }

        // Trim any trailing whitespace
        pos = path.find_last_not_of(" \t\r\n");
        if (pos != std::string::npos) {
            path = path.substr(0, pos + 1);
        }

        return path;
    }
};
