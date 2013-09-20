#ifndef MESH_H
#define MESH_H

#include "entity.h"
#include "renderable/submesh.h"

#include <vector>

struct aiScene;
struct aiMesh;

namespace Engine { namespace Entity {

class Mesh : public Entity
{
public:
    explicit Mesh(QOPENGL_FUNCTIONS* funcs);
    ~Mesh();

    bool loadFromFile(const std::string& file);

    virtual void updateRenderList(RenderList& list);

    void setMaterialAttributes(const Material::Attributes& attributes);

private:
    QOPENGL_FUNCTIONS* gl;

    bool initFromScene(const aiScene* scene, const std::string& filenName);

    void initSubMesh(const aiMesh* mesh,
        std::vector<QVector3D>& vertices,
        std::vector<QVector3D>& normals,
        std::vector<QVector3D>& tangents,
        std::vector<QVector2D>& uvs,
        std::vector<unsigned int>& indices);

    void initMaterials(const aiScene* scene, const std::string& fileName);
    bool loadMaterial(Texture::Ptr& texture, const std::string& fileName);

    std::vector<Renderable::SubMesh::Ptr> entries_;
    std::vector<Material::Ptr> materials_;
};

}}

#endif //MESH_H